Solved exercises, chapter 1 (Introduction), exercises 1.4 to 1.26

This lesson is not new material. It is a problem solving session: we take exercises from the end of chapter 1 of Sedgewick's book and solve them together. For each exercise you will find **the problem**, then **how to think about it** (the road to the solution, not the solution itself), and then **the full solution**. Every number here comes from real programs in the repository, not from my head.

:::goal The goal of this lesson:
Practice tracing union find by hand, learn to estimate running time without running anything, and see for yourself why weighting matters more than path compression.
:::

:::tip Before you start
Read the problem, close your eyes for two minutes and try. Then open only "how to think about it" and try again. The solution is at the very end. Nobody ever learned algorithms by reading finished solutions.
:::

* [src/01-fundamentals/exercises/ex1\_04\_uf\_traces.c](src:src/01-fundamentals/exercises/ex1_04_uf_traces.c)
* [src/01-fundamentals/exercises/ex1\_20\_height\_union.c](src:src/01-fundamentals/exercises/ex1_20_height_union.c)
* [src/01-fundamentals/exercises/ex1\_22\_random\_edges.c](src:src/01-fundamentals/exercises/ex1_22_random_edges.c)

### Exercises 1.4 and 1.5: tracing quick find and quick union

:::problem
Show the contents of the `id` array after each union when you use quick find (Program 1.1) on the pairs `0-2, 1-4, 2-5, 3-6, 0-4, 6-0, 1-3`, and count how many times the program accesses the array for each pair. Then do the same with quick union (Program 1.2).
:::

:::think
First, agree with yourself on exactly what you are counting. I will count every read or write of `id[]` as it appears in the book's code. In quick find: the test `id[p]==id[q]` is two reads, then `t=id[p]` is one read, then a loop over all N items (N reads), and each item that changes costs a read of `id[q]` and a write. So the cost = 3 + N + 2×(number of items that change). In quick union: every step of the for loop reads `id[i]` once in the condition and once in the update, and at the end there is a single write.
:::

:::solution
```
quick find:                         quick union:
 pair  0 1 2 3 4 5 6 7 8 9  acc      pair  0 1 2 3 4 5 6 7 8 9  acc
 0-2   2 1 2 3 4 5 6 7 8 9  15       0-2   2 1 2 3 4 5 6 7 8 9  3
 1-4   2 4 2 3 4 5 6 7 8 9  15       1-4   2 4 2 3 4 5 6 7 8 9  3
 2-5   5 4 5 3 4 5 6 7 8 9  17       2-5   2 4 5 3 4 5 6 7 8 9  3
 3-6   5 4 5 6 4 5 6 7 8 9  15       3-6   2 4 5 6 4 5 6 7 8 9  3
 0-4   4 4 4 6 4 4 6 7 8 9  19       0-4   2 4 5 6 4 4 6 7 8 9  7
 6-0   4 4 4 4 4 4 4 7 8 9  17       6-0   2 4 5 6 4 4 4 7 8 9  9
 1-3   4 4 4 4 4 4 4 7 8 9   2       1-3   2 4 5 6 4 4 4 7 8 9  8
 total 100                           total 36
```

Notice: in quick find every union walks the whole array, so it costs ≥ N even if only one item changes. The last pair `1-3` was already connected, so it cost just two reads. In quick union the union itself is cheap (one write), but find starts to get heavy as the trees grow: `6-0` walked 0→2→5→4.
:::

### Exercises 1.7, 1.8 and 1.17: the same trace with weighting and compression

:::problem
Do exercise 1.4 again, but with weighted quick union (Program 1.3), then with halving (Program 1.4), then with full path compression (Exercise 1.16).
:::

:::think
The rule in Program 1.3: if `sz[i] < sz[j]` make i a child of j, otherwise (even when they are equal) make j a child of i. So on a tie, the left one (p) stays the root. Pay attention to this point, because most mistakes in this exercise come from it. As for compression: it only makes a difference when find walks a path longer than 1.
:::

