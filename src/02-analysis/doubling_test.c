/* ============================================================
 *  doubling_test.c  —  Empirical analysis: the doubling experiment
 *  Chapter 2: Principles of Algorithm Analysis (Sections 2.1, 2.6, 2.7)
 *
 *  Section 2.1 says: implement, then run on growing inputs and watch
 *  the ratio T(2N)/T(N). Ratio ~2 => linear, ~4 => quadratic,
 *  ~2.1 => N log N, ~1 => logarithmic or constant.
 *  We time sequential search vs binary search (Programs 2.1 and 2.2)
 *  for M searches in a table of N keys, and also count comparisons,
 *  because a machine-independent count is what the analysis predicts
 *  (Properties 2.1-2.3: N/2 average for sequential hits, N for misses,
 *  never more than floor(lg N)+1 for binary search).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 2.1-2.2
 *  Build:   gcc -std=c99 -Wall -O2 doubling_test.c -o doubling_test
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static long cmps;

int search(int a[], int v, int l, int r) {          /* Program 2.1 */
    for (int i = l; i <= r; i++) { cmps++; if (v == a[i]) return i; }
    return -1;
}
int bsearch_(int a[], int v, int l, int r) {        /* Program 2.2 */
    while (r >= l) {
        int m = l + (r - l) / 2; cmps++;
        if (v == a[m]) return m;
        if (v < a[m]) r = m - 1; else l = m + 1;
    }
    return -1;
}

int main(void) {
    const int M = 2000;
    double prevS = 0, prevB = 0;
    printf("%8s %12s %7s %14s %12s %7s %12s\n",
           "N", "seq time", "ratio", "seq cmp/srch", "bin time", "ratio", "bin cmp/srch");
    for (int N = 4000; N <= 128000; N *= 2) {
        int *a = malloc(N * sizeof *a);
        for (int i = 0; i < N; i++) a[i] = 2 * i;     /* sorted, even keys */
        srand(N);
        clock_t t0 = clock(); cmps = 0;
        for (int k = 0; k < M; k++) search(a, 2 * (rand() % N), 0, N - 1);  /* hits */
        double ts = (double)(clock() - t0) / CLOCKS_PER_SEC; long cs = cmps;
        t0 = clock(); cmps = 0;
        for (int k = 0; k < M * 50; k++) bsearch_(a, rand() % (2 * N), 0, N - 1);
        double tb = (double)(clock() - t0) / CLOCKS_PER_SEC; long cb = cmps;
        printf("%8d %11.4fs %7.2f %14.1f %11.4fs %7.2f %12.2f\n", N,
               ts, prevS > 0 ? ts / prevS : 0.0, (double)cs / M,
               tb, prevB > 0 ? tb / prevB : 0.0, (double)cb / (M * 50));
        prevS = ts; prevB = tb;
        free(a);
    }
    printf("\nExpect: sequential ratio ~2 and cmp ~N/2; binary cmp grows by ~1 per doubling.\n");
    return 0;
}
