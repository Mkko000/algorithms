<div align="center">

# Algorithms in C

### Learning Project · Sedgewick Parts 1–4

**Mohammed El Sisi** · محمد السيسي

<br/>

![C99](https://img.shields.io/badge/C-C99-00599C?style=for-the-badge&logo=c&logoColor=white)
![Build](https://img.shields.io/badge/Build-43%2F43%20passing-brightgreen?style=for-the-badge)
![Course](https://img.shields.io/badge/Course-18%20lessons-blueviolet?style=for-the-badge)
![License](https://img.shields.io/badge/Use-Educational-orange?style=for-the-badge)

<br/>

[English](#english) · [العربية](#العربية) · [Interactive Course](#interactive-course--الكورس-التفاعلي) · [Build](#build--البناء)

</div>

---

## Table of contents

| | |
|---|---|
| **EN** | [About](#about) · [Repo layout](#repository-layout) · [Chapter map](#chapter-map) · [Build](#build--البناء) · [Course](#interactive-course--الكورس-التفاعلي) · [Credit](#credit) |
| **AR** | [عن المشروع](#عن-المشروع) · [بنية الريبو](#بنية-الريبو) · [خريطة الفصول](#خريطة-الفصول) · [البناء](#build--البناء) · [الكورس](#interactive-course--الكورس-التفاعلي) |

---

<a id="english"></a>

## About

A complete, hands-on learning project covering **Robert Sedgewick's *Algorithms in C* (3rd edition), Parts 1–4**: Fundamentals, Data Structures, Sorting, and Searching.

Two deliverables live in this repository:

1. **Clean C99 implementations** of the book's core algorithms under `src/`, each with a small runnable `main`.
2. **An interactive Arabic course** under `docs/` (GitHub Pages ready) that explains every chapter in natural Egyptian Arabic, with quizzes and visual widgets.

Author of this project and course: **Mohammed El Sisi**.

---

## Repository layout

```
algo_repo/
├── src/
│   ├── common_item.h          # Sedgewick-style Item macros
│   ├── 01-fundamentals/       # union-find, Euclid
│   ├── 02-analysis/           # binary search, growth table
│   ├── 03-data-structures/    # lists, Josephus, sieve, strings, graphs
│   ├── 04-adt/                # stack, queue, polynomial
│   ├── 05-recursion-trees/    # fib/DP, Hanoi, knapsack, DFS/BFS
│   ├── 06-sorting/            # selection, insertion, bubble, shell, counting
│   ├── 07-quicksort/          # basic, improved, 3-way, select
│   ├── 08-mergesort/          # top-down, bottom-up
│   ├── 09-heapsort/           # heap PQ + heapsort
│   ├── 10-radix-sort/         # LSD, MSD, 3-way radix quick
│   ├── 11-special-sorts/      # Batcher odd-even
│   ├── 12-bst/                # BST + rotations
│   ├── 13-balanced-trees/     # red-black, randomized BST
│   ├── 14-hashing/            # chaining, linear probing, universal hash
│   ├── 15-tries/              # binary trie, TST
│   └── 16-external-search/    # B-tree
├── docs/                      # interactive course (GitHub Pages)
├── Makefile
└── README.md
```

---

## Chapter map

| Ch | Topic | Code folder |
|----|-------|-------------|
| 1 | Union-Find | `src/01-fundamentals/` |
| 2 | Algorithm Analysis | `src/02-analysis/` |
| 3 | Elementary Data Structures | `src/03-data-structures/` |
| 4 | ADTs (Stack, Queue, …) | `src/04-adt/` |
| 5 | Recursion and Trees | `src/05-recursion-trees/` |
| 6 | Elementary Sorting | `src/06-sorting/` |
| 7 | Quicksort | `src/07-quicksort/` |
| 8 | Mergesort | `src/08-mergesort/` |
| 9 | Priority Queues / Heapsort | `src/09-heapsort/` |
| 10 | Radix Sorting | `src/10-radix-sort/` |
| 11 | Special-Purpose Sorts | `src/11-special-sorts/` |
| 12 | Symbol Tables / BSTs | `src/12-bst/` |
| 13 | Balanced Trees | `src/13-balanced-trees/` |
| 14 | Hashing | `src/14-hashing/` |
| 15 | Radix Search (Tries) | `src/15-tries/` |
| 16 | External Searching (B-trees) | `src/16-external-search/` |

---

<a id="العربية"></a>

## عن المشروع

<div dir="rtl">

مشروع تعلّم كامل مبني على كتاب **Algorithms in C** لـ Robert Sedgewick (الطبعة التالتة، أجزاء 1–4).

المحتوى اتنين:

1. **كود C99 نظيف** لكل الخوارزميات الأساسية في `src/` — كل ملف يتشغّل لوحده.
2. **كورس تفاعلي بالعامية المصرية** في `docs/` جاهز لـ GitHub Pages: شرح، أمثلة، كويزات، وأدوات تفاعلية.

**المؤلف:** محمد السيسي.

</div>

## بنية الريبو

<div dir="rtl">

نفس الشجرة اللي فوق. الكود في `src/` حسب رقم الفصل، والكورس في `docs/`.

</div>

## خريطة الفصول

<div dir="rtl">

الجدول الإنجليزي فوق هو المرجع. كل فصل له مجلد ودرس مقابل في الكورس.

</div>

---

## Build · البناء

```bash
# compile everything into bin/
mkdir -p bin
find src -name '*.c' | while read f; do
  out=bin/${f#src/}; out=${out%.c}
  mkdir -p "$(dirname "$out")"
  gcc -std=c99 -Wall -Wextra -O2 "$f" -o "$out" -lm
done

# or, if you have make:
make          # build all
make test     # build + run every binary
make clean
```

Example:

```bash
gcc -std=c99 -Wall -O2 src/07-quicksort/quicksort.c -o qs && ./qs
```

---

## Interactive Course · الكورس التفاعلي

<div dir="rtl">

الكورس بيحمّل الدروس بـ <code>fetch</code>، فلازم سيرفر HTTP (مش <code>file://</code>).

</div>

```bash
cd docs
python3 -m http.server 8000
# open http://localhost:8000
```

<div dir="rtl">

أو انشر مجلد <code>docs/</code> على GitHub Pages (ملف <code>.nojekyll</code> موجود).

التقدّم بيتحفظ في المتصفح تحت المفتاح: <code>algo-done</code>.

</div>

Course structure:

| Path | Role |
|------|------|
| `docs/index.html` | Home + lesson list + progress |
| `docs/lessons/NN-*.html` | Lesson fragments |
| `docs/assets/style.css` | Dark theme, Cairo + JetBrains Mono, RTL |
| `docs/assets/lessons.js` | Lesson manifest |
| `docs/assets/app.js` | Router, progress, highlighting, quizzes |
| `docs/assets/widgets.js` | Big-Oh chart, sort viz, BST, heap, UF, hash |

---

## Credit

<div dir="rtl">

الكود والشروح في المشروع ده من تأليف <b>محمد السيسي</b>، ومبني على / مقتبس من خوارزميات وأفكار كتاب:

</div>

> Robert Sedgewick — *Algorithms in C* (3rd Edition), Parts 1–4:
> Fundamentals, Data Structures, Sorting, Searching.
> Addison-Wesley.

<div dir="rtl">

الكتاب نفسه له حقوق نشر لصاحبه والناشر. المشروع التعليمي ده مش بديل عن الكتاب، ومش بيعيد نشر نصّه.

</div>

---

**Mohammed El Sisi** · محمد السيسي · Educational use
