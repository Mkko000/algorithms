"""Tiny SVG helpers for the course figures (no dependencies).
Author: Mohammed El Sisi.  Colours come from CSS classes in docs/assets/style.css,
so every figure follows the light/dark theme automatically."""
from html import escape

def svg(w, h, body, label=''):
    return (f'<svg class="fsvg" viewBox="0 0 {w} {h}" width="{w}" height="{h}" role="img" '
            f'aria-label="{escape(label)}" xmlns="http://www.w3.org/2000/svg">{body}</svg>')

def text(x, y, s, cls='t', anchor='middle', size=None):
    st = f' font-size="{size}"' if size else ''
    return f'<text x="{x:.1f}" y="{y:.1f}" class="{cls}" text-anchor="{anchor}"{st}>{escape(str(s))}</text>'

def rect(x, y, w, h, cls='cell', rx=6):
    return f'<rect x="{x:.1f}" y="{y:.1f}" width="{w:.1f}" height="{h:.1f}" rx="{rx}" class="{cls}"/>'

def line(x1, y1, x2, y2, cls='edge'):
    return f'<line x1="{x1:.1f}" y1="{y1:.1f}" x2="{x2:.1f}" y2="{y2:.1f}" class="{cls}"/>'

def arrow(x1, y1, x2, y2, cls='arr'):
    return f'<line x1="{x1:.1f}" y1="{y1:.1f}" x2="{x2:.1f}" y2="{y2:.1f}" class="{cls}" marker-end="url(#ah)"/>'

DEFS = ('<defs><marker id="ah" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" '
        'orient="auto-start-reverse"><path d="M0,0 L10,5 L0,10 z" class="ahead"/></marker></defs>')

# ---------- array traces (Sedgewick-style) ----------
def array_rows(rows, cell=26, label_w=0, title=''):
    """rows: list of (label, items, classes) where classes is dict idx->cls or list."""
    n = max(len(r[1]) for r in rows)
    W = label_w + n * cell + 10
    H = len(rows) * (cell + 6) + 10
    out = [DEFS]
    for ri, (lab, items, cls) in enumerate(rows):
        y = 5 + ri * (cell + 6)
        if label_w and lab != '':
            out.append(text(label_w - 6, y + cell * 0.68, lab, 'lab', 'end', 12))
        for i, v in enumerate(items):
            c = cls.get(i, 'cell') if isinstance(cls, dict) else (cls[i] if i < len(cls) else 'cell')
            if v is None or v == '':
                continue
            x = label_w + i * cell
            out.append(rect(x + 1, y, cell - 2, cell, c, 4))
            out.append(text(x + cell / 2, y + cell * 0.68, v, 'tc', 'middle', 13))
    return svg(W, H, ''.join(out), title)

# ---------- binary trees ----------
class N:
    def __init__(self, key, l=None, r=None, cls='node', red=False, note=None):
        self.key, self.l, self.r, self.cls, self.red, self.note = key, l, r, cls, red, note

def _layout(t, depth, pos, out):
    if t is None:
        return
    _layout(t.l, depth + 1, pos, out)
    out[id(t)] = (pos[0], depth); pos[0] += 1
    _layout(t.r, depth + 1, pos, out)

def tree(t, dx=34, dy=50, r=15, title='', show_null=False):
    P = {}
    _layout(t, 0, [0], P)
    if not P:
        return svg(60, 40, text(30, 25, '(empty)'), title)
    maxx = max(p[0] for p in P.values()); maxd = max(p[1] for p in P.values())
    top = 12 if t.note else 0          # room for the root's note label
    W = (maxx + 1) * dx + 20; H = (maxd + 1) * dy + 20 + top
    xy = lambda n: (10 + dx / 2 + P[id(n)][0] * dx, 10 + top + r + P[id(n)][1] * dy)
    edges, nodes = [], []
    def walk(n):
        if n is None: return
        x, y = xy(n)
        for side, c in enumerate((n.l, n.r)):
            if c is not None:
                cx, cy = xy(c)
                edges.append(line(x, y, cx, cy, 'edge red' if c.red else 'edge'))
                walk(c)
            elif show_null:
                cx = x + (-dx / 3 if side == 0 else dx / 3)
                edges.append(line(x, y, cx, y + dy * 0.55, 'edge null'))
        nodes.append(f'<circle cx="{x:.1f}" cy="{y:.1f}" r="{r}" class="{n.cls}{" rn" if n.red else ""}"/>')
        nodes.append(text(x, y + 5, n.key, 'tn', 'middle', 13))
        if n.note:
            nodes.append(text(x, y - r - 4, n.note, 'note', 'middle', 11))
    walk(t)
    return svg(W, H + 10, ''.join(edges + nodes), title)

def build_bst(keys, cls=None):
    root = None
    def ins(h, k):
        if h is None: return N(k)
        if k < h.key: h.l = ins(h.l, k)
        else: h.r = ins(h.r, k)
        return h
    for k in keys: root = ins(root, k)
    return root

def find(t, key):
    while t is not None and t.key != key:
        t = t.l if key < t.key else t.r
    return t

def copy(t):
    return None if t is None else N(t.key, copy(t.l), copy(t.r), t.cls, t.red, t.note)

