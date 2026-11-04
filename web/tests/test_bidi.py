import unittest

from web.lib.bidi import isolate_latin

W = '<span class="lt" dir="ltr">'


def wrapped(s):
    return f"{W}{s}</span>"


def wrapped_long(s):
    return f'<span class="lt wrap" dir="ltr">{s}</span>'


class BidiTest(unittest.TestCase):
    def test_book_sections_line(self):
        out = isolate_latin("أقسام الكتاب: 7.1 The Basic Algorithm (Programs 7.1–7.2)")
        self.assertEqual(out, "أقسام الكتاب: " + wrapped_long("7.1 The Basic Algorithm (Programs 7.1–7.2)"))

    def test_unbalanced_paren_stays_outside(self):
        out = isolate_latin("Quicksort (C. A. R. Hoare، 1960) هو")
        self.assertEqual(out, wrapped("Quicksort") + " (" + wrapped("C. A. R. Hoare") + "، 1960) هو")

    def test_enclosing_parens_move_inside(self):
        self.assertEqual(isolate_latin("القوائم المزدوجة (doubly linked)"), "القوائم المزدوجة " + wrapped("(doubly linked)"))

    def test_single_word(self):
        self.assertEqual(isolate_latin("الـ loop الداخلي"), "الـ " + wrapped("loop") + " الداخلي")

    def test_number_led_term(self):
        self.assertEqual(isolate_latin("متوسطه 2N ln N مقارنة"), "متوسطه " + wrapped("2N ln N") + " مقارنة")

    def test_trailing_sentence_punctuation_outside(self):
        self.assertEqual(isolate_latin("هو الـ heap."), "هو الـ " + wrapped("heap") + ".")

    def test_plain_numbers_untouched(self):
        self.assertEqual(isolate_latin("سنة 1960 و 7.1"), "سنة 1960 و 7.1")

    def test_code_and_svg_untouched(self):
        s = '<code>a[i] = x</code> <svg><text>A</text></svg> <pre>int x;</pre>'
        self.assertEqual(isolate_latin(s), s)

    def test_attributes_untouched(self):
        s = '<a href="/ar/heapsort/" title="Heap">قبل</a>'
        self.assertEqual(isolate_latin(s), s)

    def test_entities_survive(self):
        self.assertEqual(isolate_latin("كتب P&amp;H هنا"), "كتب " + wrapped("P&amp;H") + " هنا")

    def test_already_isolated_skipped(self):
        s = '<span class="lt" dir="ltr">Already done</span>'
        self.assertEqual(isolate_latin(s), s)
        self.assertEqual(isolate_latin(isolate_latin("الـ loop")), isolate_latin("الـ loop"))
        long = "أقسام: 7.1 The Basic Algorithm (Programs 7.1–7.2)"
        self.assertEqual(isolate_latin(isolate_latin(long)), isolate_latin(long))

    def test_caption_inside_ltr_figure(self):
        s = '<div class="figure" dir="ltr"><svg><text>A</text></svg><figcaption>تتبّع quick find هنا</figcaption></div>'
        self.assertIn("تتبّع " + wrapped("quick find") + " هنا", isolate_latin(s))

    def test_text_inside_bold(self):
        self.assertEqual(isolate_latin("في <b>partition</b> واحد"), "في <b>" + wrapped("partition") + "</b> واحد")


if __name__ == "__main__":
    unittest.main()
