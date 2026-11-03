from web.lib.csource import run_output
from web.lib.render import N as Node, tree
from web.lib.trace import Frame, Trace
from web.traces.common import line_with, snippet

SOURCE = "src/05-recursion-trees/factorial_fib.c"
PRESETS = {"book": 5, "small": 4}


class Call:
    def __init__(self, n, cid):
        self.n, self.id, self.kids, self.value = n, cid, [], None


def run(n, code):
    L_entry = line_with(code, "naive_calls++;")
    L_zero = line_with(code, "if (N == 0) return 0;")
    L_one = line_with(code, "if (N == 1) return 1;")
    L_rec = line_with(code, "return fib_naive(N - 1) + fib_naive(N - 2);")
    counter = [0]
    root = Call(n, 0)
    events = []

    def walk(c):
        events.append(("enter", c))
        if c.n <= 1:
            c.value = c.n
            events.append(("base", c))
            return c.value
        for m in (c.n - 1, c.n - 2):
            counter[0] += 1
            k = Call(m, counter[0])
            c.kids.append(k)
            walk(k)
        c.value = c.kids[0].value + c.kids[1].value
        events.append(("return", c))
        return c.value

    walk(root)
    seen, done = set(), set()

    def draw(focus):
        def conv(c):
            if c.id not in seen:
                return None
            kids = [conv(k) for k in c.kids]
            state = "s-focus" if c.id == focus else ("s-done" if c.id in done else "s-idle")
            label = f"{c.n}" if c.id not in done else f"{c.value}"
            return Node(label, kids[0] if kids else None, kids[1] if len(kids) > 1 else None, cls=state)
        return tree(conv(root), dx=30, dy=46, r=14)

    out = []
    calls = 0
    seen.add(root.id)
    out.append(Frame(None, draw(None), {"calls": "0"}, "start", {"n": str(n)}))
    for kind, c in events:
        seen.add(c.id)
        if kind == "enter":
            calls += 1
            out.append(Frame(L_entry, draw(c.id), {"calls": str(calls)}, "enter", {"n": str(c.n)}))
        elif kind == "base":
            done.add(c.id)
            out.append(Frame(L_zero if c.n == 0 else L_one, draw(c.id), {"calls": str(calls)}, "base",
                             {"n": str(c.n), "v": str(c.value)}))
        else:
            done.add(c.id)
            out.append(Frame(L_rec, draw(c.id), {"calls": str(calls)}, "return",
                             {"n": str(c.n), "v": str(c.value), "a": str(c.kids[0].value), "b": str(c.kids[1].value)}))
    out.append(Frame(None, draw(None), {"calls": str(calls)}, "done", {"n": str(n), "v": str(root.value), "c": str(calls)}))
    return tuple(out), calls


def build() -> Trace:
    code = snippet(SOURCE, "fib_naive")
    return Trace("recursion", SOURCE, code, {k: run(v, code)[0] for k, v in PRESETS.items()})


def check():
    _, calls = run(10, snippet(SOURCE, "fib_naive"))
    line = next(l for l in run_output(SOURCE).split("\n") if l.strip().startswith("N=10"))
    return calls, int(line.split("naive=")[1].split()[0])
