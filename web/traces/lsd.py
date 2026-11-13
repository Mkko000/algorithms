from web.lib.csource import run_output
from web.lib.render import array_rows
from web.lib.trace import Frame, Trace
from web.traces.common import line_with, snippet

SOURCE = "src/10-radix-sort/lsd_radix.c"
PRESETS = {
    "book": "dab add cab fad fee bad dad bee fed bed ebb ace".split(),
    "words": "now for tip ilk dim tag jot sob nob sky hut ace bet men egg few".split(),
}


def picture(words, col):
    return array_rows([(str(k), list(w), {col: "hi"} if col is not None else {}) for k, w in enumerate(words)], 26)


def run(words, code):
    line = line_with(code, "strcpy(a[i], aux[i]);")
    a = list(words)
    w = len(a[0])
    out = [Frame(None, picture(a, None), {"pass": "0", "digit": "none"}, "start", {"n": str(len(a)), "w": str(w)})]
    for p, d in enumerate(range(w - 1, -1, -1), 1):
        buckets = {}
        for word in a:
            buckets.setdefault(word[d], []).append(word)
        a = [word for c in sorted(buckets) for word in buckets[c]]
        out.append(Frame(line, picture(a, d), {"pass": str(p), "digit": str(d)}, "pass",
                         {"d": str(d), "p": str(p)}))
    out.append(Frame(None, picture(a, None), {"pass": str(w), "digit": "done"}, "done", {"n": str(len(a))}))
    return tuple(out), a


def build() -> Trace:
    code = snippet(SOURCE, "lsd_radix")
    return Trace("lsd", SOURCE, code, {k: run(v, code)[0] for k, v in PRESETS.items()})


def check():
    _, a = run(PRESETS["book"], snippet(SOURCE, "lsd_radix"))
    return a, run_output(SOURCE).split()
