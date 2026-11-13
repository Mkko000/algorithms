from web.lib.csource import run_output
from web.lib.render import CELL, NODE_R, line, node, row_parts, svg, text
from web.lib.trace import Frame, Trace, span
from web.traces.common import line_with

SOURCE = "src/01-fundamentals/union_find.c"
BOOK = [(3, 4), (4, 9), (8, 0), (2, 3), (5, 6), (5, 9), (7, 3), (4, 8), (5, 6), (0, 2), (6, 1)]
PRESETS = {"book": BOOK, "chain": [(0, 1), (1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7), (7, 8), (8, 9)]}


DY, COL, PAD = 46, 36, 8


def depth_of(ids):
    best = 0
    for k in range(len(ids)):
        d, x = 0, k
        while ids[x] != x:
            x = ids[x]
            d += 1
        best = max(best, d)
    return best


def forest_svg(ids, states, ptr, max_depth):
    n = len(ids)
    width = n * CELL
    parts, _, row_h = row_parts([str(v) for v in ids], states, ptr, PAD, PAD, True, 0)
    parts = list(parts)
    kids = {k: [c for c in range(n) if ids[c] == k and c != k] for k in range(n)}
    roots = [k for k in range(n) if ids[k] == k]
    pos = {}
    slot = [0]

    def place(k, d):
        if not kids[k]:
            x = slot[0]
            slot[0] += 1
        else:
            xs = [place(c, d + 1) for c in kids[k]]
            x = (xs[0] + xs[-1]) / 2
        pos[k] = (x, d)
        return x

    for r in roots:
        place(r, 0)
        slot[0] += 0.35
    span = max(slot[0] - 0.35, 1)
    col = min(COL, (width - 2 * NODE_R) / span) if span > 1 else COL
    x0 = PAD + (width - (span - 1) * col) / 2
    top = PAD + row_h + 16 + NODE_R
    for k, (x, d) in pos.items():
        if ids[k] != k:
            px, pd = pos[ids[k]]
            parts.append(line(x0 + x * col, top + d * DY, x0 + px * col, top + pd * DY, "e"))
    for k, (x, d) in pos.items():
        cx, cy = x0 + x * col, top + d * DY
        parts.append(node(cx, cy, k, states.get(k, "s-idle")))
    height = top + max_depth * DY + NODE_R + PAD
    return svg(width + 2 * PAD, height, "".join(parts), cls="fsvg scene")


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

    snaps = []

    def frame(line_no, say, states, ptr, **args):
        snaps.append((line_no, say, list(ids), dict(states), dict(ptr), args))
        return None

    out = []
    frame(None, "start", {}, {})
    for p, q in pairs:
        before = list(ids)
        i, j = root(p), root(q)
        halved = before != ids
        if halved:
            (frame(L_halve, "halve", {p: "s-look", q: "s-look"}, {"p": p, "q": q}, p=str(p), q=str(q)))
        if i == j:
            results.append("already connected")
            (frame(L_same, "already", {i: "s-done"}, {"p": p, "q": q}, p=str(p), q=str(q), r=str(i)))
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
        (frame(line, "link", {child: "s-focus", parent: "s-look"}, {"p": p, "q": q},
                         p=str(p), q=str(q), c=str(child), r=str(parent)))
    (frame(None, "done", {k: "s-done" for k in range(n) if ids[k] == k}, {},
                     n=str(sum(1 for k in range(n) if root(k) == k))))
    deepest = max(depth_of(snap[2]) for snap in snaps)
    for line_no, say, snap_ids, states, ptr, args in snaps:
        out.append(Frame(line_no, forest_svg(snap_ids, states, ptr, deepest),
                         {"components": str(sum(1 for k in range(n) if snap_ids[k] == k))}, say, args))
    return tuple(out), results


def build() -> Trace:
    code = span(SOURCE, "wpc_root", "wpc_union")
    return Trace("union-find", SOURCE, code, {k: run(v, code)[0] for k, v in PRESETS.items()})


def check():
    _, results = run(BOOK, span(SOURCE, "wpc_root", "wpc_union"))
    printed = [l.split(None, 1)[1].strip() for l in run_output(SOURCE).split("\n")[1:12]]
    return results, printed
