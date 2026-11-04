import tempfile
import unittest
from pathlib import Path

from web.build import build, programs
from web.check import check_house_rules, check_links
from web.lib.csource import ROOT
from web.lib.manifest import LANGS, load_arabic_only, load_course, validate_course
from web.lib.pages import code_slug


class BuildTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        cls.out = Path(cls.tmp.name) / "site"
        cls.stats = build(cls.out, "/algorithms/")

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def test_full_build_under_base(self):
        self.assertEqual(check_links(self.out, "/algorithms/"), [])

    def test_every_page_exists(self):
        course = load_course(ROOT / "content" / "course.toml")
        for lang in LANGS:
            self.assertTrue((self.out / lang / "index.html").exists())
            for l in course.lessons:
                self.assertTrue((self.out / lang / l.slug / "index.html").exists(), f"{lang}/{l.slug}")
            for p in programs():
                self.assertTrue((self.out / lang / "code" / code_slug(p) / "index.html").exists(), p)
        self.assertTrue((self.out / "index.html").exists())
        self.assertTrue((self.out / "404.html").exists())
        self.assertEqual(len(programs()), 191)

    def test_trace_data_written(self):
        self.assertTrue((self.out / "assets" / "traces" / "partition.json").exists())

    def test_used_in_lists_lessons(self):
        page = (self.out / "en" / "code" / "07-quicksort" / "quicksort" / "index.html").read_text(encoding="utf-8")
        self.assertIn('href="/algorithms/en/quicksort/"', page)

    def test_refuses_foreign_directory(self):
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / "keep.txt").write_text("x")
            with self.assertRaises(SystemExit):
                build(Path(tmp), "/")

    def test_root_build_links(self):
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp) / "site"
            build(out, "/", only="quicksort")
            errors = [e for e in check_links(out, "/") if "broken link" not in e]
            self.assertEqual(errors, [])

    def test_real_manifest_is_valid(self):
        course = load_course(ROOT / "content" / "course.toml")
        self.assertEqual(validate_course(course, ROOT / "content", load_arabic_only(ROOT / "content" / "arabic_only.txt")), [])


class HouseRulesTest(unittest.TestCase):
    def test_house_rules_catch(self):
        errors = check_house_rules([("x.md", "a — b\n- item\nwell - known\n")])
        self.assertEqual(len(errors), 3)

    def test_code_is_exempt(self):
        self.assertEqual(check_house_rules([("x.md", "Use `a - b` here.\n\n```\nx - y\n```\n")]), [])


if __name__ == "__main__":
    unittest.main()
