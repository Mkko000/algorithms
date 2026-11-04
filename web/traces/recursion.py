from web.lib.csource import run_output
from web.lib.render import line, svg, text
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

    pos = {}
    leaf = [0]
    col, dy, pad, h = 54, 54, 8, 26

    def place(c, d):
        if not c.kids:
            x = leaf[0]
            leaf[0] += 1
        else:
            xs = [place(k, d + 1) for k in c.kids]
            x = sum(xs) / len(xs)
        pos[c.id] = (pad + col / 2 + x * col, pad + h / 2 + d * dy, c)
        return x

    place(root, 0)
    width = leaf[0] * col + 2 * pad
    depth = max(y for _, y, _ in pos.values())
    height = depth + h / 2 + pad

    def draw(focus):
        parts = []
        for cid, (x, y, c) in pos.items():
            if cid not in seen:
                continue
            for k in c.kids:
                if k.id in seen:
                    kx, ky, _ = pos[k.id]
                    parts.append(line(x, y, kx, ky, "e"))
        for cid, (x, y, c) in pos.items():
            if cid not in seen:
                continue
            state = "s-focus" if cid == focus else ("s-done" if cid in done else "s-idle")
            parts.append(f'<rect x="{x - 23:.1f}" y="{y - h / 2:.1f}" width="46" height="{h}" rx="{h / 2}" class="{state}"/>')
            label = f"F{c.n}={c.value}" if cid in done else f"F{c.n}"
            parts.append(text(x, y + 4.5, label, "t-val", "middle", 12))
        return svg(width, height, "".join(parts), cls="fsvg scene")

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
