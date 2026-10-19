/* ============================================================
 *  ex8_12_counts.c  —  Exercises 8.12-8.15, 8.17-8.19, 8.30, 8.45
 *  Chapter 8: Merging and Mergesort — counting compares
 *
 *  8.12-8.14 Using the divide-and-conquer tree, prove that mergesort uses
 *       between N lg N and N lg N + N compares.  (checked for N <= 10^5)
 *  8.15 Relate the number of compares to the number of bits in the
 *       positive numbers less than N.
 *  8.17 Average length of the other subfile when the first one runs out.
 *  8.18 If Program 8.3 skips merge when a[m] < a[m+1], how many compares
 *       does that save on an already sorted file?
 *  8.19 How often is the merge skipped for random files?
 *  8.30 Number of passes (tree levels) vs the bits of N.
 *  8.45 Average compares if the split point is random instead of the middle.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 8.1-8.5, Exercises 8.12-8.45
 *  Build:   gcc -std=c99 -Wall -O2 ex8_12_counts.c -o ex8_12 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAXN 1000001
static long Cs[MAXN], W[MAXN];           /* Cs: merge costs N (Program 8.2); W: worst, N-1 */
static int bits(long k) { int b = 0; while (k) { b++; k >>= 1; } return b; }

static int A[MAXN], AUX[MAXN];
static long cmp, skipped, merges, leftover_sum, leftover_cnt;
static void merge(int l, int m, int r)          /* stops comparing when one side is empty */
{
    int i = l, j = m + 1;
    for (int k = l; k <= r; k++) AUX[k] = A[k];
    for (int k = l; k <= r; k++) {
        if (i > m) { A[k] = AUX[j++]; continue; }
        if (j > r) { A[k] = AUX[i++]; continue; }
        cmp++;
        if (AUX[j] < AUX[i]) A[k] = AUX[j++]; else A[k] = AUX[i++];
        if (i > m || j > r) { leftover_sum += (i > m) ? r - j + 1 : m - i + 1; leftover_cnt++; }
    }
}
static void msort(int l, int r, int skip)
{
    if (r <= l) return;
    int m = (l + r) / 2;
    msort(l, m, skip); msort(m + 1, r, skip);
    merges++;
    if (skip) { cmp++; if (A[m] <= A[m + 1]) { skipped++; return; } }
    merge(l, m, r);
}
static int depth(int n) { return n <= 1 ? 0 : 1 + depth((n + 1) / 2); }
static unsigned long long rs = 1618033;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }

int main(void)
{
    Cs[1] = W[1] = 0;
    long bitsum = 0; int ok_bounds = 1, ok_bits = 1, ok_w = 1;
    for (long n = 2; n < MAXN; n++) {
        Cs[n] = Cs[n / 2] + Cs[n - n / 2] + n;
        W[n] = W[n / 2] + W[n - n / 2] + n - 1;
        bitsum += bits(n - 1);                         /* bits of 1 .. n-1 */
        double lg = log2((double)n);
        if (Cs[n] < n * lg - 1e-9 || Cs[n] > n * lg + n + 1e-9) ok_bounds = 0;
        if (Cs[n] != bitsum + n - 1) ok_bits = 0;
        if (W[n] != bitsum) ok_w = 0;
    }
    printf("8.12-8.14 N lg N <= C_N <= N lg N + N for every N <= 10^6: %s\n", ok_bounds ? "yes" : "NO");
    long ns[] = { 10, 16, 31, 32, 33, 39, 1000, 1000000 };
    for (int t = 0; t < 8; t++) { long n = ns[t];
        printf("     N=%7ld: C_N=%9ld  N lg N=%11.1f  N lg N + N=%11.1f\n", n, Cs[n], n * log2(n), n * log2(n) + n); }
    printf("8.15 W_N (worst case, merge costs N-1) = total bits of 1..N-1: %s\n", ok_w ? "yes for all N <= 10^6" : "NO");
    printf("     C_N (merge costs N, Program 8.2)  = that sum + (N - 1):   %s\n", ok_bits ? "yes for all N <= 10^6" : "NO");
    printf("     e.g. N=10: bits 1+2+2+3+3+3+3+4+4 = %ld = W_10\n", W[10]);

    printf("8.17 average leftover when one subfile runs out (random files):\n");
    for (int N = 1000; N <= 1000000; N *= 10) {
        for (int i = 0; i < N; i++) A[i] = (int)rnd();
        cmp = leftover_sum = leftover_cnt = 0; msort(0, N - 1, 0);
        printf("     N=%7d: %.3f elements left over per merge, %ld compares\n", N, (double)leftover_sum / leftover_cnt, cmp);
    }
    printf("8.18 sorted file, skip merge when a[m] <= a[m+1]:\n");
    for (int N = 1000; N <= 1000000; N *= 10) {
        for (int i = 0; i < N; i++) A[i] = i;
        cmp = 0; msort(0, N - 1, 0); long full = cmp;
        cmp = skipped = merges = 0; msort(0, N - 1, 1);
        printf("     N=%7d: plain %8ld   with test %7ld (= N-1)   saved %8ld\n", N, full, cmp, full - cmp);
    }
    printf("8.19 random files, how often is the merge skipped?\n");
    for (int N = 1000; N <= 1000000; N *= 10) {
        for (int i = 0; i < N; i++) A[i] = (int)rnd();
        cmp = skipped = merges = 0; msort(0, N - 1, 1);
        printf("     N=%7d: %6ld of %7ld merges skipped (%.1f%%), skips/N = %.4f\n", N, skipped, merges, 100.0 * skipped / merges, (double)skipped / N);
    }
    int ok30 = 1;
    for (int n = 2; n <= 100000; n++) if (depth(n) != (int)ceil(log2((double)n)) || depth(n) != bits(n - 1)) ok30 = 0;
    printf("8.30 levels of the top-down tree = ceil(lg N) = bits in N-1, for all N <= 10^5: %s\n", ok30 ? "yes" : "NO");
    printf("     (N=8: %d levels, N=9: %d levels)\n", depth(8), depth(9));

    printf("8.45 random split point, C_N = N + (1/(N-1)) sum_{k=1}^{N-1} (C_k + C_{N-k}):\n");
    {
        static double R[100001]; double S = 0; R[1] = 0;
        for (int n = 2; n <= 100000; n++) { S += R[n - 1]; R[n] = n + 2.0 * S / (n - 1); }
        for (int n = 1000; n <= 100000; n *= 10)
            printf("     N=%6d: random split %10.0f   middle split %9ld   ratio %.3f   (2 ln 2 = %.3f)\n",
                   n, R[n], Cs[n], R[n] / Cs[n], 2 * log(2));
    }
    return 0;
}
