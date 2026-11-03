import re
from dataclasses import dataclass, field
from html import escape, unescape

from markdown_it import MarkdownIt

from web.lib.csource import SourceError, extract_function, extract_region, run_output, whole_file
from web.lib.highlight import highlight_c

CONTAINERS = {"goal", "tip", "oops", "problem", "think", "solution", "quiz"}
CALLOUTS = {"goal", "tip", "oops"}
LEAVES = {"code", "output", "figure", "trace", "widget"}
WIDGETS = {"bigoh"}

_md = MarkdownIt("commonmark", {"html": True}).enable("table")


class ContentError(Exception):
    pass


@dataclass
class RenderContext:
    lang: str
    figures: dict
    traces: dict
    ui: dict
    source_path: str


@dataclass
class Rendered:
    html: str
    toc: list = field(default_factory=list)
    sources: set = field(default_factory=set)
    figures: list = field(default_factory=list)
    traces: list = field(default_factory=list)


@dataclass
class Node:
    kind: str
    line: int
    name: str = ""
    args: dict = field(default_factory=dict)
    text: str = ""
    children: list = field(default_factory=list)


ARG = re.compile(r'(\w+)="([^"]*)"')


def parse_args(rest: str) -> dict:
    args = {k: v for k, v in ARG.findall(rest)}
    bare = ARG.sub("", rest).split()
    if bare:
        args["_"] = bare[0]
    return args


def _parse(text: str, ctx: RenderContext) -> list:
    lines = text.split("\n")
    root = Node("root", 0)
    stack = [root]
    buf, buf_start = [], 1

    def err(line, reason):
        return ContentError(f"{ctx.source_path}:{line}: {reason}")

    def flush():
        nonlocal buf
        if any(l.strip() for l in buf):
            stack[-1].children.append(Node("md", buf_start, text="\n".join(buf)))
        buf = []

    i = 0
    while i < len(lines):
        line = lines[i]
        n = i + 1
        stripped = line.strip()
        if stripped.startswith("```"):
            flush()
            info = stripped[3:].strip()
            j = i + 1
            while j < len(lines) and not lines[j].strip().startswith("```"):
                j += 1
            if j >= len(lines):
                raise err(n, "unclosed code fence")
            stack[-1].children.append(Node("fence", n, args=parse_args(info), name=info.split(" ")[0] if info else "",
                                           text="\n".join(lines[i + 1:j])))
            i = j + 1
            buf_start = i + 1
            continue
        if stripped == ":::":
            flush()
            if len(stack) == 1:
                raise err(n, "::: closes nothing")
            stack.pop()
            i += 1
            buf_start = i + 1
            continue
        if stripped.startswith(":::"):
            flush()
            name, _, rest = stripped[3:].partition(" ")
            if name not in CONTAINERS:
                raise err(n, f"unknown container {name}")
            if len(stack) >= 3:
                raise err(n, f":::{name} nested too deep")
            if name == "quiz" and any(s.name == "solution" for s in stack):
                raise err(n, "a quiz cannot sit inside a solution")
            node = Node("container", n, name=name, text=rest.strip())
            stack[-1].children.append(node)
            stack.append(node)
            i += 1
            buf_start = i + 1
            continue
        if stripped.startswith("::"):
            flush()
            name, _, rest = stripped[2:].partition(" ")
            if name not in LEAVES:
                raise err(n, f"unknown directive {name}")
            stack[-1].children.append(Node("leaf", n, name=name, args=parse_args(rest)))
            i += 1
            buf_start = i + 1
            continue
        if not buf:
            buf_start = n
        buf.append(line)
        i += 1
    flush()
    if len(stack) > 1:
        raise err(stack[-1].line, f"unclosed :::{stack[-1].name}")
    return root.children


def _code_block(code_html: str, caption: str = "", src: str = "") -> str:
    cap = f"<figcaption>{caption}</figcaption>" if caption else ""
    data = f' data-src="{escape(src)}"' if src else ""
    return f'<figure class="code" dir="ltr"{data}>{cap}<pre><code>{code_html}</code></pre></figure>'


def _leaf(node: Node, ctx: RenderContext, out: Rendered) -> str:
    a = node.args
    try:
        if node.name == "code":
            src = a.get("src") or ""
            if not src:
                raise SourceError("code needs src")
            if "fn" in a:
                snip = extract_function(src, a["fn"])
            elif "region" in a:
                snip = extract_region(src, a["region"])
            else:
                snip = whole_file(src)
            out.sources.add(src)
            return _code_block(highlight_c(snip.text), escape(src), src)
        if node.name == "output":
            src = a.get("src") or ""
            lines = int(a["lines"]) if "lines" in a else None
            text = run_output(src, lines)
            out.sources.add(src)
            return (f'<figure class="output" dir="ltr" data-src="{escape(src)}"><figcaption>{escape(ctx.ui.get("output", ""))}'
                    f"</figcaption><pre>{escape(text)}</pre></figure>")
    except SourceError as e:
        raise ContentError(f"{ctx.source_path}:{node.line}: {e}")
    name = a.get("_", "")
    if node.name == "figure":
        fig = ctx.figures.get(name)
        if fig is None:
            raise ContentError(f"{ctx.source_path}:{node.line}: unknown figure {name}")
        out.figures.append(name)
        cls = "steps" if fig.kind == "steps" else "figure"
        return f'<div class="{cls}" dir="ltr">{fig.html}</div>'
    if node.name == "trace":
        make = ctx.traces.get(name)
        if make is None:
            raise ContentError(f"{ctx.source_path}:{node.line}: unknown trace {name}")
        out.traces.append(name)
        return make()
    if name not in WIDGETS:
        raise ContentError(f"{ctx.source_path}:{node.line}: unknown widget {name}")
    return f'<div class="widget" data-widget="{name}" dir="ltr"></div>'


