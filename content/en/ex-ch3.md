Solved exercises, chapter 3 (Elementary Data Structures), exercises 3.11 to 3.70

Chapter 3 is full of "hands on the code" exercises: arrays, linked lists and strings. That is why this lesson has more code than the earlier ones. And more important than the code is **the way of thinking**: when to use a dummy node, how to make sure no pointers get lost, and how to measure instead of guessing.

:::goal The goal of this lesson:
Practice list processing without pointer mistakes, and connect the sieve, Josephus and random numbers to the analysis you learned in chapter 2.
:::

:::tip A golden rule for linked lists
Before you write a single line, draw the nodes and the arrows on paper, and run the code by hand on a list with 0, 1 and 2 nodes. Most bugs hide in these small cases.
:::

* [src/03-data-structures/exercises/ex3\_11\_arrays.c](src:src/03-data-structures/exercises/ex3_11_arrays.c)
* [src/03-data-structures/exercises/ex3\_30\_josephus.c](src:src/03-data-structures/exercises/ex3_30_josephus.c)
* [src/03-data-structures/exercises/ex3\_36\_list\_ops.c](src:src/03-data-structures/exercises/ex3_36_list_ops.c)
* [src/03-data-structures/exercises/ex3\_57\_strings.c](src:src/03-data-structures/exercises/ex3_57_strings.c)

### Exercise 3.11: a\[i\] = a\[a\[i\]\]

:::problem
Given `int a[99]`, what does the array contain after these two lines?<br>`for (i = 0; i < 99; i++) a[i] = 98-i;`<br>`for (i = 0; i < 99; i++) a[i] = a[a[i]];`
:::

:::think
The trap is that the second loop writes into the same array it reads from. So for each i you have to ask: when I read `a[a[i]]`, has that entry already been changed or not? `a[i]` = 98−i. If 98−i \> i (that is, i \< 49), the entry has not been changed yet, and it holds 98−(98−i) = i. If 98−i \< i, the entry has already been changed.
:::

:::solution
For i ≤ 49: a\[i\] = **i**. For i ≥ 50: a\[i\] = a\[98−i\], and entry 98−i ≤ 48 has already been changed to 98−i, so a\[i\] = **98−i**. The result: 0, 1, …, 49, 48, …, 1, 0.

```
3.11 a[] = 0 1 2 3 ... 47 48 49 48 47 ... 2 1 0
```

If that loop wrote into a different array, the result would be a\[i\] = i in every entry. So the whole difference comes from working in place.
:::

### Exercises 3.13, 3.15 and 3.16: the sieve without the if

:::problem
(3.13) Use the sieve of Eratosthenes (Program 3.5) to count the primes less than N, for N = 10³…10⁶. (3.15) Measure the effect of removing the `if (a[i])` test before the inner loop. (3.16) Analyze the program to explain what you found.
:::

