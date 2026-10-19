/* ============================================================
 *  ex2_33_recurrences.c  —  Exercises 2.33-2.35, 2.38, 2.39
 *  Chapter 2: Principles of Algorithm Analysis — solved exercises
 *
 *  2.33/2.34 Table C_N of Formula 2.2 (C_N = C_{N/2} + 1, C_1 = 1) for
 *            1 <= N <= 32 with N/2 read as floor(N/2), then ceil(N/2).
 *  2.35      Same for Formula 2.3 (C_N = C_{N/2} + N, C_1 = 0).
 *  2.38      Table of Formula 2.4 (C_N = 2 C_{N/2} + N, C_1 = 0) for
 *            (i) floor, (ii) ceil, (iii) C_floor + C_ceil.
 *  2.39      Solve Formula 2.4 with floor(N/2) through the binary
 *            representation of N; check the closed forms against the tables.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Section 2.5
 *  Build:   gcc -std=c99 -Wall -O2 ex2_33_recurrences.c -o ex2_33 -lm
 * ============================================================ */
#include <stdio.h>

#define MAXN 33
static long f22[MAXN], c22[MAXN], f23[MAXN], c23[MAXN];
static long f24[MAXN], c24[MAXN], m24[MAXN];

static int flg(long n) { int k = -1; while (n) { n >>= 1; k++; } return k; }   /* floor lg */
static int clg(long n) { int k = 0; long p = 1; while (p < n) { p <<= 1; k++; } return k; }

int main(void)
{
    f22[1] = c22[1] = 1; f23[1] = c23[1] = 0; f24[1] = c24[1] = m24[1] = 0;
    for (long n = 2; n < MAXN; n++) {
        long fl = n / 2, ce = (n + 1) / 2;
        f22[n] = f22[fl] + 1;      c22[n] = c22[ce] + 1;
        f23[n] = f23[fl] + n;      c23[n] = c23[ce] + n;
        f24[n] = 2 * f24[fl] + n;  c24[n] = 2 * c24[ce] + n;
        m24[n] = m24[fl] + m24[ce] + n;
    }
    printf(" N | 2.2fl 2.2ce | 2.3fl 2.3ce | 2.4fl 2.4ce 2.4mix | lgN+1 ceil+1 | NceilLg-2^ceil+N\n");
    int ok = 1;
    for (long n = 1; n < MAXN; n++) {
        long closed = n * clg(n) - (1L << clg(n)) + n;
        /* 2.39: C_N = sum_{i=0}^{floor lg N - 1} 2^i floor(N/2^i) */
        long s = 0; for (int i = 0; i < flg(n); i++) s += (1L << i) * (n >> i);
        if (s != f24[n] || closed != m24[n] || f22[n] != flg(n) + 1 || c22[n] != clg(n) + 1) ok = 0;
        printf("%2ld | %5ld %5ld | %5ld %5ld | %5ld %5ld %6ld | %5d %6d | %6ld\n",
               n, f22[n], c22[n], f23[n], c23[n], f24[n], c24[n], m24[n],
               flg(n) + 1, clg(n) + 1, closed);
    }
    printf("closed forms (2.2 floor = floor(lg N)+1, 2.2 ceil = ceil(lg N)+1,\n"
           "  2.4 floor = sum 2^i floor(N/2^i), 2.4 mix = N ceil(lg N) - 2^ceil(lg N) + N): %s\n",
           ok ? "all match" : "MISMATCH");
    return 0;
}
