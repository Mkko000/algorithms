import tempfile
import unittest
from pathlib import Path

from web.lib.manifest import load_arabic_only, load_course, validate_course

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

[[part]]
key = "p2"
title = { ar = "جزء ٢", en = "Part two" }

[[part.lesson]]
slug = "b"
kind = "lesson"
chapter = 2
minutes = 20
title = { ar = "ب", en = "B" }

[[part.lesson]]
slug = "c"
kind = "exercises"
chapter = 2
minutes = 30
title = { ar = "ج", en = "C" }
'''


class ManifestTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        (self.root / "ar").mkdir()
        (self.root / "en").mkdir()

    def tearDown(self):
        self.tmp.cleanup()

    def write_course(self, text=TOML):
        p = self.root / "course.toml"
        p.write_text(text, encoding="utf-8")
        return load_course(p)

    def touch(self, *names):
        for n in names:
            (self.root / n).write_text("x", encoding="utf-8")

    def test_loads_parts_and_flat_order(self):
        course = self.write_course()
        self.assertEqual([l.slug for l in course.lessons], ["a", "b", "c"])
        self.assertEqual(course.by_slug["b"].part, "p2")
        self.assertEqual(course.by_slug["c"].kind, "exercises")
        self.assertEqual(course.by_slug["a"].title["en"], "A")
        self.assertEqual(len(course.parts), 2)

    def test_missing_english_is_error_unless_allowed(self):
        course = self.write_course()
        self.touch("ar/a.md", "ar/b.md", "en/b.md", "ar/c.md", "en/c.md")
        self.assertIn("a: missing en/a.md", validate_course(course, self.root, set()))
        self.assertEqual(validate_course(course, self.root, {"a"}), [])

    def test_missing_arabic_is_always_error(self):
        course = self.write_course()
        self.touch("en/a.md", "ar/b.md", "en/b.md", "ar/c.md", "en/c.md")
        self.assertIn("a: missing ar/a.md", validate_course(course, self.root, {"a"}))

    def test_stale_allowance_is_error(self):
        course = self.write_course()
        self.touch("ar/a.md", "en/a.md", "ar/b.md", "en/b.md", "ar/c.md", "en/c.md")
        self.assertIn("a: listed in arabic_only.txt but en/a.md exists",
                      validate_course(course, self.root, {"a"}))

    def test_duplicate_slug_is_error(self):
        course = self.write_course(TOML.replace('slug = "b"', 'slug = "a"'))
        self.touch("ar/a.md", "en/a.md", "ar/c.md", "en/c.md")
        self.assertIn("duplicate slug a", validate_course(course, self.root, set()))

    def test_arabic_only_file(self):
        p = self.root / "arabic_only.txt"
        p.write_text("a\n\nb\n", encoding="utf-8")
        self.assertEqual(load_arabic_only(p), {"a", "b"})


if __name__ == "__main__":
    unittest.main()
