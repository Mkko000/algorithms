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
static void T_dump(const char *op, int line, int ia, int ib, int va, int vb)
{
    printf("%s %d %d %d %d %d", op, line, ia, ib, va, vb);
    for (int k = 0; k < T_n; k++) printf(" %d", T_base[k]);
    printf("\n");
}
static int T_less(Item *pa, Item *pb, int line) { T_dump("less", line, T_idx(pa), T_idx(pb), *pa, *pb); return *pa < *pb; }
static void T_exch(Item *pa, Item *pb, int line) { Item t = *pa; *pa = *pb; *pb = t; T_dump("exch", line, T_idx(pa), T_idx(pb), *pa, *pb); }
static void T_mark(const char *what, int line, int va, int vb) { T_dump(what, line, -1, -1, va, vb); }
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
    x: int
    y: int
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


def span(source: str, first_fn: str, last_fn: str) -> Snippet:
    a = extract_function(source, first_fn)
    b = extract_function(source, last_fn)
    lines = (ROOT / source).read_text(encoding="utf-8").split("\n")
    return Snippet(a.path, "\n".join(lines[a.first_line - 1:b.last_line]), a.first_line, b.last_line)


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
        if len(parts) < 6:
            continue
        ia, ib = int(parts[2]), int(parts[3])
        events.append(Event(parts[0], int(parts[1]), None if ia < 0 else ia, None if ib < 0 else ib,
                            int(parts[4]), int(parts[5]), tuple(int(x) for x in parts[6:])))
    return events


def load_templates(path: Path) -> dict:
    return tomllib.loads(Path(path).read_text(encoding="utf-8"))


def frames_json(trace: Trace) -> dict:
    return {"name": trace.name, "first_line": trace.code.first_line,
            "presets": {p: [{"line": f.line, "svg": f.svg, "vars": f.vars} for f in frames]
                        for p, frames in trace.presets.items()}}


def says(trace: Trace, templates: dict) -> dict:
    out = {}
    for name, frames in trace.presets.items():
        lines = []
        for f in frames:
            if f.say not in templates:
                raise KeyError(f"{trace.name}: no {f.say}")
            lines.append(templates[f.say].format(**f.args))
        out[name] = lines
    return out


def payload(trace: Trace, templates: dict) -> dict:
    text = says(trace, templates)
    data = frames_json(trace)
    for name, frames in data["presets"].items():
        for f, say in zip(frames, text[name]):
            f["say"] = say
    data["source"] = trace.source
    data["code_html"] = highlight_c(trace.code.text)
    return data


def _json(data: dict) -> str:
    return json.dumps(data, ensure_ascii=False, separators=(",", ":")).replace("</", "<\\/")


def section_html(trace: Trace, templates: dict, ui: dict, preset_labels: dict, data_url: str | None = None) -> str:
    text = says(trace, templates)
    first_preset = next(iter(trace.presets))
    first = trace.presets[first_preset][0]
    n0 = trace.code.first_line
    lines = highlight_c(trace.code.text).split("\n")
    code = "".join(
        f'<span class="ln{" on" if first.line == n0 + k else ""}" data-n="{n0 + k}">{l}</span>\n'
        for k, l in enumerate(lines))
    options = "".join(
        f'<option value="{escape(p)}">{escape(preset_labels.get(p, p))}</option>' for p in trace.presets)
    vars_html = "".join(f'<span>{escape(k)} = <b>{escape(v)}</b></span>' for k, v in first.vars.items())
    total = len(trace.presets[first_preset])
    if data_url:
        embedded = {"name": trace.name, "first_line": n0, "url": data_url, "says": text}
    else:
        embedded = {**frames_json(trace), "says": text}
    return (
        f'<section class="trace" dir="ltr" data-trace="{escape(trace.name)}" tabindex="0" '
        f'aria-label="{escape(ui.get("trace", ""))} {escape(trace.name)}">'
        f'<header class="trace-head"><span>{escape(trace.name)}</span>'
        f'<span class="trace-src" data-src="{escape(trace.source)}">{escape(trace.source)}</span></header>'
        f'<div class="trace-body"><pre class="trace-code"><code>{code}</code></pre>'
        f'<div class="trace-state"><div class="trace-scene">{first.svg}</div>'
        f'<p class="trace-say" aria-live="polite">{escape(text[first_preset][0])}</p>'
        f'<p class="trace-vars">{vars_html}</p></div></div>'
        f'<div class="trace-ctl">'
        f'<button type="button" data-a="reset" aria-label="{escape(ui.get("reset", ""))}">⟲</button>'
        f'<button type="button" data-a="back" aria-label="{escape(ui.get("step_back", ""))}">‹</button>'
        f'<button type="button" data-a="play" class="play" data-play="{escape(ui.get("play", ""))}" '
        f'data-pause="{escape(ui.get("pause", ""))}">▶ {escape(ui.get("play", ""))}</button>'
        f'<button type="button" data-a="fwd" aria-label="{escape(ui.get("step_forward", ""))}">›</button>'
        f'<input class="trace-scrub" type="range" min="0" max="{total - 1}" value="0" '
        f'aria-label="{escape(ui.get("step", ""))}">'
        f'<span class="trace-count" data-label="{escape(ui.get("step", ""))}">{escape(ui.get("step", ""))} 1 / {total}</span>'
        f'<label class="trace-preset"><span>{escape(ui.get("preset", ""))}</span><select>{options}</select></label>'
        f'</div><script type="application/json">{_json(embedded)}</script></section>'
    )
