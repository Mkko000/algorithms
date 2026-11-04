Book sections: 5.1 Recursive Algorithms (Programs 5.1 to 5.5), 5.2 Divide and Conquer (Programs 5.6 to 5.9, Properties 5.1 and 5.2)

Many people treat recursion like magic: "the function calls itself? So how does it know when to stop?". The truth is simpler: recursion is **a direct translation of a mathematical definition**, and the computer takes care of the bookkeeping with an internal stack. Problems come when you forget the stopping condition, or when the recursion branches and redoes the same work.

:::goal The goal of this lesson:
Write correct recursive functions and know how to prove they stop, understand recursion depth and its cost, and master divide and conquer through three examples: the maximum, the towers of Hanoi, and drawing a ruler. Then see the secret connection between Hanoi and the ruler.
:::

## 5.1 Recursive algorithms

**Sedgewick's definition:** a recursive algorithm solves a problem by solving smaller instances of the same problem. In C: a function that calls itself. It needs two things: a **base case** that returns without a call, and every recursive call must be on a **smaller value** that moves toward the base case.

```c listing="Program 5.1"
/* Program 5.1 */
int factorial(int N)
  { if (N == 0) return 1;
    return N*factorial(N-1); }
```

You can prove it correct by mathematical induction: it is correct for 0, and if it is correct for N−1 then it is correct for N. The recursive program **is** the proof by induction, written as code.

### A questionable program (Program 5.2)

```c listing="Program 5.2"
int puzzle(int N)
  { if (N == 1) return 1;
    if (N % 2 == 0) return puzzle(N/2);
    else return puzzle(3*N+1); }
```

Does this function stop for every N? Nobody in the world knows! (This is the Collatz problem.) The call on `3N+1` is not "smaller", so there is no guarantee. That is a lesson: if you cannot prove the argument gets smaller, you cannot prove the program stops.

```
puzzle(3)  reached 1 after 8 calls
puzzle(27) reached 1 after 112 calls     ← 27 بتطلع لحد 9232 قبل ما تنزل
```

### Euclid (Program 5.3) and prefix evaluation (Program 5.4)

`gcd(m, n) = gcd(n, m % n)`: the second argument shrinks fast (logarithmically), so the depth is small. And evaluating a prefix expression like `* + 7 * * 4 6 + 8 9 5` is natural recursion: "an expression is either a number, or an operator followed by two expressions". The function walks the text with a global pointer and calls itself twice for the two operands:

```c
int eval()
  { int x = 0;
    while (a[i] == ' ') i++;
    if (a[i] == '+') { i++; return eval() + eval(); }
    if (a[i] == '*') { i++; return eval() * eval(); }
    while (a[i] >= '0' && a[i] <= '9') x = 10*x + (a[i++]-'0');
    return x; }
```

The result is 2075, the same expression as the postfix one in lesson 08. There we needed an explicit stack; here the stack is the call stack itself.

* [src/01-fundamentals/euclid.c](src:src/01-fundamentals/euclid.c)

### Recursion on lists (Program 5.5)

A list has a recursive definition (lesson 06), so functions on it come naturally: `count(x) = 0` if it is empty, otherwise `1 + count(x->next)`. And printing in reverse? Swap two lines: call on the rest first, then print.

```
count=6  forward: 5 1 5 2 5 3  reverse: 3 5 2 5 1 5
after delete 5: 1 2 3  (count=3)
```

* [src/05-recursion-trees/recursive\_examples.c](src:src/05-recursion-trees/recursive_examples.c)

:::oops Recursion depth = memory
Every call takes a slot on the stack. `count` on a list of a million items means a million frames, and it will most likely overflow the stack. Sedgewick warns that these simple list functions are better written as loops in practice, and that the recursion here is for understanding. The rule: if the depth can reach N, think about a non recursive version. If the depth is log N (like gcd), no worries.
:::

## 5.2 Divide and conquer

Split the problem into two parts (usually halves), solve each part recursively, and combine the two results.

### The maximum (Program 5.6)

```c listing="Program 5.6"
Item max(Item a[], int l, int r)
  { Item u, v; int m = (l+r)/2;
    if (l == r) return a[l];
    u = max(a, l, m);
    v = max(a, m+1, r);
    if (u > v) return u; else return v; }
```

