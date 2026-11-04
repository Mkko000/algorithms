from web.lib.trace import Trace, run_c_harness
from web.traces.common import SORT_PRESETS, line_with, match_events, printed_letters, snippet, sort_driver, sort_frames

SOURCE = "src/06-sorting/insertion_sort.c"
FN = "insertion"


def rules(snip):
    return {
        ("less", line_with(snip, "compexch(a[i - 1], a[i])")): {"names": ("i", "i-1"), "say": ("sentinel_swap", "sentinel_keep")},
        ("exch", line_with(snip, "compexch(a[i - 1], a[i])")): {"names": ("i-1", "i"), "say": "sentinel_exch"},
        ("less", line_with(snip, "less(v, a[j - 1])")): {"names": ("v", "j-1"), "say": ("shift", "place")},
    }


def expected(values):
    a = list(values)
    l, r = 0, len(a) - 1
    out = []
    for i in range(r, l, -1):
        out.append(("less", i, i - 1, {"vars": {"i": i}}))
        if a[i] < a[i - 1]:
            a[i - 1], a[i] = a[i], a[i - 1]
            out.append(("exch", i - 1, i, {"vars": {"i": i}}))
    for i in range(l + 2, r + 1):
        j, v = i, a[i]
        while True:
            out.append(("less", None, j - 1, {"vars": {"i": i, "v": v}, "ptr": {"i": i}, "args": {}}))
            if not v < a[j - 1]:
                break
            a[j] = a[j - 1]
            j -= 1
        a[j] = v
    return out


def run(values, as_letters, snip):
    events = run_c_harness(SOURCE, [FN], sort_driver(FN, values))
    extras = match_events(events, expected(values), SOURCE)
    return sort_frames(events, values, as_letters, rules(snip), extras=extras)


def build() -> Trace:
    snip = snippet(SOURCE, FN)
    return Trace(FN, SOURCE, snip, {k: run(v, lt, snip)[0] for k, (v, lt) in SORT_PRESETS.items()})


def check():
    values, lt = SORT_PRESETS["book"]
    _, last = run(values, lt, snippet(SOURCE, FN))
    return [chr(v) for v in last], printed_letters(SOURCE)
