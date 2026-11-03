Solved exercises, chapter 5 (Recursion and Trees), exercises 5.1 to 5.90

Chapter 5 is the biggest chapter in Part 1, and its exercises have everything: plain recursion, divide and conquer, dynamic programming, and properties of trees. We picked the ones that teach you something: when recursion gets dangerously deep, how to turn a recursion into a formula, and how to compute something on a tree in a single pass.

:::goal The goal of this lesson:
Write recursive functions correctly the first time (a base case, and a problem that gets smaller), turn exponential recursion into DP, and prove and measure properties of trees.
:::

:::tip Two questions before any recursion
(1) What is the smallest case, and what is its answer? (2) Does every call really move toward that case? If you are not sure about the second one, you have a hidden infinite recursion. That is exactly the subject of exercise 5.40 below.
:::

* [src/05-recursion-trees/exercises/ex5\_01\_recursion.c](src:src/05-recursion-trees/exercises/ex5_01_recursion.c)
* [src/05-recursion-trees/exercises/ex5\_22\_ruler\_hanoi.c](src:src/05-recursion-trees/exercises/ex5_22_ruler_hanoi.c)
* [src/05-recursion-trees/exercises/ex5\_37\_dp.c](src:src/05-recursion-trees/exercises/ex5_37_dp.c)
* [src/05-recursion-trees/exercises/ex5\_86\_trees.c](src:src/05-recursion-trees/exercises/ex5_86_trees.c)

### Exercises 5.1 and 5.2: lg(N!) and N! mod M

:::problem
(5.1) Write a recursive program that computes lg(N!). (5.2) Modify Program 5.1 to compute N! mod M without overflow. Try it with M = 997 and N = 10³…10⁶, to see how your system handles deep recursion.
:::

:::think
lg(N!) = lg N + lg((N−1)!), so it has the same shape as Program 5.1, except we add logarithms instead of multiplying, so there is no overflow. For the mod: (a·b) mod M = ((a mod M)·(b mod M)) mod M, so keep every intermediate result below M. But before you run it, think: N! for any N ≥ 997 has 997 as a factor. So the answer is 0! The real exercise here is the depth of the recursion.
:::

:::solution
```c
double lgfact(int n) { return n <= 1 ? 0 : log2(n) + lgfact(n-1); }
int factmod(int n, int m)
  { return n == 0 ? 1 % m : (long)n % m * factmod(n-1, m) % m; }
```

```
5.1 lg(10!) = 21.7911 (log2(3628800) = 21.7911), lg(100!) = 524.8
5.2   1000! mod 997 =   0  (recursion depth 1001)
5.2 100000! mod 997 =   0  (recursion depth 100001)
    996! mod 997 = 996 by Wilson's theorem = p-1
```

A depth of 100 thousand worked fine, because each frame is small. But at a million, on many systems (with an 8MB stack) the program will crash with a stack overflow. That is the point Sedgewick wants you to see: recursion of depth N is dangerous for large N, even when the algorithm is correct. (And the safe case: divide and conquer has depth only lg N.)
:::

### Exercises 5.3 and 5.4: the puzzle (3N+1)

:::problem
(5.3) Give the sequence of argument values that result when Program 5.2 is called for each of the integers 1 through 9. (5.4) Find the N \< 10⁶ for which Program 5.2 makes the largest number of calls.
:::

:::think
Program 5.2: if N is even call on N/2, and if it is odd call on 3N+1. Nobody has proved that it finishes for every N (that is the Collatz conjecture)! So there is no formula. The only way is to try all million. And that is fast, because the average number of steps is about 100.
:::

:::solution
```
5.3 N=3: 3 10 5 16 8 4 2 1   (8 calls)
5.3 N=7: 7 22 11 34 17 52 26 13 40 20 10 5 16 8 4 2 1   (17 calls)
5.3 N=9: 9 28 14 7 22 11 34 17 52 26 13 40 20 10 5 16 8 4 2 1   (20 calls)
5.4 N < 10^6 with the most calls: N = 837799, 525 calls
```

Notice that 9 passes through 7, and 7 passes through 5. So if you saved the results (DP) you would save a lot. And note: 3N+1 for N close to a million sometimes exceeds the range of a 32 bit `int`, so we used `long`.
:::

### Exercises 5.5 and 5.7: Euclid without recursion, and its depth on Fibonacci

:::problem
(5.5) Write Euclid's algorithm without recursion. (5.7) Give the recursion depth of Euclid's algorithm when the input is two consecutive Fibonacci numbers (F<sub>N</sub> and F<sub>N+1</sub>).
:::

