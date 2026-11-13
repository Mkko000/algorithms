import itertools
import math
import re
from html import escape

STATES = {"s-idle", "s-look", "s-focus", "s-done", "s-faded", "s-red"}
TEXTS = {"t-val", "t-idx", "t-ptr", "t-lab"}
EDGES = {"e", "e-active", "e-strong", "e-null", "a", "a-head"}
VOCABULARY = STATES | TEXTS | EDGES | {"fsvg", "scene", "dot"}

NODE_R = 15
CELL = 34
KEY_H = 26
VALUE_SIZE = 13
LABEL_SIZE = 13


def label_width(s, size=VALUE_SIZE) -> float:
    return 0.6 * size * len(str(s))


MARK_BASE = {"cell", "node", "mnode"}
STATE_OF = {"hi": "s-focus", "piv": "s-focus", "hi2": "s-look", "bad": "s-look",
            "ok": "s-done", "dim": "s-faded"}
TEXT_OF = {"tc": "t-val", "tn": "t-val", "t": "t-val", "lab": "t-lab", "elab": "t-lab", "note": "t-lab"}


def map_classes(value: str) -> str:
    toks = value.split()
    if all(t in VOCABULARY for t in toks):
        return value
    s = set(toks)
    if "edge" in s:
        rest = s - {"edge"}
        if rest <= {"red"} and rest:
            return "e-strong"
        if rest == {"null"}:
            return "e-null"
        if rest == {"hi"} or rest == {"hi2"}:
            return "e-active"
        if not rest:
            return "e"
        raise KeyError(value)
    if "arr" in s:
        rest = s - {"arr"}
        if not rest:
            return "a"
        if rest <= {"hi", "hi2"}:
            return "a e-active"
        raise KeyError(value)
    if len(toks) == 1 and toks[0] in TEXT_OF:
        return TEXT_OF[toks[0]]
    if toks == ["sep"]:
        return "e"
    if toks == ["ahead"]:
        return "a-head"
    if toks == ["dot"]:
        return "dot"
    rest = s - MARK_BASE
    red = "rn" in rest
    rest -= {"rn", "bk"}
    if not rest:
        state = "s-idle"
    elif len(rest) == 1 and next(iter(rest)) in STATE_OF:
        state = STATE_OF[next(iter(rest))]
    elif len(rest) == 1 and next(iter(rest)) in STATES:
        state = next(iter(rest))
    else:
        raise KeyError(value)
    if not (s & MARK_BASE) and not rest and not red and not (s & {"bk"}):
        raise KeyError(value)
    return f"{state} s-red" if red else state


_CLASS = re.compile(r'class="([^"]*)"')


def _map_body(body: str) -> str:
    return _CLASS.sub(lambda m: f'class="{map_classes(m.group(1))}"', body)


_SVG = re.compile(r"<svg[\s\S]*?</svg>")


_marker_ids = itertools.count(1)


def _local_markers(svg_text: str) -> str:
    if "url(#ah)" not in svg_text and 'id="ah"' not in svg_text:
        return svg_text
    if 'id="ah"' not in svg_text:
        head = svg_text.index(">") + 1
        svg_text = svg_text[:head] + _map_body(DEFS) + svg_text[head:]
    n = next(_marker_ids)
    return svg_text.replace('id="ah"', f'id="ah{n}"').replace("url(#ah)", f"url(#ah{n})")


def map_svg_classes(html: str) -> str:
    return _SVG.sub(lambda m: _local_markers(_map_body(m.group())), html)


def svg(w, h, body, label="", cls="fsvg"):
    return (f'<svg class="{cls}" direction="ltr" viewBox="0 0 {w:g} {h:g}" width="{w:g}" height="{h:g}" '
            f'style="min-width:{round(0.7 * w)}px" role="img" aria-label="{escape(label)}" xmlns="http://www.w3.org/2000/svg">{_map_body(body)}</svg>')


def text(x, y, s, cls="t", anchor="middle", size=None):
    st = f' font-size="{size}"' if size else ""
    return f'<text x="{x:.1f}" y="{y:.1f}" class="{cls}" text-anchor="{anchor}"{st}>{escape(str(s))}</text>'


def rect(x, y, w, h, cls="cell", rx=6):
    return f'<rect x="{x:.1f}" y="{y:.1f}" width="{w:.1f}" height="{h:.1f}" rx="{rx}" class="{cls}"/>'


def node_width(label) -> float:
    lw = label_width(label)
    return lw + 12 if lw > 2 * NODE_R - 8 else 2 * NODE_R


