import random

from web.lib.csource import extract_function


def letters(word: str) -> list:
    return [ord(c) for c in word]


def shown(values, as_letters: bool) -> list:
    return [chr(v) if as_letters else str(v) for v in values]


def one(v, as_letters: bool) -> str:
    return chr(v) if as_letters else str(v)


def line_with(snippet, needle: str) -> int:
    for k, text in enumerate(snippet.text.split("\n")):
        if needle in text:
            return snippet.first_line + k
    raise KeyError(f"{snippet.path}: no line with {needle!r}")


def seeded(n: int, seed: int = 7, lo: int = 1, hi: int = 99) -> list:
    rng = random.Random(seed)
    return [rng.randint(lo, hi) for _ in range(n)]


def c_array(values) -> str:
    return "{" + ", ".join(str(v) for v in values) + "}"


def snippet(source: str, fn: str):
    return extract_function(source, fn)


def sort_frames(events, values, as_letters, rules, settled=None):
    from web.lib.render import cells
    from web.lib.trace import Frame
    out = []
    counts = {"compares": 0, "exchanges": 0}

    def add(line, a, say, look=(), focus=(), ptr=None, done=(), **args):
        states = {k: "s-done" for k in done}
        for k in look:
            if k is not None:
                states[k] = "s-look"
        for k in focus:
            if k is not None:
                states[k] = "s-focus"
        out.append(Frame(line, cells(shown(a, as_letters), states, ptr or {}),
                         {"compares": str(counts["compares"]), "exchanges": str(counts["exchanges"])}, say, args))

    add(None, values, "start", n=str(len(values)))
    last = list(values)
    for e in events:
        a = list(e.a)
        if e.op == "end":
            add(None, a, "done", done=range(len(a)))
            last = a
            continue
        rule = rules.get((e.op, e.line))
        if rule is None:
            raise ValueError(f"no rule for {e.op} at line {e.line}")
        names = rule.get("names", ("a", "b"))
        ptr = {}
        if e.i is not None:
            ptr[names[0]] = e.i
        if e.j is not None:
            ptr[names[1]] = e.j
        args = {"x": one(e.x, as_letters), "y": one(e.y, as_letters),
                "p": "" if e.i is None else str(e.i), "q": "" if e.j is None else str(e.j)}
        done = settled(e, a) if settled else ()
        if e.op == "less":
            counts["compares"] += 1
            say = rule["say"][0] if e.x < e.y else rule["say"][1]
            add(e.line, a, say, look=(e.i, e.j), ptr=ptr, done=done, **args)
        else:
            counts["exchanges"] += 1
            add(e.line, a, rule["say"], focus=(e.i, e.j), ptr=ptr, done=done, **args)
        last = a
    return tuple(out), last


def sort_driver(fn: str, values) -> str:
    return (f"int main(void) {{ Item a[] = {c_array(values)}; int n = {len(values)};\n"
            f"  T_base = a; T_n = n; {fn}(a, 0, n - 1); T_mark(\"end\", __LINE__, 0, 0); return 0; }}\n")


SORT_PRESETS = {
    "book": (letters("ASORTINGEXAMPLE"), True),
    "random": (seeded(12, 11), False),
    "sorted": (list(range(1, 11)), False),
    "reversed": (list(range(10, 0, -1)), False),
    "dups": ([3, 1, 3, 2, 1, 3, 2, 1, 3, 2], False),
}


def printed_letters(source: str) -> list:
    from web.lib.csource import run_output
    first = run_output(source).split("\n")[0]
    return first.split()
