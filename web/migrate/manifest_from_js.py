import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

PARTS = [
    ("foundations", "Foundations (chapters 1 to 2)"),
    ("data-structures", "Data structures and ADTs (chapters 3 to 4)"),
    ("recursion-trees", "Recursion and trees (chapter 5)"),
    ("sorting", "Sorting (chapters 6 to 11)"),
    ("searching", "Searching (chapters 12 to 16)"),
    ("more-structures", "More data structures from Weiss, Thareja and Lipschutz (chapters 17 to 23)"),
    ("closing", "Closing"),
]

PART_AR_FIX = {
    "more-structures": "هياكل بيانات إضافية: من كتب Weiss وThareja وLipschutz (الفصول 17–23)",
}

EN = {
    "intro": "Why algorithms? A map of the book",
    "union-find": "Union find: every flavor of the connectivity problem",
    "analysis": "Analyzing algorithms: measurement, growth and Big Oh",
    "recurrences": "Recurrences, searching, guarantees and amortized cost",
    "data-structs": "Types and arrays: the ground everything stands on",
    "linked-lists": "Linked lists, list processing and memory",
    "strings-compound": "Strings, compound structures and graphs",
    "adt": "The idea of an ADT, the stack and what it is good for",
    "queues-adt": "Generic ADTs, queues and duplicate items",
    "first-class": "First class ADTs: complex numbers and polynomials",
    "recursion": "Recursion and divide and conquer",
    "dp": "Dynamic programming",
    "trees": "Trees: definitions, properties and traversal",
    "graph-traversal": "DFS and BFS",
    "elem-sort": "Selection, insertion and bubble sort",
    "shellsort-more": "Shellsort, sorting records and lists, and counting",
    "quicksort": "Quicksort from the inside",
    "quicksort-plus": "Improving quicksort, and selection",
    "mergesort": "Mergesort: merging and its analysis",
    "mergesort-plus": "Bottom up, lists and performance",
    "heapsort": "Heaps and heapsort",
    "pq-advanced": "The full PQ ADT, index heaps and binomial queues",
    "radix-sort": "Radix sort: bits and MSD",
    "radix-more": "Three way radix quicksort, LSD and performance",
    "special": "Batcher and sorting networks",
    "external-sort": "External sorting, polyphase merging and parallel processors",
    "symbol-tables": "Symbol tables, key indexed search and binary search",
    "bst": "Binary search trees: search, insertion and performance",
    "bst-ops": "Root insertion, select, deletion and join",
    "balanced": "Randomized BSTs and splay trees",
    "rb-trees": "2-3-4 trees and red black trees",
    "skip-lists": "Skip lists, and comparing every method",
    "hashing": "Hash functions and separate chaining",
    "open-addressing": "Linear probing, double hashing and dynamic tables",
    "tries": "Digital search trees and tries",
    "patricia": "Patricia tries",
    "tst": "Multiway tries, TSTs and text applications",
    "btree": "External searching: ISAM and B trees",
    "extendible": "Extendible hashing, and perspective",
    "arrays-matrices": "Arrays and matrices: addresses, sparse and triangular",
    "linked-variants": "Header, circular, doubly linked and cursor lists, and polynomials",
    "stacks-queues-apps": "Stacks and queues: expressions, the call stack and the deque",
    "binary-trees": "Binary trees: representation, traversals, expression trees and Huffman",
    "efficient-trees": "BST, threaded, AVL, red black and splay trees",
    "multiway-trees": "Multiway trees: M way, B, B+, 2-3 and tries",
    "heaps": "Heaps: binary, d heaps, leftist, binomial and Fibonacci",
    "practice": "What to use when, and projects to build",
}

LESSON_RE = re.compile(r"\{\s*id:\s*'([^']+)',\s*file:\s*'([^']+)',\s*title:\s*'([^']+)',\s*min:\s*(\d+)\s*\}")
PART_RE = re.compile(r"\{\s*title:\s*'([^']+)',\s*lessons:")


def parse(js: str):
    parts = []
    for chunk in js.split("{ title: ")[1:]:
        m = PART_RE.match("{ title: " + chunk)
        lessons = [m2.groups() for m2 in LESSON_RE.finditer(chunk)]
        parts.append((m.group(1), lessons))
    return parts


def chapters(order):
    result = {}
    pending = []
    for slug in order:
        m = re.fullmatch(r"ex-ch(\d+)", slug)
        if m:
            n = int(m.group(1))
            for p in pending:
                result[p] = n
            pending = []
            result[slug] = n
        else:
            pending.append(slug)
    for p in pending:
        result[p] = None
    return result


def q(s: str) -> str:
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def title_en(slug: str) -> str:
    m = re.fullmatch(r"ex-ch(\d+)", slug)
    if m:
        return f"Solved exercises: chapter {m.group(1)}"
    return EN[slug]


def main():
    js = (ROOT / "docs/assets/lessons.js").read_text(encoding="utf-8")
    parts = parse(js)
    assert len(parts) == len(PARTS), len(parts)
    order = [l[0] for _, ls in parts for l in ls]
    chap = chapters(order)
    out = []
    for (ar_title, lessons), (key, en_title) in zip(parts, PARTS):
        out.append("[[part]]")
        out.append(f"key = {q(key)}")
        out.append(f"title = {{ ar = {q(PART_AR_FIX.get(key, ar_title))}, en = {q(en_title)} }}")
        out.append("")
        for slug, file, ar, minutes in lessons:
            out.append("[[part.lesson]]")
            out.append(f"slug = {q(slug)}")
            out.append(f"kind = {q('exercises' if slug.startswith('ex-ch') else 'lesson')}")
            if chap[slug] is not None:
                out.append(f"chapter = {chap[slug]}")
            out.append(f"minutes = {minutes}")
            out.append(f"old_file = {q(file)}")
            out.append(f"title = {{ ar = {q(ar)}, en = {q(title_en(slug))} }}")
            out.append("")
    (ROOT / "content/course.toml").write_text("\n".join(out), encoding="utf-8")
    (ROOT / "content/arabic_only.txt").write_text("\n".join(order) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
