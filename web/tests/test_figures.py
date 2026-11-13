import math
import re
import unittest

from web.figures import all_figures
from web.lib.render import NODE_R, label_width
from web.lib.trace import frames_json
from web.traces import all_traces

TAG = re.compile(r"<(circle|rect|text)\b([^>]*)>(?:([^<]*)</text>)?")
ATTR = re.compile(r'([\w-]+)="([^"]*)"')


def svgs():
    for name, f in all_figures().items():
        for s in re.findall(r"<svg[\s\S]*?</svg>", f.html):
            yield name, s
    for name, t in all_traces().items():
        for preset, frames in frames_json(t)["presets"].items():
            for k, fr in enumerate(frames):
                yield f"{name}/{preset}/{k}", fr["svg"]


def shapes_with_values(s):
    tags = [(m.group(1), dict(ATTR.findall(m.group(2))), m.group(3)) for m in TAG.finditer(s)]
    for (kind, a, _), (nxt, b, label) in zip(tags, tags[1:]):
        if kind in ("circle", "rect") and nxt == "text" and "t-val" in b.get("class", "").split():
            yield kind, a, b, label


class FigureScaleTest(unittest.TestCase):
    def test_labels_fit(self):
        for name, s in svgs():
            for kind, a, b, label in shapes_with_values(s):
                width = 2 * float(a["r"]) if kind == "circle" else float(a["width"])
                size = float(b.get("font-size", 13))
                self.assertLessEqual(label_width(label, size), width - 8, f"{name}: {label!r} in {kind} {width}")

    def test_one_node_size(self):
        for name, s in svgs():
            for m in re.finditer(r"<circle\b([^>]*)>", s):
                a = dict(ATTR.findall(m.group(1)))
                want = 5 if a.get("class") == "dot" else 15
                self.assertEqual(float(a["r"]), want, f"{name}: {a}")

    def test_value_box_heights(self):
        for name, s in svgs():
            for kind, a, _, label in shapes_with_values(s):
                if kind == "rect":
                    self.assertIn(float(a["height"]), (34, 26, 30), f"{name}: {label!r} {a}")

    def test_no_hatch(self):
        for name, f in all_figures().items():
            self.assertNotIn("<pattern", f.html, name)
            self.assertNotIn("hatch", f.html, name)

    def test_null_links_visible(self):
        for name, s in svgs():
            for m in re.finditer(r"<line\b([^>]*)>", s):
                a = dict(ATTR.findall(m.group(1)))
                if "e-null" in a.get("class", "").split():
                    length = math.hypot(float(a["x2"]) - float(a["x1"]), float(a["y2"]) - float(a["y1"]))
                    self.assertGreaterEqual(length - NODE_R, 20, f"{name}: {a}")

    def test_min_text_size(self):
        for name, s in svgs():
            for size in re.findall(r'<text\b[^>]*font-size="([\d.]+)"', s):
                self.assertGreaterEqual(float(size), 11, name)

    def test_labels_clear_of_edges(self):
        for name, s in svgs():
            edges = [dict(ATTR.findall(m.group(1))) for m in re.finditer(r"<line\b([^>]*)>", s)]
            edges = [e for e in edges if set(e.get("class", "").split()) & {"e", "e-active", "e-strong", "a"}]
            for m in re.finditer(r"<text\b([^>]*)>([^<]*)</text>", s):
                a = dict(ATTR.findall(m.group(1)))
                if "t-val" in a.get("class", "").split():
                    continue
                size = float(a.get("font-size", 13))
                w = label_width(m.group(2), size)
                x, y = float(a["x"]), float(a["y"])
                anchor = a.get("text-anchor", "start")
                x0 = x - w / 2 if anchor == "middle" else x - w if anchor == "end" else x
                box = (x0 + 1, y - size * 0.7 + 1, x0 + w - 1, y + size * 0.15 - 1)
                for e in edges:
                    x1, y1, x2, y2 = (float(e[k]) for k in ("x1", "y1", "x2", "y2"))
                    hit = any(box[0] < x1 + (x2 - x1) * t / 40 < box[2] and box[1] < y1 + (y2 - y1) * t / 40 < box[3]
                              for t in range(41))
                    self.assertFalse(hit, f"{name}: {m.group(2)!r} on an edge")

    def test_pointer_labels_apart(self):
        for name, s in svgs():
            boxes = []
            for m in re.finditer(r'<text\b([^>]*class="t-ptr"[^>]*)>([^<▲]+)</text>', s):
                a = dict(ATTR.findall(m.group(1)))
                w = label_width(m.group(2), float(a.get("font-size", 13)))
                x, y = float(a["x"]), float(a["y"])
                for bx0, bx1, by in boxes:
                    self.assertFalse(abs(by - y) < 13 and x - w / 2 < bx1 + 3 and bx0 < x + w / 2 + 3,
                                     f"{name}: {m.group(2)!r} touches another pointer")
                boxes.append((x - w / 2, x + w / 2, y))

    def test_fit_style(self):
        for name, s in svgs():
            head = s[:s.index(">")]
            if 'class="fsvg' in head:
                self.assertRegex(head, r'width="(\d+(?:\.\d+)?)"[^>]*style="min-width:\d+px"', name)


if __name__ == "__main__":
    unittest.main()
