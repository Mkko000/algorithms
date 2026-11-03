from web.lib.csource import run_output
from web.lib.render import cells
from web.lib.trace import Frame, Trace, span
from web.traces.common import line_with

SOURCE = "src/01-fundamentals/union_find.c"
BOOK = [(3, 4), (4, 9), (8, 0), (2, 3), (5, 6), (5, 9), (7, 3), (4, 8), (5, 6), (0, 2), (6, 1)]
PRESETS = {"book": BOOK, "chain": [(0, 1), (1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7), (7, 8), (8, 9)]}


def run(pairs, code):
    L_same = line_with(code, "if (i == j) return;")
    L_small = line_with(code, "if (sz[i] < sz[j]) { id[i] = j; sz[j] += sz[i]; }")
    L_big = line_with(code, "else               { id[j] = i; sz[i] += sz[j]; }")
    L_halve = line_with(code, "id[i] = id[id[i]]; i = id[i];")
    n = 10
    ids, sz = list(range(n)), [1] * n
    results = []

    def root(i):
        while i != ids[i]:
            ids[i] = ids[ids[i]]
            i = ids[i]
        return i

    def frame(line, say, states, ptr, **args):
        return Frame(line, cells(ids, states, ptr), {"components": str(sum(1 for k in range(n) if ids[k] == k))},
                     say, args)

    out = [frame(None, "start", {}, {})]
    for p, q in pairs:
        before = list(ids)
        i, j = root(p), root(q)
        halved = before != ids
        if halved:
            out.append(frame(L_halve, "halve", {p: "s-look", q: "s-look"}, {"p": p, "q": q}, p=str(p), q=str(q)))
        if i == j:
            results.append("already connected")
            out.append(frame(L_same, "already", {i: "s-done"}, {"p": p, "q": q}, p=str(p), q=str(q), r=str(i)))
            continue
        results.append("connected")
        if sz[i] < sz[j]:
            ids[i] = j
            sz[j] += sz[i]
            line, child, parent = L_small, i, j
        else:
            ids[j] = i
            sz[i] += sz[j]
            line, child, parent = L_big, j, i
        out.append(frame(line, "link", {child: "s-focus", parent: "s-look"}, {"p": p, "q": q},
                         p=str(p), q=str(q), c=str(child), r=str(parent)))
    out.append(frame(None, "done", {k: "s-done" for k in range(n) if ids[k] == k}, {},
                     n=str(sum(1 for k in range(n) if root(k) == k))))
    return tuple(out), results


def build() -> Trace:
    code = span(SOURCE, "wpc_root", "wpc_union")
    return Trace("union-find", SOURCE, code, {k: run(v, code)[0] for k, v in PRESETS.items()})


def check():
    _, results = run(BOOK, span(SOURCE, "wpc_root", "wpc_union"))
    printed = [l.split(None, 1)[1].strip() for l in run_output(SOURCE).split("\n")[1:12]]
    return results, printed
