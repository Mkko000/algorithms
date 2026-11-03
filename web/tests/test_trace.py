import json
import re
import subprocess
import unittest
from pathlib import Path

from web.lib.csource import CACHE, ROOT
from web.lib.trace import load_templates, payload, run_c_harness, section_html
from web.traces import all_traces
from web.traces.partition import BOOK, DRIVER, build

QS = "src/07-quicksort/quicksort.c"


def real_partition(values):
    d = CACHE / "test"
    d.mkdir(parents=True, exist_ok=True)
    src = d / "real_partition.c"
    src.write_text(
        "#define main qs_main\n"
        f'#include "{ROOT / QS}"\n'
        "#undef main\n"
        f"int main(void) {{ Item a[] = {{{', '.join(map(str, values))}}};\n"
        f"  int n = {len(values)}; partition(a, 0, n - 1);\n"
        '  for (int k = 0; k < n; k++) printf("%d ", a[k]); printf("\\n"); return 0; }\n',
        encoding="utf-8")
    exe = d / "real_partition"
    subprocess.run(["gcc", "-std=c99", "-O2", str(src), "-o", str(exe)], check=True, capture_output=True)
    return [int(x) for x in subprocess.run([str(exe)], capture_output=True, text=True, check=True).stdout.split()]


class HarnessTest(unittest.TestCase):
    def test_harness_lines_are_real(self):
        events = run_c_harness(QS, ["partition"], DRIVER(BOOK))
        lines = (ROOT / QS).read_text(encoding="utf-8").split("\n")
        less = [e for e in events if e.op == "less"]
        self.assertTrue(less)
        for e in less:
            self.assertIn("less(", lines[e.line - 1])
        for e in events:
            if e.op == "exch":
                self.assertIn("exch(", lines[e.line - 1])

    def test_final_state_matches_c(self):
        events = run_c_harness(QS, ["partition"], DRIVER(BOOK))
        self.assertEqual(list(events[-1].a), real_partition(BOOK))


class PartitionTraceTest(unittest.TestCase):
    def test_partition_frames(self):
        t = build()
        frames = t.presets["book"]
        self.assertGreater(len(frames), 10)
        self.assertIsNone(frames[0].line)
        self.assertEqual(frames[0].say, "start")
        self.assertTrue(any(f.vars.get("compares") == "1" for f in frames))
        self.assertEqual(frames[-1].say, "done")
        self.assertEqual(set(t.presets), {"book", "random", "sorted", "dups"})

    def test_every_preset_ends_partitioned(self):
        for name, frames in build().presets.items():
            with self.subTest(preset=name):
                self.assertEqual(frames[-1].say, "done")
                self.assertIn("s-focus", frames[-1].svg)

    def test_frame_lines_inside_function(self):
        t = build()
        for f in t.presets["book"]:
            if f.line is not None:
                self.assertGreaterEqual(f.line, t.code.first_line)
                self.assertLessEqual(f.line, t.code.last_line)


class PayloadTest(unittest.TestCase):
    def test_payload_templates(self):
        t = build()
        p = payload(t, load_templates(Path("content/en/traces.toml"))["partition"])
        self.assertEqual(set(p["presets"]), {"book", "random", "sorted", "dups"})
        for frames in p["presets"].values():
            for f in frames:
                self.assertTrue(f["say"])
                self.assertNotIn("{", f["say"])
        self.assertEqual(p["first_line"], t.code.first_line)

    def test_missing_template_raises(self):
        t = build()
        tpl = dict(load_templates(Path("content/en/traces.toml"))["partition"])
        del tpl["swap"]
        with self.assertRaises(KeyError) as cm:
            payload(t, tpl)
        self.assertIn("swap", str(cm.exception))

    def test_both_languages_have_same_keys(self):
        ar = load_templates(Path("content/ar/traces.toml"))
        en = load_templates(Path("content/en/traces.toml"))
        self.assertEqual(set(ar), set(en))
        self.assertEqual(set(ar), set(all_traces()))
        for name in ar:
            self.assertEqual(set(ar[name]), set(en[name]), name)

    def test_section_html(self):
        t = build()
        ui = {"step": "step", "play": "Play", "pause": "Pause", "reset": "Reset", "step_back": "Back",
              "step_forward": "Forward", "preset": "Input", "trace": "Trace"}
        tpl = load_templates(Path("content/en/traces.toml"))["partition"]
        h = section_html(t, tpl, ui, {"book": "Book example", "random": "Random", "sorted": "Sorted",
                                      "dups": "Duplicates", "reversed": "Reversed"})
        self.assertTrue(h.startswith('<section class="trace" dir="ltr" data-trace="partition"'))
        m = re.search(r'<script type="application/json">(.*?)</script>', h, re.S)
        data = json.loads(m.group(1))
        self.assertEqual(data["name"], "partition")
        self.assertNotIn("</", m.group(1).replace("<\\/", ""))
        self.assertIn('class="trace-scene"', h)
        self.assertIn('aria-live="polite"', h)


if __name__ == "__main__":
    unittest.main()