:::think
Program 5.3 uses **tail recursion**: the call is the last thing that happens, so it turns into a loop right away. For 5.7: F<sub>N+1</sub> mod F<sub>N</sub> = F<sub>N−1</sub> (because F<sub>N+1</sub> = F<sub>N</sub> + F<sub>N−1</sub> and F<sub>N−1</sub> \< F<sub>N</sub>). So each step goes down only one place in the Fibonacci sequence, which is the slowest possible descent.
:::

:::solution
```c
int gcd(int u, int v) { while (v) { int t = u % v; u = v; v = t; } return u; }
```

```
5.7 depth of gcd(F_{N+1}, F_N):  N=5 -> 5  N=15 -> 15  N=25 -> 25  N=35 -> 35  N=45 -> 45
```

The depth is about **N**. And since F<sub>N</sub> ≈ φ<sup>N</sup>/√5, the depth ≈ log<sub>φ</sub> F<sub>N</sub> ≈ 1.44 lg F<sub>N</sub>. That is the worst case for Euclid (Lamé's theorem). So Euclid never takes more than about 1.44 lg(the number) steps.
:::

### Exercises 5.21 to 5.24: the ruler and Hanoi

:::problem
(5.21) Prove that the Hanoi solution in Program 5.7 is optimal, that is, every solution needs at least 2<sup>N</sup> − 1 moves. (5.22) Write a recursive program that computes the length of mark i on a ruler with 2<sup>n</sup> − 1 marks. (5.23) Find a property of i that determines the direction of move i. (5.24) Fill an array with all the moves (like Program 5.9).
:::

:::think
5\.21: think about the **largest disk**. It has to move at least once. And at the moment it moves, all the other N−1 disks must be stacked on the third peg (there is nowhere else). And after its last move, they all have to come back on top of it. So T(N) ≥ 2T(N−1) + 1, which is exactly the recurrence for 2<sup>N</sup>−1. 5.22: the middle mark has length n, and each half ruler n−1. 5.23: write out the moves and watch.
:::

:::solution
::figure ex-hanoi

```
5.22 ruler n=4 : 1 2 1 3 1 2 1 4 1 2 1 3 1 2 1
     mark(i) = 1 + (number of trailing 0 bits of i): true for n = 10
5.23 hanoi(4): -1 +2 -1 -3 -1 +2 -1 +4 -1 +2 -1 -3 -1 +2 -1
     verified for N = 1..12: yes
5.24 nonrecursive: -1 +2 -1 -3 -1 +2 -1 +4 -1 +2 -1 -3 -1 +2 -1
5.21 N=10: BFS shortest = 1023, 2^N-1 = 1023
```

5\.23: the disk that moves at step i is 1 + the number of trailing zeros of i in binary (that is, the ruler mark), and disk k always moves in the same direction: (−1)<sup>N−k</sup>. So the whole solution can be generated in a single loop with no recursion at all (5.24). For 5.21 the proof is enough, but the program also ran a BFS over all 3<sup>N</sup> positions for N ≤ 10 and confirmed that the shortest solution really is 2<sup>N</sup>−1.
:::

### Exercises 5.37 and 5.38: Fibonacci

:::problem
(5.37) Compute F<sub>N</sub> mod M using constant space. (5.38) What is the largest N for which F<sub>N</sub> can be represented as a 64 bit integer?
:::

:::think
You don't need an array. The last two numbers are enough, like bottom up DP without the table. And take the mod at every step. For 5.38: F<sub>N</sub> ≈ φ<sup>N</sup>/√5 \< 2<sup>63</sup>, so N \< (63 ln 2 + ln √5)/ln φ ≈ 92.4. So about 92 for signed, and one more for unsigned. The program confirms it.
:::

:::solution
```
5.37 F_1000 mod 1000000007 = 517691607, F_100 mod 997 = 364
5.38 largest N: signed 64-bit F_92 = 7540113804746346429, unsigned 64-bit F_93 = 12200160415121876738
```

And Program 5.10 (plain recursive Fibonacci) would need about F<sub>92</sub> ≈ 7.5×10¹⁸ calls to reach that same number. That is hundreds of years.
:::

### Exercises 5.40 to 5.43: P<sub>N</sub> with DP (and a trap in the exercise itself)

:::problem
(5.40) Use bottom up DP to compute P<sub>N</sub> = ⌊N/2⌋ + P<sub>⌊N/2⌋</sub> + P<sub>⌈N/2⌉</sub>, for N ≥ 1 and P₀ = 0, and plot N against P<sub>N</sub> − N lg N/2. (5.41) The same with top down. (5.43) Plot the number of calls in the top down version.
:::

:::think
Apply the two questions from the tip: for N = 1, ⌈1/2⌉ = 1, so the recurrence says P₁ = 0 + P₀ + P₁. So P₁ calls itself! If you write it top down literally, it recurses forever. So you need a second base case: P₁ = 0 (which is the intended meaning: a problem of size 1 costs nothing). And then notice that this shape is exactly the number of compares used by merging in the best case.
:::

:::solution
```
5.40 N, P_N, N lg N / 2, difference:
        16     32      32.0     0.0
        64    192     192.0     0.0
      1024   5120    5120.0     0.0
     N=1000: P=4932 vs 4982.9
5.43 top-down calls: N=23:15  N=64:13  N=1000:31  N=1024:21   (top-down == bottom-up: yes)
```

At exact powers of 2, P<sub>N</sub> = N lg N/2, and in between it drops a little below (the difference is negative and oscillates; if you plot it you get a "sawtooth" shape). And top down makes only O(lg N) calls, because ⌊N/2⌋ and ⌈N/2⌉ at each level are at most two distinct numbers. So the memo catches everything.
:::

### Exercises 5.44 and 5.45: quicksort's C<sub>N</sub>

:::problem
Use DP (bottom up in 5.44, top down in 5.45) to compute C<sub>N</sub> = N + (1/N) Σ<sub>1≤k≤N</sub> (C<sub>k−1</sub> + C<sub>N−k</sub>) for N ≥ 1, with C₀ = 1.
:::

:::think
The naive computation is O(N) for each C<sub>N</sub>, so O(N²) in total. But the two terms in the sum are symmetric: Σ(C<sub>k−1</sub> + C<sub>N−k</sub>) = 2 Σ<sub>j&lt;N</sub> C<sub>j</sub>. So keep a running sum, and each step becomes O(1). This is the recurrence for the average number of compares in quicksort (chapter 7), so expect ≈ 2N ln N.
:::

:::solution
```c
sum += C[i-1];  C[i] = i + 2*sum/i;
```

```
5.44 C_N:  C_10=45.437  C_100=848.85  C_1024=13346.8   (2N ln N = 14195.7 at 1024)
5.45 top-down C_23 = 134.2460 (bottom-up 134.2460), 553 calls
```

Close to 2N ln N (the more precise form is 2(N+1)H<sub>N</sub> minus linear terms). And the naive top down version makes 553 calls for N = 23 (each level loops over all the ones before it), despite the memo. Bottom up with the running sum is much better.
:::

### Exercises 5.68 and 5.73: counting in trees

:::problem
(5.68) How many external nodes are there in an M ary tree with N internal nodes? Use that to compute the memory needed. (5.73) A Fibonacci tree of height n \> 1 has a Fibonacci tree of height n−1 as one subtree and one of height n−2 as the other. Give its height and its external path length in terms of N (the number of nodes).
:::

:::think
5\.68: count the links. Every internal node has M links, so there are MN links. And every node except the root has exactly one link coming in, so links = (N + E) − 1. That gives E = (M−1)N + 1. 5.73: N<sub>h</sub> = N<sub>h−1</sub> + N<sub>h−2</sub> + 1, so N<sub>h</sub> + 1 behaves like Fibonacci.
:::

:::solution
::figure ex-fibtree

5\.68: E = **(M−1)N + 1**. The memory is N items + MN links = (M+1)N words, of which (M−1)N + 1 links are empty (NULL). So a 3 ary tree wastes about two thirds of its links!

```c
5.73 Fibonacci trees:   h   N(nodes)  height* ext.path  N+1=F_{h+2}
                        5        12       5        50        13   EPL/((N+1) lg N) = 1.039
                       14       986      14     10225       987   EPL/((N+1) lg N) = 1.041
                       20     17710      20    260375     17711   EPL/((N+1) lg N) = 1.042
```

N + 1 = F<sub>h+2</sub>, so h ≈ log<sub>φ</sub>(√5 N) ≈ **1.44 lg N**. And the external path length ≈ **1.04 N lg N** (from the table; analytically, the average depth of an external node approaches (φ/√5)·h ≈ 0.72h). So even the worst "nearly balanced" tree is only 44% away from ideal in height, and 4% on average.
:::

### Exercises 5.74 and 5.75: the divide and conquer tree

:::problem
(5.74) The divide and conquer tree for N: its root is N, and below it are a divide and conquer tree for ⌊N/2⌋ and one for ⌈N/2⌉. Draw it for 11, 15, 16 and 23. (5.75) Prove by induction that its path length is between N lg N and N lg N + N.
:::

:::think
First understand the definition carefully: if N were the number of nodes, then 1 + ⌊N/2⌋ + ⌈N/2⌉ = N + 1, which is impossible. So N is meant to be the number of **leaves** (the problems of size 1), as in Figure 5.6. And the path length here is the sum of the depths of the leaves = the sum of the sizes of all the problems solved (except the root). Induction: L(N) = L(⌊N/2⌋) + L(⌈N/2⌉) + N.
:::

:::solution
::figure ex-dctree

The proof: L(1) = 0 and the bounds hold. Assume they hold for smaller values, and let a = ⌊N/2⌋ and b = ⌈N/2⌉. The lower bound: L(N) ≥ a lg a + b lg b + N ≥ N lg(N/2) + N = N lg N, because x lg x is convex, so it is smallest when a = b = N/2. The upper bound: L(N) ≤ a lg a + a + b lg b + b + N. Use b ≤ (N+1)/2 and the fact that lg is concave, and you reach ≤ N lg N + N. ∎ The program checked this for every N ≤ 5000:

```
        11       39       38.1       49.1
        16       64       64.0       80.0
        23      106      104.0      127.0
      5000    61808    61438.6    66438.6
     bound holds for all N <= 5000: yes
```

That is exactly why mergesort uses between roughly N lg N and N lg N + N compares (chapter 8).
:::

### Exercises 5.86 to 5.90: leaves, nodes with one child, and internal path length

:::problem
(5.86) A program that counts the leaves of a binary tree. (5.87) One that counts the nodes with one internal child and one external child. (5.88) A recursive program that computes the internal path length (Definition 5.6). (5.89) How many calls does it make? (5.90) Make it run in time proportional to the number of nodes.
:::

:::think
All of these follow the same template: `f(NULL)` = a simple case, and `f(h)` = something about h + f(right) + f(left). The trap in 5.88: if you compute IPL = IPL(L) + IPL(R) + (the number of nodes below), counting the nodes with another function inside every call, it becomes O(N²) on a skewed tree. The fix: pass the depth as a parameter, so each node just adds its own depth.
:::

:::solution
```c
int leaves(link h)   { if (!h) return 0; if (!h->l && !h->r) return 1;
                       return leaves(h->l) + leaves(h->r); }
int onechild(link h) { if (!h) return 0;
                       return ((!h->l) != (!h->r)) + onechild(h->l) + onechild(h->r); }
long ipl(link h, int d) { if (!h) return 0; return d + ipl(h->l, d+1) + ipl(h->r, d+1); }
```

```
5.86 leaves(dc tree 11) = 11   5.87 one-child nodes = 0
5.88/5.89 IPL(dc tree 11) = 58 using 43 calls = 2N+1 (N=21): linear
     fib tree h=6: 20 nodes, 8 leaves, 5 one-child nodes
```

5\.89: the number of calls is exactly **2N + 1**: one per node and one per external node (there are N+1 of them, from 5.68). The proof by induction: C(tree) = 1 + C(L) + C(R), and C(empty) = 1. And the divide and conquer tree has no node with exactly one child (every node is either a leaf or has two children), while the Fibonacci tree has many.
:::

:::quiz
Why can Program 5.1 (recursive factorial) crash at N = a million, while mergesort on a million items does not?

1. [ ] Because factorial overflows
2. [x] Because the recursion depth of factorial is N, while in mergesort it is only lg N ≈ 20
3. [ ] Because mergesort is not recursive

> The stack holds a frame for every call that has not finished yet. It is the depth, not the total number of calls, that decides.
:::

:::quiz
In Hanoi with 10 disks, which disk moves at step 48?

1. [ ] Disk 1
2. [ ] Disk 4
3. [x] Disk 5
4. [ ] Disk 6

> 48 = 110000 in binary, with 4 trailing zeros, so the disk = 4 + 1 = 5.
:::

:::quiz
A 3 ary tree has 100 internal nodes. How many external nodes (NULL links) does it have?

1. [ ] 101
2. [x] 201
3. [ ] 300

> (M−1)N + 1 = 2×100 + 1 = 201.
:::

:::quiz
The recurrence P<sub>N</sub> = ⌊N/2⌋ + P<sub>⌊N/2⌋</sub> + P<sub>⌈N/2⌉</sub> with only P₀ = 0. What goes wrong if you write it top down?

1. [ ] It is slow
2. [x] P₁ calls P₁ (because ⌈1/2⌉ = 1), so it recurses forever
3. [ ] Nothing

> You need a base case for N = 1, because the problem does not get smaller at N = 1.
:::
