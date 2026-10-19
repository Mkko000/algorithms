/* ============================================================
 *  select_nonrecursive.c  —  Selection (k-th smallest) without recursion
 *  Chapter 7: Quicksort (Section 7.8)
 *
 *  Program 7.6 recurses into ONE side, so the recursion is a tail call
 *  and turns into a loop (Program 7.7): keep shrinking [l, r] until the
 *  partitioning element lands exactly on position k. Average cost is
 *  linear (Property 7.4: about (2 + 2 ln 2) N compares for the median),
 *  worst case quadratic like quicksort. After select, a[0..k-1] <= a[k]
 *  <= a[k+1..N-1], which is exactly what "find the top k" needs.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 7.6-7.7
 *  Build:   gcc -std=c99 -Wall -O2 select_nonrecursive.c -o select_nonrecursive -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static long C;
#define less(A, B) (C++, (A) < (B))
#define exch(A, B) { int t = A; A = B; B = t; }

int partition(int a[], int l, int r) {
    int i = l - 1, j = r, v = a[r];
    for (;;) {
        while (less(a[++i], v)) ;
        while (less(v, a[--j])) if (j == l) break;
        if (i >= j) break;
        exch(a[i], a[j]);
    }
    exch(a[i], a[r]);
    return i;
}
void select_nr(int a[], int l, int r, int k) {        /* Program 7.7 */
    while (r > l) {
        int i = partition(a, l, r);
        if (i >= k) r = i - 1;
        if (i <= k) l = i + 1;
    }
}

int main(void) {
    const int N = 200000; int *a = malloc(N * sizeof *a);
    srand(17);
    for (int i = 0; i < N; i++) a[i] = rand();
    int k = N / 2; C = 0;
    select_nr(a, 0, N - 1, k);
    for (int i = 0; i < k; i++) if (a[i] > a[k]) { puts("BUG"); return 1; }
    for (int i = k + 1; i < N; i++) if (a[i] < a[k]) { puts("BUG"); return 1; }
    printf("median of %d random ints = %d\n", N, a[k]);
    printf("compares = %ld   (2+2ln2)N = %.0f   vs sorting ~2N lnN = %.0f\n",
           C, (2 + 2 * log(2.0)) * N, 2.0 * N * log((double)N));
    free(a);
    return 0;
}
