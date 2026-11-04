import re
import tomllib
from pathlib import Path

ARABIC = re.compile(r"[؀-ۿ]")
CAPTION = re.compile(r"(<figcaption>)([\s\S]*?)(</figcaption>)")
TEXT_NODE = re.compile(r">([^<>]+)<")


def arabic_strings(html: str) -> list:
    caps = [c.strip() for c in CAPTION.findall(html) for c in [c[1]] if ARABIC.search(c)]
    rest = CAPTION.sub("", html)
    labels = [t.strip() for t in TEXT_NODE.findall(rest) if ARABIC.search(t)]
    return caps + labels


def localize(html: str, mapping: dict) -> str:
    if not mapping:
        return html

    def cap(m):
        key = m.group(2).strip()
        return m.group(1) + mapping[key] + m.group(3) if key in mapping else m.group(0)

    parts = []
    last = 0
    for m in CAPTION.finditer(html):
        parts.append(_labels(html[last:m.start()], mapping))
        parts.append(cap(m))
        last = m.end()
    parts.append(_labels(html[last:], mapping))
    return "".join(parts)


def _labels(chunk: str, mapping: dict) -> str:
    def rep(m):
        raw = m.group(1)
        key = raw.strip()
        if key in mapping and ARABIC.search(key):
            return ">" + raw.replace(key, mapping[key]) + "<"
        return m.group(0)

    return TEXT_NODE.sub(rep, chunk)


def load_figure_text(path: Path) -> dict:
    path = Path(path)
    if not path.exists():
        return {}
    data = tomllib.loads(path.read_text(encoding="utf-8"))
    return {t["ar"]: t["en"] for t in data.get("t", [])}
