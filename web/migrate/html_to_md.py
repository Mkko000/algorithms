import re
from html import escape, unescape
from html.parser import HTMLParser
from pathlib import Path

from markdown_it import MarkdownIt

from web.lib.csource import ROOT, SourceError, extract_function, whole_file

KNOWN = ("b", "strong", "i", "em", "td", "tr", "th", "thead", "tbody", "table", "p", "code", "div", "button",
         "h2", "h3", "h4", "pre", "a", "sub", "sup", "li", "ul", "ol", "br", "span", "small", "x-figure", "x-code",
         "x-out", "kbd")
VOID = {"br", "x-figure", "x-code", "x-out"}
INLINE = {"b", "strong", "i", "em", "code", "a", "sub", "sup", "br", "span", "small", "kbd"}
WIDGET_TRACES = {
    "partviz": ["partition"], "heapviz": ["heap"], "netviz": ["batcher"], "lsdviz": ["lsd"],
    "ufviz": ["union-find"], "bstviz": ["bst"], "hashviz": ["chaining"], "probeviz": ["probing"],
    "recur": ["recursion"],
}
SORTVIZ = {"elem-sort": ["selection", "insertion"], "mergesort": ["mergesort"]}
LABEL_DROP = {"story", "think", "sol"}
CONTAINER = {"goal": "goal", "tip": "tip", "oops": "oops", "story": "problem", "think": "think", "sol": "solution"}

_md = MarkdownIt("commonmark", {"html": True})


class El:
    def __init__(self, tag, attrs):
        self.tag, self.attrs, self.children = tag, dict(attrs), []

    def cls(self):
        return self.attrs.get("class", "") or ""


