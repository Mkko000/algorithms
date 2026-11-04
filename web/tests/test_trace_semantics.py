import re
import unittest

from web.traces import all_traces

VAL = re.compile(r'<rect x="([\d.]+)" y="([\d.]+)" width="([\d.]+)" height="([\d.]+)" rx="[\d.]+" class="([^"]+)"/>'
                 r'<text x="[\d.]+" y="[\d.]+" class="t-val"[^>]*>([^<]*)</text>')
PTR = re.compile(r'<text x="([\d.]+)" y="[\d.]+" class="t-ptr"[^>]*>([^<▲]+)</text>')


def cells(svg):
    rows = {}
    for m in VAL.finditer(svg):
        rows.setdefault(float(m.group(2)), []).append((float(m.group(1)), float(m.group(3)), m.group(5), m.group(6)))
    top = min(rows)
    return [(x, w, cls, v) for x, w, cls, v in sorted(rows[top])]


def values(svg):
    return [v for _, _, _, v in cells(svg)]


def key(v):
    return int(v) if v.lstrip("-").isdigit() else v


def pointers(svg):
    return {name: float(x) for x, name in PTR.findall(svg)}


SORTS = ["selection", "insertion", "shellsort", "mergesort", "batcher"]


class SortTraceTest(unittest.TestCase):
    def test_sorts_end_sorted_with_the_same_items(self):
        for name in SORTS:
            t = all_traces()[name]
            for preset, frames in t.presets.items():
                with self.subTest(trace=name, preset=preset):
                    first, last = values(frames[0].svg), values(frames[-1].svg)
                    self.assertEqual(sorted(key(v) for v in first), [key(v) for v in last])

    def test_partition_splits_around_the_pivot(self):
        t = all_traces()["partition"]
        for preset, frames in t.presets.items():
            with self.subTest(preset=preset):
                last = cells(frames[-1].svg)
                vals = [key(v) for _, _, _, v in last]
                p = next(k for k, c in enumerate(last) if "s-focus" in c[2])
                self.assertTrue(all(v <= vals[p] for v in vals[:p]))
                self.assertTrue(all(v >= vals[p] for v in vals[p + 1:]))
                self.assertEqual(sorted(vals), sorted(key(v) for v in values(frames[0].svg)))

    def test_partition_pointers_sit_under_their_cells(self):
        t = all_traces()["partition"]
        for preset, frames in t.presets.items():
            for k, f in enumerate(frames):
                cs = cells(f.svg)
                ptr = pointers(f.svg)
                for name in ("i", "j"):
                    idx = int(f.vars[name])
                    with self.subTest(preset=preset, frame=k, pointer=name):
                        if 0 <= idx < len(cs):
                            x, w, _, _ = cs[idx]
                            self.assertAlmostEqual(ptr[name], x + w / 2, places=0)
                        else:
                            self.assertNotIn(name, ptr)

    def test_compare_counts_never_go_down(self):
        for name in SORTS + ["partition"]:
            for preset, frames in all_traces()[name].presets.items():
                counts = [int(f.vars.get("compares", 0)) for f in frames]
                with self.subTest(trace=name, preset=preset):
                    self.assertEqual(counts, sorted(counts))


