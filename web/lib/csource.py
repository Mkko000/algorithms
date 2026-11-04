import hashlib
import re
import subprocess
import textwrap
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CACHE = ROOT / ".cache"
CFLAGS = ["-std=c99", "-Wall", "-Wextra", "-O2"]


class SourceError(Exception):
    pass


@dataclass(frozen=True)
class Snippet:
    path: str
    text: str
    first_line: int
    last_line: int


def _abs(path: str) -> Path:
    return ROOT / path


def _rel(path: str) -> str:
    p = _abs(path).resolve()
    try:
        return p.relative_to(ROOT).as_posix()
    except ValueError:
        return str(p)


def _read(path: str) -> str:
    try:
        return _abs(path).read_text(encoding="utf-8")
    except OSError as e:
        raise SourceError(f"{path}: cannot read ({e.strerror})")


def _mask(src: str) -> str:
    out = list(src)
    i, n = 0, len(src)
    line_start = True
    while i < n:
        c = src[i]
        if line_start and c in " \t":
            i += 1
            continue
        if line_start and c == "#":
            j = i
            while j < n:
                if src[j] == "\n" and src[j - 1] != "\\":
                    break
                j += 1
            for k in range(i, j):
                if out[k] != "\n":
                    out[k] = " "
            i = j
            continue
        line_start = c == "\n"
        if src.startswith("/*", i):
            j = src.find("*/", i + 2)
            j = n if j < 0 else j + 2
            for k in range(i, j):
                if out[k] != "\n":
                    out[k] = " "
            i = j
            continue
        if src.startswith("//", i):
            j = src.find("\n", i)
            j = n if j < 0 else j
            for k in range(i, j):
                out[k] = " "
            i = j
            continue
        if c in "\"'":
            j = i + 1
            while j < n and src[j] != c and src[j] != "\n":
                j += 2 if src[j] == "\\" else 1
            for k in range(i, min(j + 1, n)):
                if out[k] != "\n":
                    out[k] = " "
            i = j + 1
            continue
        i += 1
    return "".join(out)


def _depths(masked: str) -> list:
    d, res = 0, []
    for c in masked:
        if c == "}":
            d -= 1
        res.append(d)
        if c == "{":
            d += 1
    return res


def _match_brace(masked: str, open_pos: int) -> int:
    d = 0
    for k in range(open_pos, len(masked)):
        if masked[k] == "{":
            d += 1
        elif masked[k] == "}":
            d -= 1
            if d == 0:
                return k
    raise SourceError("unbalanced braces")


def _line_no(src: str, pos: int) -> int:
    return src.count("\n", 0, pos) + 1


def _snippet(path: str, src: str, start: int, end: int) -> Snippet:
    line_start = src.rfind("\n", 0, start) + 1
    line_end = src.find("\n", end)
    line_end = len(src) if line_end < 0 else line_end
    text = src[line_start:line_end]
    first = _line_no(src, line_start)
    return Snippet(_rel(path), text, first, first + text.count("\n"))


def _decl_start(src: str, masked: str, pos: int) -> int:
    start = src.rfind("\n", 0, pos) + 1
    while start > 0:
        prev_end = start - 1
        prev_start = src.rfind("\n", 0, prev_end) + 1
        code = masked[prev_start:prev_end].strip()
        raw = src[prev_start:prev_end].strip()
        if not code or not raw or code.endswith((";", "}", "{")) or raw.startswith("#"):
            break
        start = prev_start
    return start


def _find_macro(src: str, name: str):
    m = re.search(rf"^[ \t]*#[ \t]*define[ \t]+{re.escape(name)}\b", src, re.M)
    if not m:
        return None
    end = m.start()
    while True:
        nl = src.find("\n", end)
        if nl < 0:
            nl = len(src)
        if src[nl - 1] != "\\" or nl == len(src):
            return m.start(), nl
        end = nl + 1


def _find_function(src: str, masked: str, depth: list, name: str):
    for m in re.finditer(rf"\b{re.escape(name)}\s*\(", masked):
        if depth[m.start()] != 0:
            continue
        k = m.end()
        while k < len(masked) and masked[k] not in "{;":
            k += 1
        if k < len(masked) and masked[k] == "{":
            return _decl_start(src, masked, m.start()), _match_brace(masked, k)
    return None


