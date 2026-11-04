import re
import tomllib
import unittest

from web.figures import Figure, all_figures
from web.lib.csource import ROOT
from web.lib.figlang import arabic_strings, load_figure_text, localize

DIRECTIVE = re.compile(r"^(::[a-z]+.*|:::[a-z]+|:::)\s*$", re.M)
CONTAINER_OPEN = re.compile(r"^(:::[a-z]+)(?: .*)?$", re.M)


def twins():
    for en in sorted((ROOT / "content" / "en").glob("*.md")):
        ar = ROOT / "content" / "ar" / en.name
        yield en.stem, ar.read_text(encoding="utf-8"), en.read_text(encoding="utf-8")


def skeleton(md):
    md = re.sub(r"```[\s\S]*?```", "```", md)
    lines = []
    for line in md.split("\n"):
        s = line.strip()
        if s.startswith(":::") and s != ":::":
            lines.append(CONTAINER_OPEN.match(s).group(1))
        elif s == ":::" or s.startswith("::") or s == "```":
            lines.append(s)
    return lines


class LocalizeTest(unittest.TestCase):
    def test_localize_caption_and_label(self):
        html = '<svg><text class="t-lab">قبل</text></svg><figcaption>شرح <code>x</code> هنا</figcaption>'
        mapping = {"قبل": "before", "شرح <code>x</code> هنا": "explains <code>x</code> here"}
        out = localize(html, mapping)
        self.assertIn(">before<", out)
        self.assertIn("<figcaption>explains <code>x</code> here</figcaption>", out)
        self.assertEqual(arabic_strings(out), [])

    def test_arabic_strings_found(self):
        html = '<svg><text>A</text><text>قبل</text></svg><figcaption>كلام</figcaption>'
        self.assertEqual(arabic_strings(html), ["كلام", "قبل"])

    def test_unmapped_left_alone(self):
        self.assertIn("قبل", localize("<svg><text>قبل</text></svg>", {}))


class EnglishContentTest(unittest.TestCase):
    def test_twins_share_structure(self):
        for slug, ar, en in twins():
            with self.subTest(page=slug):
                self.assertEqual(skeleton(ar), skeleton(en))
                self.assertEqual(len(re.findall(r"^#{2,3} ", ar, re.M)), len(re.findall(r"^#{2,3} ", en, re.M)))
                self.assertEqual(re.findall(r"^\d+\. \[([ x])\]", ar, re.M), re.findall(r"^\d+\. \[([ x])\]", en, re.M))

    def test_code_blocks_identical(self):
        for slug, ar, en in twins():
            with self.subTest(page=slug):
                self.assertEqual(re.findall(r"^::(?!:).*$", ar, re.M), re.findall(r"^::(?!:).*$", en, re.M))
                self.assertEqual(re.findall(r"```[\s\S]*?```", ar), re.findall(r"```[\s\S]*?```", en))

    def test_no_arabic_prose_left(self):
        for slug, ar, en in twins():
            prose = re.sub(r"```[\s\S]*?```", "", en)
            with self.subTest(page=slug):
                self.assertIsNone(re.search(r"[؀-ۿ]", prose), slug)

    def test_figures_on_english_pages_are_translated(self):
        mapping = load_figure_text(ROOT / "content" / "en" / "figures.toml")
        figs = all_figures()
        for slug, ar, en in twins():
            for name in re.findall(r"^::figure ([\w-]+)", en, re.M):
                with self.subTest(page=slug, figure=name):
                    self.assertEqual(arabic_strings(localize(figs[name].html, mapping)), [])

    def test_figure_text_keys_exist(self):
        mapping = load_figure_text(ROOT / "content" / "en" / "figures.toml")
        known = {s for f in all_figures().values() for s in arabic_strings(f.html)}
        self.assertEqual(set(mapping) - known, set())


if __name__ == "__main__":
    unittest.main()


class StalePathTest(unittest.TestCase):
    def test_no_lesson_points_at_the_old_site_folder(self):
        for lang in ("ar", "en"):
            for p in sorted((ROOT / "content" / lang).glob("*.md")):
                with self.subTest(page=f"{lang}/{p.name}"):
                    self.assertNotRegex(p.read_text(encoding="utf-8"), r"(?m)^docs/|\bdocs/lessons|make test\b")
