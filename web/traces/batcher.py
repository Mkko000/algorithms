from web.lib.csource import run_output
from web.lib.render import cells
from web.lib.trace import Frame, Trace
from web.traces.common import line_with, seeded, snippet

SOURCE = "src/11-special-sorts/batcher_network.c"
PRESETS = {
    "book": [42, 7, 99, 7, 3, 58, 21, 0],
    "random": seeded(8, 3),
    "sorted": list(range(1, 9)),
    "reversed": list(range(8, 0, -1)),
    "dups": [1, 0, 1, 0, 1, 0, 1, 0],
}


def network(n):
    pos = list(range(n))
    aux = [0] * n
    comps = []

    def shuffle(a, l, r):
        m = (l + r) // 2
        j = 0
        for i in range(l, r + 1, 2):
            aux[i] = a[l + j]
            aux[i + 1] = a[m + 1 + j]
            j += 1
        for i in range(l, r + 1):
            a[i] = aux[i]

    def unshuffle(a, l, r):
        m = (l + r) // 2
        j = 0
        for i in range(l, r + 1, 2):
            aux[l + j] = a[i]
            aux[m + 1 + j] = a[i + 1]
            j += 1
        for i in range(l, r + 1):
            a[i] = aux[i]

    def merge(l, m, r):
        if r == l + 1:
            comps.append((pos[l], pos[r]))
            return
        if r < l + 2:
            return
        unshuffle(pos, l, r)
        merge(l, (l + m) // 2, m)
        merge(m + 1, (m + 1 + r) // 2, r)
        shuffle(pos, l, r)
        for i in range(l + 1, r, 2):
            comps.append((pos[i], pos[i + 1]))

    def sort(l, r):
        if r <= l:
            return
        m = (l + r) // 2
        sort(l, m)
        sort(m + 1, r)
        merge(l, m, r)

    sort(0, n - 1)
    return comps


def run(values, code):
    line = line_with(code, "if (a[j] < a[i]) exch(a[i], a[j]);")
    a = list(values)
    out = [Frame(None, cells(a), {"comparator": "0", "swaps": "0"}, "start", {"n": str(len(a))})]
    swaps = 0
    comps = network(len(a))
    for k, (i, j) in enumerate(comps, 1):
        swap = a[j] < a[i]
        if swap:
            a[i], a[j] = a[j], a[i]
            swaps += 1
        out.append(Frame(line, cells(a, {i: "s-focus" if swap else "s-look", j: "s-focus" if swap else "s-look"},
                                     {"i": i, "j": j}),
                         {"comparator": f"{k} / {len(comps)}", "swaps": str(swaps)},
                         "swap" if swap else "keep", {"i": str(i), "j": str(j), "x": str(a[i]), "y": str(a[j])}))
    out.append(Frame(None, cells(a, {k: "s-done" for k in range(len(a))}),
                     {"comparator": f"{len(comps)} / {len(comps)}", "swaps": str(swaps)}, "done",
                     {"n": str(len(comps))}))
    return tuple(out), a, comps


def build() -> Trace:
    code = snippet(SOURCE, "run_network")
    return Trace("batcher", SOURCE, code, {k: run(v, code)[0] for k, v in PRESETS.items()})


def check():
    code = snippet(SOURCE, "run_network")
    _, a, comps = run(PRESETS["book"], code)
    out = run_output(SOURCE).split("\n")
    printed_net = out[out.index(next(l for l in out if l.startswith("network for N=8"))) + 1].split()
    printed_sorted = next(l for l in out if l.startswith("network on")).split("->")[1].split()
    return ([f"{i}:{j}" for i, j in comps], [str(x) for x in a]), (printed_net, printed_sorted)
