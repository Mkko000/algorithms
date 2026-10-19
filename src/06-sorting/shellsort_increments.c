/* ============================================================
 *  shellsort_increments.c  —  Why the increment sequence matters
 *  Chapter 6: Elementary Sorting Methods (Section 6.6)
 *
 *  Same h-sorting code, different sequences (Table 6.2 in the book):
 *    Shell's original  N/2, N/4, ..., 1   (bad: even increments only
 *                                          meet odd positions at h=1)
 *    Knuth             1 4 13 40 121 ...  (h = 3h+1)  O(N^{3/2}) worst
 *    Sedgewick 1986    1 8 23 77 281 ...  4^{i+1}+3*2^i+1  O(N^{4/3})
 *    Pratt             1 2 3 4 6 8 9 12 ... all 2^p 3^q   O(N (log N)^2)
 *                      (Property 6.11: many passes, but each is cheap)
 *  (Shell's N/2^k is the same as powers of two when N is a power of 2,
 *   the classic BAD choice.)
 *  We count comparisons on random input and on a nasty input for
 *  Shell's sequence (small keys in even positions, large in odd).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 6.5, Table 6.2
 *  Build:   gcc -std=c99 -Wall -O2 shellsort_increments.c -o shellsort_increments
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

static long C;

/* h-sort with the given increments (largest first) */
void shellsort(int a[], int N, const long inc[], int ninc) {
    for (int k = 0; k < ninc; k++) {
        long h = inc[k];
        if (h >= N) continue;
        for (long i = h; i < N; i++) {
            int v = a[i]; long j = i;
            while (j >= h && (C++, v < a[j - h])) { a[j] = a[j - h]; j -= h; }
            a[j] = v;
        }
    }
}

static int build(int which, int N, long inc[]) {
    long t[256]; int n = 0;
    if (which == 0) { for (long h = N / 2; h >= 1; h /= 2) t[n++] = h; }             /* Shell */
    if (which == 1) { long h = 1; while (h < N) { t[n++] = h; h = 3 * h + 1; } }      /* Knuth */
    if (which == 2) { t[n++] = 1; for (int i = 0; ; i++) { long h = (1L << (2*i+2)) + 3 * (1L << i) + 1; if (h >= N) break; t[n++] = h; } }
    if (which == 3) {                                                                  /* Pratt 2^p 3^q */
        for (long p2 = 1; p2 < N; p2 *= 2)
            for (long h = p2; h < N; h *= 3) t[n++] = h;
        for (int i = 1; i < n; i++) { long v = t[i]; int j = i; while (j > 0 && t[j-1] > v) { t[j] = t[j-1]; j--; } t[j] = v; }
    }
    /* return largest first */
    if (which == 0) { for (int i = 0; i < n; i++) inc[i] = t[i]; }
    else            { for (int i = 0; i < n; i++) inc[i] = t[n - 1 - i]; }
    return n;
}

int main(void) {
    const int N = 1 << 15;
    int *a = malloc(N * sizeof *a), *b = malloc(N * sizeof *b);
    const char *name[] = {"Shell N/2^k", "Knuth 3h+1", "Sedgewick 4^i+3*2^i+1", "Pratt 2^p*3^q"};
    srand(9);
    for (int i = 0; i < N; i++) b[i] = rand();
    printf("N=%d, comparisons:\n%-24s %14s %14s\n", N, "sequence", "random", "evens/odds");
    for (int w = 0; w < 4; w++) {
        long inc[256]; int n = build(w, N, inc);
        for (int i = 0; i < N; i++) a[i] = b[i];
        C = 0; shellsort(a, N, inc, n); long cr = C;
        for (int i = 1; i < N; i++) if (a[i-1] > a[i]) { puts("BUG"); return 1; }
        for (int i = 0; i < N; i++) a[i] = (i % 2 == 0) ? i / 2 : N / 2 + i / 2;   /* evens small, odds big */
        C = 0; shellsort(a, N, inc, n);
        printf("%-24s %14ld %14ld   (%d passes)\n", name[w], cr, C, n);
    }
    free(a); free(b);
    return 0;
}
