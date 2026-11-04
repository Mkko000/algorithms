import re
import unittest

from web.lib.render import (array_rows, bars, build_bst, cell_rows, cells, graph, llist, map_classes, mtree, M,
                            svg, tree, VOCABULARY)

COLOR = re.compile(r'(fill|stroke)="#')


def classes(s):
    return {tok for c in re.findall(r'class="([^"]*)"', s) for tok in c.split()}


class RenderTest(unittest.TestCase):
    def samples(self):
        return [
            cells([3, 1, 8], {2: "s-look"}, {"i": 2}),
            bars([3, 1, 8], {0: "s-focus"}),
            array_rows([("", list("AB"), {0: "hi"})]),
            tree(build_bst("SEARCH")),
            llist(list("ABC")),
            graph({0: (20, 20), 1: (80, 20)}, [(0, 1)]),
            mtree(M(["A", "B"], [M(["0"]), M(["1"]), M(["2"])])),
        ]

    def test_no_inline_colors(self):
        for s in self.samples():
            self.assertIsNone(COLOR.search(s), s[:200])

    def test_only_vocabulary(self):
        for s in self.samples():
            self.assertLessEqual(classes(s), VOCABULARY, s[:200])

    def test_legacy_classes_mapped(self):
        s = array_rows([("", list("AB"), {0: "hi"})])
        self.assertIn('class="s-focus"', s)
        self.assertNotIn('class="hi"', s)

    def test_legacy_combinations(self):
        self.assertEqual(map_classes("node hi"), "s-focus")
        self.assertEqual(map_classes("cell hi2"), "s-look")
        self.assertEqual(map_classes("node rn"), "s-idle s-red")
        self.assertEqual(map_classes("edge red"), "e-strong")
        self.assertEqual(map_classes("edge hi"), "e-active")
        self.assertEqual(map_classes("edge null"), "e-null")
        self.assertEqual(map_classes("arr hi"), "a e-active")
        self.assertEqual(map_classes("tc"), "t-val")
        self.assertEqual(map_classes("elab"), "t-lab")
        self.assertEqual(map_classes("piv"), "s-focus")
        self.assertEqual(map_classes("ok"), "s-done")
        self.assertEqual(map_classes("dim"), "s-faded")
        self.assertEqual(map_classes("s-look"), "s-look")

    def test_unknown_class_raises(self):
        with self.assertRaises(KeyError):
            array_rows([("", ["A"], {0: "sparkly"})])

    def test_raw_strings_inside_svg_are_mapped(self):
        s = svg(10, 10, '<circle cx="1" cy="1" r="1" class="node hi"/>')
        self.assertIn('class="s-focus"', s)

    def test_pointer_row(self):
        s = cells([5, 6, 7], pointers={"i": 1, "j": 1})
        self.assertIn(">i<", s)
        self.assertIn(">j<", s)
        xs = re.findall(r'<text x="([\d.]+)"[^>]*class="t-ptr"[^>]*>[ij]<', s)
        self.assertEqual(len(xs), 2)
        self.assertEqual(xs[0], xs[1])

    def test_index_row(self):
        s = cells([5, 6, 7])
        self.assertEqual(len(re.findall(r'class="t-idx"', s)), 3)

    def test_cell_rows(self):
        s = cell_rows([("a", [1, 2, 3], {0: "s-focus"}, {"k": 0}), ("aux", ["·", 5, 6], {}, {"i": 1, "j": 2})], index_base=0)
        self.assertEqual(len(re.findall(r'class="t-val"', s)), 6)
        self.assertIn(">aux<", s)
        self.assertIn(">k<", s)
        self.assertIn(">j<", s)
        ys = sorted({float(y) for y in re.findall(r'<rect x="[\d.]+" y="([\d.]+)"', s)})
        self.assertEqual(len(ys), 2)
        self.assertLessEqual(classes(s), VOCABULARY)

    def test_cells_index_base(self):
        s = cells([7, 8], index_base=1)
        self.assertEqual(re.findall(r'class="t-idx"[^>]*>(\d+)<', s), ["1", "2"])

    def test_ltr(self):
        for s in self.samples():
            self.assertRegex(s, r'^<svg [^>]*direction="ltr"')


if __name__ == "__main__":
    unittest.main()
