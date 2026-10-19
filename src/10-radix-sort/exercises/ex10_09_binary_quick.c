/* ============================================================
 *  ex10_09_binary_quick.c  —  Exercises 10.9, 10.10, 10.11, 10.12
 *  Chapter 10: Radix Sorting — binary quicksort
 *
 *  10.9  Compare the exchanges used by binary quicksort and by normal
 *        quicksort for the 3-bit file 001 011 101 110 000 001 010 111 110 010.
 *  10.10 Why is sorting the smaller subfile first less important here?
 *  10.11 What happens on the second level of partitioning when binary
 *        quicksort sorts a random permutation of 0..170?
 *  10.12 One preprocessing pass finds the leading bit positions where all
 *        keys agree; binary quicksort then skips them.  Compare times.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 7.1, 10.1, Exercises 10.9-10.12
 *  Build:   gcc -std=c99 -Wall -O2 ex10_09_binary_quick.c -o ex10_09
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static long X, Xr, maxdepth;               /* all exchanges / exchanges of two different positions */
#define exch(A, B) { unsigned t = A; A = B; B = t; X++; }
static int BITS = 3;
#define bit(A, B) (((A) >> (BITS - 1 - (B))) & 1)          /* B = 0: leftmost bit */

static int trace_level = -1, tl_l[8], tl_r[8], nlev;
static void quicksortB(unsigned a[], int l, int r, int w, int depth)   /* Program 10.1 */
{
    int i = l, j = r;
    if (r <= l || w >= BITS) return;
    if (depth > maxdepth) maxdepth = depth;
    while (j != i) {
        while (bit(a[i], w) == 0 && (i < j)) i++;
        while (bit(a[j], w) == 1 && (j > i)) j--;
        if (i != j) Xr++;
        exch(a[i], a[j]);
    }
    if (bit(a[r], w) == 0) j++;
    if (depth == trace_level && nlev < 8) { tl_l[nlev] = j - l; tl_r[nlev] = r - j + 1; nlev++; }
    quicksortB(a, l, j - 1, w + 1, depth + 1);
    quicksortB(a, j, r, w + 1, depth + 1);
}
static int partition(unsigned a[], int l, int r)       /* Program 7.2 */
{
    int i = l - 1, j = r; unsigned v = a[r];
    for (;;) {
        while (a[++i] < v) ;
        while (v < a[--j]) if (j == l) break;
        if (i >= j) break;
        Xr++; exch(a[i], a[j]);
    }
    if (i != r) Xr++;
    exch(a[i], a[r]);
    return i;
}
static void quicksort(unsigned a[], int l, int r) { if (r <= l) return; int i = partition(a, l, r); quicksort(a, l, i - 1); quicksort(a, i + 1, r); }

static unsigned long long rs = 12321;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }
static unsigned A[1000000], B[1000000];

int main(void)
{
    unsigned f[] = { 1, 3, 5, 6, 0, 1, 2, 7, 6, 2 }, g[10];
    for (int i = 0; i < 10; i++) g[i] = f[i];
    BITS = 3; X = Xr = 0; quicksortB(g, 0, 9, 0, 0); long xb = X, xbr = Xr;
    printf("10.9 binary quicksort: ");
    for (int i = 0; i < 10; i++) printf("%u%u%u ", (g[i] >> 2) & 1, (g[i] >> 1) & 1, g[i] & 1);
    printf(" %ld exch calls, %ld real exchanges\n", xb, xbr);
    for (int i = 0; i < 10; i++) g[i] = f[i];
    X = Xr = 0; quicksort(g, 0, 9);
    printf("     normal quicksort (Program 7.1): %ld exch calls, %ld real exchanges\n", X, Xr);

    /* 10.10: recursion depth is bounded by the number of bits */
    BITS = 20; int N = 1000000;
    for (int i = 0; i < N; i++) A[i] = i % (1 << 20);
    maxdepth = 0; quicksortB(A, 0, N - 1, 0, 0);
    printf("10.10 sorted 20-bit file of 10^6 keys: recursion depth %ld (never more than the %d bits)\n", maxdepth, BITS);

    /* 10.11 */
    BITS = 8; int n = 171;
    for (int i = 0; i < n; i++) A[i] = i;
    for (int i = n - 1; i > 0; i--) { int j = rnd() % (i + 1); unsigned t = A[i]; A[i] = A[j]; A[j] = t; }
    trace_level = 0; nlev = 0; quicksortB(A, 0, n - 1, 0, 0);
    printf("10.11 0..170 (8 bits, 171 = 10101011): level 0 split %d | %d\n", tl_l[0], tl_r[0]);
    for (int i = 0; i < n; i++) A[i] = i;
    for (int i = n - 1; i > 0; i--) { int j = rnd() % (i + 1); unsigned t = A[i]; A[i] = A[j]; A[j] = t; }
    trace_level = 1; nlev = 0; quicksortB(A, 0, n - 1, 0, 0);
    printf("      level 1: left subfile splits %d | %d,  right subfile splits %d | %d\n", tl_l[0], tl_r[0], tl_l[1], tl_r[1]);
    trace_level = -1;

    /* 10.12 */
    printf("10.12 32-bit keys whose top 12 bits are all equal (time in ms):\n");
    BITS = 32;
    for (N = 1000; N <= 1000000; N *= 10) {
        for (int i = 0; i < N; i++) B[i] = 0xABC00000u | (rnd() & 0xFFFFF);
        int reps = 1000000 / N; double t1 = 0, t2 = 0;
        for (int r = 0; r < reps; r++) {
            for (int i = 0; i < N; i++) A[i] = B[i];
            clock_t c0 = clock(); quicksortB(A, 0, N - 1, 0, 0); t1 += clock() - c0;
            for (int i = 0; i < N; i++) A[i] = B[i];
            c0 = clock();
            unsigned diff = 0; for (int i = 1; i < N; i++) diff |= A[i] ^ A[0];   /* one pass */
            int skip = 0; while (skip < 32 && !(diff & (0x80000000u >> skip))) skip++;
            quicksortB(A, 0, N - 1, skip, 0);
            t2 += clock() - c0;
            if (r == 0 && N == 1000) printf("     leading equal bits found: %d\n", skip);
        }
        printf("     N=%7d: standard %.3f   skip-leading %.3f   (per sort)\n", N, t1 * 1000 / CLOCKS_PER_SEC / reps, t2 * 1000 / CLOCKS_PER_SEC / reps);
    }
    return 0;
}