:::solution
```
weighted (1.3):  0-2 → 0 1 0 3 4 5 6 7 8 9     acc 8
                 1-4 → 0 1 0 3 1 5 6 7 8 9     acc 8
                 2-5 → 0 1 0 3 1 0 6 7 8 9     acc 10
                 3-6 → 0 1 0 3 1 0 3 7 8 9     acc 8
                 0-4 → 0 0 0 3 1 0 3 7 8 9     acc 10
                 6-0 → 0 0 0 0 1 0 3 7 8 9     acc 10
                 1-3 → (connected)             acc 6      total 60
halving (1.4):   same id[] after every pair, total 75
full PC (1.16):  same id[] after every pair, total 70
```

The surprise: halving and full compression **did not change a single entry** in this example, and they spent more accesses (75 and 70 instead of 60). The reason is that the trees here never had time to grow: the longest path was 2 (4→1→0). So compression is an investment: it costs a little up front, and the payoff only shows on big problems. That is exactly what Table 1.1 in the book says: P and H beat W by a small margin.
:::

### Exercises 1.10 and 1.11: estimate the time without running anything

:::problem
(1.10) Estimate the minimum time (in days) quick find would need to solve a problem with 10⁶ objects and 10⁹ pairs, on a machine that executes 10⁹ instructions per second, assuming each iteration of the inner loop takes at least 10 instructions. (1.11) Estimate the maximum time (in seconds) for weighted quick union on the same problem, if each iteration takes at most 100 instructions.
:::

:::think
You don't need to write code. You need Property 1.1 (quick find executes ≥ MN instructions) and Property 1.3 (weighted follows ≤ 2 lg N pointers per pair). Multiply, divide by the speed, and convert the units. Keep one thing in mind: "minimum time" means a lower bound, and "maximum time" means an upper bound. So the two calculations are not equally strict, but they are enough to make a decision.
:::

:::solution
Quick find: 10⁹ × 10⁶ × 10 = 10¹⁶ instructions ÷ 10⁹ = 10⁷ seconds ≈ **116 days**.

Weighted: lg 10⁶ ≈ 20, so each pair takes ≤ 2×20 = 40 iterations × 100 = 4000 instructions. That gives 10⁹ × 4000 = 4×10¹² ÷ 10⁹ ≈ **4000 seconds** (about an hour). And that is a very pessimistic upper bound, because in practice the average distance to the root is about 2 (see the next exercise), so the real time is minutes.

The difference between 116 days and an hour comes entirely from two extra lines of code. That is the whole point of chapter 1.

```
Exercise 1.10: quick find >= M*N*10 = 1e16 instr -> 10000000 s = 116 days
Exercise 1.11: weighted <= M*2lgN*100 = 3.99e+12 instr -> 3986 s (~66 min)
```

(An honest note: Program 1.1 does `continue` when the pair is already connected, so most pairs in a real problem would not run the N loop. The 116 days are the Property 1.1 calculation the book has in mind, assuming every pair does a union.)
:::

### Exercise 1.12: the average distance in the worst weighted tree

:::problem
Compute the average distance from a node to the root in a worst case tree of 2ⁿ nodes built by weighted quick union.
:::

:::think
What is the worst case? Figure 1.8: every union merges two trees of the same size. A tree of 2ⁿ nodes is two trees of 2ⁿ⁻¹ nodes, one hung under the root of the other. So the number of nodes at depth k is T(n,k) = T(n−1,k) + T(n−1,k−1), which is exactly the rule of Pascal's triangle. So the count is C(n,k) (this is called a **binomial tree**, and you will meet it again in chapter 9).
:::

:::solution
The average = (1/2ⁿ) Σ k·C(n,k) = (1/2ⁿ) · n·2ⁿ⁻¹ = **n/2**. The program confirms exactly that:

