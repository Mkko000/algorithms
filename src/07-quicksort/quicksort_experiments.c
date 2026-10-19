/* ============================================================
 *  quicksort_experiments.c  —  Measure what Chapter 7 claims
 *  Chapter 7: Quicksort (Sections 7.2, 7.4, 7.5, 7.6)
 *
 *  1) Property 7.2: ~2N ln N (= 1.39 N lg N) comparisons on random input.
 *  2) Property 7.1: ~N^2/2 on an already sorted file (naive pivot a[r]).
 *  3) Section 7.4: cutoff M for small subfiles, then ONE insertion sort
 *     pass at the end; total cost is flat for M in about 5..20.
 *  4) Section 7.5: median-of-three removes the sorted-file worst case and
 *     cuts the average by ~5%; the sentinels it plants also simplify the
 *     inner loops.
 *  5) Section 7.6: with many duplicate keys, 3-way partitioning wins big;
 *     and a partition that SCANS OVER keys equal to the pivot instead of
 *     stopping on them goes quadratic when all keys are equal.
 *  6) Timing (clock()) of the variants on 2 million random ints, to
 *     compare with the book's "20 to 25 percent" for median-of-3 + cutoff.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 7.1, 7.2, 7.4, 7.5
 *  Build:   gcc -std=c99 -Wall -O2 quicksort_experiments.c -o quicksort_experiments -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

static long C;
#define less(A, B) (C++, (A) < (B))
#define exch(A, B) { int t_ = A; A = B; B = t_; }
#define compexch(A, B) if (less(B, A)) exch(A, B)
#define eq(A, B) ((A) == (B))

static int M = 0, med3 = 0;

static int partition(int a[], int l, int r) {
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
static void insertion(int a[], int l, int r) {
    for (int i = l + 1; i <= r; i++) { int v = a[i], j = i; while (j > l && less(v, a[j-1])) { a[j] = a[j-1]; j--; } a[j] = v; }
}
static void qs(int a[], int l, int r) {
    if (r - l <= M) return;
    if (med3 && r - l > 2) {
        exch(a[(l + r) / 2], a[r - 1]);
        compexch(a[l], a[r - 1]); compexch(a[l], a[r]); compexch(a[r - 1], a[r]);
        int i = partition(a, l + 1, r - 1);
        qs(a, l, i - 1); qs(a, i + 1, r);
    } else {
        int i = partition(a, l, r);
        qs(a, l, i - 1); qs(a, i + 1, r);
    }
}
static void sort(int a[], int l, int r) { qs(a, l, r); if (M > 0 || med3) insertion(a, l, r); }

static void qs3(int a[], int l, int r) {           /* Program 7.5 */
    if (r <= l) return;
    int i = l - 1, j = r, p = l - 1, q = r, k, v = a[r];
    for (;;) {
        while (less(a[++i], v)) ;
        while (less(v, a[--j])) if (j == l) break;
        if (i >= j) break;
        exch(a[i], a[j]);
        if (eq(a[i], v)) { p++; exch(a[p], a[i]); }
        if (eq(v, a[j])) { q--; exch(a[q], a[j]); }
    }
    exch(a[i], a[r]); j = i - 1; i = i + 1;
    for (k = l; k <= p; k++, j--) exch(a[k], a[j]);
    for (k = r - 1; k >= q; k--, i++) exch(a[k], a[i]);
    qs3(a, l, j); qs3(a, i, r);
}

/* the WRONG way: scan over keys equal to v (Section 7.6, Exercise 7.36) */
static int partition_scan(int a[], int l, int r) {
    int i = l - 1, j = r, v = a[r];
    for (;;) {
        while (++i < r && (C++, a[i] <= v)) ;
        while (--j > l && (C++, a[j] >= v)) ;
        if (i >= j) break;
        exch(a[i], a[j]);
    }
    exch(a[i], a[r]);
    return i;
}
static void qs_scan(int a[], int l, int r) {
    while (r > l) {                       /* loop on the big side: no deep recursion */
        int i = partition_scan(a, l, r);
        if (i - l < r - i) { qs_scan(a, l, i - 1); l = i + 1; }
        else               { qs_scan(a, i + 1, r); r = i - 1; }
    }
}

