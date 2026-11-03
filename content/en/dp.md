Book sections: 5.3 Dynamic Programming (Programs 5.10 to 5.13, Property 5.3)

Divide and conquer works nicely when the two parts are **independent**. But if the parts overlap, with the same small problem showing up in both branches, the recursion redoes the same work thousands or billions of times. The fix is so simple it is almost funny: **write the answer down and never compute it again.** That is dynamic programming.

:::goal The goal of this lesson:
Understand why recursive Fibonacci is exponential, turn any recursive function into a DP version in two ways (bottom up and top down), solve the knapsack problem, and know when DP will not help.
:::

## The problem: Fibonacci (Program 5.10)

```c listing="Program 5.10"
int F(int i)
  { if (i < 1) return 0;
    if (i == 1) return 1;
    return F(i-1) + F(i-2); }
```

It looks innocent and matches the definition exactly. But `F(i-1)` itself calls `F(i-2)` again, and each of those branches out… The number of calls grows like F(N) itself, about φ^N where φ ≈ 1.618. Sedgewick puts it in practical terms: if F(N) takes a second, F(N+9) takes more than a minute, and F(N+18) more than an hour.

```
calls to compute F(N): naive vs top-down memo
  N=10  naive=     177  memo= 19
  N=20  naive=   21891  memo= 39
  N=30  naive= 2692537  memo= 59
```

Look how the first column gets multiplied by about 11 every 5 steps (φ⁵ ≈ 11). The second one only grows by 10.

::figure fib-tree

## The first fix: bottom up

Compute the values in order from smallest to largest, each new value from the ones before it:

```c
F[0] = 0; F[1] = 1;
for (i = 2; i <= N; i++) F[i] = F[i-1] + F[i-2];
```

Linear. The only requirement: you need to know an **order** that guarantees every value you need has been computed before it. In Fibonacci the order is obvious; in other problems it is harder.

## The second fix: top down, or memoization (Program 5.11)

Leave the recursive function as it is, and add two lines **mechanically**: first, check whether the value is already saved, and if so return it. Last, save the value before returning it.

```c listing="Program 5.11"
int F(int i)
  { int t;
    if (knownF[i] != unknown) return knownF[i];
    if (i == 0) t = 0;
    if (i == 1) t = 1;
    if (i > 1)  t = F(i-1) + F(i-2);
    return knownF[i] = t; }
```

The advantage: you don't need to think about the order, because the recursion discovers it by itself. And it computes only the values it actually needs.

* [src/05-recursion-trees/factorial\_fib.c](src:src/05-recursion-trees/factorial_fib.c)

:::tip Property 5.3
Dynamic programming reduces the running time of a recursive function to at most "the time to evaluate the function for all arguments less than or equal to the given argument", treating the cost of a recursive call as constant. In other words: the number of distinct values × the work per value.
:::

## The knapsack problem

A thief breaks into a warehouse with a knapsack of capacity M. There are N **types** of items, each with a size and a value, and an unlimited supply of each type (that is the book's definition). The goal is the maximum value. The book's example:

<table>
<tr><th>Type</th><th>A</th><th>B</th><th>C</th><th>D</th><th>E</th></tr>
<tr><td>Size</td><td>3</td><td>4</td><td>7</td><td>8</td><td>9</td></tr>
<tr><td>Value</td><td>4</td><td>5</td><td>10</td><td>11</td><td>13</td></tr>
</table>

The recursion (Program 5.12): "for every type that fits, try putting it in, and solve the rest for a smaller capacity". `knap(cap) = max(knap(cap − size[i]) + value[i])`. And it has the same disease as Fibonacci: `knap(10)` gets computed along a huge number of paths.

Program 5.13 is the same mechanical transformation: `maxKnown[cap]` for the value, and `itemKnown[cap]` so we know **which item** we chose and can rebuild the solution.

```
knapsack top-down  (cap=17): 24
items used: A(size 3, value 4) C(size 7, value 10) C(size 7, value 10)

calls: recursive vs top-down memo
  cap=10  recursive=       16  memo=  14
  cap=20  recursive=      315  memo=  60
  cap=30  recursive=     6235  memo= 110
  cap=40  recursive=   122118  memo= 160
```

The recursive version doubles about 20 times for every 10 units of capacity. The memoized one is linear: every capacity from 0 to M is computed once and tries N types.

And straight from Property 5.3 (the book derives it right after): dynamic programming solves the knapsack problem in time proportional to **N·M**.

::figure knap-table

* [src/05-recursion-trees/knapsack.c](src:src/05-recursion-trees/knapsack.c)

:::oops N·M is not as "linear" as it looks
M here is the *value* of the capacity, not the size of the input. If M = 10⁹, the table has a billion entries and the time does too. The book is clear: DP solves the knapsack problem easily when the capacity is not huge; for huge capacities the time and space become impossible. (That is why knapsack is considered "hard" in complexity theory despite this solution.)
:::

## When does DP help, and when not?

* **It helps** when the small problems **repeat** (overlapping subproblems), and the number of distinct values is small enough to store.
* **It is pointless** for divide and conquer whose parts are independent (like mergesort or max): nothing repeats at all, so saving answers just wastes space.
* **It will not work** if the argument is not a small integer (a real number, or a long string): you need a symbol table instead of an array, and that costs.

:::problem
"Dynamic programming" is a name that has nothing to do with dynamic programs. Richard Bellman chose it in the 1950s, and "programming" back then meant "planning or scheduling", as in "linear programming". The real idea: a table that you fill in.
:::

:::quiz
Why is naive recursive Fibonacci exponential?

1. [ ] Because addition is expensive
2. [x] Because the same values get computed many, many times in different branches
3. [ ] Because the stack fills up

> F(N−2) is computed from F(N) and from F(N−1), and each of those branches out again. The depth is only N, so the stack is not the problem.
:::

:::quiz
What is the main difference between bottom up and top down?

1. [ ] Bottom up is always slower
2. [x] With bottom up you must choose a correct order of computation yourself; with top down the recursion orders it for you and computes only what is needed
3. [ ] Top down stores nothing

> Both store values. Top down is a mechanical transformation of the recursive function.
:::

:::quiz
Knapsack with N = 5 and M = 10⁹. How does DP do?

1. [ ] Fast, because N is small
2. [x] Slow, and it needs a huge amount of space, because the time ∝ N·M
3. [ ] It cannot give a correct answer

> The answer will be correct, but the table is the size of M.
:::

:::quiz
Can we apply memoization to mergesort and gain something?

1. [ ] Yes, it becomes linear
2. [x] No, because the subproblems are independent and never repeat
3. [ ] Yes, but only on sorted arrays

> DP only wins when there is repetition. In mergesort every part is processed exactly once anyway.
:::

:::quiz
In Program 5.13, what is `itemKnown[]` for?

1. [ ] It speeds up the computation
2. [x] It lets us rebuild the solution itself (which items we chose), not just its value
3. [ ] It prevents repetition

> Starting at capacity M: take itemKnown\[M\], subtract its size, and repeat.
:::
