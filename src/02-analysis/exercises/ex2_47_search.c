/* ============================================================
 *  ex2_47_search.c  —  Exercises 2.47-2.49
 *  Chapter 2: Principles of Algorithm Analysis — solved exercises
 *
 *  2.47 Average number of comparisons used by sequential search
 *       (Program 2.1) when a fraction a of the N searches succeed.
 *  2.48 Estimate the probability that at least one of M random
 *       10-digit numbers matches one of a set of N given values.
 *  2.49 Driver: put M random integers in an array, then count how many
 *       of N random integers match one of them, using sequential search,
 *       for M, N = 10, 100, 1000.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 2.1, Property 2.1, Exercises 2.47-2.49
 *  Build:   gcc -std=c99 -Wall -O2 ex2_47_search.c -o ex2_47 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static long cmps;
static int search(const long a[], long v, int l, int r)      /* Program 2.1 */
{
    for (int i = l; i <= r; i++) { cmps++; if (v == a[i]) return i; }
    return -1;
}

static unsigned long long rs = 2463534242ULL;
static unsigned long long rnd64(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return rs; }

int main(void)
{
    const int N = 1000;
    long a[1000];
    for (int i = 0; i < N; i++) a[i] = i;                 /* distinct keys */
    printf("2.47: N=%d, fraction a successful -> measured vs a(N+1)/2 + (1-a)N\n", N);
    for (double frac = 0; frac <= 1.0001; frac += 0.25) {
        const int S = 200000; cmps = 0;
        for (int s = 0; s < S; s++) {
            int hit = (double)(rnd64() % 1000000) / 1e6 < frac;
            long v = hit ? (long)(rnd64() % N) : N + 1;
            search(a, v, 0, N - 1);
        }
        printf("  a=%.2f  %7.1f  formula %7.1f\n", frac, (double)cmps / S,
               frac * (N + 1) / 2 + (1 - frac) * N);
    }

    printf("\n2.48: P(at least one of M random 10-digit numbers hits a set of N)\n"
           "      = 1 - (1 - N/1e10)^M  ~  M N / 1e10\n");
    for (int M = 10; M <= 1000; M *= 10)
        for (int n = 1000; n <= 1000000; n *= 10)
            printf("  M=%4d N=%7d  %.3g\n", M, n, 1 - pow(1 - n / 1e10, M));

    printf("\n2.49: matches among N random 10-digit numbers vs an array of M\n");
    for (int M = 10; M <= 1000; M *= 10) {
        long *b = malloc(M * sizeof *b);
        for (int i = 0; i < M; i++) b[i] = (long)(rnd64() % 10000000000ULL);
        for (int n = 10; n <= 1000; n *= 10) {
            int hits = 0; cmps = 0;
            for (int s = 0; s < n; s++)
                if (search(b, (long)(rnd64() % 10000000000ULL), 0, M - 1) >= 0) hits++;
            printf("  M=%4d N=%4d  matches %d, comparisons %ld%s\n", M, n, hits, cmps,
                   cmps == (long)M * n ? " (= M*N: every search missed)" : "");
        }
        free(b);
    }
    return 0;
}
