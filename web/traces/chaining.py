from web.lib.csource import run_output
from web.lib.render import array_rows
from web.lib.trace import Frame, Trace
from web.traces.common import line_with, snippet

SOURCE = "src/14-hashing/hash_chaining.c"
WORDS = ["search", "hash", "table", "chain", "insert", "delete", "key"]
PRESETS = {"book": WORDS, "more": WORDS + ["bucket", "list", "probe", "load", "factor", "miss", "hit"]}
M = 10


def h(word, m=M):
    v = 0
    for c in word:
        v = (127 * v + ord(c)) % m
    return v


def picture(heads, focus=None, look=None):
    rows = []
    for b in range(M):
        items = heads[b] or [""]
        cls = {}
        if focus is not None and focus[0] == b:
            cls[0] = "hi"
        if look is not None and look == b and not heads[b]:
            cls = {}
        rows.append((str(b), items if heads[b] else ["·"], cls if heads[b] else {0: "dim"}))
    return array_rows(rows, 64, 30)


def run(words, code):
    L_hash = line_with(code, "int i = hash(k, M);")
    L_ins = line_with(code, "heads[i] = NEW(k, v, heads[i]);")
    heads = [[] for _ in range(M)]
    out = [Frame(None, picture(heads), {"N": "0", "M": str(M)}, "start", {"m": str(M)})]
    for n, w in enumerate(words, 1):
        b = h(w)
        out.append(Frame(L_hash, picture(heads, look=b), {"N": str(n - 1), "M": str(M)}, "hash",
                         {"x": w, "b": str(b)}))
        heads[b].insert(0, w)
        out.append(Frame(L_ins, picture(heads, focus=(b, 0)), {"N": str(n), "M": str(M)}, "insert",
                         {"x": w, "b": str(b), "len": str(len(heads[b]))}))
    out.append(Frame(None, picture(heads), {"N": str(len(words)), "M": str(M)}, "done",
                     {"n": str(len(words)), "m": str(M)}))
    return tuple(out), {w: h(w) for w in words}


def build() -> Trace:
    code = snippet(SOURCE, "STinsert")
    return Trace("chaining", SOURCE, code, {k: run(v, code)[0] for k, v in PRESETS.items()})


def check():
    _, buckets = run(WORDS, snippet(SOURCE, "STinsert"))
    printed = {}
    for l in run_output(SOURCE).split("\n"):
        if "-> bucket" in l:
            printed[l.split()[0]] = int(l.split("bucket")[1].split(",")[0])
    return buckets, printed