def node(x, y, label, cls="s-idle"):
    w = node_width(label)
    if w > 2 * NODE_R:
        shape = rect(x - w / 2, y - NODE_R, w, 2 * NODE_R, cls, NODE_R)
    else:
        shape = f'<circle cx="{x:.1f}" cy="{y:.1f}" r="{NODE_R}" class="{cls}"/>'
    return shape + text(x, y + 4.5, label, "tn", "middle", VALUE_SIZE)


def line(x1, y1, x2, y2, cls="edge"):
    return f'<line x1="{x1:.1f}" y1="{y1:.1f}" x2="{x2:.1f}" y2="{y2:.1f}" class="{cls}"/>'


def arrow(x1, y1, x2, y2, cls="arr"):
    return f'<line x1="{x1:.1f}" y1="{y1:.1f}" x2="{x2:.1f}" y2="{y2:.1f}" class="{cls}" marker-end="url(#ah)"/>'


DEFS = ('<defs><marker id="ah" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" '
        'orient="auto-start-reverse"><path d="M0,0 L10,5 L0,10 z" class="ahead"/></marker></defs>')


def array_rows(rows, label_w=0, title=""):
    longest = max((len(str(v)) for r in rows for v in r[1] if v is not None), default=1)
    cell = max(KEY_H, label_width("0" * longest) + 12)
    ch = KEY_H
    n = max(len(r[1]) for r in rows)
    W = label_w + n * cell + 10
    H = len(rows) * (ch + 6) + 10
    out = [DEFS]
    for ri, (lab, items, cls) in enumerate(rows):
        y = 5 + ri * (ch + 6)
        if label_w and lab != "":
            out.append(text(label_w - 6, y + ch * 0.68, lab, "lab", "end", LABEL_SIZE))
        for i, v in enumerate(items):
            c = cls.get(i, "cell") if isinstance(cls, dict) else (cls[i] if i < len(cls) else "cell")
            if v is None or v == "":
                continue
            x = label_w + i * cell
            out.append(rect(x + 1, y, cell - 2, ch, c, 0))
            out.append(text(x + cell / 2, y + ch * 0.68, v, "tc", "middle", VALUE_SIZE))
    return svg(W, H, "".join(out), title)


class N:
    def __init__(self, key, l=None, r=None, cls="node", red=False, note=None):
        self.key, self.l, self.r, self.cls, self.red, self.note = key, l, r, cls, red, note


def _layout(t, depth, pos, out):
    if t is None:
        return
    _layout(t.l, depth + 1, pos, out)
    out[id(t)] = (pos[0], depth)
    pos[0] += 1
    _layout(t.r, depth + 1, pos, out)


def tree(t, dx=34, dy=50, title="", show_null=False):
    r = NODE_R
    P = {}
    _layout(t, 0, [0], P)
    if not P:
        return svg(60, 40, text(30, 25, "(empty)"), title)
    maxx = max(p[0] for p in P.values())
    maxd = max(p[1] for p in P.values())
    top = 12 if t.note else 0
    W = (maxx + 1) * dx + 20
    H = (maxd + 1) * dy + 20 + top

    def xy(n):
        return (10 + dx / 2 + P[id(n)][0] * dx, 10 + top + r + P[id(n)][1] * dy)

    edges, nodes = [], []

    def walk(n, side=None):
        if n is None:
            return
        x, y = xy(n)
        for s, c in enumerate((n.l, n.r)):
            if c is not None:
                cx, cy = xy(c)
                edges.append(line(x, y, cx, cy, "edge red" if c.red else "edge"))
                walk(c, s)
            elif show_null:
                ux, uy = (-dx * 0.3 if s == 0 else dx * 0.3), dy * 0.75
                k = (r + 24) / math.hypot(ux, uy)
                edges.append(line(x, y, x + ux * k, y + uy * k, "edge null"))
        nodes.append(node(x, y, n.key, f'{n.cls}{" rn" if n.red else ""}'))
        if n.note:
            if side is None:
                nodes.append(text(x, y - r - 4, n.note, "note", "middle", LABEL_SIZE))
            elif side == 0:
                nodes.append(text(x - r - 2, y - r + 2, n.note, "note", "end", LABEL_SIZE))
            else:
                nodes.append(text(x + r + 2, y - r + 2, n.note, "note", "start", LABEL_SIZE))

    walk(t)
    return svg(W, H + 10, "".join(edges + nodes), title)


def build_bst(keys, cls=None):
    root = None

    def ins(h, k):
        if h is None:
            return N(k)
        if k < h.key:
            h.l = ins(h.l, k)
        else:
            h.r = ins(h.r, k)
        return h

    for k in keys:
        root = ins(root, k)
    return root


