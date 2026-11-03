Book sections: 1.2 A Sample Problem: Connectivity, 1.3 Union Find Algorithms (Programs 1.1 to 1.4, Properties 1.1 to 1.3, Table 1.1)

Imagine you run a computer network, and every so often someone tells you "connect machine 3 to machine 8". The question that keeps coming up: are 3 and 8 already connected (directly or through other machines)? If not, connect them and print the pair. If they are, ignore it.

This is the **connectivity problem** (also called dynamic equivalence). The very first algorithm in the book is about it, and that is no accident: it is simple, and it carries a deep lesson about how "the right algorithm" is the difference between seconds and days.

:::goal The goal of this lesson:
Understand the problem exactly, know the book's four algorithms (quick find, quick union, weighted quick union, and the kinds of path compression), work out what each one costs, and see it in real numbers.
:::

## 1.2 The problem, exactly

The input is a sequence of pairs `p-q`, where each pair means "p is connected to q". Connection is an **equivalence** relation:

* **Reflexive:** p is connected to itself.
* **Symmetric:** if p is connected to q, then q is connected to p.
* **Transitive:** if p is connected to q and q to r, then p is connected to r. (This is what makes the problem nontrivial.)

What we want: for each new pair, print it **only** if it does not follow from the pairs before it. So we build a "spanning tree" of the components without needing to know the path itself, only whether a path exists. That is less than what graph programs compute, and that is why we can solve it much faster.

Applications: networks (are these two machines on the same network?), electrical circuits (points joined by a wire), compilers (`EQUIVALENCE` declarations in Fortran: two variables that refer to the same memory), and images (neighboring pixels of the same color form one region).

We split the problem into two abstract operations: **find** (which component does this item belong to?) and **union** (merge two components). Every algorithm is a way of representing components that makes these two operations cheap.

## 1.3 The algorithms

### 1) Quick find, Program 1.1

An array `id[]`: every item has a "component name". p and q are connected ⟺ `id[p] == id[q]`. At the start, `id[i] = i`.

```c listing="Program 1.1"
/* Program 1.1 — الفكرة */
if (id[p] == id[q]) continue;          /* already connected */
for (t = id[p], i = 0; i < N; i++)
    if (id[i] == t) id[i] = id[q];     /* rename the whole component */
```

Find is a single comparison. Union walks over the **entire** array. **Property 1.1:** M union operations on N items take about **M·N** instructions. For N = 10⁹ and M = 10¹⁰ that is 10¹⁹. Years.

::figure uf-quickfind

### 2) Quick union, Program 1.2

We change the meaning: `id[i]` is now the **parent** of i. Each component is a tree, and its root is its name (the item where `id[i]==i`). Find means climbing to the root. Union means linking one root to another (a single line!).

```c listing="Program 1.2"
for (i = p; i != id[i]; i = id[i]) ;
for (j = q; j != id[j]; j = id[j]) ;
if (i == j) continue;
id[i] = j;                             /* one root under the other */
```

Union is now cheap, but find walks the height of the tree. **Property 1.2:** if M \> N, it can take more than **M·N/2**. How? The pairs `1-0, 2-1, 3-2, …` build a chain of length N, like a strand of spaghetti, and every find after that walks the whole chain.

::figure uf-qu-bad

### 3) Weighted quick union, Program 1.3

The simple, brilliant fix: keep the size of each tree in `sz[]`, and **always link the smaller tree under the larger one**.

```c listing="Program 1.3"
if (sz[i] < sz[j]) { id[i] = j; sz[j] += sz[i]; }
else               { id[j] = i; sz[i] += sz[j]; }
```

**Property 1.3:** we follow at most **2 lg N** pointers to decide whether two items are connected. The proof is lovely: a node gets one level deeper only when its tree (the smaller one) is hung under a tree at least as big, which means the size of its tree **at least doubles**. A size cannot double more than lg N times before it reaches N. Done.

::figure uf-weighted

### 4) Path compression, Program 1.4 and the other kinds

While we are climbing to the root, why not make the nodes we pass point straight at the root? Then the next find is shorter. There are a few ways to do it:

* **Full path compression:** a second pass after reaching the root, making every node on the path point to it directly. The strongest flattening, but two passes.
* **Path halving (Program 1.4):** in the same pass, every node points to its grandparent: `id[i] = id[id[i]]`. One line, and almost the same effect as the full version.
* **Path splitting:** like halving, but for every node instead of every other node (an exercise in the book).

