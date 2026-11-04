import re
from html import escape, unescape

SKIP = {"code", "pre", "svg", "script", "style", "kbd", "samp", "textarea", "title", "math"}
VOID = {"br", "img", "hr", "input", "meta", "link", "wbr", "source", "area", "col", "embed", "track", "base"}
TAG = re.compile(r"(<[^>]+>)")
TAG_NAME = re.compile(r"^<\s*(/?)\s*([a-zA-Z][\w-]*)")
RUN = re.compile(r"[A-Za-z0-9][A-Za-z0-9 .,:;'’/+*=^_\-–—()\[\]<>&!?#%|~·²³ⁿ√≈≤≥×→←∑⌊⌋⌈⌉]*")
LATIN = re.compile(r"[A-Za-z]")
TRAIL = " .,:;·-–—"
LONG = 28


def _balanced(s: str) -> str:
    depth = []
    for k, c in enumerate(s):
        if c == "(":
            depth.append(k)
        elif c == ")":
            if not depth:
                return s[:k]
            depth.pop()
    return s[:depth[0]] if depth else s


def _text(chunk: str) -> str:
    text = unescape(chunk)
    out = []
    i = 0
    while True:
        m = RUN.search(text, i)
        if not m:
            break
        run = _balanced(m.group()).rstrip(TRAIL)
        if not run:
            run = m.group()[0]
        start, end = m.start(), m.start() + len(run)
        if LATIN.search(run) and start > i and text[start - 1] == "(" and end < len(text) and text[end] == ")":
            start, end, run = start - 1, end + 1, "(" + run + ")"
        out.append(escape(text[i:start], quote=False))
        if LATIN.search(run):
            cls = "lt wrap" if len(run) > LONG else "lt"
            out.append(f'<span class="{cls}" dir="ltr">{escape(run, quote=False)}</span>')
        else:
            out.append(escape(run, quote=False))
        i = end
    out.append(escape(text[i:], quote=False))
    return "".join(out)


def isolate_latin(html: str) -> str:
    out = []
    stack = []
    for part in TAG.split(html):
        if not part:
            continue
        if part.startswith("<"):
            out.append(part)
            m = TAG_NAME.match(part)
            if not m or part.startswith("<!"):
                continue
            closing, name = m.group(1) == "/", m.group(2).lower()
            self_closing = part.rstrip().endswith("/>") or name in VOID
            if stack:
                if closing:
                    stack.pop()
                elif not self_closing:
                    stack.append(name)
            elif not closing and not self_closing and (name in SKIP or 'class="lt' in part):
                stack.append(name)
            continue
        out.append(part if stack else _text(part))
    return "".join(out)
