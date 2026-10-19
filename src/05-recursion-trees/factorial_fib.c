/* ============================================================
 *  factorial_fib.c  —  Recursion basics + Fibonacci with DP
 *  Chapter 5: Recursion and Trees
 *
 *  Shows the trap of naive recursion (exponential Fib) and the
 *  fix with memoization / bottom-up dynamic programming.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 5.1, 5.10, 5.11
 *  Build:   gcc -std=c99 -Wall -O2 factorial_fib.c -o factorial_fib
 * ============================================================ */
#include <stdio.h>

int factorial(int N) { return (N == 0) ? 1 : N * factorial(N - 1); }

/* Naive Fib: each call fans out into two. Exponential. Don't do this for N>40. */
static long naive_calls, memo_calls;
long fib_naive(int N) {
    naive_calls++;
    if (N == 0) return 0;
    if (N == 1) return 1;
    return fib_naive(N - 1) + fib_naive(N - 2);
}

/* Bottom-up DP: fill a table once. Linear. */
long fib_dp(int N) {
    if (N < 2) return N;
    long a = 0, b = 1;
    for (int i = 2; i <= N; i++) { long t = a + b; a = b; b = t; }
    return b;
}

/* Top-down with memoization. */
#define MAXF 100
static long memo[MAXF];
long fib_memo(int N) {
    memo_calls++;
    if (N < 2) return N;
    if (memo[N]) return memo[N];
    return memo[N] = fib_memo(N - 1) + fib_memo(N - 2);
}

int main(void) {
    printf("10! = %d\n", factorial(10));
    printf("fib_dp(40)   = %ld\n", fib_dp(40));
    printf("fib_memo(40) = %ld\n", fib_memo(40));
    printf("fib_naive(30)= %ld  (slowish; try 40 and you'll wait)\n", fib_naive(30));
    printf("\ncalls to compute F(N): naive vs top-down memo\n");
    for (int n = 10; n <= 30; n += 5) {
        naive_calls = memo_calls = 0;
        for (int i = 0; i < MAXF; i++) memo[i] = 0;
        fib_naive(n); fib_memo(n);
        printf("  N=%2d  naive=%8ld  memo=%3ld\n", n, naive_calls, memo_calls);
    }
    return 0;
}
