from web.lib.csource import run_output
from web.lib.render import cells
from web.lib.trace import Frame, Trace
from web.traces.common import line_with, snippet

SOURCE = "src/14-hashing/hash_linear_probing.c"
M = 13
PRESETS = {"book": "ASERCHINGXMPL", "cluster": "AHOVBIPWC"}


def h(c):
    return 7 * (ord(c) - ord("A") + 1) % M


def run(keys, code):
    L_probe = line_with(code, "while (t[i]) { probed[i] = 1; i = (i + k) % M; p++; }")
    L_place = line_with(code, "probed[i] = 1; t[i] = c; probesHit[c - 'A'] = p;")
    t = [""] * M
    total = 0

    def pic(states, ptr):
        return cells([c or "·" for c in t], states, ptr)

    out = [Frame(None, pic({}, {}), {"keys": "0", "probes": "0"}, "start", {"m": str(M)})]
    for n, c in enumerate(keys, 1):
        i, p = h(c), 1
        out.append(Frame(L_probe, pic({i: "s-look"}, {c: i}), {"keys": str(n - 1), "probes": str(total)}, "hash",
                         {"x": c, "h": str(i)}))
        while t[i]:
            i = (i + 1) % M
            p += 1
            out.append(Frame(L_probe, pic({i: "s-look"}, {c: i}), {"keys": str(n - 1), "probes": str(total)},
                             "occupied", {"x": c, "i": str(i)}))
        t[i] = c
        total += p
        out.append(Frame(L_place, pic({i: "s-focus"}, {}), {"keys": str(n), "probes": str(total)}, "place",
                         {"x": c, "i": str(i), "p": str(p)}))
    out.append(Frame(None, pic({k: "s-done" for k in range(M) if t[k]}, {}), {"keys": str(len(keys)), "probes": str(total)},
                     "done", {"n": str(len(keys)), "avg": f"{total / len(keys):.2f}"}))
    return tuple(out), t


def build() -> Trace:
    code = snippet(SOURCE, "build")
    return Trace("probing", SOURCE, code, {k: run(v, code)[0] for k, v in PRESETS.items()})


def check():
    _, t = run(PRESETS["book"], snippet(SOURCE, "build"))
    lines = run_output(SOURCE).split("\n")
    end = lines.index("") if "" in lines else len(lines)
    table = [x.lstrip("*") for x in lines[end - 1].split()]
    return [c or "." for c in t], table
