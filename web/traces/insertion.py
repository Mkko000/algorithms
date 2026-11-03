from web.lib.trace import Trace, run_c_harness
from web.traces.common import SORT_PRESETS, line_with, printed_letters, snippet, sort_driver, sort_frames

SOURCE = "src/06-sorting/insertion_sort.c"
FN = "insertion"


def rules(snip):
    return {
        ("less", line_with(snip, "compexch(a[i - 1], a[i])")): {"names": ("i", "i-1"), "say": ("sentinel_swap", "sentinel_keep")},
        ("exch", line_with(snip, "compexch(a[i - 1], a[i])")): {"names": ("i-1", "i"), "say": "sentinel_exch"},
        ("less", line_with(snip, "less(v, a[j - 1])")): {"names": ("v", "j-1"), "say": ("shift", "place")},
    }


def run(values, as_letters, snip):
    events = run_c_harness(SOURCE, [FN], sort_driver(FN, values))
    return sort_frames(events, values, as_letters, rules(snip))


def build() -> Trace:
    snip = snippet(SOURCE, FN)
    return Trace(FN, SOURCE, snip, {k: run(v, lt, snip)[0] for k, (v, lt) in SORT_PRESETS.items()})


def check():
    values, lt = SORT_PRESETS["book"]
    _, last = run(values, lt, snippet(SOURCE, FN))
    return [chr(v) for v in last], printed_letters(SOURCE)
