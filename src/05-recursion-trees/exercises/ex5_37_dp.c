/* ============================================================
 *  ex5_37_dp.c  —  Exercises 5.37, 5.38, 5.40, 5.41, 5.43, 5.44, 5.45
 *  Chapter 5: Recursion and Trees — solved exercises
 *
 *  5.37 Compute F_N mod M with only constant extra space.
 *  5.38 Largest N for which F_N fits in a 64-bit integer.
 *  5.40 Bottom-up DP for  P_N = floor(N/2) + P_floor(N/2) + P_ceil(N/2),
 *       P_0 = 0; compare with N lg N / 2.
 *  5.41/5.43 Top-down DP for the same, and count its recursive calls.
 *  5.44/5.45 Bottom-up and top-down DP for
 *       C_N = N + (1/N) sum_{1<=k<=N} (C_{k-1} + C_{N-k}),  C_0 = 1.
 *
 *  Note: read literally, the 5.40 recurrence gives P_1 = P_1 (ceil(1/2)=1),
 *  so we take P_1 = 0 as the second base case.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Section 5.3, Exercises 5.37-5.45
 *  Build:   gcc -std=c99 -Wall -O2 ex5_37_dp.c -o ex5_37 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdint.h>
#include <math.h>

static long fibmod(long n, long m)                        /* 5.37 */
{
    long a = 0, b = 1 % m;
    for (long i = 0; i < n; i++) { long t = (a + b) % m; a = b; b = t; }
    return a;
}

#define MAXN 1025
static long Pb[MAXN], Pt[MAXN];
static long tcalls;
static long ptop(int n)                                   /* 5.41 */
{
    tcalls++;
    if (n <= 1) return 0;
    if (Pt[n] >= 0) return Pt[n];
    return Pt[n] = n / 2 + ptop(n / 2) + ptop((n + 1) / 2);
}

static double Cb[MAXN], Ct[MAXN];
static long ccalls;
static double ctop(int n)                                 /* 5.45 */
{
    ccalls++;
    if (n == 0) return 1;
    if (Ct[n] > 0) return Ct[n];
    double s = 0;
    for (int k = 1; k <= n; k++) s += ctop(k - 1) + ctop(n - k);
    return Ct[n] = n + s / n;
}

int main(void)
{
    printf("5.37 F_1000 mod 1000000007 = %ld, F_100 mod 997 = %ld\n", fibmod(1000, 1000000007L), fibmod(100, 997));

    uint64_t a = 0, b = 1; int n = 0;
    while (b >= a) { uint64_t t = a + b; if (t < b) break; a = b; b = t; n++; }
    int64_t sa = 0, sb = 1; int sn = 0;
    while (sb <= INT64_MAX - sa) { int64_t t = sa + sb; sa = sb; sb = t; sn++; }
    printf("5.38 largest N: signed 64-bit F_%d = %lld, unsigned 64-bit F_%d = %llu\n",
           sn + 1, (long long)sb, n + 1, (unsigned long long)b);

    Pb[0] = Pb[1] = 0;
    for (int i = 2; i < MAXN; i++) Pb[i] = i / 2 + Pb[i / 2] + Pb[(i + 1) / 2];   /* 5.40 */
    printf("5.40 N, P_N, N lg N / 2, difference:\n");
    for (int i = 16; i < MAXN; i *= 4) printf("     %5d %6ld %9.1f %7.1f\n", i, Pb[i], i * log2(i) / 2, Pb[i] - i * log2(i) / 2);
    printf("     N=1000: P=%ld vs %.1f\n", Pb[1000], 1000 * log2(1000) / 2);

    int ok = 1;
    printf("5.43 top-down calls for P_N (fresh table each N):");
    for (int N = 2; N < MAXN; N++) {
        for (int i = 0; i < MAXN; i++) Pt[i] = -1;
        tcalls = 0; if (ptop(N) != Pb[N]) ok = 0;
        if (N == 23 || N == 64 || N == 1000 || N == 1024) printf("  N=%d:%ld", N, tcalls);
    }
    printf("   (top-down == bottom-up: %s)\n", ok ? "yes" : "NO");

    Cb[0] = 1;
    for (int i = 1; i < MAXN; i++) {                       /* 5.44, O(N) per step with a running sum */
        static double sum = 0; sum += Cb[i - 1];
        Cb[i] = i + 2 * sum / i;
    }
    for (int i = 0; i < MAXN; i++) Ct[i] = 0;
    ccalls = 0; double c23 = ctop(23);
    printf("5.44 C_N:  C_10=%.3f  C_100=%.2f  C_1024=%.1f   (2(N+1)H_N-ish: 2N ln N = %.1f at 1024)\n",
           Cb[10], Cb[100], Cb[1024], 2 * 1024 * log(1024));
    printf("5.45 top-down C_23 = %.4f (bottom-up %.4f), %ld calls\n", c23, Cb[23], ccalls);
    return 0;
}
