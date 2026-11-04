from web.lib.render import cells
from web.lib.trace import Frame, Trace, run_c_harness
from web.traces.common import c_array, letters, line_with, one, seeded, shown, snippet

SOURCE = "src/07-quicksort/quicksort.c"
BOOK = letters("ASORTINGEXAMPLE")
PRESETS = {
    "book": (BOOK, True),
    "random": (seeded(10), False),
    "sorted": (list(range(1, 11)), False),
    "dups": ([5, 3, 5, 5, 1, 5, 3, 5], False),
}


def DRIVER(values) -> str:
    return (f"int main(void) {{ Item a[] = {c_array(values)}; int n = {len(values)};\n"
            "  T_base = a; T_n = n; partition(a, 0, n - 1); return 0; }\n")


def frames_for(values, as_letters, snip) -> tuple:
    L_i = line_with(snip, "less(a[++i], v)")
    L_j = line_with(snip, "less(v, a[--j])")
    L_cross = line_with(snip, "if (i >= j) break;")
    L_swap = line_with(snip, "exch(a[i], a[j]);")
    L_place = line_with(snip, "exch(a[i], a[r]);")
    L_ret = line_with(snip, "return i;")
    n = len(values)
    r = n - 1
    v = values[r]
    i, j, compares = -1, r, 0
    out = []

    def frame(line, a, say, look=(), focus=(r,), done_upto=None, **args):
        states = {}
        if done_upto is not None:
            for k in range(0, max(0, done_upto)):
                states[k] = "s-done"
        for k in look:
            states[k] = "s-look"
        for k in focus:
            states[k] = "s-focus"
        ptr = {}
        if 0 <= i < n:
            ptr["i"] = i
        if 0 <= j < n:
            ptr["j"] = j
        args = {"v": one(v, as_letters), "i": str(i), "j": str(j), **args}
        out.append(Frame(line, cells(shown(a, as_letters), states, ptr), {
            "v": one(v, as_letters), "i": str(i), "j": str(j), "compares": str(compares)}, say, args))

    frame(None, values, "start")
    for e in run_c_harness(SOURCE, ["partition"], DRIVER(values)):
        a = list(e.a)
        if e.op == "less" and e.line == L_i:
            i = e.i
            compares += 1
            x = one(a[i], as_letters)
            frame(L_i, a, "i_moves" if a[i] < v else "i_stops", look=(i,), done_upto=i, x=x)
        elif e.op == "less" and e.line == L_j:
            j = e.j
            compares += 1
            x = one(a[j], as_letters)
            frame(L_j, a, "j_moves" if v < a[j] else "j_stops", look=(j,), done_upto=i, x=x)
        elif e.op == "exch" and e.line == L_swap:
            frame(L_swap, a, "swap", look=(i, j), done_upto=i + 1,
                  x=one(a[i], as_letters), y=one(a[j], as_letters))
        elif e.op == "exch" and e.line == L_place:
            before = list(a)
            before[e.i], before[e.j] = before[e.j], before[e.i]
            frame(L_cross, before, "cross", done_upto=i)
            frame(L_place, a, "place_pivot", focus=(i,), done_upto=i)
            frame(L_ret, a, "done", focus=(i,), done_upto=i)
        else:
            raise ValueError(f"unexpected event {e}")
    return tuple(out)


def build() -> Trace:
    snip = snippet(SOURCE, "partition")
    return Trace("partition", SOURCE, snip,
                 {name: frames_for(vals, lt, snip) for name, (vals, lt) in PRESETS.items()})
