from web.lib.trace import Trace, run_c_harness
from web.traces.common import SORT_PRESETS, line_with, printed_letters, snippet, sort_driver, sort_frames

SOURCE = "src/06-sorting/shellsort.c"
FN = "shellsort"
PRESETS = SORT_PRESETS


def rules(snip):
    return {("less", line_with(snip, "less(v, a[j - h])")): {"names": ("v", "j-h"), "say": ("shift", "place")}}


def run(values, as_letters, snip):
    events = run_c_harness(SOURCE, [FN], sort_driver(FN, values))
    return sort_frames(events, values, as_letters, rules(snip))


def build() -> Trace:
    snip = snippet(SOURCE, FN)
    return Trace(FN, SOURCE, snip, {k: run(v, lt, snip)[0] for k, (v, lt) in PRESETS.items()})


def check():
    values, lt = PRESETS["book"]
    _, last = run(values, lt, snippet(SOURCE, FN))
    return [chr(v) for v in last], printed_letters(SOURCE)
