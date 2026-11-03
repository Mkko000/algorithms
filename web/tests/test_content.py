import re
import unittest

from web.figures import Figure
from web.lib.content import ContentError, RenderContext, render_page
from web.tests.support import sample_c

UI = {"output": "Output", "solution": "Solution", "listing": "From the book:", "problem": "Problem",
      "think": "How to think about it", "goal": "Goal", "tip": "Tip", "oops": "Watch out"}


def ctx(lang="en"):
    return RenderContext(
        lang=lang,
        figures={"f1": Figure("f1", "fig", "<svg></svg>"),
                 "s1": Figure("s1", "steps", '<figure class="frame"><svg></svg></figure>')},
        traces={"t1": lambda: '<section class="trace" dir="ltr" data-trace="t1"></section>'},
        ui=UI,
        source_path="content/en/x.md",
    )


class ContentTest(unittest.TestCase):
    def setUp(self):
        self.fix = sample_c()

    def test_code_by_function(self):
        r = render_page(f'::code src="{self.fix}" fn="partition"\n', ctx())
        self.assertEqual(r.html.count('<figure class="code" dir="ltr"'), 1)
        self.assertIn(f'data-src="{self.fix}"', r.html)
        self.assertIn('<span class="k">int</span>', r.html)
        self.assertEqual(r.sources, {self.fix})

    def test_code_by_region(self):
        r = render_page(f'::code src="{self.fix}" region="demo"\n', ctx())
        self.assertIn("twice", r.html)
        self.assertNotIn("partition", r.html)

    def test_unknown_function_is_content_error(self):
        with self.assertRaises(ContentError) as cm:
            render_page(f'::code src="{self.fix}" fn="nope"\n', ctx())
        msg = str(cm.exception)
        self.assertTrue(msg.startswith("content/en/x.md:1: "), msg)
        self.assertTrue(msg.endswith("no definition of nope"), msg)

    def test_error_line_number(self):
        with self.assertRaises(ContentError) as cm:
            render_page("Hello\n\nworld\n\n::figure nope\n", ctx())
        self.assertIn("content/en/x.md:5: unknown figure nope", str(cm.exception))

    def test_output_lines(self):
        r = render_page(f'::output src="{self.fix}" lines="2"\n', ctx())
        self.assertIn("<pre>1\n2\n...</pre>", r.html)
        self.assertIn(f'<figure class="output" dir="ltr" data-src="{self.fix}"><figcaption>Output</figcaption>', r.html)
        self.assertEqual(r.sources, {self.fix})

    def test_listing_caption(self):
        r = render_page('```c listing="Program 7.1"\nint x;\n```\n', ctx())
        self.assertIn("<figcaption>From the book: Program 7.1</figcaption>", r.html)
        self.assertIn('<span class="k">int</span>', r.html)

    def test_plain_fence(self):
        r = render_page("```\na < b\n```\n", ctx())
        self.assertIn('<pre dir="ltr"><code>a &lt; b</code></pre>', r.html)
        self.assertNotIn("figcaption", r.html)

    def test_figure_and_steps(self):
        r = render_page("::figure f1\n\n::figure s1\n", ctx())
        self.assertIn('<div class="figure" dir="ltr"><svg></svg></div>', r.html)
        self.assertIn('<div class="steps" dir="ltr"><figure class="frame">', r.html)
        self.assertEqual(r.figures, ["f1", "s1"])

    def test_trace(self):
        r = render_page("::trace t1\n", ctx())
        self.assertIn('data-trace="t1"', r.html)
        self.assertEqual(r.traces, ["t1"])

    def test_widget(self):
        r = render_page("::widget bigoh\n", ctx())
        self.assertIn('<div class="widget" data-widget="bigoh" dir="ltr"></div>', r.html)

    def test_quiz_markup(self):
        text = (":::quiz\nWhat is guaranteed?\n\n1. [ ] Sorted\n2. [x] Pivot in place\n3. [ ] Nothing\n\n"
                "> Because the pivot ends at its final index.\n:::\n")
        r = render_page(text, ctx())
        self.assertEqual(r.html.count("data-ok"), 1)
        m = re.search(r'<button type="button" data-ok>(.*?)</button>', r.html)
        self.assertEqual(m.group(1), "Pivot in place")
        self.assertIn('<p class="q">What is guaranteed?</p>', r.html)
        self.assertRegex(r.html, r'<div class="why" hidden><p>Because the pivot ends at its final index.</p>\s*</div>')

    def test_quiz_without_answer_is_error(self):
        with self.assertRaises(ContentError):
            render_page(":::quiz\nQ?\n\n1. [ ] a\n2. [ ] b\n:::\n", ctx())

    def test_solution_is_details(self):
        r = render_page(":::solution\nThe answer.\n:::\n", ctx())
        self.assertIn('<details class="solution"><summary>Solution</summary>', r.html)
        self.assertIn("<p>The answer.</p>", r.html)

    def test_callout_title(self):
        r = render_page(":::tip Mind the gap\nText.\n:::\n", ctx())
        self.assertIn('<aside class="callout tip"><p class="callout-title">Mind the gap</p>', r.html)

    def test_callout_default_title(self):
        r = render_page(":::goal\nText.\n:::\n", ctx())
        self.assertIn('<p class="callout-title">Goal</p>', r.html)

    def test_problem_and_think(self):
        r = render_page(":::problem\nShow it.\n:::\n\n:::think\nLike this.\n:::\n", ctx())
        self.assertIn('<section class="problem"><p class="block-label">Problem</p>', r.html)
        self.assertIn('<section class="think"><p class="block-label">How to think about it</p>', r.html)

    def test_code_inside_container(self):
        r = render_page(f':::solution\n::code src="{self.fix}" fn="partition"\n:::\n', ctx())
        self.assertIn('<details class="solution">', r.html)
        self.assertIn('<figure class="code"', r.html)

    def test_quiz_inside_solution_is_error(self):
        with self.assertRaises(ContentError):
            render_page(":::solution\n:::quiz\nQ?\n\n1. [x] a\n:::\n:::\n", ctx())

    def test_unknown_directive(self):
        with self.assertRaises(ContentError) as cm:
            render_page("::chart x\n", ctx())
        self.assertIn("unknown directive chart", str(cm.exception))

    def test_unclosed_container(self):
        with self.assertRaises(ContentError) as cm:
            render_page(":::tip\nText\n", ctx())
        self.assertIn("unclosed :::tip", str(cm.exception))

    def test_fence_protects_directives(self):
        r = render_page("```\n::figure nope\n```\n", ctx())
        self.assertIn("::figure nope", r.html)

    def test_toc_ids(self):
        r = render_page("## 7.1 The basic algorithm\n\n## الخوارزمية\n\n### Details\n", ctx())
        self.assertIn('<h2 id="7-1-the-basic-algorithm">', r.html)
        self.assertIn('<h2 id="s-2">الخوارزمية</h2>', r.html)
        self.assertEqual(r.toc, [(2, "7-1-the-basic-algorithm", "7.1 The basic algorithm"),
                                 (2, "s-2", "الخوارزمية"), (3, "details", "Details")])

    def test_toc_text_is_plain(self):
        r = render_page("## P&H and B&D\n", ctx())
        self.assertEqual(r.toc, [(2, "p-h-and-b-d", "P&H and B&D")])

    def test_duplicate_heading_ids(self):
        r = render_page("## Intro\n\n## Intro\n", ctx())
        self.assertIn('id="intro"', r.html)
        self.assertIn('id="intro-2"', r.html)

    def test_arabic_text_untouched(self):
        text = "الـ pivot — بيقف هنا، وده «مهم»."
        r = render_page(text + "\n", ctx("ar"))
        self.assertIn(f"<p>{text}</p>", r.html)

    def test_tables(self):
        r = render_page("| a | b |\n|---|---|\n| 1 | 2 |\n", ctx())
        self.assertIn("<table>", r.html)

    def test_raw_html_passes(self):
        r = render_page('<span class="x">hi</span>\n', ctx())
        self.assertIn('<span class="x">hi</span>', r.html)


if __name__ == "__main__":
    unittest.main()
