from web.lib.trace import Trace, run_c_harness, span
from web.traces.common import SORT_PRESETS, letters, line_with, printed_letters, sort_driver, sort_frames

SOURCE = "src/08-mergesort/mergesort.c"
GLOBALS = "#define maxN 1000\nstatic Item aux[maxN];\n"
PRESETS = {**SORT_PRESETS, "book": (letters("MERGESORTEXAMPLE"), True)}


def rules(code):
    return {("less", line_with(code, "a[k] = less(aux[i], aux[j]) ? aux[i++] : aux[j--];")): {"say": ("take_left", "take_right")}}


def run(values, as_letters, code):
    events = run_c_harness(SOURCE, ["merge", "mergesort"], sort_driver("mergesort", values), GLOBALS)
    return sort_frames(events, values, as_letters, rules(code))


def build() -> Trace:
    code = span(SOURCE, "merge", "mergesort")
    return Trace("mergesort", SOURCE, code, {k: run(v, lt, code)[0] for k, (v, lt) in PRESETS.items()})


def check():
    values, lt = PRESETS["book"]
    _, last = run(values, lt, span(SOURCE, "merge", "mergesort"))
    return [chr(v) for v in last], printed_letters(SOURCE)
