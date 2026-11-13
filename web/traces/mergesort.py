from web.lib.render import cell_rows
from web.lib.trace import Frame, Trace, run_c_harness, span
from web.traces.common import SORT_PRESETS, letters, line_with, one, printed_letters, shown, sort_driver

SOURCE = "src/08-mergesort/mergesort.c"
GLOBALS = "#define maxN 1000\nstatic Item aux[maxN];\n"
PRESETS = {**SORT_PRESETS, "book": (letters("MERGESORTEXAMPLE"), True)}
EMPTY = "·"


def code_span():
    return span(SOURCE, "merge", "mergesort")


def run(values, as_letters, code):
    L_call = line_with(code, "merge(a, l, m, r);")
    L_copy = line_with(code, "for (j = m; j < r; j++)")
    L_take = line_with(code, "a[k] = less(aux[i], aux[j]) ? aux[i++] : aux[j--];")
    a = list(values)
    n = len(a)
    aux = [None] * n
    out = []
    count = {"compares": 0}

    def frame(line, say, a_states, aux_states, a_ptr, aux_ptr, **args):
        rows = [("a", shown(a, as_letters), a_states, a_ptr),
                ("aux", [EMPTY if v is None else one(v, as_letters) for v in aux], aux_states, aux_ptr)]
        out.append(Frame(line, cell_rows(rows), {"compares": str(count["compares"])}, say, args))

    def outside(l, r):
        return {k: "s-faded" for k in range(n) if k < l or k > r}

    def merge(l, m, r):
        frame(L_call, "merge_start", outside(l, r), {}, {"l": l, "r": r}, {}, l=str(l), m=str(m), m1=str(m + 1), r=str(r))
        for k in range(l, m + 1):
            aux[k] = a[k]
        for j in range(m, r):
            aux[r + m - j] = a[j + 1]
        frame(L_copy, "copied", outside(l, r), {k: "s-look" for k in range(l, r + 1)}, {}, {"l": l, "r": r},
              l=str(l), r=str(r))
        i, j = l, r
        for k in range(l, r + 1):
            count["compares"] += 1
            take_left = aux[i] < aux[j]
            x, y = one(aux[i], as_letters), one(aux[j], as_letters)
            src = i if take_left else j
            a[k] = aux[src]
            a_states = {**outside(l, r), **{q: "s-done" for q in range(l, k)}, k: "s-focus"}
            frame(L_take, "take_left" if take_left else "take_right", a_states, {src: "s-focus"},
                  {"k": k}, {"i": i, "j": j}, x=x, y=y, k=str(k))
            if take_left:
                i += 1
            else:
                j -= 1

    def sort(l, r):
        if r <= l:
            return
        m = (r + l) // 2
        sort(l, m)
        sort(m + 1, r)
        merge(l, m, r)

    frame(None, "start", {}, {}, {}, {}, n=str(n))
    sort(0, n - 1)
    frame(None, "done", {k: "s-done" for k in range(n)}, {}, {}, {}, c=str(count["compares"]))
    return tuple(out), a, count["compares"]


def harness_compares(values):
    events = run_c_harness(SOURCE, ["merge", "mergesort"], sort_driver("mergesort", values), GLOBALS)
    return sum(1 for e in events if e.op == "less"), list(events[-1].a)


def build() -> Trace:
    code = code_span()
    return Trace("mergesort", SOURCE, code, {k: run(v, lt, code)[0] for k, (v, lt) in PRESETS.items()})


def check():
    values, lt = PRESETS["book"]
    _, final, compares = run(values, lt, code_span())
    c_compares, c_final = harness_compares(values)
    return ([chr(v) for v in final], compares, final), (printed_letters(SOURCE), c_compares, c_final)