```
2^4  =     16 nodes: average distance 2.000 (n/2 = 2.0), max 4
2^8  =    256 nodes: average distance 4.000 (n/2 = 4.0), max 8
2^16 =  65536 nodes: average distance 8.000 (n/2 = 8.0), max 16
```

So even in the worst case, the average is half of lg N.
:::

### Exercises 1.14, 1.15 and 1.18: build a path of length 4

:::problem
Give a sequence of pairs that makes weighted quick union (Program 1.3) produce a path of length 4. Then do the same for halving (Program 1.4), and for full path compression.
:::

:::think
To get a path of length 4 with weighting, you need at least 2⁴ = 16 nodes (Property 1.3). So build the worst case: merge pairs, then pairs of pairs, and so on. The trick for the other two versions: compression only works on the path that find walks. If you only ever give it **roots**, find never takes a step, and compression has nothing to compress.
:::

:::solution
```
0-1 2-3 4-5 6-7 8-9 10-11 12-13 14-15
0-2 4-6 8-10 12-14
0-4 8-12
0-8
```

Both ends of every pair here are roots at that moment, and their sizes are equal. After the last pair, node 15 walks 15→14→12→8→0: **4 links**. The exact same sequence gives a path of length 4 with halving and with full compression, because no find ever took a step. Compression improves what comes *later*, not what has already been built.
:::

### Exercises 1.16 and 1.19: full path compression, and why it is not enough alone

:::problem
(1.16) Show how to modify Program 1.3 to do full path compression: every node touched during the union should point to the root of the new tree. (1.19) Give an example showing that adding full path compression to quick union **without weighting** is not enough to guarantee there are no long paths.
:::

:::think
For 1.16: a first pass to find the root, and a second pass that walks the same path again and makes every node point to the root. For 1.19: think about the weak spot in the method itself. Compression fixes paths *after* find walks them. So if you can find a sequence where find always starts at a root, the tree keeps growing without anyone touching it.
:::

:::solution
```c
int root(int x)
  { int i = x;
    while (i != id[i]) i = id[i];          /* pass 1: find the root   */
    while (x != i)                          /* pass 2: point all at it */
      { int next = id[x]; id[x] = i; x = next; }
    return i; }
```

After that, the union is exactly like Program 1.3: `i = root(p); j = root(q);`. And for 1.19: the pairs `0-1, 1-2, 2-3, …, 8-9` with quick union (which always does `id[i] = j`). The left end of every pair is a root, so find takes 0 steps:

```
 8-9   1 2 3 4 5 6 7 8 9 9   3
 depth of node 0 after the chain: 9 (= N-1)
```

