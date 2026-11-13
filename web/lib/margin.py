import re
from html.parser import HTMLParser

VOID = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "source", "track", "wbr"}
WIDE = re.compile(r'\A<(?:div class="(?:figure|steps|widget)[" ]|section class="trace[" ])')
MOVE_PREV = re.compile(r'\A<(?:aside class="callout (?:tip|oops)"|ul class="src-links")')
GOAL = re.compile(r'\A<aside class="callout goal"')
CALLOUT = re.compile(r'\A<aside class="callout (goal|tip|oops)"')
SIDENOTE = re.compile(r'<span class="sn"><sup class="m">(\d+)</sup><span class="sn-t">(.*?)</span></span>', re.S)
H2_ID = re.compile(r'\A<h2 id="([^"]+)"')


class _Splitter(HTMLParser):
    def __init__(self, text):
        super().__init__(convert_charrefs=False)
        self.text = text
        self.lines = [0]
        for m in re.finditer("\n", text):
            self.lines.append(m.end())
        self.depth = 0
        self.start = None
        self.spans = []

    def _offset(self):
        line, col = self.getpos()
        return self.lines[line - 1] + col

    def handle_starttag(self, tag, attrs):
        if self.depth == 0:
            self.start = self._offset()
        if tag in VOID:
            if self.depth == 0:
                self.spans.append((self.start, self._offset() + len(self.get_starttag_text())))
            return
        self.depth += 1

    def handle_startendtag(self, tag, attrs):
        if self.depth == 0:
            pos = self._offset()
            self.spans.append((pos, pos + len(self.get_starttag_text())))

    def handle_endtag(self, tag):
        if tag in VOID or self.depth == 0:
            return
        self.depth -= 1
        if self.depth == 0:
            end = self.text.index(">", self._offset()) + 1
            self.spans.append((self.start, end))

    def handle_data(self, data):
        if self.depth == 0 and data.strip():
            pos = self._offset()
            self.spans.append((pos, pos + len(data)))


def split_blocks(html: str) -> list:
    parser = _Splitter(html)
    parser.feed(html)
    parser.close()
    return [html[a:b].strip() for a, b in parser.spans if html[a:b].strip()]


def _as_note(block: str) -> str:
    m = CALLOUT.match(block)
    if m:
        return block.replace(f'class="callout {m.group(1)}"', f'class="callout {m.group(1)} note"', 1)
    return block


def _pull_sidenotes(block: str) -> tuple:
    notes = [f'<p class="note sn"><sup class="m">{n}</sup> {text}</p>' for n, text in SIDENOTE.findall(block)]
    cleaned = SIDENOTE.sub(lambda m: f'<span class="sn"><sup class="m">{m.group(1)}</sup></span>', block)
    return cleaned, notes


def build_rows(html: str, heading_notes: dict) -> str:
    blocks = split_blocks(html)
    if not blocks:
        return ""
    kept, notes, wide = [], {}, set()
    pending_goal = []
    for block in blocks:
        if GOAL.match(block):
            if kept:
                notes.setdefault(len(kept) - 1, []).append(_as_note(block))
            else:
                pending_goal.append(_as_note(block))
            continue
        if MOVE_PREV.match(block) and kept:
            notes.setdefault(len(kept) - 1, []).append(_as_note(block))
            continue
        index = len(kept)
        block, sidenotes = _pull_sidenotes(block)
        front = []
        m = H2_ID.match(block)
        if m and m.group(1) in heading_notes:
            front.append(heading_notes[m.group(1)])
        front += sidenotes
        if pending_goal:
            front = pending_goal + front
            pending_goal = []
        if front:
            notes[index] = front + notes.get(index, [])
        if WIDE.match(block):
            wide.add(index)
        kept.append(block)

    out, main, side = [], [], []

    def close():
        if main or side:
            aside = f'<aside class="side">{"".join(side)}</aside>' if side else ""
            out.append(f'<div class="mrow"><div class="main">{chr(10).join(main)}</div>{aside}</div>')

    for i, block in enumerate(kept):
        if i in wide:
            close()
            main, side = [], []
            out.append(f'<div class="mrow wide"><div class="main">{block}</div></div>')
            if i in notes:
                out.append(f'<div class="mrow"><div class="main"></div><aside class="side">{"".join(notes[i])}</aside></div>')
            continue
        if i in notes:
            close()
            main, side = [block], list(notes[i])
            continue
        main.append(block)
    close()
    return "\n".join(out)
