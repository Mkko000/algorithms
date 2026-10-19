/* ============================================================
 *  ex7_34_equal_select.c  —  Exercises 7.34-7.36, 7.41-7.44, 7.46
 *  Chapter 7: Quicksort — equal keys and selection
 *
 *  7.34 What happens when Program 7.5 (three-way partitioning) runs on a
 *       random file with (i) two and (ii) three distinct key values?
 *  7.35 Modify Program 7.1 to return if all keys in the subfile are equal;
 *       compare with Program 7.1 for t = 2, 5, 10 distinct values.
 *  7.36 If the scans in Program 7.2 skip over keys equal to the partitioning
 *       element instead of stopping, show that Program 7.1 becomes quadratic.
 *  7.41 Average compares for select() to find the smallest of N.
 *  7.42 ... to find the (aN)th smallest for a = 0.1 .. 0.9.
 *  7.43 Worst-case compares for select() to find the median.
 *  7.44/7.46 Selection with three-way partitioning: all keys equal to the
 *       median end in place; fast for files with t distinct values.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 7.1, 7.2, 7.5, 7.6, Exercises 7.34-7.46
 *  Build:   gcc -std=c99 -Wall -O2 ex7_34_equal_select.c -o ex7_34 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef int Item;
static long C;
#define less(A, B) (C++, (A) < (B))
#define eq(A, B)   (C++, (A) == (B))
#define exch(A, B) { Item t = A; A = B; B = t; }

static int partition(Item a[], int l, int r)              /* Program 7.2 */
{
    int i = l - 1, j = r; Item v = a[r];
    for (;;) {
        while (less(a[++i], v)) ;
        while (less(v, a[--j])) if (j == l) break;
        if (i >= j) break;
        exch(a[i], a[j]);
    }
    exch(a[i], a[r]);
    return i;
}
static void quicksort(Item a[], int l, int r)             /* Program 7.1 */
{
    if (r <= l) return;
    int i = partition(a, l, r);
    quicksort(a, l, i - 1); quicksort(a, i + 1, r);
}

/* 7.35: first check whether the whole subfile is equal (r-l compares) */
static void quick_alleq(Item a[], int l, int r)
{
    if (r <= l) return;
    int k = l + 1;
    while (k <= r && eq(a[k], a[l])) k++;
    if (k > r) return;
    int i = partition(a, l, r);
    quick_alleq(a, l, i - 1); quick_alleq(a, i + 1, r);
}

/* 7.36: scans that skip over keys equal to v (<= instead of <) */
static int partition_skip(Item a[], int l, int r)
{
    int i = l - 1, j = r; Item v = a[r];
    for (;;) {
        while (i < r - 1 && (C++, a[++i] <= v)) ;
        while (j > l && (C++, v <= a[--j])) ;
        if (i >= j) break;
        exch(a[i], a[j]);
    }
    if (a[i] > v) exch(a[i], a[r]) else i = r;
    return i;
}
static void quick_skip(Item a[], int l, int r)
{
    while (r > l) {                                        /* loop on the big side: no deep recursion */
        int i = partition_skip(a, l, r);
        if (i - l < r - i) { quick_skip(a, l, i - 1); l = i + 1; }
        else               { quick_skip(a, i + 1, r); r = i - 1; }
    }
}

/* Program 7.5: three-way partitioning (Bentley-McIlroy) */
static int stages;
static void quick3(Item a[], int l, int r)
{
    if (r <= l) return;
    stages++;
    Item v = a[r];
    int i = l - 1, j = r, p = l - 1, q = r, k;
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
    quick3(a, l, j); quick3(a, i, r);
}

/* Program 7.6: selection (iterative) */
static void select(Item a[], int l, int r, int k)
{
    while (r > l) {
        int i = partition(a, l, r);
        if (i >= k) r = i - 1;
        if (i <= k) l = i + 1;
    }
}
/* 7.44/7.46: selection with three-way partitioning; returns with every key
   equal to a[k] in place, smaller to the left, larger to the right      */
static void select3(Item a[], int l, int r, int k)
{
    while (r > l) {
        Item v = a[r];
        int i = l - 1, j = r, p = l - 1, q = r, m;
        for (;;) {
            while (less(a[++i], v)) ;
            while (less(v, a[--j])) if (j == l) break;
            if (i >= j) break;
            exch(a[i], a[j]);
            if (eq(a[i], v)) { p++; exch(a[p], a[i]); }
            if (eq(v, a[j])) { q--; exch(a[q], a[j]); }
        }
        exch(a[i], a[r]); j = i - 1; i = i + 1;
        for (m = l; m <= p; m++, j--) exch(a[m], a[j]);
        for (m = r - 1; m >= q; m--, i++) exch(a[m], a[i]);
        if (k <= j) r = j; else if (k >= i) l = i; else return;   /* k inside the equal block */
    }
}

static unsigned long long rs = 314159;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }
static Item a[1000000], b[1000000];
/* 7.43: a file on which every partition element is alternately the largest
   and the smallest key left, so the median survives N-1 stages.  Program 7.2
   leaves a subfile untouched when its pivot is the max, and moves a[l] to
   a[r] when its pivot is the min; we replay those moves on "slots" and give
   each slot its value at the moment it becomes a pivot.                  */