:::think
Instead of measuring time (which depends on the machine), we count how many times the inner loop runs. With the test, the loop only runs for prime i, and takes about N/p − p steps. Without it, it runs for every i ≤ √N. So we are comparing Σ<sub>p prime</sub> N/p with Σ<sub>i</sub> N/i. The first is about N ln ln √N (Mertens' theorem), and the second about N ln √N.
:::

:::solution
```
     N        primes   inner-loop ops (with test)  (without)   ratio
     1000      168           1409                    2543   1.80
    10000     1229          16979                   36872   2.17
   100000     9592         193076                  483516   2.50
  1000000    78498        2122046                 5985493   2.82
3.16 N=1e6 predictions: with test ~ 2118018, without ~ 5985471
```

The number of primes: 168, 1229, 9592, 78498 (well known values, π(N)). Without the test the program is **still correct** (all it does is cross out numbers already crossed out), but it does more work. The gap grows with N: the ratio ln √N ÷ ln ln √N. And the analytic prediction matched the actual count to within 0.2%. That is chapter 2 at work right in front of you.
:::

### Exercises 3.18 and 3.19: the first repeat, and every value

:::problem
Write a program that determines empirically how many random positive integers less than 1000 you can expect to generate before getting a repeated value (3.18), and before seeing every value at least once (3.19).
:::

:::think
Both need the same tool: an array of 1000 flags (this is key indexed, exactly Program 3.5). The theoretical expectation: 3.18 is the birthday problem (√(πM/2)), and 3.19 is the coupon collector problem (M·H<sub>M</sub>). You will come back to both in hashing (chapter 14).
:::

:::solution
```
3.18 first repeat after 40.3 numbers on average (sqrt(pi*1000/2)+1 = 40.6)
3.19 all 1000 values seen after 7472 numbers on average (1000 H_1000 = 7485)
```

Only 40 numbers until the first repeat among 1000 values! That is why collisions in a hash table start very early. But to see all 1000 you need about 7500 numbers, 7.5 times as many. Experiment and theory agree in both cases.
:::

### Exercises 3.28, 3.30 and 3.33: Josephus

:::problem
(3.28) Program 3.9 sets twice as many links as it needs while building the list, because it closes the circle after every insertion. Modify it so it does not do this extra work. (3.30) Use Program 3.9 to compute the Josephus function for M = 2, 3, 5, 10 and N = 10³…10⁶. (3.33) Write a version using an array of indices instead of pointers (Figure 3.6).
:::

:::think
For 3.28: build the nodes in an array, with every node pointing to the next and the last one to the first. That way every link is set exactly once. And to trust the big numbers, write **three** independent versions: pointers, an array of indices, and the mathematical recurrence J(N) = (J(N−1) + M − 1) mod N + 1, and compare them. If they agree, the answer is probably right.
:::

:::solution
```
small check, N=9 M=5 (Figure 3.5): 8
3.30  J(N,M)        N=1000     10000    100000   1000000
      M=2               977      3617     68929    951425
      M=3               604      2692     92620    637798
      M=5               763       646     40333    718997
      M=10               63      9143     77328    630538
list, index-array and recurrence agree: yes
```

For M = 2 there is a closed form: if N = 2^m + L (0 ≤ L \< 2^m), then J = 2L + 1. For example 1000 = 512 + 488, so J = 977 ✓. And the array code is an exact copy of the list version, except `x->next` became `next[x]`:

```c
for (i = 1; i <= N; i++) next[i] = i % N + 1;
x = N;
while (x != next[x])
  { for (i = 1; i < M; i++) x = next[x];
    next[x] = next[next[x]]; }
```

The time: every deletion takes M−1 steps, so O(MN) (that is exercise 3.29). For M = 10 and N = 10⁶ that is 10⁷ steps, under a second.
:::

### Exercises 3.34, 3.35 and 3.36: moving nodes

:::problem
(3.34) Write a function that moves the largest item in a list to the end. (3.35) Move the smallest item to the front. (3.36) Rearrange the list so that the nodes in even positions come after the nodes in odd positions, keeping the order within each group.
:::

:::think
To remove a node from a list you need the node **before it**. So look for "the node before the largest", not the largest itself. And if the largest is the first node, there is no node before it. The fix: a **dummy head** makes the first node like any other. For 3.36: walk once, dealing the nodes onto two lists (odd and even) that have tails, and at the end attach the tail of the odd list to the head of the even list. And don't forget to end the even list with NULL.
:::

:::solution
```c
link odd_then_even(link h)
  { struct node od = {0, NULL}, ev = {0, NULL};
    link o = &od, e = &ev;
    for (int pos = 1; h; h = h->next, pos++)
      if (pos % 2) o = o->next = h; else e = e->next = h;
    e->next = NULL; o->next = ev.next;
    return od.next; }
```

```
list:                   5 9 2 7 1 8 3 6 4
3.34 max to end:        5 2 7 1 8 3 6 4 9
3.35 min to front:      1 5 2 7 8 3 6 4 9
3.36 odd pos, even pos: 1 2 8 6 9 5 7 3 4
```

Check it yourself: the odd positions in 1 5 2 7 8 3 6 4 9 are 1, 2, 8, 6, 9 and the even ones are 5, 7, 3, 4. And there is not a single `malloc`: we are relinking the same nodes.
:::

### Exercises 3.38 and 3.39: copying, and deleting by a condition

:::problem
(3.38) A function that takes a link to a list and returns a link to a copy of it (a new list with the same items in the same order). (3.39) A function that takes a list and a function, and removes every item for which the function returns nonzero.
:::

:::think
For the copy: the common mistake is building the list by inserting at the front, so it comes out reversed. The fix: keep a tail. For the deletion: when you remove `p->next`, **do not move** forward, because the new node now in `p->next` has not been checked yet. That is the most famous bug in this topic: two items in a row need removing, you remove the first and skip over the second.
:::

:::solution
```c
link remove_if(link h, int (*bad)(link))
  { struct node d = {0, h}; link p = &d;
    while (p->next)
      if (bad(p->next)) { link t = p->next; p->next = t->next; free(t); }
      else p = p->next;
    return d.next; }
```

```
3.38 copy:              1 5 2 7 8 3 6 4 9
3.39 remove evens:      1 9 5 7 3        (from 1 2 8 6 9 5 7 3 4)
original untouched:     1 5 2 7 8 3 6 4 9
```

Notice that 2, 8 and 6 were in a row, and all of them were removed. And the original was not affected, because the deletion happened on the copy.
:::

### Exercises 3.53 and 3.54: how many nodes can you reach, with constant memory?

:::problem
(3.53) You have nodes with no NULL links (every node points to itself or to another node). Prove that if you start at any node and follow the links, you eventually enter a cycle. (3.54) Write code that counts how many different nodes you can reach from a given node, without modifying any node and with constant extra memory.
:::

:::think
3\.53: the number of nodes is finite, so if you walk more than N steps you must visit some node twice (pigeonhole). From then on you repeat the same trip forever, because every node has only one next. So the shape is like the letter ρ: a tail of length μ followed by a cycle of length λ. The answer is μ + λ. But you cannot mark the nodes or store them. The idea: "the tortoise and the hare", two pointers moving at different speeds must meet inside the cycle.
:::

:::solution
Using Brent's method: the tortoise waits in place, and the hare runs in steps of powers of 2 (1, 2, 4, …). As soon as it gets back to the tortoise, the number of steps since the last "move" = λ. Then put one pointer at the start and another λ steps ahead of it, and move them together. They meet exactly at the first node of the cycle after μ steps.

```
3.54 from node 0: tail 7 + cycle 12 = 19 distinct nodes (expected 19)
     from a node on the cycle: tail 0 + cycle 12 = 12
```

The time is O(μ + λ) and the memory is 3 pointers and two counters. (And exercise 3.55, whether two links reach the same cycle? Compute λ first, then check whether a point on the other one's cycle shows up within those λ steps.)
:::

### Exercises 3.56, 3.57 and 3.60: strings

:::problem
(3.56) A table of every character in a string and how many times it appears. (3.57) Check whether a string is a palindrome (reads the same both ways), ignoring spaces: "if i had a hifi" must pass. (3.60) Replace every run of spaces (more than one) with a single space.
:::

:::think
3\.56: an array `freq[256]` indexed by the character itself (key indexed again!). 3.57: two pointers from the two ends, each skipping spaces before comparing. 3.60: a read pointer and a write pointer on the same string. Don't write a space if the last character you wrote was a space.
:::

:::solution
```
3.56 "abracadabra":  a 5  b 2  c 1  d 1  r 2
3.57 "if i had a hifi" -> palindrome
3.57 "never odd or even" -> palindrome
3.57 "a man a plan" -> no
3.60 "too many blanks here"
```

All three solutions are O(N) with no extra memory (apart from the 256 counters).
:::

### Exercise 3.62: the longest run of spaces with the fewest reads

:::problem
Write an efficient program that finds the length of the longest run of spaces in a string, examining as few characters as possible. Hint: your program should become faster as the run gets longer.
:::

:::think
The same idea as Boyer and Moore: if the longest run found so far has length b, and you are at position i, look **only** at i+b. If it is not a space, no run longer than b can start anywhere from i to i+b (any run of length b+1 starting there would have to cover i+b). So jump to i+b+1. If it is a space, measure the run around it backward and forward.
:::

:::solution
```
3.62 N=1000000, longest run    10: examined  125068 chars (12.5% of N)
3.62 N=1000000, longest run   100: examined   19873 chars (2.0% of N)
3.62 N=1000000, longest run  1000: examined    2070 chars (0.2% of N)
3.62 N=1000000, longest run 10000: examined   10219 chars (1.0% of N)
```

(A string with a space every 7 characters, and one long run near the start.) The longer the run, the fewer the reads, about N/b. The last case went up again because measuring the run itself costs b reads (10000). So the cost is about N/b + b, which is smallest at b ≈ √N.
:::

### Exercises 3.69 and 3.70: the graph as a matrix and as lists

:::problem
Show the adjacency matrix built by Program 3.18 (3.69), and the adjacency lists built by Program 3.19 (3.70), from the pairs `0-2, 1-4, 2-5, 3-6, 0-4, 6-0, 1-3`.
:::

:::think
In the matrix: the graph is undirected, so every edge puts a 1 in two places (it is symmetric). And notice that Program 3.18 puts 1 on the diagonal too (every vertex is connected to itself). In the lists: Program 3.19 adds every edge **at the front** of the lists of both endpoints. So the order in each list is the reverse of the order in which the edges appeared.
:::

:::solution
```
adjacency matrix:        adjacency lists:
  1 0 1 0 1 0 1            0: 6 4 2
  0 1 0 1 1 0 0            1: 3 4
  1 0 1 0 0 1 0            2: 5 0
  0 1 0 1 0 0 1            3: 1 6
  1 1 0 0 1 0 0            4: 0 1
  0 0 1 0 0 1 0            5: 2
  1 0 0 1 0 0 1            6: 0 3
```

The list for 0 has 6 first because `6-0` was the last edge to touch 0. If the graph were directed (exercise 3.71), each edge would appear only once: in the row or list of its first vertex.
:::

:::quiz
In remove\_if, why can't we do `p = p->next` after a deletion?

1. [ ] Because of the memory leak
2. [x] Because the new node now in p-\>next has not been checked yet, so if two in a row need removing we would skip the second
3. [ ] It's fine, you can

> Try it on 1 2 8 6 9: without this care, the 8 slips through.
:::

:::quiz
Roughly how many random numbers from 0 to 999 come before the first repeat?

1. [ ] 500
2. [x] About 40
3. [ ] 1000

> √(π·1000/2) ≈ 39.6, and the experiment gave 40.3. That is the "birthday paradox".
:::

:::quiz
Why does the sieve without `if (a[i])` still give the right answer?

1. [ ] Because the compiler adds it by itself
2. [x] Because it crosses out multiples of non primes, and those are already crossed out as multiples of their prime factors
3. [ ] It does not give the right answer

> That test is only a performance improvement (about 2.8 times at a million), not part of the algorithm's correctness.
:::
