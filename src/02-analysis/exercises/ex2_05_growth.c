/* ============================================================
 *  ex2_05_growth.c  —  Exercises 2.5-2.9, 2.12-2.14
 *  Chapter 2: Principles of Algorithm Analysis — solved exercises
 *
 *  2.5  For what values of N is 10 N lg N > 2 N^2 ?
 *  2.6  For what N is N^(3/2) between N (lg N)^2 / 2 and 2 N (lg N)^2 ?
 *  2.7  For what N is 2 N H_N - N < N lg N + 10 N ?
 *  2.9  floor(lg N) + 1 is the number of bits needed to write N in binary.
 *  2.12 Compute H_N using the log function from the math library.
 *  2.13 Compute ceil(lg lg N) efficiently, without library functions.
 *  2.14 How many decimal digits does 1,000,000! have?
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Section 2.3
 *  Build:   gcc -std=c99 -Wall -O2 ex2_05_growth.c -o ex2_05 -lm
 * ============================================================ */
#include <stdio.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* 2.12: H_N = ln N + gamma + 1/(2N) - 1/(12N^2) + ...  (Table 2.3 / Knuth) */
static double harmonic(double n)
{
    const double gamma = 0.57721566490153286;
    return log(n) + gamma + 1.0 / (2 * n) - 1.0 / (12 * n * n);
}
static double harmonic_exact(long n) { double h = 0; for (long k = n; k >= 1; k--) h += 1.0 / k; return h; }

/* 2.13: ceil(lg x) for x >= 1 with shifts only, then apply it twice.
   ceil(lg lg N) = smallest k with 2^(2^k) >= N.                         */
static int ceil_lg(unsigned long long x)        /* smallest k: 2^k >= x */
{
    int k = 0; unsigned long long p = 1;
    while (p < x) { p <<= 1; k++; }
    return k;
}
static int ceil_lglg(unsigned long long n) { return ceil_lg((unsigned long long)ceil_lg(n)); }

static int bits(unsigned long long n) { int b = 0; while (n) { n >>= 1; b++; } return b; }

int main(void)
{
    int lo = -1, hi = -1;
    for (int n = 1; n <= 1000; n++)
        if (10.0 * n * log2(n) > 2.0 * n * n) { if (lo < 0) lo = n; hi = n; }
    printf("2.5  10 N lg N > 2 N^2  for  %d <= N <= %d\n", lo, hi);

    /* 2.6: divide by N: sqrt(N) between (lg N)^2/2 and 2 (lg N)^2.
       The set of N is NOT one interval, so print every run of N.       */
    printf("2.6  N^1.5 between N(lgN)^2/2 and 2N(lgN)^2 for N in:");
    int in = 0; long start = 0;
    for (long n = 2; n <= 2000000; n++) {
        double l2 = log2((double)n), r = sqrt((double)n);
        int now = r > l2 * l2 / 2 && r < 2 * l2 * l2;
        if (now && !in) { start = n; in = 1; }
        if (!now && in) { printf(" [%ld..%ld]", start, n - 1); in = 0; }
    }
    printf("\n");

    long last7 = 0;
    for (long n = 1; n < 200000000L; n += (n < 1000 ? 1 : n / 1000)) {
        double h = n < 100000 ? harmonic_exact(n) : harmonic(n);
        if (2 * n * h - n < n * log2(n) + 10.0 * n) last7 = n; else break;
    }
    printf("2.7  2N H_N - N < N lg N + 10N  holds up to N ~ %ld (about 4.8e7)\n", last7);

    printf("2.9  bits(N) vs floor(lg N)+1:");
    int okb = 1;
    for (unsigned long long n = 1; n < 5000000; n++)
        if (bits(n) != (int)floor(log2((double)n)) + 1) okb = 0;
    printf(" %s for N < 5e6\n", okb ? "equal" : "DIFFERENT");

    printf("2.12 H_N via log:   N=10 %.6f (exact %.6f)   N=1e6 %.9f (exact %.9f)\n",
           harmonic(10), harmonic_exact(10), harmonic(1e6), harmonic_exact(1000000));

    printf("2.13 ceil(lg lg N):");
    unsigned long long tests[] = { 2, 4, 5, 16, 17, 256, 65536, 65537, 1000000, 4294967296ULL };
    for (int i = 0; i < 10; i++) printf(" %llu->%d", tests[i], ceil_lglg(tests[i]));
    printf("\n");

    double d = 0;
    for (long k = 2; k <= 1000000; k++) d += log10((double)k);
    printf("2.14 1000000! has %ld decimal digits (log10 = %.4f);"
           " Stirling: %.4f\n", (long)floor(d) + 1, d,
           (1e6 * log(1e6) - 1e6 + 0.5 * log(2 * M_PI * 1e6)) / log(10));
    return 0;
}
