Book sections: 1.1 Algorithms, 1.4 Perspective, 1.5 Summary of Topics

Before we open any code, let me ask you something simple: what is the difference between a program that works and a program that works **well**?

Both can give the same answer. But one takes a second and the other takes an hour. One uses a megabyte and the other uses a gigabyte. The difference is not the language and not the computer. The difference is the **algorithm**.

:::goal The goal of this lesson:
Understand what an algorithm and a data structure are, why Sedgewick insists that we write real code and measure it, and how the whole book (16 chapters) is spread over the 40 lessons ahead of you.
:::

## 1.1 What is an algorithm, anyway?

Sedgewick's definition: **a method for solving a problem that is suitable for implementation as a computer program**. Not just any method. A method that stops in the end, solves every valid input, and whose cost (time and space) we can measure.

Most important algorithms work on **data organized in a particular way**. That is what we call **data structures**. The two are tightly linked: a simple algorithm may need a complicated structure, and a simple structure may need a clever algorithm. You will see this for yourself in every chapter.

An example from everyday life: you want to find a name in a paper phone book. You can turn the pages one by one (sequential search). Or you can open it in the middle, see whether the name comes before or after, and repeat (binary search). Both are "correct". But the second one finishes in about 20 steps for a million names, while the first can take a million.

:::problem
Sedgewick says something important in the first chapter: computers keep getting faster, but the problems we solve grow even faster. If your algorithm is N², a machine 10 times faster lets you solve a problem only about 3 times bigger (√10). If it is N log N, close to 10 times bigger. In other words, the right algorithm wins more than the more expensive hardware.
:::

## The book's method: start simple, measure, improve

The whole first chapter is one long example (union find, the next lesson) of the approach that repeats through the entire book:

1. Write a simple, correct solution to a well defined problem.
2. Understand its performance: run it on big inputs and analyze it mathematically.
3. Improve it step by step, and every improvement needs a clear reason.
4. Know when to stop: "better" in theory is not always better in practice.

## 1.4 A wider view (Perspective)

Sedgewick warns against two opposite mistakes: using a slow solution because "it works, that's enough", and spending a week tuning a program that will run once. The right question is: **how big is this problem, and how many times will it be solved?** If N = 100 and it runs once, any solution will do. If N = 10⁹ and it runs every second, choosing the algorithm is the whole project.

Also: many problems that look new turn out to be an old problem in disguise (sorting, searching, connectivity). Half the skill is recognizing them.

## 1.5 A map of the book and a map of the course

<table class="comp-table">
<tr><th>Part</th><th>Chapter</th><th>What it covers</th><th>Our lessons</th></tr>
<tr><td rowspan="2">Fundamentals</td><td>1</td><td>Introduction and union find</td><td>1 to 2</td></tr>
<tr><td>2</td><td>Analysis of algorithms: growth of functions, Big Oh, recurrences, guarantees</td><td>3 to 4</td></tr>
<tr><td rowspan="3">Data Structures</td><td>3</td><td>Arrays, linked lists, memory, strings, compound structures</td><td>5 to 7</td></tr>
<tr><td>4</td><td>ADTs: stack, queue, generalized queues, first class ADTs</td><td>8 to 10</td></tr>
<tr><td>5</td><td>Recursion, divide and conquer, DP, trees, DFS and BFS</td><td>11 to 14</td></tr>
<tr><td rowspan="6">Sorting</td><td>6</td><td>Elementary sorts, shellsort, index and list sorting</td><td>15 to 16</td></tr>
<tr><td>7</td><td>Quicksort with all its improvements, and selection</td><td>17 to 18</td></tr>
<tr><td>8</td><td>Merging and mergesort</td><td>19 to 20</td></tr>
<tr><td>9</td><td>Priority queues, heapsort and binomial queues</td><td>21 to 22</td></tr>
<tr><td>10</td><td>Radix sorting</td><td>23 to 24</td></tr>
<tr><td>11</td><td>Batcher, sorting networks, external and parallel sorting</td><td>25 to 26</td></tr>
<tr><td rowspan="5">Searching</td><td>12</td><td>Symbol tables and BSTs</td><td>27 to 29</td></tr>
<tr><td>13</td><td>Randomized, splay, 2-3-4, red black trees and skip lists</td><td>30 to 32</td></tr>
<tr><td>14</td><td>Hashing</td><td>33 to 34</td></tr>
<tr><td>15</td><td>Radix search: DSTs, tries, Patricia, TSTs</td><td>35 to 37</td></tr>
<tr><td>16</td><td>External searching: ISAM, B trees, extendible hashing</td><td>38 to 39</td></tr>
</table>

The order is not random: every chapter builds on the one before it. You will not understand why red black trees are complicated until you have seen a plain BST get lopsided, and you will not appreciate the heap until you have seen that a sorted array and an unordered list are both bad priority queues.

## How our repository is laid out

```
src/
  01-fundamentals/      union_find.c (5 نسخ), euclid.c
  02-analysis/          recurrences.c, doubling_test.c, binary_search.c …
  03-data-structures/   linked_list.c, list_memory.c, strings.c, point_closest.c …
  …                     (فولدر لكل فصل من الـ 16)
  16-external-search/   btree_pages.c, isam.c, extendible_hash.c
docs/                   ← الكورس اللي إنت فيه دلوقتي
```

Every C file has a small `main()` that runs on its own and prints numbers you can compare with what the lesson says. Build any file like this:

```
gcc -std=c99 -Wall -O2 src/07-quicksort/quicksort.c -o qs
./qs
# أو كله مرة واحدة:
make && make test
```

* [src/07-quicksort/quicksort.c](src:src/07-quicksort/quicksort.c)

:::tip A deal between us
I will not tell you "in this lesson we will learn". I will tell you what happens if you make a particular mistake, and I will show you numbers. If something is not clear, run the file in `src/` and change it. Trying it beats a thousand sentences.
:::

:::quiz
What is the main point of studying algorithms?

1. [ ] Memorize algorithm names for the exam
2. [x] Choose and design methods whose cost we understand and that fit the size of the problem
3. [ ] Replace the programming language with a faster one

> An algorithm is a design decision. A language is a tool. The same wrong algorithm stays slow in any language.
:::

:::quiz
A machine is 10 times faster. If your algorithm is N², how much bigger a problem can you solve in the same time?

1. [ ] 10 times bigger
2. [x] About 3 times bigger (√10)
3. [ ] 100 times bigger

> (kN)² = 10·N² means k = √10 ≈ 3.16. That is why improving the algorithm matters more than buying a machine.
:::

:::quiz
A program will run once on 200 items. What do you do?

1. [ ] Spend two days implementing the most complicated algorithm
2. [x] The simplest correct solution is enough; optimizing here is a waste of time
3. [ ] Don't write it at all

> This is the Perspective lesson (1.4): the size of the problem and how often it runs decide how much an improvement is worth.
:::

:::quiz
The order of the book's parts means something. Which order is right?

1. [ ] Sorting, then Searching, then Data Structures
2. [x] Fundamentals, then Data Structures, then Sorting, then Searching
3. [ ] Searching first, because searching matters most

> Without structures and analysis, sorting and searching are empty talk. Sedgewick builds the bottom layer first.
:::
