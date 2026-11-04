Solved exercises, chapter 2 (Principles of Algorithm Analysis), exercises 2.5 to 2.49

Chapter 2 has two kinds of exercises: a "light math" kind (compare functions, compute logarithms, solve recurrences) and a "try it yourself" kind (write a driver and measure). We will solve some of each. Every number here comes from a program in the repository, so if you doubt something, run the program yourself.

:::goal The goal of this lesson:
Practice comparing growth functions, solve the basic recurrences exactly (not just approximately), and see how analysis and experiment complete each other.
:::

:::tip Before you start
Write the solution on paper first. "How to think about it" has the road only, and the solution comes at the end.
:::

* [src/02-analysis/exercises/ex2\_05\_growth.c](src:src/02-analysis/exercises/ex2_05_growth.c)
* [src/02-analysis/exercises/ex2\_33\_recurrences.c](src:src/02-analysis/exercises/ex2_33_recurrences.c)
* [src/02-analysis/exercises/ex2\_47\_search.c](src:src/02-analysis/exercises/ex2_47_search.c)

### Exercise 2.5: when is 10 N lg N \> 2N²?

:::problem
For what values of N is 10 N lg N greater than 2N²?
:::

:::think
Divide both sides by 2N (N is positive): the inequality becomes 5 lg N \> N. On the left is a slowly growing function, and on the right a straight line. So N must win in the end. The question then is: where do they cross? Try powers of 2: N = 16 gives 20 \> 16 ✓, and N = 32 gives 25 \< 32 ✗. So they cross between 16 and 32, and then we narrow it down by trial.
:::

:::solution
5 lg 22 = 22.3 \> 22 ✓ and 5 lg 23 = 22.6 \< 23 ✗. At N = 1, the left side is zero. So the answer is: **2 ≤ N ≤ 22**.

```
2.5  10 N lg N > 2 N^2  for  2 <= N <= 22
```

The lesson here: an algorithm costing N lg N, even with a constant 5 times bigger, beats the N² one from N = 23 on. Constants only matter for small problems.
:::

### Exercise 2.6: N^{3/2} between N(lg N)²/2 and 2N(lg N)²

:::problem
For what values of N is N^{3/2} between N(lg N)²/2 and 2N(lg N)²?
:::

:::think
Divide by N: the question becomes when √N lies between (lg N)²/2 and 2(lg N)². Both grow, but at very different rates: at first (lg N)² grows faster, and later √N overtakes it. So the answer may turn out to be **more than one interval**. Don't assume it is a single interval; write a program that scans.
:::

:::solution
```
2.6  N^1.5 between N(lgN)^2/2 and 2N(lgN)^2 for N in: [2..3] [6381..519978]
```

Two intervals! N = 2 and 3 by accident (the numbers are small). After that, from 4 to 6380 √N is below the lower bound, from 6381 to about half a million it is inside, and after that it passes the upper bound. The practical moral: an N^{3/2} algorithm (like shellsort with certain increments, chapter 6) and an "N (lg N)²" one can be very close over the range we actually work in. That is exactly what Sedgewick means when he talks about how hard it is to tell these functions apart by experiment.
:::

### Exercise 2.7: 2N H<sub>N</sub> − N against N lg N + 10N

:::problem
For what values of N is 2N H<sub>N</sub> − N \< N lg N + 10N?
:::

:::think
Divide by N: 2H<sub>N</sub> − 1 \< lg N + 10. Use H<sub>N</sub> ≈ ln N + γ (γ ≈ 0.577). That gives 2 ln N + 0.154 \< 1.443 ln N + 10, so 0.557 ln N \< 9.85, so ln N \< 17.7, so N \< e^{17.7} ≈ 4.8×10⁷. (This is the difference between the number of compares in quicksort, 2N ln N, and N lg N. You will meet it in chapter 7.)
:::

:::solution
For roughly **every N up to about 4.7×10⁷**, and after that the inequality flips.

