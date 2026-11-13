import re
import tempfile
import unittest
from pathlib import Path

from web.figures import Figure
from web.lib.csource import ROOT
from web.lib.content import RenderContext, render_page
from web.lib.manifest import load_course
from web.lib.pages import (Site, fill, load_ui, render_code_index, render_code_page, render_home,
                           render_lesson, render_missing, render_root, resolve_sources, url)

def flat(html):
    return re.sub(r">\s+<", "><", re.sub(r"\s+", " ", html))


TOML = '''
[[part]]
key = "p1"
title = { ar = "جزء", en = "Part one" }

[[part.lesson]]
slug = "a"
kind = "lesson"
chapter = 1
minutes = 10
title = { ar = "أ", en = "A" }

[[part.lesson]]
slug = "ex-ch1"
kind = "exercises"
chapter = 1
minutes = 20
title = { ar = "ب", en = "B" }
'''


class PagesTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        p = Path(cls.tmp.name) / "course.toml"
        p.write_text(TOML, encoding="utf-8")
        cls.site = Site(course=load_course(p), base="/algorithms/", ui=load_ui(Path("content/ui.toml")),
                        arabic_only={"ex-ch1"})

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def rendered(self, lang, text="Hello\n"):
        ctx = RenderContext(lang=lang, figures={"f1": Figure("f1", "fig", "<svg></svg>")},
                            traces={"t1": lambda: '<section class="trace" dir="ltr" data-trace="t1"></section>'},
                            ui=self.site.ui[lang], source_path="x.md")
        return render_page(text, ctx)

    def lesson(self, lang, text="Hello\n", slug="a"):
        return render_lesson(self.site, lang, self.site.course.by_slug[slug], self.rendered(lang, text))

    def test_url(self):
        self.assertEqual(url(self.site, "en", "a"), "/algorithms/en/a/")
        self.assertEqual(url(self.site, "ar"), "/algorithms/ar/")

    def test_theme_script_first(self):
        h = self.lesson("en")
        script = re.search(r"<script>(.*?)</script>", h, re.S)
        self.assertLess(h.index("<script>"), h.index('<link rel="stylesheet"'))
        self.assertLessEqual(len(script.group(1).encode()), 300)
        self.assertIn("algo-theme", script.group(1))

    def test_twin_links(self):
        ar = self.lesson("ar")
        self.assertIn('href="/algorithms/en/a/" hreflang="en"', flat(ar))
        self.assertIn('<html lang="ar" dir="rtl">', ar)
        en = self.lesson("en")
        self.assertIn('href="/algorithms/ar/a/" hreflang="ar"', flat(en))
        self.assertIn('<link rel="alternate" hreflang="ar" href="/algorithms/ar/a/">', en)

    def test_missing_twin(self):
        h = render_missing(self.site, self.site.course.by_slug["ex-ch1"])
        self.assertIn('href="/algorithms/ar/ex-ch1/"', h)
        self.assertIn("This lesson is not in English yet.", h)
        self.assertIn('<html lang="en" dir="ltr">', h)

    def test_fill_rejects_leftovers(self):
        with self.assertRaises(ValueError):
            fill("{{a}} {{b}}", a="x")

    def test_rtl_blocks_stay_ltr(self):
        text = ('::code src="src/07-quicksort/quicksort.c" fn="partition"\n\n'
                '::output src="src/07-quicksort/quicksort.c" lines="1"\n\n::figure f1\n\n::trace t1\n')
        h = self.lesson("ar", text)
        self.assertIn('dir="rtl"', h)
        for cls in ('figure class="code"', 'figure class="output"', 'div class="figure"', 'section class="trace"'):
            self.assertRegex(h, cls + r'[^>]*dir="ltr"')

    def test_code_caption_links(self):
        html = '<figure class="code" dir="ltr" data-src="src/07-quicksort/quicksort.c"><figcaption>src/07-quicksort/quicksort.c</figcaption>'
        out = resolve_sources(html, self.site, "ar")
        self.assertIn('<a href="/algorithms/ar/code/07-quicksort/quicksort/">src/07-quicksort/quicksort.c</a>', out)

    def test_internal_link_schemes(self):
        out = resolve_sources('<a href="lesson:a">x</a> <a href="src:src/07-quicksort/quicksort.c">y</a>', self.site, "en")
        self.assertIn('<a href="/algorithms/en/a/">x</a>', out)
        self.assertIn('<a href="/algorithms/en/code/07-quicksort/quicksort/">y</a>', out)

    def test_header_link_goes_to_github(self):
        out = resolve_sources('<a href="src:src/common_item.h">h</a>', self.site, "en")
        self.assertIn('href="https://github.com/Mkko000/algorithms/blob/main/src/common_item.h"', out)

    def test_unknown_lesson_link_raises(self):
        with self.assertRaises(ValueError):
            resolve_sources('<a href="lesson:nope">x</a>', self.site, "en")

    def test_arabic_comma(self):
        h = self.lesson("ar")
        self.assertIn("درس 1 من 2، حوالي 10 دقيقة", h)

    def test_pager_and_done(self):
        h = self.lesson("en")
        self.assertIn('href="/algorithms/en/ex-ch1/"', h)
        self.assertIn('data-slug="a"', h)
        self.assertIn("Lesson 1 of 2", h)

    def test_home_contents(self):
        h = render_home(self.site, "en")
        self.assertIn("Part one", h)
        self.assertIn('href="/algorithms/en/a/"', h)
        self.assertIn('class="row ex', h)

    def test_english_home_marks_arabic_only(self):
        en = render_home(self.site, "en")
        self.assertIn('class="row ex todo"', en)
        self.assertIn("Arabic only for now", en)
        self.assertNotIn("todo", render_home(self.site, "ar"))

    def test_root_defaults_to_arabic(self):
        h = render_root(self.site)
        self.assertIn('"ar"', h)
        self.assertNotIn("navigator.language", h)
        self.assertLess(h.index('hreflang="ar"'), h.index('hreflang="en"'))

    def test_footer_credits_maintainer(self):
        names = {"ar": "سعيد", "en": "Saeed"}
        authors = {"ar": "محمد السيسي", "en": "Mohammed El Sisi"}
        for lang in ("ar", "en"):
            h = render_home(self.site, lang)
            self.assertIn(f'<a href="https://github.com/saeeedhany">{names[lang]}</a>', h)
            self.assertIn(f'<a href="https://github.com/Mkko000">{authors[lang]}</a>', h)
            self.assertNotIn(">github.com/saeeedhany<", h)

    def test_menu_button_and_panel(self):
        h = self.lesson("en", "## One\n\n## Two\n\n## Three\n")
        self.assertIn('class="menu-btn"', h)
        self.assertIn('aria-controls="menu"', h)
        self.assertIn('id="menu"', h)
        panel = h[h.index('id="menu"'):]
        self.assertIn('href="#one"', panel)

    def test_arabic_pages_isolate_english_runs(self):
        h = self.lesson("ar", "الـ loop الداخلي\n")
        self.assertIn('<span class="lt" dir="ltr">loop</span>', h)
        self.assertNotIn('class="lt"', self.lesson("en", "the loop\n"))

    def test_code_page(self):
        h = render_code_page(self.site, "en", "src/07-quicksort/quicksort.c", [self.site.course.by_slug["a"]])
        self.assertIn("https://github.com/Mkko000/algorithms/blob/main/src/07-quicksort/quicksort.c", h)
        self.assertIn('<figure class="output"', h)
        self.assertIn('href="/algorithms/en/a/"', h)

    def test_code_index(self):
        h = render_code_index(self.site, "en", ["src/07-quicksort/quicksort.c", "src/07-quicksort/select.c"])
        self.assertIn('href="/algorithms/en/code/07-quicksort/select/"', h)

    def test_no_middle_dot_in_chrome(self):
        self.assertNotIn(" · ", render_home(self.site, "en"))
        self.assertNotIn(" · ", self.lesson("en"))

    def test_book_sections_move_to_header(self):
        text = ("Book sections: 6.1 Rules of the Game (Program 6.1), 6.2 Selection Sort (Programs 6.2 and 6.3)\n\n"
                "Intro paragraph.\n")
        h = self.lesson("en", text)
        head = h[h.index('<header class="lesson-head">'):h.index('<div class="lesson-body">')]
        self.assertIn('<details class="book-sections">', head)
        self.assertIn('<span class="bs-nums">6.1, 6.2</span>', head)
        self.assertIn('<span class="sec-n">6.2</span><span class="sec-t">Selection Sort <small>Programs 6.2 and 6.3</small>', head)
        self.assertNotIn("Book sections:", h)
        self.assertIn('<div class="lesson-body"><div class="mrow"><div class="main"><p>Intro paragraph.</p>', flat(h))

    def test_book_ref_beside_heading(self):
        text = ("Book sections: 6.1 Rules (Program 6.1), 6.2 Selection Sort (Program 6.2)\n\n"
                "Intro.\n\n## 6.2 Selection Sort\n\nBody.\n")
        h = self.lesson("en", text)
        note = ('<aside class="side"><div class="note book"><p class="n-label">From the book</p>'
                '<p><b>6.2</b> Selection Sort</p><p class="n-sub">Program 6.2</p></div></aside>')
        self.assertIn(note, h)
        head = h[h.index('<header class="lesson-head">'):h.index('<div class="lesson-body">')]
        self.assertIn('<span class="bs-nums">6.1</span>', head)
        self.assertNotIn('<span class="sec-n">6.2</span>', head)

    def test_book_ref_arabic_heading_with_latin(self):
        text = "أقسام الكتاب: 6.2 Selection Sort (Program 6.2)\n\nمقدمة\n\n## 6.2 Selection Sort\n\nنص.\n"
        h = self.lesson("ar", text)
        self.assertIn('<div class="note book"><p class="n-label">من الكتاب</p><p><b>6.2</b>', h)
        self.assertNotIn('<details class="book-sections">', h)

    def test_property_heading_not_matched(self):
        text = "Book sections: 6.6 Stability\n\nIntro.\n\n## Property 6.6: stability\n\nBody.\n"
        h = self.lesson("en", text)
        self.assertNotIn('class="note book"', h)
        self.assertIn('<span class="sec-n">6.6</span>', h)

    def test_lesson_body_in_rows(self):
        h = self.lesson("en", ":::goal Goal\nLearn.\n:::\n\nIntro.\n\n## One\n\nText.\n")
        self.assertIn('<div class="mrow"><div class="main"><p>Intro.</p>', h)
        self.assertIn('<aside class="callout goal note">', h)
        self.assertIn('<a class="h-anchor" href="#one"', h)

    def test_book_sections_arabic(self):
        h = self.lesson("ar", "أقسام الكتاب: 1.1 Algorithms · 1.2 Connectivity (Program 1.1)\n\nمقدمة\n")
        self.assertIn('<span class="bs-nums">1.1، 1.2</span>', h)
        self.assertNotIn("<p>أقسام الكتاب", h)

    def test_lesson_without_book_sections(self):
        self.assertNotIn("book-sections", self.lesson("en", "Just text.\n"))

    def test_source_link_lists(self):
        h = self.lesson("en", "Text.\n\n* [src/07-quicksort/quicksort.c](src:src/07-quicksort/quicksort.c)\n")
        self.assertIn('<ul class="src-links">', h)
        self.assertNotIn('class="src-links"', self.lesson("en", "* one\n* two\n"))

    def test_nav_marks_current_page(self):
        home = render_home(self.site, "en")
        self.assertEqual(flat(home).count('href="/algorithms/en/" aria-current="page"'), 2)
        code = render_code_index(self.site, "en", ["src/07-quicksort/quicksort.c"])
        self.assertIn('href="/algorithms/en/code/" aria-current="page"', flat(code))
        self.assertNotIn("aria-current", self.lesson("en"))

    def test_home_stats(self):
        h = render_home(self.site, "en", {"lessons": 70, "programs": 191, "traces": 13})
        self.assertIn('<dl class="stats">', h)
        self.assertIn("<dt>191</dt><dd>C programs</dd>", h)
        self.assertNotIn('class="stats"', render_home(self.site, "en"))

    def test_code_index_groups_by_chapter(self):
        paths = ["src/07-quicksort/quicksort.c", "src/07-quicksort/exercises/ex7_01_quick_basics.c"]
        lesson = self.site.course.by_slug["a"]
        h = render_code_index(self.site, "en", paths, {"src/07-quicksort/quicksort.c": [lesson]})
        self.assertIn('<h2>Chapter 7<small dir="ltr">07-quicksort</small></h2>', h)
        self.assertIn('<code dir="ltr">quicksort.c</code><span class="lead" aria-hidden="true"></span><span class="uses">A</span>', h)
        self.assertIn('<p class="prog-sub">Exercises</p>', h)
        self.assertLess(h.index("quicksort.c"), h.index("ex7_01_quick_basics.c"))

    def test_theme_button_has_icons_and_label(self):
        h = render_home(self.site, "en")
        self.assertEqual(h.count('class="theme-toggle"'), 2)
        self.assertIn('aria-label="Theme: auto"', h)
        self.assertIn('<span class="theme-text">Theme: auto</span>', h)

    def test_missing_page_links_home(self):
        h = render_missing(self.site, self.site.course.by_slug["ex-ch1"])
        self.assertIn('class="button" href="/algorithms/ar/ex-ch1/"', h)
        self.assertIn('<a href="/algorithms/en/">Go to the contents</a>', h)

    def test_heading_anchor_links(self):
        h = self.lesson("en", "## One\n\n### Two\n\n## Three\n")
        self.assertIn('<h2 id="one">One<a class="h-anchor" href="#one" aria-label="Link to this section">#</a></h2>', h)
        self.assertIn('href="#two" aria-label="Link to this section">#</a></h3>', h)
        self.assertIn('<li class="l2"><a href="#one">One</a></li>', h)

    def test_code_index_filter_box(self):
        h = render_code_index(self.site, "ar", ["src/07-quicksort/quicksort.c"])
        self.assertIn('<div class="prog-filter" hidden><input type="search" placeholder="دوّر باسم الملف أو الدرس"', flat(h))
        self.assertIn('<p class="prog-empty" hidden>', h)

    def test_code_page_lists_lessons_with_numbers(self):
        h = render_code_page(self.site, "en", "src/07-quicksort/quicksort.c", [self.site.course.by_slug["a"]])
        self.assertIn('<span class="num">1</span><span class="t">A</span>', h)
        self.assertIn('<h1><span dir="ltr">quicksort.c</span></h1>', h)
        self.assertIn('class="button ghost"', h)

    def test_script_strings_present(self):
        h = render_home(self.site, "ar")
        for key in ("quiz_title", "quiz_score", "quiz_retry", "copy", "copied", "no_results"):
            self.assertIn(f'"{key}":', h)

    def test_no_sketch_script(self):
        for lang in ("ar", "en"):
            self.assertNotIn("sketch.js", render_home(self.site, lang))
        app = (ROOT / "web" / "assets" / "js" / "app.js").read_text(encoding="utf-8")
        self.assertNotIn("Sketch.", app)
        self.assertFalse((ROOT / "web" / "assets" / "js" / "sketch.js").exists())

    def test_ui_keys_match(self):
        self.assertEqual(set(self.site.ui["ar"]), set(self.site.ui["en"]))


if __name__ == "__main__":
    unittest.main()
