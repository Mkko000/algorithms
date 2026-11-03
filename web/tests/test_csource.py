import unittest

from web.lib.csource import (SourceError, extract_function, extract_region, header_comment,
                             run_output, whole_file)
from web.tests.support import SAMPLE_C, line_of, sample_c


class ExtractTest(unittest.TestCase):
    def setUp(self):
        self.fix = sample_c()

    def test_extract_function_spans_braces_in_literals(self):
        s = extract_function(self.fix, "partition")
        self.assertTrue(s.text.startswith("int partition(int a[], int l, int r)\n{"))
        self.assertTrue(s.text.rstrip().endswith("}"))
        self.assertIn("return l < r ? l : r;", s.text)
        self.assertEqual(s.first_line, SAMPLE_C.splitlines().index("int partition(int a[], int l, int r)") + 1)
        self.assertEqual(s.last_line, s.first_line + len(s.text.splitlines()) - 1)

    def test_extract_function_skips_prototype(self):
        s = extract_function(self.fix, "partition")
        self.assertNotIn(";\n\n", s.text.splitlines()[0])
        self.assertGreater(len(s.text.splitlines()), 3)

    def test_extract_function_excludes_comment_above(self):
        s = extract_function(self.fix, "partition")
        self.assertNotIn("/*", s.text.splitlines()[0])
        self.assertNotIn("the partition step", s.text)

    def test_extract_macro(self):
        self.assertEqual(extract_function(self.fix, "less").text, "#define less(A,B) ((A) < (B))")

    def test_extract_typedef_struct(self):
        self.assertEqual(extract_function(self.fix, "rec").text, "typedef struct { int key; } rec;")

    def test_missing_name_raises(self):
        with self.assertRaises(SourceError) as cm:
            extract_function(self.fix, "nope")
        self.assertIn("no definition of nope", str(cm.exception))

    def test_region(self):
        s = extract_region(self.fix, "demo")
        self.assertEqual(s.text, "static int twice(int x) { return 2 * x; }")
        self.assertEqual(s.first_line, line_of(SAMPLE_C, "static int twice"))

    def test_missing_region_raises(self):
        with self.assertRaises(SourceError):
            extract_region(self.fix, "nope")

    def test_whole_file_drops_header(self):
        s = whole_file(self.fix)
        self.assertTrue(s.text.startswith("#include <stdio.h>"))
        self.assertEqual(s.first_line, 4)

    def test_header_comment(self):
        self.assertEqual(header_comment(self.fix), "sample.c  a fixture")

    def test_real_partition(self):
        s = extract_function("src/07-quicksort/quicksort.c", "partition")
        self.assertEqual(s.first_line, 12)
        self.assertEqual(s.path, "src/07-quicksort/quicksort.c")


class RunTest(unittest.TestCase):
    def test_run_output_cut(self):
        self.assertEqual(run_output(sample_c(), 2), "1\n2\n...")

    def test_run_output_full(self):
        self.assertEqual(run_output(sample_c()), "1\n2\n3")

    def test_run_output_includes_header_deps(self):
        out = run_output("src/07-quicksort/quicksort.c", 3)
        self.assertTrue(out)


if __name__ == "__main__":
    unittest.main()