```
2.7  2N H_N - N < N lg N + 10N  holds up to N ~ 47013693 (about 4.8e7)
```

(The program steps by 0.1% after a thousand, so the number is accurate to 0.1%.) The hand calculation and the program agree.
:::

### Exercises 2.8 and 2.9: logarithms

:::problem
(2.8) What is the smallest N for which log₁₀ log₁₀ N \> 8? (2.9) Prove that ⌊lg N⌋ + 1 is the number of bits needed to write N in binary.
:::

:::think
For 2.8, undo the logarithm twice: log₁₀ N \> 10⁸, so N \> 10^{10⁸}. For 2.9: having b bits means N lies between the smallest and largest numbers with b bits, so 2^{b−1} ≤ N \< 2^b. Take lg of both sides.
:::

:::solution
2\.8: the smallest N is **10^{100000000} + 1**: a number with 100 million digits! So log log N is constant for all practical purposes, below 8 for anything you could ever meet.

2\.9: if N needs exactly b bits (the top bit is 1), then 2^{b−1} ≤ N \< 2^b. Taking lg: b−1 ≤ lg N \< b, so ⌊lg N⌋ = b−1, so b = ⌊lg N⌋ + 1. ∎ The program checked this for every N below 5 million:

```
2.9  bits(N) vs floor(lg N)+1: equal for N < 5e6
```
:::

### Exercises 2.12 and 2.13: H<sub>N</sub> and ⌈lg lg N⌉

:::problem
(2.12) Write a C function that computes H<sub>N</sub> using the `log` function from the math library. (2.13) Write an efficient C function that computes ⌈lg lg N⌉ without any library function.
:::

:::think
For 2.12: don't add up N fractions (that is O(N)). Use the approximation H<sub>N</sub> = ln N + γ + 1/(2N) − 1/(12N²) + … (Table 2.3 in the book has the first two terms). For 2.13: ⌈lg x⌉ is the smallest k with 2^k ≥ x, which you can compute with shifts. Then apply it twice: ⌈lg ⌈lg N⌉⌉ = ⌈lg lg N⌉. (Why is that right? Because ⌈lg y⌉ depends only on the smallest power of 2 that is ≥ y, and taking the ceiling never passes a power of 2.)
:::

:::solution
```c
double harmonic(double n)
  { return log(n) + 0.57721566490153286 + 1/(2*n) - 1/(12*n*n); }

int ceil_lg(unsigned long long x)   /* smallest k with 2^k >= x */
  { int k = 0; unsigned long long p = 1;
    while (p < x) { p <<= 1; k++; }
    return k; }
int ceil_lglg(unsigned long long n) { return ceil_lg(ceil_lg(n)); }
```

```
2.12 H_N via log:   N=10 2.928967 (exact 2.928968)   N=1e6 14.392726723 (exact 14.392726723)
2.13 ceil(lg lg N): 2->0 4->1 5->2 16->2 17->3 256->3 65536->4 65537->5 1000000->5 4294967296->5
```

The approximation is accurate to 6 digits even at N = 10. And notice that ⌈lg lg N⌉ jumps exactly at 2^{2^k}+1 (5, 17, 257, 65537).
:::

### Exercise 2.14: how many digits in a million factorial?

:::problem
How many digits are there in the decimal representation of 1,000,000!?
:::

:::think
The number of digits of x is ⌊log₁₀ x⌋ + 1 (the same idea as 2.9, but in decimal). And log₁₀ N! = Σ log₁₀ k. Or use Stirling: ln N! ≈ N ln N − N + ½ ln(2πN).
:::

:::solution
log₁₀(10⁶!) ≈ 5565708.917, so **5,565,709 digits**. The direct sum and Stirling agree to 4 decimal places:

```
2.14 1000000! has 5565709 decimal digits (log10 = 5565708.9172); Stirling: 5565708.9172
```