It is not faster than an ordinary loop, but it is a model of the pattern. **Property 5.1:** a function that divides a problem of size N into two independent, nonempty parts that it solves recursively calls itself **fewer than N times** (counting the original call and the leaves: 2N−1). On 11 items we measured exactly **21 calls**.

### The towers of Hanoi (Program 5.7)

To move N disks: move N−1 to the spare peg, move the largest, move the N−1 on top of it. **Property 5.2:** the number of moves `T(N) = 2T(N−1) + 1` = **2^N − 1**. For 64 disks at one move per second? More than 5 billion centuries.

```c listing="Program 5.7"
void hanoi(int N, int d)
  { if (N == 0) return;
    hanoi(N-1, -d);
    shift(N, d);
    hanoi(N-1, -d); }
```

* [src/05-recursion-trees/hanoi.c](src:src/05-recursion-trees/hanoi.c)

### The ruler (Programs 5.8 and 5.9) and the secret

You want to draw the marks on a ruler: a long mark in the middle, shorter ones at the quarters, shorter still at the eighths… That has exactly the same shape as Hanoi:

```c
void rule(int l, int r, int h)
  { int m = (l+r)/2;
    if (h > 0)
      { rule(l, m, h-1);
        mark(m, h);
        rule(m, r, h-1); } }
```

And here is the lovely observation: **the disk moved at step k in Hanoi = the length of mark k on the ruler** = 1 + the number of trailing zeros of k in binary. So you can solve Hanoi with no recursion at all: the smallest disk moves on every odd step, and on even steps there is only one legal move.

```
height(i) = 1 + trailing zeros of i: 1 2 1 3 1 2 1 4 1 2 1 3 1 2 1
```

And Program 5.9 draws the ruler **bottom up** with no recursion: draw all the marks of length 1, then of length 2, and so on. The same result in a different order. That is the same difference as between top down and bottom up mergesort (lesson 19).

```
recursive, inorder (Program 5.8): order 1 2 3 4 5 6 7 8 ...
recursive, preorder:              order 8 4 2 1 3 6 5 7 ...
nonrecursive, bottom-up (5.9):    order 1 3 5 7 9 11 13 15 2 6 10 14 4 12 8
```

* [src/05-recursion-trees/ruler.c](src:src/05-recursion-trees/ruler.c)

:::problem
The book shows "star fractal" drawings built on the same idea: a square in the middle and four smaller copies at the corners, recursively. And it ends by noting that many algorithms to come are divide and conquer: binary search (one part only), mergesort and quicksort (two parts), and the FFT. The shape is the same; the difference is how much the split costs and how much the combine costs.
:::

:::tip How do you think recursively without getting lost?
Don't trace every call in your head. Assume the smaller call works correctly (just like induction), and ask yourself only: am I using its result correctly? And is the base case right?
:::

:::quiz
Why can't we prove that `puzzle` (Program 5.2) stops?

1. [ ] Because it has no base case
2. [x] Because the call on 3N+1 is not smaller, so no value is guaranteed to shrink
3. [ ] Because it uses %

> It has a base case (N==1), but no guarantee that we ever reach it. This is the open Collatz problem.
:::

:::quiz
Max by divide and conquer on 16 items: how many calls to the function in total?

1. [ ] 16
2. [x] 31
3. [ ] 64

> 2N − 1: every call that is not a leaf splits, giving 16 leaves + 15 internal nodes.
:::

:::quiz
In Hanoi, which disk moves on move number 12?

1. [ ] 1
2. [x] 3
3. [ ] 4

> 12 = 1100 in binary, with two trailing zeros, so 1 + 2 = 3, just like the ruler mark.
:::

:::quiz
Which function's recursion is most dangerous for the stack if N = a million?

1. [ ] gcd(N, M)
2. [ ] max by divide and conquer
3. [x] count on a list of length N

> count has depth N. gcd has logarithmic depth, and max has depth log N as well.
:::

:::quiz
Why did recursive prefix evaluation not need an explicit stack?

1. [ ] Because prefix has no operators
2. [x] Because the call stack itself holds the waiting operands
3. [ ] Because it reads from the right

> Every call to `eval()` waits for the results of the calls inside it, which is exactly LIFO.
:::