def _fence(node: Node, ctx: RenderContext) -> str:
    lang = node.name
    if lang == "c":
        code = highlight_c(node.text)
    else:
        code = escape(node.text)
    if "listing" in node.args:
        cap = f'{escape(ctx.ui.get("listing", ""))} {escape(node.args["listing"])}'
        return _code_block(code, cap)
    if lang == "c":
        return _code_block(code)
    return f'<pre dir="ltr"><code>{code}</code></pre>'


OPTION = re.compile(r"^\s*\d+\.\s+\[( |x)\]\s+(.*)$")


def _quiz(node: Node, ctx: RenderContext) -> str:
    if len(node.children) != 1 or node.children[0].kind != "md":
        raise ContentError(f"{ctx.source_path}:{node.line}: a quiz holds only text")
    q_lines, opts, why = [], [], []
    for line in node.children[0].text.split("\n"):
        m = OPTION.match(line)
        if m:
            opts.append((m.group(1) == "x", m.group(2)))
        elif line.lstrip().startswith(">"):
            why.append(re.sub(r"^\s*>\s?", "", line))
        elif not opts:
            q_lines.append(line)
        elif line.strip():
            raise ContentError(f"{ctx.source_path}:{node.line}: unexpected quiz line {line.strip()}")
    if sum(ok for ok, _ in opts) != 1:
        raise ContentError(f"{ctx.source_path}:{node.line}: a quiz needs exactly one [x] option")
    q = _md.renderInline("\n".join(q_lines).strip())
    items = "".join(
        f'<li><button type="button"{" data-ok" if ok else ""}>{_md.renderInline(t)}</button></li>' for ok, t in opts)
    why_html = _md.render("\n".join(why)) if why else ""
    return (f'<div class="quiz"><p class="q">{q}</p><ol class="opts">{items}</ol>'
            f'<div class="why" hidden>{why_html}</div></div>')


def _render_nodes(nodes: list, ctx: RenderContext, out: Rendered) -> str:
    html = []
    for node in nodes:
        if node.kind == "md":
            html.append(_md.render(node.text))
        elif node.kind == "fence":
            html.append(_fence(node, ctx))
        elif node.kind == "leaf":
            html.append(_leaf(node, ctx, out))
        elif node.name == "quiz":
            html.append(_quiz(node, ctx))
        else:
            inner = _render_nodes(node.children, ctx, out)
            title = escape(node.text) if node.text else escape(ctx.ui.get(node.name, ""))
            if node.name in CALLOUTS:
                html.append(f'<aside class="callout {node.name}"><p class="callout-title">{title}</p>{inner}</aside>')
            elif node.name == "solution":
                html.append(f'<details class="solution"><summary>{title}</summary>{inner}</details>')
            else:
                html.append(f'<section class="{node.name}"><p class="block-label">{title}</p>{inner}</section>')
    return "\n".join(html)


HEADING = re.compile(r"<h([23])>(.*?)</h\1>", re.S)
TAG = re.compile(r"<[^>]+>")


def _slug(text: str) -> str:
    latin = re.sub(r"[^\x00-\x7f]", " ", text).lower()
    return "-".join(re.findall(r"[a-z0-9]+", latin))


def _add_ids(html: str, out: Rendered) -> str:
    seen = {}
    count = [0]

    def rep(m):
        count[0] += 1
        level, inner = int(m.group(1)), m.group(2)
        plain = unescape(TAG.sub("", inner)).strip()
        has_arabic = re.search(r"[؀-ۿ]", plain) is not None
        base = _slug(plain)
        if not base or (has_arabic and not re.search(r"[a-z]", base)):
            base = f"s-{count[0]}"
        hid = base
        k = 2
        while hid in seen:
            hid = f"{base}-{k}"
            k += 1
        seen[hid] = True
        out.toc.append((level, hid, plain))
        return f'<h{level} id="{hid}">{inner}</h{level}>'

    return HEADING.sub(rep, html)


def render_page(text: str, ctx: RenderContext) -> Rendered:
    out = Rendered(html="")
    nodes = _parse(text, ctx)
    out.html = _add_ids(_render_nodes(nodes, ctx, out), out)
    return out