This will matter to you in chapter 8: lg N! ≈ N lg N − 1.44N is the lower bound on the number of compares for any sort.
:::

### Exercise 2.20: O(1) is the same as O(2)

:::problem
Prove that O(1) is the same as O(2).
:::

:::think
O(f) is not a number. It is a **set** of functions: g ∈ O(f) if there are constants c₀ and N₀ such that |g(N)| \< c₀ f(N) for all N \> N₀. So you want to prove that the two sets contain the same functions, meaning each set is inside the other.
:::

:::solution
If g ∈ O(1): |g| \< c₀·1 = (c₀/2)·2, so g ∈ O(2) with the constant c₀/2. And if g ∈ O(2): |g| \< c₀·2 = (2c₀)·1, so g ∈ O(1) with the constant 2c₀. ∎ The general idea: O(cf) = O(f) for any constant c \> 0, because the constant is "swallowed" by c₀. That is exactly why we write O(N lg N) without caring about the base of the logarithm.
:::

### Exercises 2.33 to 2.35: tables for Formula 2.2 and 2.3

:::problem
Make a table of the values of C<sub>N</sub> in Formula 2.2 (C<sub>N</sub> = C<sub>N/2</sub> + 1, C₁ = 1) for 1 ≤ N ≤ 32, taking N/2 to mean ⌊N/2⌋. Then with ⌈N/2⌉. Then the same for Formula 2.3 (C<sub>N</sub> = C<sub>N/2</sub> + N).
:::

:::think
The table itself is easy (an array and a loop). The important part is guessing the exact formula from the table: watch where the value goes up. With floor you will see it go up at powers of 2 (2, 4, 8, 16, 32). With ceiling it goes up just after powers of 2 (3, 5, 9, 17). That means ⌊lg N⌋+1 and ⌈lg N⌉+1.
:::

:::solution
```
 N | 2.2fl 2.2ce | 2.3fl 2.3ce
 1 |     1     1 |     0     0
 2 |     2     2 |     2     2
 3 |     2     3 |     3     5
 4 |     3     3 |     6     6
 5 |     3     4 |     7    10
 8 |     4     4 |    14    14
 9 |     4     5 |    15    19
16 |     5     5 |    30    30
17 |     5     6 |    31    36
31 |     5     6 |    56    61
32 |     6     6 |    62    62
```

Formula 2.2: floor gives **⌊lg N⌋ + 1** (exactly the worst case number of compares for binary search, Property 2.3), and ceiling gives **⌈lg N⌉ + 1**. Formula 2.3 at exact powers of 2 = 2N − 2. The floor version is always below 2N, while the ceiling version can pass it a little (C₁₇ = 36 \> 34), but both are "about 2N", as the book says. All of the difference is in the numbers that are not powers of 2.
:::

### Exercises 2.38 and 2.39: Formula 2.4 in three readings, and solving it in binary

:::problem
Make a table of Formula 2.4 (C<sub>N</sub> = 2C<sub>N/2</sub> + N, C₁ = 0) for N ≤ 32 under three readings: (i) ⌊N/2⌋, (ii) ⌈N/2⌉, (iii) 2C<sub>N/2</sub> meaning C<sub>⌊N/2⌋</sub> + C<sub>⌈N/2⌉</sub>. (2.39) Solve reading (i) using the binary representation of N, like the proof of Formula 2.2.
:::

:::think
Reading (iii) is what actually happens in mergesort (we split N into two halves that can differ by one). For 2.39: unroll the recurrence: C<sub>N</sub> = N + 2⌊N/2⌋ + 4⌊N/4⌋ + … and every term 2^i⌊N/2^i⌋ is N with its lowest i bits cleared. So every term is "N without its tail".
:::

:::solution
```
 N | 2.4fl 2.4ce 2.4mix
 3 |     3     7      5
 5 |     9    19     12
 7 |    13    23     20
 8 |    24    24     24
15 |    41    63     59
16 |    64    64     64
17 |    65   111     70
32 |   160   160    160
closed forms ... : all match
```

