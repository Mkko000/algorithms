from web.lib.trace import Trace, run_c_harness
from web.traces.common import SORT_PRESETS, line_with, match_events, printed_letters, snippet, sort_driver, sort_frames

SOURCE = "src/06-sorting/shellsort.c"
FN = "shellsort"
PRESETS = SORT_PRESETS


def rules(snip):
    return {("less", line_with(snip, "less(v, a[j - h])")): {"names": ("v", "j-h"), "say": ("shift", "place")}}


def expected(values):
    a = list(values)
    l, r = 0, len(a) - 1
    out = []
    h = 1
    while h <= (r - l) // 9:
        h = 3 * h + 1
    while h > 0:
        for i in range(l + h, r + 1):
            j, v = i, a[i]
            while j >= l + h:
                out.append(("less", None, j - h, {"vars": {"h": h, "v": v}, "ptr": {"i": i}, "args": {"h": h}}))
                if not v < a[j - h]:
                    break
                a[j] = a[j - h]
                j -= h
            a[j] = v
        h //= 3
    return out


def run(values, as_letters, snip):
    events = run_c_harness(SOURCE, [FN], sort_driver(FN, values))
    extras = match_events(events, expected(values), SOURCE)
    return sort_frames(events, values, as_letters, rules(snip), extras=extras)


def build() -> Trace:
    snip = snippet(SOURCE, FN)
    return Trace(FN, SOURCE, snip, {k: run(v, lt, snip)[0] for k, (v, lt) in PRESETS.items()})


def check():
    values, lt = PRESETS["book"]
    _, last = run(values, lt, snippet(SOURCE, FN))
    return [chr(v) for v in last], printed_letters(SOURCE)
