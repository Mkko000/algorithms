Book sections: 2.5 Basic Recurrences (Formulas 2.1 to 2.5), 2.6 Examples of Algorithm Analysis (Properties 2.1 to 2.3), 2.7 Guarantees, Predictions, and Limitations

Most algorithms in the book are recursive: they solve a problem by solving a smaller version of it. That is why their cost is written the same way: **the cost for N = the cost of the smaller part + the extra work**. This is called a **recurrence**. Just five recurrences cover almost everything you will meet in the book.

:::goal The goal of this lesson:
Know the five recurrences and their solutions and how to solve them by hand (telescoping), analyze sequential and binary search precisely, tell a guarantee from a prediction, and know what amortized and randomized mean.
:::

## 2.5 The five basic recurrences

<table class="comp-table">
<tr><th>Recurrence</th><th>Solution</th><th>Where it comes from</th></tr>
<tr><td>C<sub>N</sub> = C<sub>N−1</sub> + N, C<sub>1</sub>=1</td><td>N(N+1)/2 ≈ N²/2</td><td>a loop over the items that removes one each time (selection sort)</td></tr>
<tr><td>C<sub>N</sub> = C<sub>N/2</sub> + 1, C<sub>1</sub>=1</td><td>≈ lg N</td><td>halves the input in one step (binary search)</td></tr>
<tr><td>C<sub>N</sub> = C<sub>N/2</sub> + N, C<sub>1</sub>=0</td><td>≈ 2N</td><td>halves, but touches everything first</td></tr>
<tr><td>C<sub>N</sub> = 2C<sub>N/2</sub> + N, C<sub>1</sub>=0</td><td>≈ N lg N</td><td>split in two halves, solve both, merge in linear time (mergesort)</td></tr>
<tr><td>C<sub>N</sub> = 2C<sub>N/2</sub> + 1, C<sub>1</sub>=1</td><td>≈ 2N</td><td>split in two halves with a constant step (maximum by divide and conquer)</td></tr>
</table>

::figure recur-tree

### How to solve them by hand: telescoping

Take the fourth one, and assume N = 2ⁿ so the halving comes out clean:

```
C(2ⁿ) = 2 C(2ⁿ⁻¹) + 2ⁿ
اقسم على 2ⁿ:   C(2ⁿ)/2ⁿ = C(2ⁿ⁻¹)/2ⁿ⁻¹ + 1
                         = C(2ⁿ⁻²)/2ⁿ⁻² + 1 + 1
                         = …  = n
يعني C(N) = N lg N
```

The trick: divide by something that makes both sides "the same shape" up to a constant difference, then unroll the chain. The first one is easier: C<sub>N</sub> = N + (N−1) + … + 1 = N(N+1)/2. And the third: N + N/2 + N/4 + … = 2N (a geometric series).

And if N is not a power of 2? The solution stays very close (the difference is small terms). The program below computes them exactly so you can check with your own eyes.

::trace recursion

* [src/02-analysis/recurrences.c](src:src/02-analysis/recurrences.c)

:::oops The classic mistake
Confusing C<sub>N/2</sub> + N (solution 2N, linear!) with 2C<sub>N/2</sub> + N (solution N lg N). The difference: do you solve one half or both halves? Quickselect solves one half, so it is linear; quicksort solves both, so it is N lg N.
:::

## 2.6 Real analysis examples: searching

**Property 2.1 (sequential search):** when searching among N numbers, a search miss always examines **N**, and a search hit examines about **N/2** on average (if every key is equally likely to be the one we want: (1+2+…+N)/N = (N+1)/2).

**Property 2.2:** if the table is sorted, a search miss can stop as soon as it passes the key, so it takes N/2 on average instead of N. An improvement by a constant factor of 2 only.

**Property 2.3 (binary search):** never examines more than **⌊lg N⌋ + 1** numbers. The proof is exactly the second recurrence: every step throws away half of the space.

```c
int search(int a[], int v, int l, int r)      /* Program 2.2 */
  { while (r >= l)
      { int m = (l+r)/2;
        if (v == a[m]) return m;
        if (v < a[m]) r = m-1; else l = m+1;
      }
    return -1;
  }
```

What that means in practice: a million keys and a million searches. Sequential ≈ 5·10¹¹ compares (hours). Binary ≈ 2·10⁷ (under a second). That is the price of sorting the table once first.

## 2.7 Guarantees, predictions, and limitations

### Worst case = a guarantee

If we prove that an algorithm never exceeds f(N), that is a **guarantee** whatever the input, even if an adversary chooses it. Very useful for systems that cannot tolerate surprises (aircraft, stock exchanges).

### Average case = a prediction

It assumes a model of the inputs (for example, a random order). If real data does not look like the model, the prediction can be wrong. Quicksort has an excellent average, but on a sorted file (something that happens a lot!) it can become N².

### The fix: put the randomness inside the algorithm (randomized)

Instead of hoping the input is random, the algorithm flips a coin itself (a random pivot, a randomized BST, a skip list). Then the "average" is over the algorithm's choices rather than over the input, and there is no particular bad input. The chance of poor performance becomes vanishingly small (smaller than the chance that the machine itself fails in the middle of the job).

### Amortized: one expensive operation, but a cheap total

An example: an array that doubles when it fills up. One particular push might copy a million items! But over N pushes, the total copying is ≤ 2N. So it is **O(1) per operation, averaged over the sequence**, and this is not a probabilistic average: it is a guarantee for any sequence. The same idea appears in path compression and splay trees.

```
$ ./recurrences    (آخر جزء)
  1000000 pushes ->  1048575 element copies  (1.05 per push)
```

### The limits

Analysis has clear limits: the constants can be large, the math problems are sometimes still open (shellsort still has no complete analysis), and the model may not apply. That is why the book always combines analysis with experiment.

:::quiz
What is the solution of C<sub>N</sub> = 2C<sub>N/2</sub> + N?

1. [ ] 2N
2. [x] N lg N
3. [ ] N²/2

> lg N levels, and each level does N work in total. That is mergesort.
:::

:::quiz
What is the solution of C<sub>N</sub> = C<sub>N/2</sub> + N?

1. [ ] N lg N
2. [x] About 2N
3. [ ] lg N

> N + N/2 + N/4 + … = 2N. It only goes down into one half, so the work shrinks fast.
:::

:::quiz
Binary search in 1000 items: what is the maximum number of compares?

1. [ ] 500
2. [x] 10 (⌊lg 1000⌋ + 1)
3. [ ] 1000

> Property 2.3. And a billion items? Only 30.
:::

:::quiz
Push onto a doubling array: what is its amortized cost?

1. [ ] O(N), because sometimes we copy everything
2. [x] O(1): the total copying over N operations is about ≤ 2N
3. [ ] O(log N)

> The expensive operation is rare, and it is paid for by all the cheap operations before it.
:::

:::quiz
What advantage does a randomized algorithm have over ordinary average case analysis?

1. [ ] It is always faster in the worst case
2. [x] Good performance does not depend on the input being random; there is no particular bad input
3. [ ] It needs no analysis at all

> The randomness lives inside the algorithm. A sorted or repetitive input is no longer a problem.
:::
