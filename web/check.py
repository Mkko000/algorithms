import re
import subprocess
import sys
import tempfile
import tomllib
from pathlib import Path
from urllib.parse import unquote

from web.lib.csource import ROOT
from web.lib.manifest import load_arabic_only, load_course, validate_course

ATTR = re.compile(r'(?:href|src)="([^"]*)"')
JSON_URL = re.compile(r'"url":"([^"]+)"')
IDS = re.compile(r'id="([^"]+)"')
DASHES = re.compile(r"[—–]")
SPACED_HYPHEN = re.compile(r"\w - \w")
DASH_ITEM = re.compile(r"^\s*- ", re.M)


def check_links(out: Path, base: str) -> list:
    errors = []
    ids = {}
    for page in sorted(out.rglob("*.html")):
        rel = page.relative_to(out)
        html = page.read_text(encoding="utf-8")
        targets = ATTR.findall(html) + [u.replace("\\/", "/") for u in JSON_URL.findall(html)]
        for t in targets:
            if t.startswith(("lesson:", "src:")):
                errors.append(f"{rel}: unresolved link {t}")
                continue
            if not t.startswith("/"):
                continue
            if not t.startswith(base):
                errors.append(f"{rel}: {t} does not start with {base}")
                continue
            path, _, frag = t[len(base):].partition("#")
            path = unquote(path.split("?")[0])
            file = out / path
            if path == "" or path.endswith("/"):
                file = file / "index.html"
            if not file.exists():
                errors.append(f"{rel}: broken link {t}")
                continue
            if frag:
                if file not in ids:
                    ids[file] = set(IDS.findall(file.read_text(encoding="utf-8")))
                if frag not in ids[file]:
                    errors.append(f"{rel}: missing anchor {t}")
    return errors


def english_strings() -> list:
    found = []
    for path in sorted((ROOT / "content" / "en").glob("*.md")):
        found.append((str(path.relative_to(ROOT)), path.read_text(encoding="utf-8")))
    ui = tomllib.loads((ROOT / "content" / "ui.toml").read_text(encoding="utf-8"))
    found += [(f"content/ui.toml [en] {k}", v) for k, v in ui["en"].items()]
    tr = tomllib.loads((ROOT / "content" / "en" / "traces.toml").read_text(encoding="utf-8"))
    found += [(f"content/en/traces.toml {t}.{k}", v) for t, keys in tr.items() for k, v in keys.items()]
    course = tomllib.loads((ROOT / "content" / "course.toml").read_text(encoding="utf-8"))
    for part in course["part"]:
        found.append((f"course.toml part {part['key']}", part["title"]["en"]))
        found += [(f"course.toml {l['slug']}", l["title"]["en"]) for l in part["lesson"]]
    readme = ROOT / "web" / "readme"
    if readme.exists():
        for name in ("README.md", "README.ar.md"):
            if (ROOT / name).exists():
                found.append((name, (ROOT / name).read_text(encoding="utf-8")))
        for p in sorted(readme.glob("*.in")):
            found.append((str(p.relative_to(ROOT)), p.read_text(encoding="utf-8")))
    return found


def prose(text: str) -> str:
    text = re.sub(r"```[\s\S]*?```|~~~[\s\S]*?~~~", "", text)
    text = re.sub(r"`[^`\n]*`", "", text)
    return re.sub(r"^::.*$", "", text, flags=re.M)


def check_house_rules(items: list) -> list:
    errors = []
    for where, text in items:
        body = prose(text)
        if DASHES.search(body):
            errors.append(f"{where}: em or en dash")
        if SPACED_HYPHEN.search(body):
            errors.append(f"{where}: hyphen used as punctuation")
        if DASH_ITEM.search(body):
            errors.append(f"{where}: list item starting with a hyphen")
    return errors


def main() -> int:
    errors = []
    r = subprocess.run(["make", "check-c"], cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        errors.append("make check-c failed:\n" + r.stdout[-2000:] + r.stderr[-2000:])
    course = load_course(ROOT / "content" / "course.toml")
    errors += validate_course(course, ROOT / "content", load_arabic_only(ROOT / "content" / "arabic_only.txt"))
    from web.build import build
    with tempfile.TemporaryDirectory() as tmp:
        out = Path(tmp) / "site"
        try:
            build(out, "/algorithms/")
            errors += check_links(out, "/algorithms/")
        except Exception as e:
            errors.append(f"build failed: {e}")
    errors += check_house_rules(english_strings())
    for e in errors:
        print(e)
    if errors:
        return 1
    print("ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
