import re
import tempfile
import unittest
from pathlib import Path

from web.figures import Figure
from web.lib.content import RenderContext, render_page
from web.lib.manifest import load_course
from web.lib.pages import (Site, fill, load_ui, render_code_index, render_code_page, render_home,
                           render_lesson, render_missing, render_root, resolve_sources, url)

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
        self.assertIn('href="/algorithms/en/a/" hreflang="en"', ar)
        self.assertIn('<html lang="ar" dir="rtl">', ar)
        en = self.lesson("en")
        self.assertIn('href="/algorithms/ar/a/" hreflang="ar"', en)
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

    def test_ui_keys_match(self):
        self.assertEqual(set(self.site.ui["ar"]), set(self.site.ui["en"]))


if __name__ == "__main__":
    unittest.main()
