from web.lib.csource import run_output
from web.lib.render import line, svg, text
from web.lib.trace import Frame, Trace
from web.traces.common import line_with, snippet

SOURCE = "src/12-bst/bst.c"
WORDS = ["search", "binary", "tree", "symbol", "table", "insert", "delete"]
PRESETS = {"book": list("ASERCHINGXMPL"), "words": WORDS}


class T:
    def __init__(self, key):
        self.key, self.l, self.r = key, None, None


def node_width(key):
    k = str(key)
    return 30 if len(k) <= 2 else 9 * len(k) + 18


def draw(root, states, all_keys, max_levels=1):
    order = sorted(set(all_keys))
    widest = max(node_width(k) for k in order)
    col = widest + 8
    pad, dy, h = 8, 52, 28
    depth = {}

    def walk(t, d):
        if t is None:
            return
        depth[t.key] = d
        walk(t.l, d + 1)
        walk(t.r, d + 1)

    walk(root, 0)
    levels = max(depth.values(), default=0) + 1

    def pos(key):
        return pad + order.index(key) * col + col / 2, pad + h / 2 + depth[key] * dy

    parts = []

    def edges(t):
        if t is None:
            return
        for c in (t.l, t.r):
            if c is not None:
                (x1, y1), (x2, y2) = pos(t.key), pos(c.key)
                parts.append(line(x1, y1, x2, y2, "e"))
                edges(c)

    edges(root)
    for key, d in depth.items():
        x, y = pos(key)
        w = node_width(key)
        cls = states.get(key, "s-idle")
        if w == 30:
            parts.append(f'<circle cx="{x:.1f}" cy="{y:.1f}" r="15" class="{cls}"/>')
        else:
            parts.append(f'<rect x="{x - w / 2:.1f}" y="{y - h / 2:.1f}" width="{w}" height="{h}" rx="{h / 2}" class="{cls}"/>')
        parts.append(text(x, y + 5, key, "t-val", "middle", 13))
    total_levels = max(levels, max_levels)
    return svg(len(order) * col + 2 * pad, pad * 2 + h + (total_levels - 1) * dy, "".join(parts), cls="fsvg scene")


def keys(t):
    return [] if t is None else keys(t.l) + [t.key] + keys(t.r)


def final_levels(items):
    root = None

    def ins(t, k, d):
        if t is None:
            return T(k), d
        if k < t.key:
            t.l, dd = ins(t.l, k, d + 1)
        else:
            t.r, dd = ins(t.r, k, d + 1)
        return t, dd

    deepest = 0
    for k in items:
        root, d = ins(root, k, 0)
        deepest = max(deepest, d)
    return deepest + 1


def run(items, code):
    L_new = line_with(code, "if (h == z) return NEW(item, z, z, 1);")
    L_left = line_with(code, "if (less(v, t)) h->l = insertR(h->l, item);")
    L_right = line_with(code, "else            h->r = insertR(h->r, item);")
    root = None
    levels = final_levels(items)
    count = {"compares": 0}
    out = []

    def add(line_no, say, states, **args):
        out.append(Frame(line_no, draw(root, states, items, levels), {"N": str(len(keys(root))), "compares": str(count["compares"])},
                         say, args))

    add(None, "start", {})
    for key in items:
        add(None, "insert", {}, x=str(key))
        if root is None:
            root = T(key)
            add(L_new, "new_node", {key: "s-focus"}, x=str(key))
            continue
        h = root
        while True:
            count["compares"] += 1
            go_left = key < h.key
            add(L_left if go_left else L_right, "go_left" if go_left else "go_right", {h.key: "s-look"},
                x=str(key), y=str(h.key))
            nxt = h.l if go_left else h.r
            if nxt is None:
                if go_left:
                    h.l = T(key)
                else:
                    h.r = T(key)
                add(L_new, "new_node", {key: "s-focus"}, x=str(key))
                break
            h = nxt
    add(None, "done", {k: "s-done" for k in keys(root)}, n=str(len(keys(root))))
    return tuple(out), root


def build() -> Trace:
    code = snippet(SOURCE, "insertR")
    return Trace("bst", SOURCE, code, {k: run(v, code)[0] for k, v in PRESETS.items()})


def check():
    _, root = run(WORDS, snippet(SOURCE, "insertR"))
    line = next(l for l in run_output(SOURCE).split("\n") if l.startswith("inorder:"))
    return keys(root), [w.split(":")[0] for w in line.split()[1:]]