def _find_struct(src: str, masked: str, depth: list, name: str):
    for m in re.finditer(rf"\bstruct\s+{re.escape(name)}\s*\{{", masked):
        if depth[m.start()] != 0:
            continue
        close = _match_brace(masked, m.end() - 1)
        semi = masked.find(";", close)
        return (_decl_start(src, masked, m.start()), semi if semi >= 0 else close)
    for m in re.finditer(r"\btypedef\b", masked):
        if depth[m.start()] != 0:
            continue
        k = m.end()
        while k < len(masked):
            if masked[k] == "{":
                k = _match_brace(masked, k) + 1
                continue
            if masked[k] == ";":
                break
            k += 1
        if re.search(rf"\b{re.escape(name)}\s*$", masked[m.start():k]):
            return m.start(), k
    return None


def extract_function(path: str, name: str) -> Snippet:
    src = _read(path)
    macro = _find_macro(src, name)
    masked = _mask(src)
    depth = _depths(masked)
    found = _find_function(src, masked, depth, name) or _find_struct(src, masked, depth, name)
    if found is None and macro is not None:
        found = macro
    if found is None:
        raise SourceError(f"{path}: no definition of {name}")
    return _snippet(path, src, found[0], found[1])


def extract_region(path: str, mark: str) -> Snippet:
    lines = _read(path).split("\n")
    a = next((i for i, l in enumerate(lines) if "//>>" + mark in l), None)
    b = next((i for i, l in enumerate(lines) if "//<<" + mark in l), None)
    if a is None or b is None or b <= a:
        raise SourceError(f"{path}: no region {mark}")
    body = lines[a + 1:b]
    return Snippet(_rel(path), "\n".join(body), a + 2, b)


def _header_end(src: str) -> int:
    s = src.lstrip()
    if not s.startswith("/*"):
        return 0
    off = len(src) - len(s)
    end = src.find("*/", off)
    if end < 0:
        return 0
    nl = src.find("\n", end)
    return len(src) if nl < 0 else nl + 1


def whole_file(path: str) -> Snippet:
    src = _read(path)
    start = _header_end(src)
    while start < len(src) and src[start] == "\n":
        start += 1
    text = src[start:].rstrip("\n")
    first = _line_no(src, start)
    return Snippet(_rel(path), text, first, first + text.count("\n"))


def header_comment(path: str) -> str:
    src = _read(path)
    end = _header_end(src)
    if not end:
        return ""
    block = src[:end].strip()
    block = block[2:]
    block = block[: block.rfind("*/")]
    lines = []
    for line in block.split("\n"):
        line = re.sub(r"^\s*\*", "", line).rstrip()
        if re.fullmatch(r"\s*[=\-*]{3,}\s*", line):
            continue
        lines.append(line)
    text = textwrap.dedent("\n".join(lines))
    return text.strip("\n").strip()


_outputs = {}


def _deps(src_path: Path) -> list:
    r = subprocess.run(["gcc", "-MM", str(src_path)], capture_output=True, text=True, cwd=ROOT)
    if r.returncode:
        raise SourceError(f"{_rel(str(src_path))}: compile failed\n{r.stderr}")
    deps = r.stdout.replace("\\\n", " ").split(":", 1)[1].split()
    return [(src_path.parent / d) if not Path(d).is_absolute() else Path(d) for d in deps]


def compile_program(path: str) -> Path:
    src = _abs(path)
    h = hashlib.sha1(" ".join(CFLAGS).encode())
    for d in _deps(src):
        p = d if d.is_absolute() else ROOT / d
        h.update(p.read_bytes())
    exe = CACHE / "bin" / h.hexdigest()
    if not exe.exists():
        exe.parent.mkdir(parents=True, exist_ok=True)
        r = subprocess.run(["gcc", *CFLAGS, str(src), "-o", str(exe), "-lm"],
                           capture_output=True, text=True, cwd=ROOT)
        if r.returncode:
            raise SourceError(f"{_rel(path)}: compile failed\n{r.stderr}")
    return exe


def run_output(path: str, max_lines: int | None = None) -> str:
    key = _rel(path)
    if key not in _outputs:
        exe = compile_program(path)
        try:
            p = subprocess.run([str(exe)], capture_output=True, cwd=ROOT, timeout=60,
                               stdin=subprocess.DEVNULL)
        except subprocess.TimeoutExpired:
            raise SourceError(f"{key}: timed out")
        if p.returncode:
            raise SourceError(f"{key}: exited with {p.returncode}\n{p.stderr.decode(errors='replace')}")
        _outputs[key] = p.stdout.decode("utf-8", errors="replace").rstrip("\n")
    out = _outputs[key]
    if max_lines is None:
        return out
    lines = out.split("\n")
    if len(lines) <= max_lines:
        return out
    return "\n".join(lines[:max_lines] + ["..."])