static void fill(int *a, int n, int kind) {
    for (int i = 0; i < n; i++) a[i] = kind == 1 ? i : kind == 2 ? rand() % 10 : rand();
}
static void check(int *a, int n) { for (int i = 1; i < n; i++) if (a[i-1] > a[i]) { puts("BUG"); exit(1); } }

int main(void) {
    int N = 100000; int *a = malloc(N * sizeof *a);
    srand(42);
    fill(a, N, 0); C = 0; M = 0; med3 = 0; sort(a, 0, N - 1); check(a, N);
    printf("random  N=%d: %ld compares, 2N ln N = %.0f\n", N, C, 2 * N * log(N));

    int Ns = 4000; fill(a, Ns, 1); C = 0; sort(a, 0, Ns - 1); check(a, Ns);
    printf("sorted  N=%d (naive pivot): %ld compares, N^2/2 = %d\n", Ns, C, Ns * Ns / 2);
    fill(a, Ns, 1); C = 0; med3 = 1; sort(a, 0, Ns - 1); check(a, Ns); med3 = 0;
    printf("sorted  N=%d (median-of-3): %ld compares\n", Ns, C);

    printf("cutoff M (random N=%d):", N);
    for (M = 0; M <= 40; M += 5) { srand(42); fill(a, N, 0); C = 0; sort(a, 0, N - 1); check(a, N); printf(" M=%d:%ld", M, C); }
    putchar('\n'); M = 0;
    srand(42); fill(a, N, 0); C = 0; med3 = 1; M = 10; sort(a, 0, N - 1); check(a, N);
    printf("median-of-3 + M=10 (Program 7.4): %ld compares\n", C); med3 = 0; M = 0;

    srand(7); fill(a, N, 2); C = 0; sort(a, 0, N - 1); check(a, N);
    printf("10 distinct keys, standard : %ld compares\n", C);
    srand(7); fill(a, N, 2); C = 0; qs3(a, 0, N - 1); check(a, N);
    printf("10 distinct keys, 3-way    : %ld compares\n", C);

    int Ne = 20000;
    for (int i = 0; i < Ne; i++) a[i] = 7;
    C = 0; sort(a, 0, Ne - 1);
    printf("all %d keys equal, stop on equal  : %9ld compares\n", Ne, C);
    for (int i = 0; i < Ne; i++) a[i] = 7;
    C = 0; qs_scan(a, 0, Ne - 1);
    printf("all %d keys equal, scan over equal: %9ld compares (quadratic)\n", Ne, C);
    free(a);

    /* timing on a big random file */
    int Nt = 2000000; int *b = malloc(Nt * sizeof *b), *c = malloc(Nt * sizeof *c);
    srand(1); for (int i = 0; i < Nt; i++) b[i] = rand();
    const char *nm[] = {"Program 7.1 (no cutoff)", "cutoff M=10", "median-of-3 + M=10 (7.4)", "3-way (7.5)"};
    double base = 0;
    printf("time, N=%d random:\n", Nt);
    for (int v = 0; v < 4; v++) {
        for (int i = 0; i < Nt; i++) c[i] = b[i];
        M = (v == 1 || v == 2) ? 10 : 0; med3 = (v == 2);
        clock_t t0 = clock();
        if (v == 3) qs3(c, 0, Nt - 1); else sort(c, 0, Nt - 1);
        double t = (double)(clock() - t0) / CLOCKS_PER_SEC;
        check(c, Nt);
        if (v == 0) base = t;
        printf("  %-26s %.3fs  (%+.0f%%)\n", nm[v], t, 100 * (t - base) / base);
    }
    M = 0; med3 = 0;
    free(b); free(c);
    return 0;
}
