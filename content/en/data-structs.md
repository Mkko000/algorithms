Book sections: 3.1 Building Blocks (Definition 3.1, Programs 3.1 to 3.4), 3.2 Arrays (Programs 3.5 to 3.8)

An algorithm without a data structure is like an engine without wheels. Chapter 3 starts at the very bottom: what a "type" is, how to build a new type out of existing ones, and then the simplest and most important structure in the world: **the array**.

:::goal The goal of this lesson:
Understand the definition of a data type, build a `point` type with an interface and an implementation, and use arrays in the book's 4 examples: the sieve, dynamic allocation, a coin flipping simulation, and closest points.
:::

## 3.1 The building blocks

**Definition 3.1:** a data type = **a set of values + a set of operations on those values**. Sedgewick makes an important point: the operations belong to the type, not the other way around. `int` is not just "a whole number", it is also "+, −, \*, / and % behaving this particular way" (division drops the fraction!).

The book uses `typedef` so that a program is not tied to one particular type. Program 3.2 computes the average and standard deviation of random numbers, and if you change `typedef int Number;` to `float`, the program works on other types without touching the rest. That is the seed of the ADT idea (lesson 8).

### A new type: point (Programs 3.3 and 3.4)

We put two things together in a `struct`, and write the functions that work on it in a file of their own:

```c listing="Program 3.3"
/* Point.h — الواجهة (Program 3.3) */
typedef struct { float x; float y; } point;
float distance(point a, point b);

/* Point.c — التنفيذ (Program 3.4) */
#include <math.h>
float distance(point a, point b)
  { float dx = a.x - b.x, dy = a.y - b.y;
    return sqrt(dx*dx + dy*dy); }
```

Why separate them? So that any program that uses points can `#include "Point.h"` and not care about the details. This is the first "contract" between an interface and an implementation in the book.

## 3.2 The array: random access in one step

An array is a row of slots one after another in memory. `a[i]` is computed as "start address + i × slot size", so it takes O(1) whatever i is. That is the main reason arrays are everywhere: the cache loves them, and the code is simple. The downside: the size is usually fixed, and inserting in the middle pushes everything after it along.

### Example 1: the sieve of Eratosthenes (Program 3.5)

Want all the primes up to N? An array of flags: `a[i]=1` means "could still be prime". For each prime i, cross out its multiples. The array here is not "data". It is a **set** represented by flags, and the index is the element. The cost is ≈ N ln ln N, so almost linear.

```c listing="Program 3.5"
for (i = 2; i < N; i++) a[i] = 1;
for (i = 2; i < N; i++)
  if (a[i])
    for (j = i; i*j < N; j++) a[i*j] = 0;
```

* [src/03-data-structures/sieve.c](src:src/03-data-structures/sieve.c)

### Example 2: the size at run time (Program 3.6)

Instead of `#define N 10000`, we read N from the user and ask for the memory with `malloc`. And check the return value! If it is `NULL`, memory ran out.

```c listing="Program 3.6"
int *a = malloc(N * sizeof(int));
if (a == NULL) { printf("Insufficient memory.\n"); return; }
```

### Example 3: the array as counters, flipping coins (Program 3.7)

Flip a coin N times, repeat the experiment M times, and count in `f[i]` how many experiments came up with i heads. Print stars: you get a **bell curve** (the normal approximation to the binomial distribution) out of nothing but an array of counters. The index here is a value, and the slot is how often it occurred. It is the same idea we come back to in key indexed counting (lesson 16).

```
$ ./coin_flip 32 1000
14  116 ************
15  125 *************
16  150 ***************
17  124 *************
```

* [src/03-data-structures/coin\_flip.c](src:src/03-data-structures/coin_flip.c)

### Example 4: an array of structs, closest points (Program 3.8)

N random points in the unit square: how many pairs are closer than d? The direct solution checks every pair (≈ N²/2 distances). It works, but it cannot handle a big N. In lesson 7 we will see Program 3.20 solve it with a grid of lists and do **a thousand times fewer** calculations with the same result.

```c
for (i = 0; i < N; i++)
  for (j = i+1; j < N; j++)
    if (distance(a[i], a[j]) < d) cnt++;
```

* [src/03-data-structures/point\_closest.c](src:src/03-data-structures/point_closest.c)

:::oops Classic array mistakes
(1) Writing `a[N]` in an array of size N. The indices run from 0 to N−1; C will not tell you, it will quietly overwrite the variable next to it. (2) Forgetting to check what `malloc` returned. (3) Thinking that `sizeof(a)` inside a function gives the size of the array. It gives only the size of the pointer.
:::

:::quiz
According to Definition 3.1, a data type is…

1. [ ] A place in memory
2. [x] A set of values and a set of operations on those values
3. [ ] A variable name

> The operations are part of the definition. The same bits behave differently if the type is int or float.
:::

:::quiz
Why does a\[i\] take constant time?

1. [ ] Because the compiler stores all the values in a table
2. [x] Because the address is computed directly: start of the array + i × size of an element
3. [ ] Because arrays are small

> No walking, just one address calculation. That is what linked lists cannot do.
:::

:::quiz
In the sieve, what does the array represent?

1. [ ] The list of primes in order
2. [x] A set: the index is the number, and the value 1 or 0 means still possibly prime or crossed out
3. [ ] A multiplication table

> Representing a set with indexed flags is a trick that comes up again and again (visited in DFS, key indexed search).
:::

:::quiz
In the coin simulation, what does f\[i\] count?

1. [ ] The result of flip number i
2. [x] The number of experiments that came up with exactly i heads
3. [ ] The theoretical probability

> The index is the outcome, the slot is how often it happened. The histogram comes out bell shaped.
:::
