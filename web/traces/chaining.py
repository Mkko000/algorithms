from web.lib.csource import run_output
from web.lib.render import line, rect, svg, text
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


DEFS = ('<defs><marker id="ahc" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" '
        'orient="auto-start-reverse"><path d="M0,0 L10,5 L0,10 z" class="a-head"/></marker></defs>')
ROW, BOX_H, HEAD_W, PTR_W, GAP, PAD, LABEL_W = 36, 26, 24, 14, 22, 8, 26


def node_w(word):
    return 9 * len(word) + 16


def row_width(chain):
    return HEAD_W + sum(GAP + node_w(w) + PTR_W for w in chain)


def picture(heads, width, focus=None, look=None):
    parts = [DEFS]
    for b in range(M):
        y = PAD + b * ROW
        mid = y + BOX_H / 2
        parts.append(text(PAD + LABEL_W - 8, mid + 4, b, "t-idx", "end", 11))
        x = PAD + LABEL_W
        head_cls = "s-look" if look == b else "s-idle"
        parts.append(rect(x, y, HEAD_W, BOX_H, head_cls, 0))
        if not heads[b]:
            parts.append(line(x + 5, y + BOX_H - 5, x + HEAD_W - 5, y + 5, "e"))
            continue
        start = x + HEAD_W / 2
        x += HEAD_W
        for k, word in enumerate(heads[b]):
            parts.append(f'<line x1="{start:.1f}" y1="{mid:.1f}" x2="{x + GAP - 2:.1f}" y2="{mid:.1f}" class="a" marker-end="url(#ahc)"/>')
            x += GAP
            w = node_w(word)
            cls = "s-focus" if focus == (b, k) else "s-idle"
            parts.append(rect(x, y, w + PTR_W, BOX_H, cls, 0))
            parts.append(text(x + w / 2, mid + 4.5, word, "t-val", "middle", 12))
            parts.append(line(x + w, y, x + w, y + BOX_H, "e"))
            start = x + w + PTR_W / 2
            x += w + PTR_W
            if k == len(heads[b]) - 1:
                parts.append(line(x - PTR_W + 3, y + BOX_H - 4, x - 3, y + 4, "e"))
    return svg(PAD * 2 + LABEL_W + width + 4, PAD * 2 + M * ROW - (ROW - BOX_H), "".join(parts), cls="fsvg scene")


def run(words, code):
    L_hash = line_with(code, "int i = hash(k, M);")
    L_ins = line_with(code, "heads[i] = NEW(k, v, heads[i]);")
    final = [[] for _ in range(M)]
    for w in words:
        final[h(w)].insert(0, w)
    width = max(row_width(c) for c in final)
    heads = [[] for _ in range(M)]
    out = [Frame(None, picture(heads, width), {"N": "0", "M": str(M)}, "start", {"m": str(M)})]
    for n, w in enumerate(words, 1):
        b = h(w)
        out.append(Frame(L_hash, picture(heads, width, look=b), {"N": str(n - 1), "M": str(M)}, "hash",
                         {"x": w, "b": str(b)}))
        heads[b].insert(0, w)
        out.append(Frame(L_ins, picture(heads, width, focus=(b, 0), look=b), {"N": str(n), "M": str(M)}, "insert",
                         {"x": w, "b": str(b), "len": str(len(heads[b]))}))
    out.append(Frame(None, picture(heads, width), {"N": str(len(words)), "M": str(M)}, "done",
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
