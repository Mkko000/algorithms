import tempfile
import unittest
from pathlib import Path

from web.check import check_house_rules
from web.lib.csource import ROOT
from web.lib.manifest import load_course
from web.lib.readme import chapters, counts, render_readme

TOML = '''
[[part]]
key = "p"
title = { ar = "جزء", en = "Part" }

[[part.lesson]]
slug = "union-find"
kind = "lesson"
chapter = 1
minutes = 10
title = { ar = "التوصيل", en = "Union find" }

[[part.lesson]]
slug = "ex-ch1"
kind = "exercises"
chapter = 1
minutes = 10
title = { ar = "تمارين", en = "Exercises 1" }

[[part.lesson]]
slug = "analysis"
kind = "lesson"
chapter = 2
minutes = 10
title = { ar = "التحليل", en = "Analysis" }
'''


class ReadmeTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        p = Path(self.tmp.name) / "course.toml"
        p.write_text(TOML, encoding="utf-8")
        self.course = load_course(p)

    def tearDown(self):
        self.tmp.cleanup()

    def test_chapters_from_first_lesson(self):
        ch = chapters(self.course, "en")
        self.assertEqual([c[0] for c in ch], [1, 2])
        self.assertEqual(ch[0][1], "Union find")
        self.assertEqual(ch[0][2], "src/01-fundamentals/")
        self.assertEqual(ch[0][3], "union-find")

    def test_render_fills_everything(self):
        tpl = "{{program_count}} programs, {{lesson_count}} lessons, {{exercise_page_count}} exercise pages.\n\n{{chapter_map}}\n\nSite: {{site_url}}\n"
        out = render_readme(tpl, self.course, "en", {"programs": 191})
        self.assertIn("191 programs, 2 lessons, 1 exercise pages.", out)
        self.assertIn("1. [Union find](https://mkko000.github.io/algorithms/en/union-find/)", out)
        self.assertIn("`src/02-analysis/`", out)
        self.assertNotIn("{{", out)
        self.assertEqual(check_house_rules([("README", out)]), [])

    def test_unknown_placeholder_raises(self):
        with self.assertRaises(ValueError):
            render_readme("{{nope}}", self.course, "en", {"programs": 1})

    def test_counts_match_repo(self):
        self.assertEqual(counts()["programs"], 191)

    def test_committed_readmes_are_current(self):
        course = load_course(ROOT / "content" / "course.toml")
        for lang, name in (("en", "README.md"), ("ar", "README.ar.md")):
            tpl = (ROOT / "web" / "readme" / f"{name}.in").read_text(encoding="utf-8")
            self.assertEqual((ROOT / name).read_text(encoding="utf-8"), render_readme(tpl, course, lang, counts()), name)


if __name__ == "__main__":
    unittest.main()