class Builder(HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.root = El("root", {})
        self.stack = [self.root]

    def handle_starttag(self, tag, attrs):
        el = El(tag, attrs)
        self.stack[-1].children.append(el)
        if tag not in VOID:
            self.stack.append(el)

    def handle_startendtag(self, tag, attrs):
        self.stack[-1].children.append(El(tag, attrs))

    def handle_endtag(self, tag):
        if tag in VOID:
            return
        for k in range(len(self.stack) - 1, 0, -1):
            if self.stack[k].tag == tag:
                del self.stack[k:]
                return

    def handle_data(self, data):
        self.stack[-1].children.append(data)


def preprocess(html: str) -> str:
    html = re.sub(r"<!--fig:([\w-]+)-->.*?<!--/fig:\1-->", lambda m: f'<x-figure name="{m.group(1)}">', html, flags=re.S)
    html = re.sub(r"<!--code:([^:>]+)(?::([\w-]+))?-->",
                  lambda m: f'<x-code src="{m.group(1)}"' + (f' region="{m.group(2)}"' if m.group(2) else "") + ">", html)
    html = re.sub(r"<!--out:([^:>]+)(?::(\d+))?-->",
                  lambda m: f'<x-out src="{m.group(1)}"' + (f' lines="{m.group(2)}"' if m.group(2) else "") + ">", html)
    html = re.sub(r"<(?!/?(?:" + "|".join(re.escape(k) for k in KNOWN) + r")\b|!--)", "&lt;", html)
    return html


def parse(html: str) -> El:
    b = Builder()
    b.feed(preprocess(html))
    b.close()
    return b.root


def text_of(n) -> str:
    if isinstance(n, str):
        return n
    if n.tag == "br":
        return "\n"
    return "".join(text_of(c) for c in n.children)


def serialize(n) -> str:
    if isinstance(n, str):
        return escape(n, quote=False)
    if n.tag == "a" and "data-src" in n.attrs:
        path = n.attrs["data-src"]
        inner = "".join(serialize(c) for c in n.children) or escape(path)
        return f'<a href="src:{escape(path)}">{inner}</a>'
    if n.tag == "a" and (n.attrs.get("href") or "").startswith("#/"):
        inner = "".join(serialize(c) for c in n.children)
        return f'<a href="lesson:{escape(n.attrs["href"][2:])}">{inner}</a>'
    attrs = "".join(f' {k}="{escape(v or "", quote=True)}"' if v is not None else f" {k}" for k, v in n.attrs.items())
    if n.tag in VOID:
        return f"<{n.tag}{attrs}>"
    return f"<{n.tag}{attrs}>" + "".join(serialize(c) for c in n.children) + f"</{n.tag}>"


MD_SPECIAL = re.compile(r"([\\`*_\[\]<>])")


def esc(text: str) -> str:
    text = text.replace("&", "&amp;")
    return MD_SPECIAL.sub(r"\\\1", text)


def esc_start(line: str) -> str:
    if re.match(r"^\s*(#|>|\+|-|=|:)", line):
        return re.sub(r"^(\s*)(.)", r"\1\\\2", line, count=1)
    m = re.match(r"^(\s*\d+)([.)])", line)
    if m:
        return m.group(1) + "\\" + line[len(m.group(1)):]
    return line


def code_span(text: str) -> str:
    text = text.replace("\n", " ")
    ticks = "``" if "`" in text else "`"
    pad = " " if text.startswith("`") or text.endswith("`") or (text.startswith(" ") and text.endswith(" ") and text.strip()) else ""
    return f"{ticks}{pad}{text}{pad}{ticks}"


def wrap(marker, inner):
    m = re.match(r"^(\s*)(.*?)(\s*)$", inner, re.S)
    lead, core, trail = m.groups()
    if not core:
        return inner
    return f"{lead}{marker}{core}{marker}{trail}"


def inline(nodes) -> str:
    out = []
    for n in nodes:
        if isinstance(n, str):
            out.append(esc(re.sub(r"\s*\n\s*", " ", n)))
        elif n.tag in ("b", "strong"):
            out.append(wrap("**", inline(n.children)))
        elif n.tag in ("i", "em"):
            out.append(wrap("*", inline(n.children)))
        elif n.tag == "code":
            out.append(code_span(text_of(n)))
        elif n.tag == "a":
            label = inline(n.children)
            if "data-src" in n.attrs:
                path = n.attrs["data-src"]
                out.append(f"[{label or esc(path)}](src:{path})")
            else:
                href = n.attrs.get("href", "")
                if href.startswith("#/"):
                    href = "lesson:" + href[2:]
                out.append(f"[{label}]({href})")
        else:
            out.append(serialize(n))
    return "".join(out)


def visible(html: str) -> str:
    s = re.sub(r"<[^>]+>", "", html)
    return re.sub(r"\s+", "", unescape(s))


def count_tags(html: str) -> tuple:
    return (len(re.findall(r"<(strong|b)>", html)), len(re.findall(r"<code>", html)),
            len(re.findall(r"<(em|i)>", html)), len(re.findall(r"<a ", html)))


def filled(html: str) -> str:
    return re.sub(r'<a data-src="([^"]+)"></a>', r'<a data-src="\1">\1</a>', html)


def only_sources(nodes) -> bool:
    real = [n for n in nodes if not (isinstance(n, str) and not n.strip())]
    return bool(real) and all(not isinstance(n, str) and n.tag == "a" and "data-src" in n.attrs for n in real)


def paragraph(nodes) -> str:
    if only_sources(nodes):
        return "\n".join(f"* {inline([n])}" for n in nodes if not isinstance(n, str))
    md = esc_start(inline(nodes).strip())
    original = filled("".join(serialize(n) for n in nodes))
    rendered = _md.renderInline(md)
    if visible(rendered) == visible(original) and count_tags(rendered) == count_tags(original):
        return md
    return "".join(serialize(n) for n in nodes).strip()


def is_inline(n) -> bool:
    return isinstance(n, str) or n.tag in INLINE


class Converter:
    def __init__(self, slug: str, chapter: int | None):
        self.slug = slug
        self.warnings = []
        self.candidates = self._candidates(chapter)
        self._fns = {}

    def _candidates(self, chapter):
        if not chapter:
            return []
        folders = sorted((ROOT / "src").glob(f"{chapter:02d}-*"))
        files = []
        for f in folders:
            files += sorted(str(p.relative_to(ROOT)) for p in f.glob("*.c"))
            files += sorted(str(p.relative_to(ROOT)) for p in f.glob("exercises/*.c"))
        return files

    def functions(self, path):
        if path not in self._fns:
            src = (ROOT / path).read_text(encoding="utf-8")
            names = []
            for m in re.finditer(r"^(?:static\s+)?(?:inline\s+)?[A-Za-z_][\w\s\*]*?\b([A-Za-z_]\w*)\s*\([^;{]*\)\s*\{",
                                 src, re.M):
                if m.group(1) not in ("if", "while", "for", "switch") and m.group(1) not in names:
                    names.append(m.group(1))
            table = {}
            for name in names:
                try:
                    table.setdefault(re.sub(r"\s+", "", extract_function(path, name).text), name)
                except SourceError:
                    pass
            try:
                table.setdefault(re.sub(r"\s+", "", whole_file(path).text), None)
            except SourceError:
                pass
            self._fns[path] = table
        return self._fns[path]

    def match_code(self, text, extra):
        key = re.sub(r"\s+", "", text)
        if len(key) < 30:
            return None
        for path in list(dict.fromkeys(extra + self.candidates)):
            if not (ROOT / path).exists():
                continue
            table = self.functions(path)
            if key in table:
                fn = table[key]
                return f'::code src="{path}" fn="{fn}"' if fn else f'::code src="{path}"'
        return None

    def pre(self, n, extra, before=""):
        text = text_of(n).strip("\n")
        directive = self.match_code(text, extra)
        if directive:
            return directive
        first = text.split("\n", 1)[0]
        label = None
        if first.lstrip().startswith(("/*", "//")):
            m = re.search(r"Programs? (\d+\.\d+)", first)
            label = m.group(1) if m else None
        is_c = bool(re.search(r"[;{}]\s*$", text, re.M)) or "#include" in text
        near = set(re.findall(r"Program (\d+\.\d+)", before))
        if label is None and is_c and len(near) == 1:
            label = next(iter(near))
        info = "c" if is_c else ""
        if label:
            info = f'c listing="Program {label}"'
        fence = "~~~" if "```" in text else "```"
        return f"{fence}{info}\n{text}\n{fence}"

    def quiz(self, n):
        q, opts, why = "", [], []
        for c in n.children:
            if isinstance(c, str):
                if c.strip():
                    self.warnings.append(f"{self.slug}: stray quiz text {c.strip()[:30]}")
                continue
            if c.tag == "p" and "q" in c.cls().split():
                q = paragraph(c.children)
            elif c.tag == "button":
                opts.append(("data-ok" in c.attrs, paragraph(c.children)))
            elif c.tag == "p" and "why" in c.cls().split():
                why.append(paragraph(c.children))
            elif c.tag == "div":
                for b in c.children:
                    if not isinstance(b, str) and b.tag == "button":
                        opts.append(("data-ok" in b.attrs, paragraph(b.children)))
            else:
                self.warnings.append(f"{self.slug}: unexpected quiz element {c.tag}")
        lines = [":::quiz", q, ""]
        lines += [f"{k}. [{'x' if ok else ' '}] {t}" for k, (ok, t) in enumerate(opts, 1)]
        if why:
            lines += [""] + ["> " + w for w in why]
        lines.append(":::")
        return "\n".join(lines)

    def container(self, n, kind, extra):
        kids = list(n.children)
        title = ""
        while kids and isinstance(kids[0], str) and not kids[0].strip():
            kids.pop(0)
        if kids and not isinstance(kids[0], str) and kids[0].tag in ("b", "strong"):
            label = text_of(kids.pop(0)).strip()
            if kind not in LABEL_DROP:
                title = label
        body = self.blocks(kids, extra)
        head = f":::{CONTAINER[kind]}" + (f" {title}" if title else "")
        return f"{head}\n{body}\n:::" if body else f"{head}\n:::"

    def widget(self, n):
        name = n.attrs.get("data-w", "")
        if name == "bigoh":
            return "::widget bigoh"
        if name == "sortviz":
            names = SORTVIZ.get(self.slug)
        else:
            names = WIDGET_TRACES.get(name)
        if not names:
            self.warnings.append(f"{self.slug}: no trace for widget {name}")
            return ""
        return "\n\n".join(f"::trace {t}" for t in names)

    def listing(self, n):
        items = [c for c in n.children if not isinstance(c, str) or c.strip()]
        if any(isinstance(c, str) or c.tag != "li" or any(not is_inline(k) for k in c.children) for c in items):
            return serialize(n)
        marker = "1." if n.tag == "ol" else "*"
        out = []
        for k, li in enumerate(items, 1):
            text = paragraph(li.children)
            out.append(f"{k}. {text}" if n.tag == "ol" else f"{marker} {text}")
        return "\n".join(out)

    def blocks(self, nodes, extra) -> str:
        out, run = [], []

        def flush():
            if any(isinstance(r, str) and r.strip() or not isinstance(r, str) for r in run):
                out.append(paragraph(run))
            run.clear()

        for n in nodes:
            if is_inline(n):
                run.append(n)
                continue
            flush()
            tag, cls = n.tag, n.cls().split()
            if tag == "p" and n.attrs.get("style"):
                out.append(serialize(n))
            elif tag == "p":
                out.append(paragraph(n.children))
            elif tag in ("h2", "h3", "h4"):
                out.append("#" * int(tag[1]) + " " + inline(n.children).strip())
            elif tag == "pre":
                out.append(self.pre(n, extra, "\n".join(out[-2:])))
            elif tag == "div" and "quiz" in cls:
                out.append(self.quiz(n))
            elif tag == "div" and cls and cls[0] in CONTAINER:
                out.append(self.container(n, cls[0], extra))
            elif tag == "div" and "exs" in cls:
                inner = self.blocks(n.children, extra)
                if inner:
                    out.append(inner)
            elif tag == "div" and "w" in cls:
                w = self.widget(n)
                if w:
                    out.append(w)
            elif tag == "x-figure":
                out.append(f"::figure {n.attrs['name']}")
            elif tag == "x-code":
                region = f' region="{n.attrs["region"]}"' if n.attrs.get("region") else ""
                out.append(f'::code src="{n.attrs["src"]}"{region}')
            elif tag == "x-out":
                lines = f' lines="{n.attrs["lines"]}"' if n.attrs.get("lines") else ""
                out.append(f'::output src="{n.attrs["src"]}"{lines}')
            elif tag in ("ul", "ol"):
                out.append(self.listing(n))
            elif tag == "table":
                out.append(re.sub(r"\n\s*\n", "\n", serialize(n)))
            else:
                self.warnings.append(f"{self.slug}: raw {tag}.{'.'.join(cls)}")
                out.append(serialize(n))
        flush()
        return "\n\n".join(b for b in out if b.strip())


def convert(html: str, slug: str, chapter: int | None) -> tuple:
    root = parse(html)
    extra = re.findall(r'data-src="(src/[^"]+\.c)"', html) + re.findall(r"<!--(?:code|out):([^:>]+)", html)
    c = Converter(slug, chapter)
    md = c.blocks(root.children, extra)
    return md.strip() + "\n", c.warnings


HEAP_NOTE = """
:::tip ليه الريبو مش بيستخدم a+l-1؟
الكتاب بيكتب `pq = a+l-1` عشان `pq[1]` يبقى `a[l]`. المشكلة إن لما `l` تبقى 0، المؤشر ده بيشاور على خانة قبل أول المصفوفة، ولغة C مش بتسمح بكده حتى لو عمرنا ما قرينا الخانة دي. فالـ `heap.c` اللي في الريبو بيحتفظ بنفس الفهرسة من 1، بس بيحسبها كـ offset: `H(k)` معناها `a[l + k - 1]`. نفس الخوارزمية بالظبط، من غير المؤشر ده.
:::

::code src="src/09-heapsort/heap.c" fn="H"

::code src="src/09-heapsort/heap.c" fn="heapsort"
"""

POST_EDITS = {
    "heapsort": [
        ("لاحظ الحيلة `pq = a+l-1` عشان `pq[1]` يبقى `a[l]` ونحتفظ بالفهرسة من 1.\n",
         "لاحظ الحيلة `pq = a+l-1` عشان `pq[1]` يبقى `a[l]` ونحتفظ بالفهرسة من 1.\n" + HEAP_NOTE),
        ("سادجوك بيستخدم الفهرسة من 1 وحيلة `a+l-1`؛ اختار واحدة والتزم.",
         "سادجوك بيستخدم الفهرسة من 1 وحيلة `a+l-1` (والريبو بيعمل نفس الفهرسة بـ offset بدل المؤشر ده)؛ اختار واحدة والتزم."),
    ],
}


def post_edit(slug: str, md: str) -> str:
    for old, new in POST_EDITS.get(slug, []):
        if md.count(old) != 1:
            raise ValueError(f"{slug}: edit anchor not found once: {old[:40]}")
        md = md.replace(old, new)
    return md


def source_for(old_file: str) -> Path:
    ds = ROOT / "tools" / "ds_src" / old_file
    return ds if ds.exists() else ROOT / "docs" / "lessons" / old_file


def main():
    import tomllib
    data = tomllib.loads((ROOT / "content" / "course.toml").read_text(encoding="utf-8"))
    warnings = []
    for part in data["part"]:
        for l in part["lesson"]:
            html = source_for(l["old_file"]).read_text(encoding="utf-8")
            md, w = convert(html, l["slug"], l.get("chapter"))
            md = post_edit(l["slug"], md)
            (ROOT / "content" / "ar" / f"{l['slug']}.md").write_text(md, encoding="utf-8")
            warnings += w
    for w in warnings:
        print(w)
    print(f"{len(warnings)} warnings")


if __name__ == "__main__":
    main()