```c listing="Program 1.4"
/* halving — Program 1.4 */
for (i = p; i != id[i]; i = id[i]) id[i] = id[id[i]];

/* full compression — two passes */
r = p; while (r != id[r]) r = id[r];
while (p != r) { next = id[p]; id[p] = r; p = next; }
```

With weighting, the cost per operation is constant for all practical purposes: the theory (Tarjan) says it is close to α(N), the inverse Ackermann function, which is ≤ 5 for any N in the universe. Sedgewick says it plainly: no exactly linear algorithm is known, but this is "good enough" by a tiny margin.

::trace union-find

:::tip A worked example
Start with 0..9. The pairs: 3-4, 4-9, 8-0, 2-3, 5-6, 2-9 (already connected, not printed), 5-9, 7-3, 4-8, 5-6 (a repeat), 0-2 (connected), 6-1. Only 9 pairs get printed for 10 items, which means one component. Step through it in the trace above and watch the tree.
:::

## Our own Table 1.1: the same pairs, five algorithms

The file `union_find.c` runs all five versions on 20000 random pairs with N = 10000 and counts the pointers and entries it touches. On my machine it came out roughly as:

<table>
<tr><th>Algorithm</th><th>Operations</th></tr>
<tr><td>quick-find</td><td>≈ 98,000,000</td></tr>
<tr><td>quick-union</td><td>≈ 21,000,000</td></tr>
<tr><td>weighted QU</td><td>≈ 56,000</td></tr>
<tr><td>weighted + halving</td><td>≈ 29,000</td></tr>
<tr><td>weighted + full compression</td><td>≈ 69,000 (counting both passes)</td></tr>
</table>

Notice: the gap between quick find and the weighted version is **a thousand times** even for a small N, and it grows with N. And notice that full compression "pays" for a second pass, so its number here is higher than halving. That is exactly why the book picks halving: almost the same effect for less work.

* [src/01-fundamentals/union\_find.c](src:src/01-fundamentals/union_find.c)

:::oops Watch out for this mistake
Forgetting the weighting and relying on path compression alone. It is better than nothing, but without the weights the worst case can come back. The second mistake: doing the union on p and q themselves instead of their roots (`id[p]=q`). That breaks the tree and cuts nodes off from their root.
:::

<table>
<tr><th>Algorithm</th><th>Find</th><th>Union</th><th>Worst case for M operations</th></tr>
<tr><td>Quick-find</td><td>1</td><td>N</td><td>M·N</td></tr>
<tr><td>Quick-union</td><td>tree height</td><td>tree height</td><td>M·N/2</td></tr>
<tr><td>Weighted QU</td><td>lg N</td><td>lg N</td><td>M lg N</td></tr>
<tr><td>+ path compression</td><td colspan="2">≈ constant (α(N))</td><td>≈ M</td></tr>
</table>

:::quiz
Why is quick find called that, even though its union is expensive?

1. [ ] Because find uses binary search
2. [x] Because checking a connection (find) is one step: compare id\[p\] and id\[q\]
3. [ ] Because the array is small

> The name is about the cheap operation. The price is paid in union, which walks over all N entries.
:::

:::quiz
Which input brings plain quick union to its knees?

1. [ ] Random pairs
2. [x] Pairs like 1-0, 2-1, 3-2 … that build one long chain
3. [ ] Repeated pairs only

> Every union hangs the old root under a new item, so the tree becomes a line, and every find walks N steps.
:::

:::quiz
Why is the height of weighted quick union ≤ lg N?

1. [ ] Because the trees are always complete
2. [x] Because a node gets deeper only when the size of its tree at least doubles, and that can happen at most lg N times
3. [ ] Because path compression shortens the trees

> This is the proof of Property 1.3, with no compression at all.
:::

:::quiz
What is the difference between full path compression and path halving?

1. [ ] Halving does not change the tree
2. [x] The full version makes a second pass and points every node at the root; halving, in the same pass, points every node at its grandparent
3. [ ] The full version is always faster on every operation

> Both reach a nearly constant cost. Halving is simpler and cheaper per pass, and it is what Program 1.4 uses.
:::

:::quiz
You have millions of items and millions of operations. Which version do you use?

1. [ ] Quick find, because find is fast
2. [x] Weighted quick union with path compression (or halving)
3. [ ] Any of them; the difference is theoretical

> The difference is not theoretical. Our table above shows a thousand times for N = 10⁴, and Table 1.1 in the book shows hours against seconds.
:::
