import unittest

from web.lib.margin import build_rows, split_blocks

GOAL = '<aside class="callout goal"><p class="callout-title">Goal</p><p>Learn it.</p></aside>'
TIP = '<aside class="callout tip"><p class="callout-title">Tip</p><p>Careful.</p></aside>'
SRC = '<ul class="src-links">\n<li><a href="/en/code/x/">src/x.c</a></li>\n</ul>'
CODE = '<figure class="code" dir="ltr"><pre><code>int x;</code></pre></figure>'
FIG = '<div class="figure" dir="ltr"><svg><rect x="1" y="1" width="2" height="2"/></svg></div>'


def rows(html):
    return build_rows(html, {})


class SplitTest(unittest.TestCase):
    def test_split_blocks_nesting(self):
        html = '<p>a<br>b</p>\n<div class="x"><div><img src="i.png"><p>in</p></div></div>\n<hr>\n' + FIG
        self.assertEqual(split_blocks(html), ['<p>a<br>b</p>', '<div class="x"><div><img src="i.png"><p>in</p></div></div>',
                                              '<hr>', FIG])


    def test_split_blocks_stray_end_tag(self):
        self.assertEqual(split_blocks("<p>a</p>\n</div>\n<p>b</p>"), ["<p>a</p>", "<p>b</p>"])


class RowsTest(unittest.TestCase):
    def test_goal_first_attaches_next(self):
        out = rows(GOAL + "\n<p>Intro.</p>\n<p>More.</p>")
        self.assertEqual(out.count('class="mrow"'), 1)
        main, side = out.split('<aside class="side">')
        self.assertIn("<p>Intro.</p>", main)
        self.assertIn("<p>More.</p>", main)
        self.assertIn('<aside class="callout goal note">', side)

    def test_goal_after_paragraph_attaches_previous(self):
        out = rows("<p>Intro.</p>\n" + GOAL + "\n<p>Next.</p>")
        self.assertEqual(out.count('<div class="mrow">'), 1)
        self.assertTrue(out.startswith('<div class="mrow"><div class="main"><p>Intro.</p>'))
        self.assertIn('<aside class="side"><aside class="callout goal note">', out)

    def test_tip_and_sources_share_side(self):
        out = rows("<p>Before.</p>\n" + CODE + "\n" + TIP + "\n" + SRC)
        self.assertEqual(out.count('<div class="mrow">'), 2)
        second = out[out.rindex('<div class="mrow">'):]
        self.assertIn(CODE, second)
        self.assertLess(second.index("callout tip note"), second.index("src-links"))
        self.assertNotIn("Before.", second)

    def test_rows_without_notes_join_previous(self):
        out = rows("<p>A.</p>\n<p>B.</p>\n<p>C.</p>")
        self.assertEqual(out.count('<div class="mrow">'), 1)
        self.assertNotIn('class="side"', out)

    def test_wide_blocks(self):
        out = rows("<p>A.</p>\n" + FIG + "\n<p>B.</p>")
        self.assertIn('<div class="mrow wide"><div class="main">' + FIG + "</div></div>", out)
        self.assertEqual(out.count('<div class="mrow">'), 2)

    def test_sources_after_wide_get_side_row(self):
        out = rows(FIG + "\n" + SRC + "\n<p>B.</p>")
        self.assertIn('<div class="mrow wide"><div class="main">' + FIG + "</div></div>", out)
        self.assertIn('<div class="mrow"><div class="main"></div><aside class="side">' + SRC + "</aside></div>", out)

    def test_nested_callout_stays(self):
        sol = '<details class="solution"><summary>S</summary>' + TIP + '</details>'
        out = rows("<p>A.</p>\n" + sol)
        self.assertIn(sol, out)
        self.assertNotIn("callout tip note", out)

    def test_sidenote_moves(self):
        para = '<p>Text<span class="sn"><sup class="m">1</sup><span class="sn-t">the <code>note</code></span></span> end.</p>'
        out = rows(para)
        self.assertIn('<p>Text<span class="sn"><sup class="m">1</sup></span> end.</p>', out)
        self.assertIn('<aside class="side"><p class="note sn"><sup class="m">1</sup> the <code>note</code></p></aside>', out)

    def test_heading_note(self):
        out = build_rows('<p>A.</p>\n<h2 id="s-1">6.2 Sel</h2>\n<p>B.</p>', {"s-1": '<div class="note book">6.2</div>'})
        last = out[out.rindex('<div class="mrow">'):]
        self.assertIn('<h2 id="s-1">6.2 Sel</h2>', last)
        self.assertIn("<p>B.</p>", last)
        self.assertIn('<aside class="side"><div class="note book">6.2</div></aside>', last)

    def test_empty_body(self):
        self.assertEqual(rows(""), "")
