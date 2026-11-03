from web.lib.trace import Trace, run_c_harness
from web.traces.common import SORT_PRESETS, line_with, printed_letters, snippet, sort_driver, sort_frames

SOURCE = "src/06-sorting/selection_sort.c"
FN = "selection"


def rules(snip):
    return {
        ("less", line_with(snip, "less(a[j], a[min])")): {"names": ("j", "min"), "say": ("new_min", "keep_min")},
        ("exch", line_with(snip, "exch(a[i], a[min])")): {"names": ("i", "min"), "say": "swap"},
    }


def run(values, as_letters, snip):
    settled = {"upto": 0}

    def done(e, a):
        if e.op == "exch":
            settled["upto"] = e.i + 1
        return range(settled["upto"])

    events = run_c_harness(SOURCE, [FN], sort_driver(FN, values))
    return sort_frames(events, values, as_letters, rules(snip), done)


def build() -> Trace:
    snip = snippet(SOURCE, FN)
    return Trace(FN, SOURCE, snip, {k: run(v, lt, snip)[0] for k, (v, lt) in SORT_PRESETS.items()})


def check():
    values, lt = SORT_PRESETS["book"]
    _, last = run(values, lt, snippet(SOURCE, FN))
    return [chr(v) for v in last], printed_letters(SOURCE)