static void worst_median(Item *x, int n)
{
    int *s = malloc(n * sizeof *s), l = 0, r = n - 1, lo = 0, hi = n - 1, turn = 0;
    for (int i = 0; i < n; i++) s[i] = i;
    while (r > l) {
        if (turn ^= 1) { x[s[r]] = hi--; r--; }
        else { x[s[r]] = lo++; int t = s[l]; s[l] = s[r]; s[r] = t; l++; }
    }
    x[s[l]] = lo;
    free(s);
}
static int sorted(const Item *x, int n) { for (int i = 1; i < n; i++) if (x[i - 1] > x[i]) return 0; return 1; }

int main(void)
{
    int N = 100000;
    printf("7.34 Program 7.5 on random files with t distinct values (N=%d):\n", N);
    for (int t = 2; t <= 3; t++) {
        for (int i = 0; i < N; i++) a[i] = rnd() % t;
        C = 0; stages = 0; quick3(a, 0, N - 1);
        printf("     t=%d: %d partitioning stages, %ld compares (%.2f per key), sorted %s\n", t, stages, C, (double)C / N, sorted(a, N) ? "yes" : "NO");
    }
    printf("7.35 compares, Program 7.1 vs 'return if all equal' vs Program 7.5 (N=%d):\n", N);
    for (int t = 2; t <= 10; t += (t == 2 ? 3 : 5)) {
        for (int i = 0; i < N; i++) b[i] = rnd() % t;
        long c1, c2, c3;
        for (int i = 0; i < N; i++) a[i] = b[i];
        C = 0; quicksort(a, 0, N - 1); c1 = C;
        for (int i = 0; i < N; i++) a[i] = b[i];
        C = 0; quick_alleq(a, 0, N - 1); c2 = C;
        int ok = sorted(a, N);
        for (int i = 0; i < N; i++) a[i] = b[i];
        C = 0; stages = 0; quick3(a, 0, N - 1); c3 = C;
        printf("     t=%2d: %9ld   %9ld (%s)   %9ld\n", t, c1, c2, ok ? "sorted" : "BAD", c3);
    }
    printf("7.36 scans that skip equal keys, all keys equal:\n");
    for (int n = 1000; n <= 8000; n *= 2) {
        for (int i = 0; i < n; i++) a[i] = 7;
        C = 0; quick_skip(a, 0, n - 1);
        printf("     N=%5d: %10ld compares   N^2 = %10ld   (Program 7.1: ", n, C, (long)n * n);
        for (int i = 0; i < n; i++) a[i] = 7;
        C = 0; quicksort(a, 0, n - 1); printf("%ld)\n", C);
    }
    printf("7.41/7.42 average compares / N for select (N=%d, 20 trials):\n", N);
    double alphas[] = { 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
    for (int t = 0; t < 10; t++) {
        double al = alphas[t], tot = 0; int k = (int)(al * N);
        for (int rep = 0; rep < 20; rep++) {
            for (int i = 0; i < N; i++) a[i] = (int)rnd();
            C = 0; select(a, 0, N - 1, k); tot += C;
            if (rep == 0) { for (int i = 0; i < N; i++) if ((i < k && a[i] > a[k]) || (i > k && a[i] < a[k])) { printf("BAD select\n"); return 1; } }
        }
        double theory = (al == 0) ? 2 : 2 + 2 * (-al * log(al) - (1 - al) * log(1 - al));
        printf("     a=%.1f: %.3f   (theory 2 + 2H(a) = %.3f)\n", al, tot / 20 / N, theory);
    }
    printf("7.43 median, sorted file vs alternating min/max file:\n");
    for (int n = 1000; n <= 8000; n *= 2) {
        long cs, cw;
        for (int i = 0; i < n; i++) a[i] = i;
        C = 0; select(a, 0, n - 1, n / 2); cs = C;
        worst_median(a, n);
        C = 0; select(a, 0, n - 1, n / 2); cw = C;
        printf("     N=%5d: sorted %9ld (3N^2/8 = %ld)   worst %9ld (N^2/2 = %ld)  a[N/2]=%d\n", n, cs, 3L * n * n / 8, cw, (long)n * n / 2, a[n / 2]);
    }
    printf("7.44/7.46 select3 for the median, t distinct values (N=%d):\n", N);
    for (int t = 2; t <= 10; t += (t == 2 ? 3 : 5)) {
        for (int i = 0; i < N; i++) b[i] = rnd() % t;
        long c1, c3;
        for (int i = 0; i < N; i++) a[i] = b[i];
        C = 0; select(a, 0, N - 1, N / 2); c1 = C;
        for (int i = 0; i < N; i++) a[i] = b[i];
        C = 0; select3(a, 0, N - 1, N / 2); c3 = C;
        int m = a[N / 2], lo = N / 2, hi = N / 2, ok = 1;
        while (lo > 0 && a[lo - 1] == m) lo--;
        while (hi < N - 1 && a[hi + 1] == m) hi++;
        for (int i = 0; i < N; i++) if ((i < lo && a[i] >= m) || (i > hi && a[i] <= m)) ok = 0;
        printf("     t=%2d: select %8ld   select3 %8ld   median %d, equal block [%d..%d] %s\n", t, c1, c3, m, lo, hi, ok ? "ok" : "BAD");
    }
    return 0;
}