# ---------- multiway nodes (2-3-4, B-trees, tries) ----------
class M:
    def __init__(self, keys, kids=None, cls='mnode', label=None):
        self.keys, self.kids, self.cls, self.label = list(keys), list(kids or []), cls, label

def mtree(t, kw=22, gap=14, dy=58, title='', edge_labels=False):
    """Draw a multiway tree. Each node is a row of key boxes."""
    widths = {}
    def width(n):
        w_self = max(1, len(n.keys)) * getattr(n, 'bw', kw) + 6
        if not n.kids:
            widths[id(n)] = w_self; return w_self
        s = sum(width(c) for c in n.kids) + gap * (len(n.kids) - 1)
        widths[id(n)] = max(w_self, s); return widths[id(n)]
    total = width(t)
    out = [DEFS]; maxd = [0]
    def place(n, x0, d):
        maxd[0] = max(maxd[0], d)
        bw = getattr(n, 'bw', kw)
        w = widths[id(n)]; nw = max(1, len(n.keys)) * bw
        cx = x0 + w / 2; y = 10 + d * dy
        nx = cx - nw / 2
        if n.kids:
            cxs = []; xx = x0 + (w - (sum(widths[id(c)] for c in n.kids) + gap * (len(n.kids) - 1))) / 2
            for c in n.kids:
                cw = widths[id(c)]; cxs.append((xx, cw)); xx += cw + gap
            k = len(n.kids)
            for i, (c, (xx, cw)) in enumerate(zip(n.kids, cxs)):
                px = nx + (nw * i / (k - 1) if k > 1 else nw / 2)
                if len(n.keys) + 1 == k:   # B-tree style: links between keys
                    px = nx + i * bw
                if n.cls != 'virtual':
                    out.append(line(px, y + kw, xx + cw / 2, y + dy, 'edge'))
                if edge_labels and c.label is not None:
                    out.append(text((px + xx + cw / 2) / 2 + 6, (y + kw + y + dy) / 2 + 2, c.label, 'elab', 'middle', 12))
                place(c, xx, d + 1)
        if n.cls == 'virtual':
            pass
        elif n.keys:
            for i, k in enumerate(n.keys):
                out.append(rect(nx + i * bw, y, bw, kw, n.cls, 3))
                out.append(text(nx + i * bw + bw / 2, y + kw * 0.7, k, 'tc', 'middle', 12))
        else:
            out.append(f'<circle cx="{cx:.1f}" cy="{y + kw / 2:.1f}" r="5" class="dot"/>')
    if t.cls == 'virtual':
        out_shift = -dy
        place(t, 10, 0)
        body = ''.join(out)
        return svg(total + 20, 10 + maxd[0] * dy - dy + kw + 14,
                   f'<g transform="translate(0,{out_shift})">{body}</g>', title)
    place(t, 10, 0)
    return svg(total + 20, 10 + (maxd[0] + 1) * dy - dy + kw + 14, ''.join(out), title)

# ---------- linked lists ----------
def llist(items, cls=None, title='', extra=None, null=True, w=44, h=28, gap=26, extra_h=0):
    cls = cls or {}
    out = [DEFS]; x = 10
    for i, v in enumerate(items):
        c = cls.get(i, 'cell')
        out.append(rect(x, 14, w, h, c, 4)); out.append(text(x + w * 0.4, 14 + h * 0.68, v, 'tc', 'middle', 13))
        out.append(line(x + w * 0.78, 14, x + w * 0.78, 14 + h, 'sep'))
        if i < len(items) - 1 or null:
            out.append(arrow(x + w * 0.89, 14 + h / 2, x + w + gap - 2, 14 + h / 2))
        x += w + gap
    if null:
        out.append(text(x + 6, 14 + h * 0.68, 'NULL' if null is True else null, 'lab', 'start', 11))
        x += 40
    if extra: out.append(extra)
    return svg(x + 10, h + 40 + extra_h, ''.join(out), title)

# ---------- generic graph ----------
def graph(pos, edges, title='', r=14, cls=None, ecls=None, directed=False, W=None, H=None):
    cls = cls or {}; ecls = ecls or {}
    W = W or max(p[0] for p in pos.values()) + 30; H = H or max(p[1] for p in pos.values()) + 30
    out = [DEFS]
    for (a, b) in edges:
        (x1, y1), (x2, y2) = pos[a], pos[b]
        c = ecls.get((a, b), ecls.get((b, a), 'edge'))
        if directed:
            import math
            d = math.hypot(x2 - x1, y2 - y1) or 1
            out.append(arrow(x1, y1, x2 - (x2 - x1) * r / d, y2 - (y2 - y1) * r / d, c))
        else:
            out.append(line(x1, y1, x2, y2, c))
    for k, (x, y) in pos.items():
        out.append(f'<circle cx="{x}" cy="{y}" r="{r}" class="{cls.get(k, "node")}"/>')
        out.append(text(x, y + 5, k, 'tn', 'middle', 13))
    return svg(W, H, ''.join(out), title)

def frame(svg_s, cap):
    return f'<figure class="frame">{svg_s}<figcaption>{cap}</figcaption></figure>'

def fig(svg_s, cap):
    return f'{svg_s}<figcaption>{cap}</figcaption>'
