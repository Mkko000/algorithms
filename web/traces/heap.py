from web.lib.render import N as Node, cells, svg, tree
from web.lib.trace import Frame, Trace, run_c_harness, span
from web.traces.common import c_array, letters, line_with, one, seeded

SOURCE = "src/09-heapsort/heap.c"
FUNCTIONS = ["fixUp", "fixDown", "PQinit", "PQinsert", "PQdelmax"]
GLOBALS = "static Item *pq;\nstatic int N;\n"
PRESETS = {
    "book": (letters("ASORTINGEXAMPLE")[:10], True, 3),
    "random": (seeded(9, 5), False, 3),
    "sorted": (list(range(1, 9)), False, 2),
    "reversed": (list(range(8, 0, -1)), False, 2),
    "dups": ([4, 2, 4, 1, 4, 2, 4], False, 2),
}


def driver(values, deletes):
    n = len(values)
    return (f"int main(void) {{ Item v[] = {c_array(values)}; int n = {n};\n"
            f"  PQinit(n); T_base = pq; T_n = n + 1;\n"
            f"  for (int k = 0; k < n; k++) {{ T_mark(\"ins\", __LINE__, v[k], N); PQinsert(v[k]); }}\n"
            f"  T_mark(\"built\", __LINE__, 0, N);\n"
            f"  for (int k = 0; k < {deletes}; k++) {{ T_mark(\"del\", __LINE__, pq[1], N); PQdelmax(); }}\n"
            f"  T_mark(\"end\", __LINE__, 0, N); return 0; }}\n")


def heap_svg(a, size, as_letters, states):
    if size <= 0:
        return svg(60, 40, "")

    def node(k):
        if k > size:
            return None
        return Node(one(a[k], as_letters), node(2 * k), node(2 * k + 1), cls=states.get(k, "s-idle"))

    return tree(node(1), dx=34, dy=48, r=15)


def run(values, as_letters, deletes, code):
    L_up = line_with(code, "less(a[k / 2], a[k])")
    L_upx = line_with(code, "exch(a[k], a[k / 2])")
    L_kids = line_with(code, "less(a[j], a[j + 1])")
    L_down = line_with(code, "if (!less(a[k], a[j])) break;")
    L_downx = line_with(code, "exch(a[k], a[j]);")
    L_del = line_with(code, "exch(pq[1], pq[N]);")
    L_ins = line_with(code, "void PQinsert")
    out = []
    size = 0
    counts = {"compares": 0}
    removed = []

    def add(line, a, say, states=None, **args):
        states = dict(states or {})
        out.append(Frame(line, heap_svg(a, size, as_letters, states),
                         {"N": str(size), "compares": str(counts["compares"]),
                          "removed": " ".join(removed) or "none"}, say, args))

    events = run_c_harness(SOURCE, FUNCTIONS, driver(values, deletes), GLOBALS)
    blank = [0] * (len(values) + 1)
    add(None, blank, "start", n=str(len(values)))
    a = blank
    for e in events:
        a = list(e.a)
        x, y = one(e.x, as_letters), one(e.y, as_letters)
        if e.op == "ins":
            size = e.y + 1
            a[size] = e.x
            add(L_ins, a, "insert", {size: "s-focus"}, x=x)
        elif e.op == "built":
            add(None, a, "built", n=str(size))
        elif e.op == "del":
            add(L_del, a, "delmax", {1: "s-focus", size: "s-look"}, x=x)
        elif e.op == "end":
            add(None, a, "done", n=str(size))
        elif e.op == "less":
            counts["compares"] += 1
            if e.line == L_up:
                add(L_up, a, "up_swap" if e.x < e.y else "up_stop", {e.i: "s-look", e.j: "s-look"}, x=x, y=y)
            elif e.line == L_kids:
                add(L_kids, a, "kids", {e.i: "s-look", e.j: "s-look"}, x=x, y=y)
            elif e.line == L_down:
                add(L_down, a, "down_swap" if e.x < e.y else "down_stop", {e.i: "s-look", e.j: "s-look"}, x=x, y=y)
            else:
                raise ValueError(e)
        elif e.op == "exch":
            if e.line == L_del:
                removed.append(one(a[size], as_letters))
                size -= 1
                add(L_del, a, "removed", {}, x=one(a[size + 1], as_letters))
            elif e.line in (L_upx, L_downx):
                add(e.line, a, "swapped", {e.i: "s-focus", e.j: "s-focus"}, x=x, y=y)
            else:
                raise ValueError(e)
    return tuple(out), removed, a, size


def build() -> Trace:
    code = span(SOURCE, "fixUp", "PQdelmax")
    return Trace("heap", SOURCE, code,
                 {k: run(v, lt, d, code)[0] for k, (v, lt, d) in PRESETS.items()})


def check():
    code = span(SOURCE, "fixUp", "PQdelmax")
    values = [30, 10, 50, 20, 40]
    _, removed, _, _ = run(values, False, len(values), code)
    from web.lib.csource import run_output
    line = next(l for l in run_output(SOURCE).split("\n") if l.startswith("PQ delmax order:"))
    return removed, line.split(":")[1].split()
