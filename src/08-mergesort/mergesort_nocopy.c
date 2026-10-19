/* ============================================================
 *  mergesort_nocopy.c  —  Two-way merge, and the improved mergesort
 *                         that never copies back
 *  Chapter 8: Merging and Mergesort (Sections 8.1, 8.4, 8.6)
 *
 *  Program 8.1: mergeAB merges sorted a[0..N-1] and b[0..M-1] into c.
 *  Program 8.4: improvements to top-down mergesort:
 *    (1) cutoff to insertion sort for small subfiles (~15% faster),
 *    (2) eliminate the copy to the auxiliary array by switching the
 *        roles of a[] and aux[] at each level of the recursion,
 *    (3) skip the merge entirely if a[m] <= a[m+1] (already in order) —
 *        makes mergesort linear on sorted input (a common extra
 *        tweak, not part of Program 8.4 itself).
 *  We count comparisons and compare with Property 8.1:
 *  about N lg N compares for any input of size N.
 *  Section 8.6 / Table 8.1: a small timing table on 1M random ints:
 *  basic top-down (Program 8.3 with the bitonic merge of 8.2), the
 *  improved Program 8.4, bottom-up (Program 8.5), and quicksort with
 *  median-of-3 + cutoff (Program 7.4) for reference.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 8.1, 8.4
 *  Build:   gcc -std=c99 -Wall -O2 mergesort_nocopy.c -o mergesort_nocopy -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

static long C;
#define less(A, B) (C++, (A) < (B))

/* Program 8.1: merge two separate sorted arrays */
void mergeAB(int c[], const int a[], int N, const int b[], int M) {
    for (int i = 0, j = 0, k = 0; k < N + M; k++) {
        if (i == N) { c[k] = b[j++]; continue; }
        if (j == M) { c[k] = a[i++]; continue; }
        c[k] = less(a[i], b[j]) ? a[i++] : b[j++];
    }
}

static void insertion(int a[], int l, int r) {
    for (int i = l + 1; i <= r; i++) { int v = a[i], j = i; while (j > l && less(v, a[j-1])) { a[j] = a[j-1]; j--; } a[j] = v; }
}

/* Program 8.4: sort b[] into a[] (with aux roles swapped per level) */
static int cutoff = 10, skip_sorted = 1;
static void mergesortABr(int a[], int b[], int l, int r) {
    int m = (l + r) / 2;
    if (r - l <= cutoff) { insertion(a, l, r); return; }
    mergesortABr(b, a, l, m);           /* sort the halves INTO b ...           */
    mergesortABr(b, a, m + 1, r);
    if (skip_sorted && !less(b[m + 1], b[m])) {   /* already in order: just move */
        for (int k = l; k <= r; k++) a[k] = b[k];
        return;
    }
    mergeAB(a + l, b + l, m - l + 1, b + m + 1, r - m);  /* ... and merge back into a */
}
void mergesort(int a[], int l, int r) {
    int *aux = malloc((r + 1) * sizeof *aux);
    for (int i = l; i <= r; i++) aux[i] = a[i];   /* both arrays start equal */
    mergesortABr(a, aux, l, r);
    free(aux);
}

/* ---- for the timing table ---- */
static int *aux2;
static void merge_bitonic(int a[], int l, int m, int r) {      /* Program 8.2 */
    int i, j, k;
    for (i = m + 1; i > l; i--) aux2[i - 1] = a[i - 1];
    for (j = m; j < r; j++)     aux2[r + m - j] = a[j + 1];
    for (k = l; k <= r; k++) a[k] = (aux2[j] < aux2[i]) ? aux2[j--] : aux2[i++];
}
static void ms_basic(int a[], int l, int r) {                    /* Program 8.3 */
    if (r <= l) return;
    int m = (l + r) / 2;
    ms_basic(a, l, m); ms_basic(a, m + 1, r); merge_bitonic(a, l, m, r);
}
static void ms_bu(int a[], int l, int r) {                       /* Program 8.5 */
    for (int m = 1; m <= r - l; m += m)
        for (int i = l; i <= r - m; i += m + m)
            merge_bitonic(a, i, i + m - 1, (i + m + m - 1 < r) ? i + m + m - 1 : r);
}
static void ins(int a[], int l, int r) {
    for (int i = l + 1; i <= r; i++) { int v = a[i], j = i; while (j > l && v < a[j-1]) { a[j] = a[j-1]; j--; } a[j] = v; }
}
#define EXCH(A, B) { int t_ = A; A = B; B = t_; }
static void qs74(int a[], int l, int r) {                        /* Program 7.4 */
    if (r - l <= 10) return;
    EXCH(a[(l + r) / 2], a[r - 1]);
    if (a[r-1] < a[l]) EXCH(a[l], a[r-1]);
    if (a[r] < a[l])   EXCH(a[l], a[r]);
    if (a[r] < a[r-1]) EXCH(a[r-1], a[r]);
    int i = l, j = r - 1, v = a[r - 1];
    for (;;) { while (a[++i] < v) ; while (v < a[--j]) ; if (i >= j) break; EXCH(a[i], a[j]); }
    EXCH(a[i], a[r - 1]);
    qs74(a, l, i - 1); qs74(a, i + 1, r);
}

int main(void) {
    int A[] = {1, 4, 9, 12}, B[] = {2, 3, 10, 11, 20}, Cc[9];
    mergeAB(Cc, A, 4, B, 5);
    printf("mergeAB: "); for (int i = 0; i < 9; i++) printf("%d ", Cc[i]); putchar('\n');

    const int N = 1 << 16; int *a = malloc(N * sizeof *a);
    double nlgn = N * log2((double)N);
    for (int round = 0; round < 3; round++) {
        cutoff = round == 0 ? 0 : 10; skip_sorted = round == 2;
        srand(4); for (int i = 0; i < N; i++) a[i] = rand();
        C = 0; mergesort(a, 0, N - 1);
        for (int i = 1; i < N; i++) if (a[i-1] > a[i]) { puts("BUG"); return 1; }
        long cr = C; C = 0; mergesort(a, 0, N - 1);          /* now sorted input */
        printf("cutoff=%-2d skip=%d: random %8ld compares (N lgN=%.0f), sorted input %8ld\n",
               cutoff, skip_sorted, cr, nlgn, C);
    }
    free(a);

    int Nt = 1000000; int *b = malloc(Nt * sizeof *b), *c = malloc(Nt * sizeof *c);
    aux2 = malloc(Nt * sizeof *aux2);
    srand(11); for (int i = 0; i < Nt; i++) b[i] = rand();
    const char *nm[] = {"top-down, Program 8.3", "improved, Program 8.4", "bottom-up, Program 8.5", "quicksort, Program 7.4"};
    printf("time, N=%d random ints:\n", Nt);
    for (int v = 0; v < 4; v++) {
        for (int i = 0; i < Nt; i++) c[i] = b[i];
        cutoff = 10; skip_sorted = 0;
        clock_t t0 = clock();
        if (v == 0) ms_basic(c, 0, Nt - 1);
        if (v == 1) mergesort(c, 0, Nt - 1);
        if (v == 2) ms_bu(c, 0, Nt - 1);
        if (v == 3) { qs74(c, 0, Nt - 1); ins(c, 0, Nt - 1); }
        double t = (double)(clock() - t0) / CLOCKS_PER_SEC;
        for (int i = 1; i < Nt; i++) if (c[i-1] > c[i]) { puts("BUG"); return 1; }
        printf("  %-24s %.3fs\n", nm[v], t);
    }
    free(b); free(c); free(aux2);
    return 0;
}