def find(t, key):
    while t is not None and t.key != key:
        t = t.l if key < t.key else t.r
    return t


def copy(t):
    return None if t is None else N(t.key, copy(t.l), copy(t.r), t.cls, t.red, t.note)


class M:
    def __init__(self, keys, kids=None, cls="mnode", label=None):
        self.keys, self.kids, self.cls, self.label = list(keys), list(kids or []), cls, label


def mtree(t, gap=14, dy=58, title="", edge_labels=False):
    kw = KEY_H
    widths = {}

    def bw_of(n):
        if hasattr(n, "bw"):
            return n.bw
        longest = max((len(str(k)) for k in n.keys), default=1)
        return max(kw, label_width("0" * longest) + 12)

    def width(n):
        w_self = max(1, len(n.keys)) * bw_of(n) + 6
        if not n.kids:
            widths[id(n)] = w_self
            return w_self
        s = sum(width(c) for c in n.kids) + gap * (len(n.kids) - 1)
        widths[id(n)] = max(w_self, s)
        return widths[id(n)]

    total = width(t)
    out = [DEFS]
    maxd = [0]

    def place(n, x0, d):
        maxd[0] = max(maxd[0], d)
        bw = bw_of(n)
        w = widths[id(n)]
        nw = max(1, len(n.keys)) * bw
        cx = x0 + w / 2
        y = 10 + d * dy
        nx = cx - nw / 2
        if n.kids:
            cxs = []
            xx = x0 + (w - (sum(widths[id(c)] for c in n.kids) + gap * (len(n.kids) - 1))) / 2
            for c in n.kids:
                cw = widths[id(c)]
                cxs.append((xx, cw))
                xx += cw + gap
            k = len(n.kids)
            ang = []
            for i, (xx, cw) in enumerate(cxs):
                px = nx + (nw * i / (k - 1) if k > 1 else nw / 2)
                if len(n.keys) + 1 == k:
                    px = nx + i * bw
                ang.append(math.atan2(dy - kw, xx + cw / 2 - px))
            gaps = [math.inf] + [a - b for a, b in zip(ang, ang[1:])] + [math.inf]
            left, taken = [], set()
            for i in range(k):
                use_left = i not in taken and gaps[i] > gaps[i + 1]
                taken.add(i if use_left else i + 1)
                left.append(use_left)
            for i, (c, (xx, cw)) in enumerate(zip(n.kids, cxs)):
                px = nx + (nw * i / (k - 1) if k > 1 else nw / 2)
                if len(n.keys) + 1 == k:
                    px = nx + i * bw
                if n.cls != "virtual":
                    out.append(line(px, y + kw, xx + cw / 2, y + dy, "edge"))
                if edge_labels and c.label is not None:
                    ex, ey = xx + cw / 2 - px, dy - kw
                    nl = math.hypot(ex, ey)
                    ox, oy = (-ey / nl, ex / nl) if left[i] else (ey / nl, -ex / nl)
                    lx, ly = (px + xx + cw / 2) / 2 + ox * 10, (y + kw + y + dy) / 2 + oy * 10
                    out.append(text(lx, ly + LABEL_SIZE * 0.35, c.label, "elab", "middle", LABEL_SIZE))
                place(c, xx, d + 1)
        if n.cls == "virtual":
            pass
        elif n.keys:
            for i, k in enumerate(n.keys):
                out.append(rect(nx + i * bw, y, bw, kw, n.cls, 0))
                out.append(text(nx + i * bw + bw / 2, y + kw * 0.68, k, "tc", "middle", VALUE_SIZE))
        else:
            out.append(f'<circle cx="{cx:.1f}" cy="{y + kw / 2:.1f}" r="5" class="dot"/>')

    if t.cls == "virtual":
        place(t, 10, 0)
        body = "".join(out)
        return svg(total + 20, 10 + maxd[0] * dy - dy + kw + 14,
                   f'<g transform="translate(0,{-dy})">{body}</g>', title)
    place(t, 10, 0)
    return svg(total + 20, 10 + (maxd[0] + 1) * dy - dy + kw + 14, "".join(out), title)