class OtherTraceTest(unittest.TestCase):
    def test_union_find_components_match_roots(self):
        for preset, frames in all_traces()["union-find"].presets.items():
            for k, f in enumerate(frames):
                ids = [int(v) for v in values(f.svg)]
                roots = sum(1 for i, p in enumerate(ids) if i == p)
                with self.subTest(preset=preset, frame=k):
                    self.assertEqual(int(f.vars["components"]), roots)

    def test_linear_probing_has_no_gaps_before_a_key(self):
        from web.traces.probing import M, h
        for preset, frames in all_traces()["probing"].presets.items():
            table = values(frames[-1].svg)
            with self.subTest(preset=preset):
                for slot, c in enumerate(table):
                    if c == "·":
                        continue
                    s = h(c)
                    while s != slot:
                        self.assertNotEqual(table[s], "·", f"{c} at {slot} skipped empty {s}")
                        s = (s + 1) % M

    def test_lsd_passes_sort_by_growing_suffixes(self):
        from web.traces.lsd import PRESETS, run
        from web.traces.common import snippet
        code = snippet("src/10-radix-sort/lsd_radix.c", "lsd_radix")
        for preset, words in PRESETS.items():
            frames, final = run(words, code)
            with self.subTest(preset=preset):
                self.assertEqual(final, sorted(words))

    def test_heap_removes_largest_first_and_keeps_heap_order(self):
        from web.traces.heap import PRESETS, run
        from web.lib.trace import span
        code = span("src/09-heapsort/heap.c", "fixUp", "PQdelmax")
        for preset, (vals, as_letters, deletes) in PRESETS.items():
            frames, removed, a, size = run(vals, as_letters, deletes, code)
            with self.subTest(preset=preset):
                want = sorted(vals, reverse=True)[:deletes]
                got = [ord(x) if as_letters else int(x) for x in removed]
                self.assertEqual(got, want)
                for k in range(2, size + 1):
                    self.assertGreaterEqual(a[k // 2], a[k])

    def test_bst_reads_in_order(self):
        from web.traces.bst import PRESETS, keys, run
        from web.traces.common import snippet
        code = snippet("src/12-bst/bst.c", "insertR")
        for preset, items in PRESETS.items():
            _, root = run(items, code)
            with self.subTest(preset=preset):
                self.assertEqual(keys(root), sorted(items))

    def test_chaining_puts_every_key_in_its_bucket(self):
        from web.traces.chaining import PRESETS, h, run
        from web.traces.common import snippet
        code = snippet("src/14-hashing/hash_chaining.c", "STinsert")
        for preset, words in PRESETS.items():
            _, buckets = run(words, code)
            with self.subTest(preset=preset):
                self.assertEqual(buckets, {w: h(w) for w in words})

    def test_recursion_counts_calls(self):
        from web.traces.recursion import PRESETS
        fib = [0, 1]
        while len(fib) < 30:
            fib.append(fib[-1] + fib[-2])
        for preset, n in PRESETS.items():
            last = all_traces()["recursion"].presets[preset][-1]
            with self.subTest(preset=preset):
                self.assertEqual(int(last.vars["calls"]), 2 * fib[n + 1] - 1)
                self.assertEqual(last.args["v"], str(fib[n]))


if __name__ == "__main__":
    unittest.main()


class SimulationMatchesCTest(unittest.TestCase):
    def test_insertion_and_shellsort_show_v_and_h(self):
        for name, need in (("insertion", {"v", "i"}), ("shellsort", {"v", "h"})):
            for preset, frames in all_traces()[name].presets.items():
                shifted = [f for f in frames if f.say in ("shift", "place")]
                with self.subTest(trace=name, preset=preset):
                    self.assertTrue(shifted)
                    for f in shifted:
                        self.assertLessEqual(need, set(f.vars))

    def test_shellsort_gaps_follow_the_c_sequence(self):
        for preset, frames in all_traces()["shellsort"].presets.items():
            gaps = [int(f.vars["h"]) for f in frames if "h" in f.vars]
            with self.subTest(preset=preset):
                self.assertEqual(gaps, sorted(gaps, reverse=True))
                self.assertEqual(gaps[-1], 1)


class TraceUsageTest(unittest.TestCase):
    def test_every_trace_appears_in_a_lesson(self):
        from web.lib.csource import ROOT
        used = set()
        for p in (ROOT / "content" / "ar").glob("*.md"):
            used |= set(re.findall(r"^::trace ([\w-]+)", p.read_text(encoding="utf-8"), re.M))
        self.assertEqual(set(all_traces()) - used, set())
