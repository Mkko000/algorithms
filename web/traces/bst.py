from web.lib.csource import run_output
from web.lib.render import N as Node, svg, tree
from web.lib.trace import Frame, Trace
from web.traces.common import line_with, snippet

SOURCE = "src/12-bst/bst.c"
WORDS = ["search", "binary", "tree", "symbol", "table", "insert", "delete"]
PRESETS = {"book": list("ASERCHINGXMPL"), "words": WORDS}


class T:
    def __init__(self, key):
        self.key, self.l, self.r = key, None, None


def draw(root, states):
    if root is None:
        return svg(60, 40, "")

    def conv(t):
        return None if t is None else Node(t.key, conv(t.l), conv(t.r), cls=states.get(t.key, "s-idle"))

    wide = max(len(str(k)) for k in keys(root)) > 1
    return tree(conv(root), dx=58 if wide else 34, dy=50, r=22 if wide else 15)


def keys(t):
    return [] if t is None else keys(t.l) + [t.key] + keys(t.r)


def run(items, code):
    L_new = line_with(code, "if (h == z) return NEW(item, z, z, 1);")
    L_left = line_with(code, "if (less(v, t)) h->l = insertR(h->l, item);")
    L_right = line_with(code, "else            h->r = insertR(h->r, item);")
    root = None
    count = {"compares": 0}
    out = []

    def add(line, say, states, **args):
        out.append(Frame(line, draw(root, states), {"N": str(len(keys(root))), "compares": str(count["compares"])},
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
