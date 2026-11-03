import re
import unittest

from web.lib.csource import ROOT

CSS = ROOT / "web" / "assets" / "css"
FONTS = ROOT / "web" / "assets" / "fonts"

LIGHT = {"--bg": "#faf8f3", "--ink": "#1f1d1a", "--muted": "#6b655c", "--code-bg": "#f0ece2",
         "--rule": "#e2dccd", "--accent": "#b4441f", "--fill": "#1f1d1a14", "--hl": "#e9dfc6"}
DARK = {"--bg": "#1c1a17", "--ink": "#e9e4da", "--muted": "#9c958a", "--code-bg": "#24211d",
        "--rule": "#34302a", "--accent": "#e0754f", "--fill": "#e9e4da1f", "--hl": "#3a342b"}
PROPS = ["--bg", "--ink", "--muted", "--code-bg", "--rule", "--accent", "--fill", "--hl",
         "--measure", "--gutter", "--font-text", "--font-ar", "--font-mono"]


def block(css, selector_regex):
    m = re.search(selector_regex + r"\s*\{([^{}]*)\}", css)
    return m.group(1) if m else ""


def props(body):
    return dict(re.findall(r"(--[\w-]+)\s*:\s*([^;]+);", body))


def lum(hexv):
    h = hexv.lstrip("#")[:6]
    rgb = [int(h[i:i + 2], 16) / 255 for i in (0, 2, 4)]
    lin = [c / 12.92 if c <= 0.03928 else ((c + 0.055) / 1.055) ** 2.4 for c in rgb]
    return 0.2126 * lin[0] + 0.7152 * lin[1] + 0.0722 * lin[2]


def contrast(a, b):
    la, lb = sorted([lum(a), lum(b)], reverse=True)
    return (la + 0.05) / (lb + 0.05)


class TokensTest(unittest.TestCase):
    def setUp(self):
        self.tokens = (CSS / "tokens.css").read_text(encoding="utf-8")

    def test_tokens_both_themes(self):
        light = props(block(self.tokens, r":root"))
        for p in PROPS:
            self.assertIn(p, light, p)
        for k, v in LIGHT.items():
            self.assertEqual(light[k].strip().lower(), v, k)
        media = re.search(r"@media \(prefers-color-scheme: dark\)\s*\{\s*:root:not\(\[data-theme=\"light\"\]\)\s*\{([^{}]*)\}",
                          self.tokens)
        self.assertIsNotNone(media)
        forced = props(block(self.tokens, r':root\[data-theme="dark"\]'))
        for body in (props(media.group(1)), forced):
            for k, v in DARK.items():
                self.assertEqual(body[k].strip().lower(), v, k)

    def test_one_accent(self):
        allowed = set(LIGHT.values()) | set(DARK.values())
        for f in CSS.glob("*.css"):
            text = re.sub(r"@font-face\{[^}]*\}", "", f.read_text(encoding="utf-8"))
            for h in re.findall(r"#[0-9a-fA-F]{3,8}\b", text):
                self.assertIn(h.lower(), allowed, f"{f.name}: {h}")
            if f.name != "tokens.css":
                self.assertNotRegex(text, r"\brgba?\(", f.name)
                self.assertNotRegex(text, r":\s*(white|black|red|blue|green|gray|grey)\b", f.name)

    def test_fonts_present(self):
        urls = re.findall(r'url\("\.\./fonts/([^"]+)"\)', self.tokens)
        self.assertTrue(urls)
        for u in urls:
            self.assertTrue((FONTS / u).exists(), u)
        self.assertTrue((FONTS / "OFL.txt").exists())

    def test_contrast_aa(self):
        for theme in (LIGHT, DARK):
            bg, ink, muted, code, accent = theme["--bg"], theme["--ink"], theme["--muted"], theme["--code-bg"], theme["--accent"]
            self.assertGreaterEqual(contrast(ink, bg), 4.5)
            self.assertGreaterEqual(contrast(muted, bg), 4.5)
            self.assertGreaterEqual(contrast(ink, code), 4.5)
            self.assertGreaterEqual(contrast(muted, code), 4.5)
            self.assertGreaterEqual(contrast(accent, bg), 3.0)
            self.assertGreaterEqual(contrast(bg, accent), 3.0)

    def test_logical_properties(self):
        for name in ("base.css", "components.css"):
            text = (CSS / name).read_text(encoding="utf-8")
            self.assertNotRegex(text, r"(margin|padding|border)-(left|right)\s*:", name)
            self.assertNotRegex(text, r"(?<![\w-])(left|right)\s*:", name)
            self.assertNotRegex(text, r"text-align\s*:\s*(left|right)", name)
            self.assertNotRegex(text, r"float\s*:\s*(left|right)", name)

    def test_reduced_motion(self):
        text = (CSS / "base.css").read_text(encoding="utf-8")
        self.assertIn("prefers-reduced-motion: reduce", text)

    def test_no_comments(self):
        for f in CSS.glob("*.css"):
            self.assertNotIn("/*", f.read_text(encoding="utf-8"), f.name)


if __name__ == "__main__":
    unittest.main()
