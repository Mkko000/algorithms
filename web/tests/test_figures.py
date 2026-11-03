import re
import unittest
from pathlib import Path

from web.figures import all_figures
from web.lib.csource import ROOT, run_output
from web.lib.render import VOCABULARY

CHECKS = {
    "sel-trace": ("src/06-sorting/selection_sort.c", ""),
    "ins-trace": ("src/06-sorting/insertion_sort.c", ""),
    "bub-trace": ("src/06-sorting/bubble_sort.c", ""),
    "shell-trace": ("src/06-sorting/shellsort.c", ""),
    "merge-td": ("src/08-mergesort/mergesort_bu.c", "sorted:"),
    "merge-bu": ("src/08-mergesort/mergesort_bu.c", "sorted:"),
    "ex6-sel": ("src/06-sorting/exercises/ex6_11_elementary.c", "9 "),
    "ex6-ins": ("src/06-sorting/exercises/ex6_11_elementary.c", "9 "),
    "ex6-bub": ("src/06-sorting/exercises/ex6_11_elementary.c", "9 "),
    "ex6-index": ("src/06-sorting/exercises/ex6_11_elementary.c", "9 "),
    "ex6-kic": ("src/06-sorting/exercises/ex6_57_index_kic.c", "result"),
    "ex8-td": ("src/08-mergesort/exercises/ex8_06_merges.c", "6-by-6"),
    "ex8-bu": ("src/08-mergesort/exercises/ex8_06_merges.c", "6-by-6"),
    "linear-probe": ("src/14-hashing/hash_linear_probing.c", "G  X"),
    "double-hash": ("src/14-hashing/hash_linear_probing.c", "M *R"),
    "ex13-234": ("src/13-balanced-trees/exercises/ex13_39_234_rb.c", "+N "),
    "ex16-3456": ("src/16-external-search/exercises/ex16_05_btree.c", "level 2:"),
    "ex16-btree": ("src/16-external-search/exercises/ex16_01_isam.c", "127 "),
}


def final_row(html):
    last = re.findall(r"<svg[\s\S]*?</svg>", html)[-1]
    rows = {}
    for m in re.finditer(r'<text x="([\d.]+)" y="([\d.]+)" class="t-val"[^>]*>([^<]*)<', last):
        rows.setdefault(float(m.group(2)), []).append((float(m.group(1)), m.group(3)))
    return [v for _, v in sorted(rows[max(rows)])]


CONTAINERS = {"frame", "fig", "steps", "fside", "fside-item", "comp-table"}


def old_marker_names():
    names = set()
    lessons = ROOT / "docs" / "lessons"
    content = ROOT / "content" / "ar"
    if lessons.exists():
        for p in lessons.glob("*.html"):
            names |= set(re.findall(r"<!--fig:([\w-]+)-->", p.read_text(encoding="utf-8")))
    else:
        for p in content.glob("*.md"):
            names |= set(re.findall(r"^::figure ([\w-]+)", p.read_text(encoding="utf-8"), re.M))
    return names


class FiguresTest(unittest.TestCase):
    def test_all_figures_load(self):
        self.assertGreaterEqual(len(all_figures()), 149)

    def test_every_marker_has_a_figure(self):
        missing = old_marker_names() - set(all_figures())
        self.assertEqual(missing, set())

    def test_kinds(self):
        self.assertEqual({f.kind for f in all_figures().values()}, {"fig", "steps"})

    def test_vocabulary_only(self):
        for f in all_figures().values():
            for svg in re.findall(r"<svg[\s\S]*?</svg>", f.html):
                toks = {t for c in re.findall(r'class="([^"]*)"', svg) for t in c.split()}
                self.assertLessEqual(toks, VOCABULARY, f.name)
            outside = re.sub(r"<svg[\s\S]*?</svg>", "", f.html)
            toks = {t for c in re.findall(r'class="([^"]*)"', outside) for t in c.split()}
            self.assertLessEqual(toks, CONTAINERS, f.name)

    def test_no_inline_colors(self):
        for f in all_figures().values():
            self.assertIsNone(re.search(r'(fill|stroke|color|background)\s*[:=]\s*"?#', f.html), f.name)

    def test_final_rows_match_programs(self):
        figs = all_figures()
        for name, (program, prefix) in CHECKS.items():
            with self.subTest(figure=name):
                row = final_row(figs[name].html)
                self.assertGreaterEqual(len(row), 4)
                lines = [l.strip() for l in run_output(program).splitlines() if l.strip().startswith(prefix)]
                self.assertTrue(lines, f"{program}: no line starting with {prefix!r}")
                printed = "".join(re.findall(r"[A-Za-z0-9]+", lines[0]))
                self.assertTrue(printed.endswith("".join(row)), f"{name}: {row} vs {lines[0]}")


if __name__ == "__main__":
    unittest.main()


class CaptionColorTest(unittest.TestCase):
    def test_no_stale_color_names(self):
        stale = re.compile("الأزرق|البنفسجي|البرتقالي|الأخضر|أزرق|بنفسجي|برتقالي|أخضر")
        for f in all_figures().values():
            self.assertIsNone(stale.search(f.html), f.name)