A path of length N−1. If you then ask about 0, you pay N−1 steps once and the tree flattens. So the average cost over the long run is not bad (that is Tarjan's analysis), but **there is no guarantee per operation**, and without weighting there is no lg N.
:::

### Exercises 1.20 and 1.21: union by height

:::problem
Modify Program 1.3 to use the **height** of the trees (the longest path to the root) instead of their size to decide which one becomes the child. Run experiments comparing it with Program 1.3. (1.21) Show that Property 1.3 still holds for this version.
:::

:::think
Store `h[i]` for every root. When you merge two trees of different heights, hang the shorter one under the taller one, and the height does not change. If they are equal, the height grows by 1. For the proof: you want to show that a tree of height h has ≥ 2ʰ nodes. By induction: the height becomes h+1 only when we merge two trees of height h, each with ≥ 2ʰ nodes, so the total is ≥ 2ʰ⁺¹.
:::

:::solution
```c
if (h[i] < h[j]) id[i] = j;
else if (h[i] > h[j]) id[j] = i;
else { id[j] = i; h[i]++; }
```

The proof: from the induction above, N ≥ 2ʰ, so h ≤ lg N. So find walks ≤ lg N, and a pair costs ≤ 2 lg N: **Property 1.3 holds**. In the experiment (random pairs until everything is connected, with the same pairs for both versions):

```
        N | by size avg/max    | by height avg/max  | lg N
     1000 |    1.90 / 5        |    1.96 / 5        | 10.0
    10000 |    1.98 / 6        |    2.05 / 6        | 13.3
   100000 |    2.05 / 7        |    2.14 / 7        | 16.6
  1000000 |    2.06 / 8        |    2.13 / 8        | 19.9
```

The two versions are very close, and size is slightly better on average. Both are far from lg N: a maximum depth of 8 for a million nodes. So the theoretical bound is very pessimistic for random data.
:::

### Exercises 1.22 to 1.24: how many random edges until everything is connected?

:::problem
Modify Program 1.4 to generate random pairs between 0 and N−1 instead of reading them from the input, and keep looping until it has done N−1 unions. Run it for N = 10³, 10⁴, 10⁵, 10⁶, and print the total number of edges. (1.24) Give an approximate formula for that number in terms of N.
:::

:::think
Notice that the last isolated object is what holds everything up. This is close to the coupon collector problem: to "touch" all N objects with random pairs you need about (N ln N)/2 pairs (each pair touches two). And once every object has been touched, the random graph is almost always connected (this comes from Erdős and Rényi). So the guess is **½ N ln N**. The experiment decides.
:::

:::solution
```
        N  edges (avg)   (1/2) N ln N   ratio
     1000         3737           3454   1.082
    10000        48610          46052   1.056
   100000       606351         575646   1.053
  1000000      6696582        6907755   0.969
```

The ratio is close to 1, so **M ≈ ½ N ln N**. That is an important fact: in Table 1.1, M is much bigger than N (about ln N/2 times), so there are many more finds than unions. That is exactly why quick union (whose find is expensive) came out slower than quick find in that table.
:::

### Exercises 1.25 and 1.26: a machine 10 times faster

:::problem
Suppose we use weighted quick union on 10 times as many connections, on a machine 10 times faster than the old one. How much longer does the new job take than the old one? (1.26) The same question for an algorithm that takes N³ instructions.
:::

:::think
Write a ratio: (cost of the new problem ÷ speed of the new machine) ÷ (cost of the old one ÷ speed of the old one). For weighted, the cost is about c·M lg N. And when a problem is "10 times bigger", N usually grows 10 times too. So let us work out both cases.
:::

:::solution
Weighted, with N fixed: (10M lg N / 10)/(M lg N) = **1**, the same time. If N also grows 10 times: the ratio is lg(10N)/lg N, and for N = 10⁶ that is 23.3/19.9 ≈ **1.17**, so only about 17% longer.

N³: ((10N)³/10)/N³ = **100 times longer**. So a machine 10 times faster combined with a cubic algorithm makes you 100 times slower on exactly the problems you need it for. Chapter 2 comes back to this idea in detail (Figure 2.5).
:::

:::quiz
In exercise 1.4, why did the pair 1-3 cost only two reads with quick find?

1. [ ] Because 1 and 3 are next to each other
2. [x] Because id\[1\] == id\[3\] (already connected), so it hits continue before the loop
3. [ ] Because N is small

> Program 1.1 tests `if (id[p] == id[q]) continue;` first. Both were 4, so we were done after two reads.
:::

:::quiz
Why did halving spend more accesses than plain weighted quick union in exercise 1.8?

1. [ ] Because it is wrong
2. [x] Because it writes on every find step, and the trees in the example were short, so its benefit never showed
3. [ ] Because it changes the shape of the tree

> Compression is an investment that pays off over the long run on big problems. With 7 pairs on 10 objects there is no long run.
:::

:::quiz
Random pairs on a million objects. Roughly how many pairs until everything is connected?

1. [ ] One million
2. [x] About 7 million (½ N ln N)
3. [ ] A million times a million

> ½ × 10⁶ × 13.8 ≈ 6.9 million, and the experiment gave 6.7 million.
:::
