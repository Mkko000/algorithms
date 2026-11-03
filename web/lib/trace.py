import hashlib
import json
import subprocess
import tomllib
from dataclasses import dataclass, field
from html import escape
from pathlib import Path

from web.lib.csource import CACHE, ROOT, SourceError, Snippet, extract_function
from web.lib.highlight import highlight_c

PRELUDE = r'''#include <stdio.h>
#include <stdlib.h>
typedef int Item;
static Item *T_base;
static int T_n;
static int T_idx(Item *p) { return (T_base && p >= T_base && p < T_base + T_n) ? (int)(p - T_base) : -1; }
static void T_dump(const char *op, int line, int ia, int ib)
{
    printf("%s %d %d %d", op, line, ia, ib);
    for (int k = 0; k < T_n; k++) printf(" %d", T_base[k]);
    printf("\n");
}
static int T_less(Item *pa, Item *pb, int line) { T_dump("less", line, T_idx(pa), T_idx(pb)); return *pa < *pb; }
static void T_exch(Item *pa, Item *pb, int line) { Item t = *pa; *pa = *pb; *pb = t; T_dump("exch", line, T_idx(pa), T_idx(pb)); }
static void T_mark(const char *what, int line) { T_dump(what, line, -1, -1); }
#define key(A) (A)
#define less(A, B) T_less(&(A), &(B), __LINE__)
#define exch(A, B) T_exch(&(A), &(B), __LINE__)
#define compexch(A, B) if (less(B, A)) exch(A, B)
#define eq(A, B) (!less(A, B) && !less(B, A))
'''


@dataclass(frozen=True)
class Event:
    op: str
    line: int
    i: int | None
    j: int | None
    a: tuple


@dataclass(frozen=True)
class Frame:
    line: int | None
    svg: str
    vars: dict
    say: str
    args: dict = field(default_factory=dict)


@dataclass(frozen=True)
class Trace:
    name: str
    source: str
    code: Snippet
    presets: dict


def harness_source(source: str, functions: list, driver: str, globals_: str = "") -> str:
    parts = [PRELUDE, globals_]
    for fn in functions:
        snip = extract_function(source, fn)
        parts.append(f'#line {snip.first_line} "{source}"\n{snip.text}\n')
    parts.append(f'#line 1 "driver"\n{driver}\n')
    return "\n".join(parts)


def run_c_harness(source: str, functions: list, driver: str, globals_: str = "") -> list:
    code = harness_source(source, functions, driver, globals_)
    h = hashlib.sha1(code.encode()).hexdigest()
    d = CACHE / "trace"
    d.mkdir(parents=True, exist_ok=True)
    src, exe = d / f"{h}.c", d / h
    if not exe.exists():
        src.write_text(code, encoding="utf-8")
        r = subprocess.run(["gcc", "-std=gnu99", "-O0", str(src), "-o", str(exe)], capture_output=True, text=True)
        if r.returncode:
            raise SourceError(f"{source}: trace harness failed to compile\n{r.stderr}")
    p = subprocess.run([str(exe)], capture_output=True, text=True, timeout=30, cwd=ROOT)
    if p.returncode:
        raise SourceError(f"{source}: trace harness exited with {p.returncode}\n{p.stderr}")
    events = []
    for line in p.stdout.splitlines():
        parts = line.split()
        if len(parts) < 4:
            continue
        ia, ib = int(parts[2]), int(parts[3])
        events.append(Event(parts[0], int(parts[1]), None if ia < 0 else ia, None if ib < 0 else ib,
                            tuple(int(x) for x in parts[4:])))
    return events


def load_templates(path: Path) -> dict:
    return tomllib.loads(Path(path).read_text(encoding="utf-8"))


def payload(trace: Trace, templates: dict) -> dict:
    presets = {}
    for name, frames in trace.presets.items():
        out = []
        for f in frames:
            if f.say not in templates:
                raise KeyError(f"{trace.name}: no {f.say}")
            out.append({"line": f.line, "svg": f.svg, "vars": f.vars, "say": templates[f.say].format(**f.args)})
        presets[name] = out
    return {"name": trace.name, "source": trace.source, "code_html": highlight_c(trace.code.text),
            "first_line": trace.code.first_line, "presets": presets}


def _json(data: dict) -> str:
    return json.dumps(data, ensure_ascii=False, separators=(",", ":")).replace("</", "<\\/")


def section_html(trace: Trace, templates: dict, ui: dict, preset_labels: dict) -> str:
    data = payload(trace, templates)
    first = next(iter(data["presets"].values()))[0]
    lines = data["code_html"].split("\n")
    n0 = data["first_line"]
    code = "".join(
        f'<span class="ln{" on" if first["line"] == n0 + k else ""}" data-n="{n0 + k}">{l}</span>\n'
        for k, l in enumerate(lines))
    options = "".join(
        f'<option value="{escape(p)}">{escape(preset_labels.get(p, p))}</option>' for p in data["presets"])
    vars_html = "".join(f'<span>{escape(k)} = <b>{escape(v)}</b></span>' for k, v in first["vars"].items())
    total = len(next(iter(data["presets"].values())))
    return (
        f'<section class="trace" dir="ltr" data-trace="{escape(trace.name)}" tabindex="0" '
        f'aria-label="{escape(ui.get("trace", ""))} {escape(trace.name)}">'
        f'<header class="trace-head"><span>{escape(ui.get("trace", ""))} · {escape(trace.name)}()</span>'
        f'<span class="trace-src" data-src="{escape(trace.source)}">{escape(trace.source)}</span></header>'
        f'<div class="trace-body"><pre class="trace-code"><code>{code}</code></pre>'
        f'<div class="trace-state"><div class="trace-scene">{first["svg"]}</div>'
        f'<p class="trace-say" aria-live="polite">{escape(first["say"])}</p>'
        f'<p class="trace-vars">{vars_html}</p></div></div>'
        f'<div class="trace-ctl">'
        f'<button type="button" data-a="reset" aria-label="{escape(ui.get("reset", ""))}">⟲</button>'
        f'<button type="button" data-a="back" aria-label="{escape(ui.get("step_back", ""))}">‹</button>'
        f'<button type="button" data-a="play" class="play" data-play="{escape(ui.get("play", ""))}" '
        f'data-pause="{escape(ui.get("pause", ""))}">▶ {escape(ui.get("play", ""))}</button>'
        f'<button type="button" data-a="fwd" aria-label="{escape(ui.get("step_forward", ""))}">›</button>'
        f'<input class="trace-scrub" type="range" min="0" max="{total - 1}" value="0" '
        f'aria-label="{escape(ui.get("step", ""))}">'
        f'<span class="trace-count">{escape(ui.get("step", ""))} 1 / {total}</span>'
        f'<label class="trace-preset"><span>{escape(ui.get("preset", ""))}</span><select>{options}</select></label>'
        f'</div><script type="application/json">{_json(data)}</script></section>'
    )