(i) C<sub>N</sub> = **Σ\_{i=0}^{⌊lg N⌋−1} 2^i ⌊N/2^i⌋**. At powers of 2 every term = N, which gives exactly N lg N.

(iii) C<sub>N</sub> = **N⌈lg N⌉ − 2^{⌈lg N⌉} + N**: this is the exact worst case number of compares for top down mergesort (you will see it again in chapter 8).

(ii) The odd one: C₁₇ = 111 instead of about 70. Why? Because ⌈17/2⌉ = 9, and 9 goes to 5, 5 to 3, and 3 to 2. Every step rounds up instead of down. The moral: the "small details" of a recurrence change the constants, but both are Θ(N lg N). The program checks both formulas for every N ≤ 32.
:::

### Exercise 2.47: sequential search with a success rate a

:::problem
Give the average number of compares used by Program 2.1 when aN of the searches are successful, for 0 ≤ a ≤ 1.
:::

:::think
A weighted average. Property 2.1: a search hit costs (N+1)/2 on average, and a miss costs exactly N. The overall average = a×(hit) + (1−a)×(miss).
:::

:::solution
**a(N+1)/2 + (1−a)N** = N − a(N−1)/2. For N = 1000:

```
  a=0.00   1000.0  formula  1000.0
  a=0.25    875.5  formula   875.1
  a=0.50    750.6  formula   750.2
  a=0.75    626.1  formula   625.4
  a=1.00    499.4  formula   500.5
```
:::

### Exercises 2.48 and 2.49: the chance of a match, and the driver

:::problem
(2.48) Estimate the probability that at least one of M random 10 digit numbers matches one of a set of N values, for M = 10, 100, 1000 and N = 10³…10⁶. (2.49) Write a driver that generates M random numbers into an array and counts how many of N random numbers match a number in the array (using sequential search), for M, N = 10, 100, 1000.
:::

:::think
Each random number matches with probability N/10¹⁰. The M numbers are independent, so the probability that none matches = (1 − N/10¹⁰)^M. When MN is tiny compared with 10¹⁰, that is about MN/10¹⁰. The moral for the driver: if you use random 10 digit numbers, **almost every search will miss**. So you are only measuring search misses, and you need to know that before you interpret the numbers.
:::

:::solution
```
  M=  10 N=   1000  1e-06
  M= 100 N= 100000  0.001
  M=1000 N=1000000  0.0952
...
  M=1000 N=1000  matches 0, comparisons 1000000 (= M*N: every search missed)
```

Even in the biggest case the probability is under 10%. In the driver not a single number matched, and the number of compares is exactly M×N: every search walks the whole array. So this driver measures the worst case of sequential search, not the general average. If you want to measure search hits, you have to generate keys from the array itself. And that is an important point of all of chapter 2: **know what your data is actually measuring**.
:::

:::quiz
C<sub>N</sub> = C<sub>⌊N/2⌋</sub> + 1 and C₁ = 1. What is C₁₀₀₀?

1. [ ] 9
2. [x] 10
3. [ ] 11

> ⌊lg 1000⌋ + 1 = 9 + 1 = 10. That is the worst case number of compares for binary search on 1000 items.
:::

:::quiz
Why is O(N log₂ N) = O(N log₁₀ N)?

1. [ ] They are not equal
2. [x] Because log₂ N = 3.32·log₁₀ N, and the constant is swallowed by the O (as in exercise 2.20)
3. [ ] Because both are less than N²

> Changing the base of a logarithm means multiplying by a constant, and O(cf) = O(f).
:::

:::quiz
A driver searches for random 10 digit numbers in an array of 1000 random numbers. What does it actually measure?

1. [ ] The average for hits
2. [x] Only search misses (every search walks all N)
3. [ ] A half and half mix

> The probability of a match is about MN/10¹⁰, which is tiny. So nearly every search misses.
:::
