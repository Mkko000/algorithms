import re
import unittest

from web.lib.csource import ROOT

CSS = ROOT / "web" / "assets" / "css"
FONTS = ROOT / "web" / "assets" / "fonts"

LIGHT = {"--bg": "#fdfcf9", "--ink": "#1c1b19", "--accent": "#b4441f", "--mark": "#fde68a",
         "--muted": "#686865", "--frame": "#989794", "--hatch": "#bab8b6", "--rule": "#dddcda",
         "--fill": "#e9e8e5", "--code-bg": "#f4f3f0", "--hl": "#fdf2c7"}
DARK = {"--bg": "#161513", "--ink": "#ece9e2", "--accent": "#e0754f", "--mark": "#6a5814",
        "--muted": "#a3a19c", "--frame": "#767470", "--hatch": "#565551", "--rule": "#343330",
        "--fill": "#292826", "--code-bg": "#1f1d1b", "--hl": "#3c3313"}
FROM_INK = {"--muted": .66, "--frame": .45, "--hatch": .30, "--rule": .14, "--fill": .09, "--code-bg": .04}
PROPS = ["--bg", "--ink", "--muted", "--code-bg", "--rule", "--accent", "--fill", "--hl", "--mark", "--hatch",
         "--frame", "--font-hand", "--wobble", "--measure", "--gutter", "--font-text", "--font-ar", "--font-mono"]


def read_css(name):
    text = (CSS / name).read_text(encoding="utf-8")
    return re.sub(r":\s+", ":", re.sub(r"\s*([{};,>])\s*", r"\1", text))


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


def mix(a, b, p):
    ca = [int(a.lstrip("#")[i:i + 2], 16) for i in (0, 2, 4)]
    cb = [int(b.lstrip("#")[i:i + 2], 16) for i in (0, 2, 4)]
    return "#" + "".join(f"{round(x * p + y * (1 - p)):02x}" for x, y in zip(ca, cb))


def contrast(a, b):
    la, lb = sorted([lum(a), lum(b)], reverse=True)
    return (la + 0.05) / (lb + 0.05)


class TokensTest(unittest.TestCase):
    def setUp(self):
        self.tokens = read_css("tokens.css")

    def test_tokens_both_themes(self):
        light = props(block(self.tokens, r":root"))
        for p in PROPS:
            self.assertIn(p, light, p)
        for k, v in LIGHT.items():
            self.assertEqual(light[k].strip().lower(), v, k)
        media = re.search(r"@media \(prefers-color-scheme:dark\)\s*\{\s*:root:not\(\[data-theme=\"light\"\]\)\s*\{([^{}]*)\}",
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
            self.assertGreaterEqual(contrast(ink, theme["--mark"]), 4.5)

    def test_derived_from_ink(self):
        for theme in (LIGHT, DARK):
            for k, p in FROM_INK.items():
                self.assertEqual(theme[k], mix(theme["--ink"], theme["--bg"], p), k)
            self.assertEqual(theme["--hl"], mix(theme["--mark"], theme["--bg"], .45))

    def test_hand_font(self):
        light = props(block(self.tokens, r":root"))
        self.assertTrue(light["--font-hand"].strip().startswith('"Kalam"'))
        ar = re.search(r":lang\(ar\)\s*\{([^{}]*)\}", self.tokens)
        self.assertIsNotNone(ar)
        self.assertTrue(props(ar.group(1))["--font-hand"].strip().startswith('"Aref Ruqaa"'))

    def test_logical_properties(self):
        for name in ("base.css", "components.css"):
            text = read_css(name)
            self.assertNotRegex(text, r"(margin|padding|border)-(left|right)\s*:", name)
            self.assertNotRegex(text, r"(?<![\w-])(left|right)\s*:", name)
            self.assertNotRegex(text, r"text-align\s*:\s*(left|right)", name)
            self.assertNotRegex(text, r"float\s*:\s*(left|right)", name)

    def test_reduced_motion(self):
        text = read_css("base.css")
        self.assertIn("prefers-reduced-motion:reduce", text)

    def test_no_comments(self):
        for f in CSS.glob("*.css"):
            self.assertNotIn("/*", f.read_text(encoding="utf-8"), f.name)

    def test_state_marks(self):
        css = read_css("figures.css")
        self.assertIn("fill:var(--mark)", block(css, r"\.s-focus"))
        look = block(css, r"\.s-look")
        self.assertIn("stroke:var(--accent)", look)
        self.assertNotIn("fill:var(--accent)", look)
        self.assertIn("fill:var(--fill)", block(css, r"\.s-done"))
        for sel, body in re.findall(r"([^{}]+)\{([^{}]*)\}", css):
            for w in re.findall(r"stroke-width:([^;}]+)", body):
                if w.strip() != "var(--stroke)":
                    self.assertTrue(all(k.strip() in (".e-strong", ".s-red", ".e-grid") for k in sel.split(",")), sel)
        self.assertIn("font-family:var(--font-hand)", block(css, r"\.t-lab"))
        fsvg = block(css, r"\.fsvg")
        self.assertIn("max-width:100%", fsvg)
        self.assertIn("height:auto", fsvg)
        self.assertNotIn("sk-", css)

    def test_handmade_rules(self):
        comp = read_css("components.css")
        base = read_css("base.css")
        self.assertIn("border-radius:var(--wobble)", block(comp, r"\.figure,\.steps"))
        self.assertIn("text-decoration-style:wavy", block(base, r"\.lesson-body :is\(p,li\) a"))
        rules = re.findall(r"([^{}]+)\{([^{}]*)\}", comp)
        for target in (".callout-title", ".n-label", ".lesson-head .context"):
            self.assertTrue(any(target in sel and "font-family:var(--font-hand)" in body for sel, body in rules), target)

    def test_centered_overflow_stays_reachable(self):
        for f in (ROOT / "web" / "figures").glob("*.py"):
            self.assertNotRegex(f.read_text(encoding="utf-8"), r"justify-content\s*:\s*center", f.name)
        css = read_css("components.css")
        self.assertIn("justify-content:safe center", block(css, r"\.fside"))


if __name__ == "__main__":
    unittest.main()
