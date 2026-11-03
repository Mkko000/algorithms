import unittest

from web.lib.highlight import highlight_c


class HighlightTest(unittest.TestCase):
    def test_keywords_numbers_comments(self):
        h = highlight_c("int x = 1; /* a<b */")
        self.assertIn('<span class="k">int</span>', h)
        self.assertIn('<span class="m">1</span>', h)
        self.assertIn('<span class="c">/* a&lt;b */</span>', h)

    def test_functions_and_strings(self):
        h = highlight_c('printf("%d")')
        self.assertIn('<span class="f">printf</span>', h)
        self.assertIn('<span class="s">&quot;%d&quot;</span>', h)

    def test_preprocessor(self):
        self.assertIn('<span class="k">#include</span>', highlight_c("#include <stdio.h>"))

    def test_escapes_plain_text(self):
        self.assertEqual(highlight_c("a < b && c"), "a &lt; b &amp;&amp; c")

    def test_escaped_quote_in_string(self):
        h = highlight_c(r'"a\"b" x')
        self.assertIn(r'<span class="s">&quot;a\&quot;b&quot;</span>', h)
        self.assertTrue(h.endswith(" x"))


if __name__ == "__main__":
    unittest.main()
