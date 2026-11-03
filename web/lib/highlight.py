import re
from html import escape

KEYWORDS = (
    "typedef struct union enum if else return const static volatile extern inline register void "
    "bool true false for while do break continue switch case default goto sizeof int char float "
    "double unsigned signed long short size_t NULL"
).split()

PREPROCESSOR = "include define undef ifdef ifndef if elif else endif pragma error line".split()

TOKEN = re.compile(
    r"(?P<c>/\*[\s\S]*?\*/|//[^\n]*)"
    r"|(?P<s>\"(?:\\.|[^\"\\\n])*\"|'(?:\\.|[^'\\\n])*')"
    r"|(?P<k>#\s*(?:" + "|".join(PREPROCESSOR) + r")\b|\b(?:" + "|".join(KEYWORDS) + r")\b)"
    r"|(?P<f>\b[A-Za-z_][A-Za-z0-9_]*)(?=\s*\()"
    r"|(?P<m>\b(?:0[xX][0-9A-Fa-f]+|\d+\.?\d*(?:[eE][+-]?\d+)?[fFuUlL]*)\b)"
)


def highlight_c(code: str) -> str:
    out = []
    pos = 0
    for m in TOKEN.finditer(code):
        out.append(escape(code[pos:m.start()]))
        out.append(f'<span class="{m.lastgroup}">{escape(m.group())}</span>')
        pos = m.end()
    out.append(escape(code[pos:]))
    return "".join(out)
