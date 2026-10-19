/* ============================================================
 *  ex3_11_arrays.c  —  Exercises 3.11, 3.13, 3.15, 3.16, 3.18, 3.19
 *  Chapter 3: Elementary Data Structures — solved exercises
 *
 *  3.11 int a[99]; for (i) a[i] = 98-i; for (i) a[i] = a[a[i]];  contents?
 *  3.13 Use the sieve of Eratosthenes to count primes < N, N = 10^3..10^6.
 *  3.15 Measure the effect of removing the test  if (a[i])  that guards
 *       the inner loop of Program 3.5.   3.16 Explain it.
 *  3.18 How many random integers < 1000 until the first repeat?
 *  3.19 How many random integers < 1000 until every value has appeared?
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 3.5, Exercises 3.11-3.19
 *  Build:   gcc -std=c99 -Wall -O2 ex3_11_arrays.c -o ex3_11 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Program 3.5 with an operation counter; guard = keep the if (a[i]) test */
static long sieve(int n, int guard, long *ops)
{
    char *a = malloc(n);
    long cnt = 0; *ops = 0;
    for (int i = 2; i < n; i++) a[i] = 1;
    for (int i = 2; i < n; i++)
        if (!guard || a[i])
            for (long j = i; (long)i * j < n; j++) { a[i * j] = 0; (*ops)++; }
    for (int i = 2; i < n; i++) cnt += a[i];
    free(a);
    return cnt;
}

static unsigned long long rs = 1181783497276652981ULL;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }

int main(void)
{
    int a[99], i;
    for (i = 0; i < 99; i++) a[i] = 98 - i;
    for (i = 0; i < 99; i++) a[i] = a[a[i]];
    printf("3.11 a[] = ");
    for (i = 0; i < 99; i++) if (i < 4 || (i > 46 && i < 52) || i > 95) printf("%d ", a[i]); else if (i == 4 || i == 52) printf("... ");
    printf("\n");

    printf("3.13/3.15  N        primes   inner-loop ops (with test)  (without)   ratio\n");
    for (int n = 1000; n <= 1000000; n *= 10) {
        long o1, o2, p = sieve(n, 1, &o1);
        sieve(n, 0, &o2);
        printf("     %8d %8ld   %12ld            %12ld   %.2f\n", n, p, o1, o2, (double)o2 / o1);
    }
    {   /* 3.16: with the test only primes p <= sqrt(N) run the inner loop:
               sum (N/p - p) ~ N (ln ln sqrt(N) + 0.2615) (Mertens);
               without it every i <= sqrt(N) does:  sum (N/i - i) ~ N (H_sqrtN - 1) - N/2 */
        double n = 1e6, hs = 0; for (int k = 1; k <= 1000; k++) hs += 1.0 / k;
        printf("3.16 N=1e6 predictions: with test ~ %.0f, without ~ %.0f\n",
               n * (log(log(1000.0)) + 0.2615) - 76127, n * (hs - 1) - n / 2);
    }

    const int T = 100000;
    double rep = 0, all = 0;
    char seen[1000];
    for (int t = 0; t < T; t++) {
        for (i = 0; i < 1000; i++) seen[i] = 0;
        int k = 0, v;
        do { v = rnd() % 1000; k++; } while (!seen[v] && (seen[v] = 1));
        rep += k;
    }
    for (int t = 0; t < T / 10; t++) {
        for (i = 0; i < 1000; i++) seen[i] = 0;
        int k = 0, left = 1000;
        while (left) { int v = rnd() % 1000; k++; if (!seen[v]) { seen[v] = 1; left--; } }
        all += k;
    }
    printf("3.18 first repeat after %.1f numbers on average (sqrt(pi*1000/2)+1 = %.1f)\n",
           rep / T, sqrt(M_PI * 1000 / 2) + 1);
    double h = 0; for (i = 1; i <= 1000; i++) h += 1.0 / i;
    printf("3.19 all 1000 values seen after %.0f numbers on average (1000 H_1000 = %.0f)\n",
           all / (T / 10), 1000 * h);
    return 0;
}
