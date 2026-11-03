Book sections: 2.1 Implementation and Empirical Analysis, 2.2 Analysis of Algorithms, 2.3 Growth of Functions, 2.4 Big Oh Notation

Fine, we know some algorithms are faster than others. But how do we measure it? "This one is fast" is not science. Chapter 2 teaches us to speak precisely without drowning in heavy math, and Sedgewick is upfront about it: his goal is for you to be able to **predict** how your program will perform before you run it on real data.

:::goal The goal of this lesson:
Run a proper measurement experiment (doubling), know what we analyze and why, get a feel for growth functions through numbers, and understand Big Oh, ~, Θ and Ω without fear.
:::

## 2.1 First: implement and measure (empirical analysis)

Before any formula: write the program and run it on growing inputs. The simplest and most powerful experiment: **double N and see what the time gets multiplied by**.

* Ratio ≈ 2 means linear (N)
* Ratio ≈ 4 means quadratic (N²)
* Ratio a little above 2 means N log N
* Ratio ≈ 1 means logarithmic or constant

The book splits the inputs we test on into 3 kinds: **real data** (what the program will actually see), **random data** (so we can compare with the analysis), and **perverse data** (to expose the worst case). A program that passes the first two and fails the third will fail in production one day.

```
$ ./doubling_test
       N     seq time   ratio   seq cmp/srch     bin time   ratio  bin cmp/srch
    4000      0.0013s    0.00         2000.3      0.0086s    0.00        11.48
    8000      0.0027s    1.99         4069.3      0.0091s    1.06        12.47
   16000      0.0052s    1.94         7956.9      0.0100s    1.10        13.47
  128000      0.0396s    1.99        64577.9      0.0114s    1.03        16.47
```

Look: for sequential search the ratio is about 2 and the compares are ≈ N/2. Binary search adds **one compare** every time N doubles.

* [src/02-analysis/doubling\_test.c](src:src/02-analysis/doubling_test.c)

:::oops Measurement mistakes
(1) Measuring something so small that the time is all noise. Repeat the operation thousands of times. (2) Comparing two programs where one was written carefully and the other in a rush. That comparison is unfair. (3) Forgetting that the compiler can remove code whose result is never used. Sedgewick says it clearly: the easiest thing in the world is to fool yourself with benchmarks.
:::

## 2.2 Analysis: what exactly do we count?

Time depends on the machine, the compiler and the cache. So we set those aside and count **the basic operation in the inner loop** (a compare, an exchange, an array access) as a function of **the main parameter N** (the size of the input). The steps:

1. Identify the abstract operations the program is built on (the inner loop).
2. Build a mathematical model of the inputs (for example: every ordering is equally likely).
3. Count how many times each operation runs, in the **average** and the **worst case**.

The result looks like `time ≈ c · f(N)`: the constant c comes from experiment, and the function f from analysis. Analysis alone without measurement can mislead you, and measurement without analysis cannot tell you what happens when N gets 1000 times bigger.

## 2.3 Growth of functions: get a feel for it

The running time of most algorithms in the book is proportional to one of these:

<table>
<tr><th>Function</th><th>What it means in practice</th><th>Example</th></tr>
<tr><td>1</td><td>constant, however big N gets</td><td>a[i], push</td></tr>
<tr><td>log N</td><td>grows very slowly: N=10⁹ gives about 30</td><td>binary search</td></tr>
<tr><td>N</td><td>linear: you touch every item once</td><td>sequential search</td></tr>
<tr><td>N log N</td><td>divide, conquer and combine</td><td>mergesort, quicksort</td></tr>
<tr><td>N²</td><td>every pair</td><td>insertion sort, brute force closest pair</td></tr>
<tr><td>N³</td><td>every triple</td><td>naive matrix multiplication</td></tr>
<tr><td>2ᴺ</td><td>every subset; it dies around N≈60</td><td>brute force knapsack</td></tr>
</table>

Other functions come up again and again: **lg N** (log base 2: how many times you can halve), **ln N** (natural log; ln N ≈ 0.693 lg N), the harmonic numbers **H<sub>N</sub> = 1 + 1/2 + … + 1/N ≈ ln N + 0.57721** (they show up in the analysis of quicksort and BSTs), and **N!**, which Stirling approximates: lg N! ≈ N lg N − 1.44N.

::widget bigoh

:::tip Feel the number
On a machine doing a billion operations per second: N=10⁶ with an N² algorithm is 10¹² operations, **a quarter of an hour**. The same N with N lg N ≈ 2·10⁷ is **0.02 seconds**. N=10⁹ with N²? **30 years**. This is not a theoretical difference. It is the difference between "finished" and "dead".
:::

* [src/02-analysis/growth.c](src:src/02-analysis/growth.c)

## 2.4 Big Oh without fear

**The definition:** `g(N) = O(f(N))` if there are constants c and N₀ such that g(N) ≤ c·f(N) for all N \> N₀. In other words, g "does not exceed" the order of f. A ceiling, not an equality.

* **O** = a ceiling (upper bound): "no worse than".
* **Ω** = a floor (lower bound): "no better than".
* **Θ** = both: the exact order.
* **~** (tilde) = the same leading term **including its constant**: g ~ f if g/f → 1. Sedgewick likes it because the constant matters in practice: "~N²/2 compares" says more than "O(N²)".

Why is Big Oh useful? It lets us drop the small terms: `3N² + 100N + 50 = O(N²)`, and write things like `N(N−1)/2 = N²/2 + O(N)`, where the important part is clear and the rest is marked as "smaller". And why is it dangerous? Because O is only a ceiling: an "O(N²)" algorithm might actually be linear. It also hides constants: 1000·N is slower than N² until N=1000.

:::oops Watch out
`(l+r)/2` can overflow when l and r are large. The safe version: `l + (r-l)/2`. A tiny detail in the analysis, a disaster in production.
:::

* [src/02-analysis/binary\_search.c](src:src/02-analysis/binary_search.c)

:::quiz
You doubled N and the time went up about 4 times. The algorithm is most likely…

1. [ ] linear
2. [x] quadratic
3. [ ] logarithmic

> (2N)² = 4N². That is exactly the question a doubling experiment answers.
:::

:::quiz
What is the order of 3N² + 1000N + 7?

1. [ ] O(N)
2. [x] O(N²), and with tilde: ~3N²
3. [ ] O(N³)

> The leading term is N². The ~ keeps the constant 3 because it matters when you compare in practice.
:::

:::quiz
Why do we also test on "perverse" data?

1. [ ] To make the numbers look nicer
2. [x] To expose the worst case before the users find it
3. [ ] Because real data is always random

> Naive quicksort on a sorted file, or insertion sort on a reversed one: very real data that turns out to be perverse.
:::

:::quiz
What is the difference between O and Θ?

1. [ ] There is no difference
2. [x] O is a ceiling; Θ is a ceiling and a floor together (the exact order)
3. [ ] Θ is always worse than O

> N = O(N²) is true, but N is not Θ(N²).
:::

:::quiz
How fast does H<sub>N</sub> = 1 + 1/2 + … + 1/N grow?

1. [ ] Like N
2. [x] Like ln N
3. [ ] Like √N

> H<sub>N</sub> ≈ ln N + 0.577. You will meet it in the average cost of quicksort and BSTs.
:::
