import re
import tomllib
import unittest
from collections import Counter
from html import unescape

from web.figures import all_figures
from web.lib.content import RenderContext, render_page
from web.lib.csource import ROOT
from web.lib.pages import load_ui
from web.migrate.html_to_md import CONTAINER, LABEL_DROP, El, parse, source_for, text_of


def lessons():
    data = tomllib.loads((ROOT / "content" / "course.toml").read_text(encoding="utf-8"))
    return [l for p in data["part"] for l in p["lesson"]]


def old_visible(n, out):
    if isinstance(n, str):
        out.append(n)
        return
    cls = n.cls().split()
    if n.tag in ("pre", "x-figure", "x-code", "x-out") or (n.tag == "div" and "w" in cls):
        return
    if n.tag == "a" and "data-src" in n.attrs and not text_of(n).strip():
        out.append(n.attrs["data-src"])
        return
    kids = list(n.children)
    if n.tag == "div" and cls and cls[0] in LABEL_DROP:
        while kids and isinstance(kids[0], str) and not kids[0].strip():
            kids.pop(0)
        if kids and not isinstance(kids[0], str) and kids[0].tag in ("b", "strong"):
            kids.pop(0)
    for k in kids:
        old_visible(k, out)


def old_counts(n, c):
    if isinstance(n, str):
        return
    cls = n.cls().split()
    if n.tag in ("pre", "x-code", "x-out"):
        c["code"] += 1
        return
    if n.tag == "x-figure":
        c["fig:" + n.attrs["name"]] += 1
    if n.tag == "div" and "quiz" in cls:
        buttons = [b for b in walk(n) if isinstance(b, El) and b.tag == "button"]
        c["quiz"] += 1
        c["ok:" + str([("data-ok" in b.attrs) for b in buttons].index(True))] += 1
    for k in n.children:
        old_counts(k, c)


def walk(n):
    yield n
    if isinstance(n, El):
        for k in n.children:
            yield from walk(k)


EDITS = {
    "heapsort": [
        "ليه الريبو مش بيستخدم a+l-1؟",
        "الكتاب بيكتب pq = a+l-1 عشان pq[1] يبقى a[l]. المشكلة إن لما l تبقى 0، المؤشر ده بيشاور على خانة قبل أول المصفوفة، ولغة C مش بتسمح بكده حتى لو عمرنا ما قرينا الخانة دي. فالـ heap.c اللي في الريبو بيحتفظ بنفس الفهرسة من 1، بس بيحسبها كـ offset: H(k) معناها a[l + k - 1]. نفس الخوارزمية بالظبط، من غير المؤشر ده.",
        " (والريبو بيعمل نفس الفهرسة بـ offset بدل المؤشر ده)",
    ],
}
EXTRA_CODE = {"heapsort": 2}

BLOCKS = [
    r'<figure class="(?:code|output)"[\s\S]*?</figure>',
    r"<pre>[\s\S]*?</pre>",
    r'<div class="(?:figure|steps)" dir="ltr">[\s\S]*?</div><!--end-->',
    r'<section class="trace"[\s\S]*?</section>',
    r'<div class="widget"[^>]*></div>',
    r'<p class="block-label">[\s\S]*?</p>',
    r"<summary>[\s\S]*?</summary>",
]


def new_visible(html):
    for pat in BLOCKS:
        html = re.sub(pat, "", html)
    return re.sub(r"\s+", "", unescape(re.sub(r"<[^>]+>", "", html)))


def new_counts(html, md):
    c = Counter()
    c["code"] = len(re.findall(r'<figure class="(?:code|output)"', html))
    bare = re.sub(r'<figure class="(?:code|output)"[\s\S]*?</figure>', "", html)
    c["code"] += len(re.findall(r"<pre>", bare))
    for name in re.findall(r"^::figure ([\w-]+)", md, re.M):
        c["fig:" + name] += 1
    for quiz in re.findall(r'<div class="quiz">([\s\S]*?)</ol>', html):
        c["quiz"] += 1
        buttons = re.findall(r"<button[^>]*>", quiz)
        c["ok:" + str(["data-ok" in b for b in buttons].index(True))] += 1
    return c


class MigrateTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        figs = {k: type(v)(v.name, v.kind, "") for k, v in all_figures().items()}
        ui = load_ui(ROOT / "content" / "ui.toml")["ar"]
        traces = {t: (lambda: "") for t in ("partition", "selection", "insertion", "shellsort", "mergesort", "heap",
                                             "batcher", "lsd", "union-find", "bst", "chaining", "probing", "recursion")}
        cls.pages = []
        for l in lessons():
            old_html = source_for(l["old_file"]).read_text(encoding="utf-8")
            md = (ROOT / "content" / "ar" / f"{l['slug']}.md").read_text(encoding="utf-8")
            ctx = RenderContext("ar", figs, traces, ui, f"content/ar/{l['slug']}.md")
            new_html = render_page(md, ctx).html
            new_html = re.sub(r'(<div class="(?:figure|steps)" dir="ltr">)</div>', r"\1</div><!--end-->", new_html)
            cls.pages.append((l["slug"], parse(old_html), md, new_html))

    def test_page_count(self):
        self.assertEqual(len(self.pages), 70)

    def test_text_parity(self):
        for slug, old, md, new in self.pages:
            with self.subTest(page=slug):
                out = []
                old_visible(old, out)
                a = re.sub(r"\s+", "", "".join(out))
                b = new_visible(new)
                for snippet in EDITS.get(slug, []):
                    squeezed = re.sub(r"\s+", "", snippet)
                    self.assertIn(squeezed, b)
                    b = b.replace(squeezed, "", 1)
                if a != b:
                    k = next(i for i in range(min(len(a), len(b))) if a[i] != b[i]) if a[:len(b)] != b[:len(a)] else min(len(a), len(b))
                    self.fail(f"differs at {k}: old ...{a[max(0, k - 40):k + 40]}... new ...{b[max(0, k - 40):k + 40]}...")

    def test_structure_parity(self):
        for slug, old, md, new in self.pages:
            with self.subTest(page=slug):
                c = Counter()
                old_counts(old, c)
                c["code"] += EXTRA_CODE.get(slug, 0)
                self.assertEqual(c, new_counts(new, md))

    def test_containers_are_known(self):
        self.assertEqual(set(CONTAINER.values()), {"goal", "tip", "oops", "problem", "think", "solution"})


if __name__ == "__main__":
    unittest.main()