def llist(items, cls=None, title="", extra=None, null=True, w=44, gap=26, extra_h=0):
    h = KEY_H
    cls = cls or {}
    out = [DEFS]
    x = 10
    for i, v in enumerate(items):
        c = cls.get(i, "cell")
        out.append(rect(x, 14, w, h, c, 0))
        out.append(text(x + w * 0.4, 14 + h * 0.68, v, "tc", "middle", VALUE_SIZE))
        out.append(line(x + w * 0.78, 14, x + w * 0.78, 14 + h, "sep"))
        if i < len(items) - 1 or null:
            out.append(arrow(x + w * 0.89, 14 + h / 2, x + w + gap - 2, 14 + h / 2))
        x += w + gap
    if null:
        label = "NULL" if null is True else null
        out.append(text(x + 6, 14 + h * 0.68, label, "lab", "start", LABEL_SIZE))
        x += max(40, 7 * len(str(label)) + 12)
    if extra:
        out.append(extra)
    return svg(x + 10, h + 40 + extra_h, "".join(out), title)


def graph(pos, edges, title="", cls=None, ecls=None, directed=False, W=None, H=None):
    r = NODE_R
    cls = cls or {}
    ecls = ecls or {}
    W = W or max(p[0] for p in pos.values()) + 30
    H = H or max(p[1] for p in pos.values()) + 30
    out = [DEFS]
    for (a, b) in edges:
        (x1, y1), (x2, y2) = pos[a], pos[b]
        c = ecls.get((a, b), ecls.get((b, a), "edge"))
        if directed:
            d = math.hypot(x2 - x1, y2 - y1) or 1
            out.append(arrow(x1, y1, x2 - (x2 - x1) * r / d, y2 - (y2 - y1) * r / d, c))
        else:
            out.append(line(x1, y1, x2, y2, c))
    for k, (x, y) in pos.items():
        out.append(node(x, y, k, cls.get(k, "node")))
    return svg(W, H, "".join(out), title)


def frame(svg_s, cap):
    return f'<figure class="frame">{svg_s}<figcaption>{cap}</figcaption></figure>'


def fig(svg_s, cap):
    return f'{svg_s}<figcaption>{cap}</figcaption>'


def row_parts(values, states, pointers, x0, y0, index_row=True, index_base=0):
    cell = CELL
    states = states or {}
    pointers = pointers or {}
    n = len(values)
    out = []
    for i, v in enumerate(values):
        x = x0 + i * cell
        out.append(rect(x, y0, cell, cell, states.get(i, "s-idle"), 0))
        out.append(text(x + cell / 2, y0 + cell * 0.64, v, "t-val", "middle", VALUE_SIZE))
    y = y0 + cell
    if index_row:
        for i in range(n):
            out.append(text(x0 + i * cell + cell / 2, y + 16, i + index_base, "t-idx", "middle", 11))
        y += 20
    by_index = {}
    for name, idx in pointers.items():
        if idx is not None and 0 <= idx < n:
            by_index.setdefault(idx, []).append(name)
    placed, depth = [], 0
    for idx in sorted(by_index):
        x = x0 + idx * cell + cell / 2
        out.append(text(x, y + 14, "▲", "t-ptr", "middle", 12))
        k = 0
        for name in by_index[idx]:
            half = label_width(name) / 2 + 2
            while any(r == k and x - half < b1 and a0 < x + half for r, a0, b1 in placed):
                k += 1
            placed.append((k, x - half, x + half))
            out.append(text(x, y + 30 + 15 * k, name, "t-ptr", "middle", 13))
            depth = max(depth, k + 1)
            k += 1
    y += 18 + 15 * depth if depth else 0
    return out, n * cell, y - y0


def cells(values, states=None, pointers=None, index_row=True, title="", index_base=0):
    pad = 6
    out, w, h = row_parts(values, states, pointers, pad, pad, index_row, index_base)
    return svg(w + 2 * pad, h + 2 * pad, "".join(out), title, cls="fsvg scene")


def cell_rows(rows, label_w=44, index_base=0, gap=10, title=""):
    cell = CELL
    pad = 6
    out = []
    y = pad
    width = 0
    for label, values, states, pointers in rows:
        if label:
            out.append(text(pad + label_w - 8, y + cell * 0.64, label, "t-lab", "end", LABEL_SIZE))
        parts, w, h = row_parts(values, states, pointers, pad + label_w, y, True, index_base)
        out += parts
        width = max(width, w)
        y += h + gap
    return svg(pad + label_w + width + pad, y - gap + pad, "".join(out), title, cls="fsvg scene")


def bars(values, states=None, width=12, height=120, gap=3, title=""):
    states = states or {}
    top = max(values) if values else 1
    pad = 6
    W = len(values) * (width + gap) - gap + 2 * pad
    out = []
    for i, v in enumerate(values):
        h = max(2.0, height * v / top)
        out.append(rect(pad + i * (width + gap), pad + height - h, width, h, states.get(i, "s-idle"), 0))
    return svg(W, height + 2 * pad, "".join(out), title, cls="fsvg scene")
