/* ============================================================
 *  ex7_01_quick_basics.c  —  Exercises 7.1, 7.2, 7.5-7.10
 *  Chapter 7: Quicksort — solved exercises
 *
 *  7.1  Show how quicksort sorts  E A S Y Q U E S T I O N.
 *  7.2  Show how the 0-1 file 1 0 0 1 1 1 0 0 0 0 0 1 0 1 0 0 is partitioned.
 *  7.5  Max number of times the largest element can be moved.
 *  7.6  Six files of 10 elements that cost as many compares as sorted order.
 *  7.7  Exact C_N vs 2N ln N for N = 10^3, 10^4, 10^5.
 *  7.8  Compares for N equal keys.   7.9  Compares for two key values.
 *  7.10 Build a best-case file: every partition splits within 1.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 7.1-7.2, Exercises 7.1-7.10
 *  Build:   gcc -std=c99 -Wall -O2 ex7_01_quick_basics.c -o ex7_01 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef int Item;
static long C;                      /* compares */
static int  trace, BIG, bigmoves, N0;
#define less(A, B) (C++, (A) < (B))
static void exch(Item *a, int i, int j) { if (a[i] == BIG || a[j] == BIG) bigmoves += (i != j); Item t = a[i]; a[i] = a[j]; a[j] = t; }

static int partition(Item a[], int l, int r)      /* Program 7.2 */
{
    int i = l - 1, j = r; Item v = a[r];
    for (;;) {
        while (less(a[++i], v)) ;
        while (less(v, a[--j])) if (j == l) break;
        if (i >= j) break;
        exch(a, i, j);
    }
    exch(a, i, r);
    return i;
}
static void show(Item *a, int n, int l, int r, int p)
{
    printf("   ");
    for (int k = 0; k < n; k++) {
        if (k < l || k > r) printf("  ");
        else printf(k == p ? "[%c]" : "%c ", a[k]);
        if (k == p) continue;
    }
    printf("\n");
}
static void quicksort(Item a[], int l, int r)     /* Program 7.1 */
{
    if (r <= l) return;
    int i = partition(a, l, r);
    if (trace) show(a, N0, l, r, i);
    quicksort(a, l, i - 1);
    quicksort(a, i + 1, r);
}

static long count_sort(const int *src, int n) { int b[16]; memcpy(b, src, n * sizeof *b); C = 0; quicksort(b, 0, n - 1); return C; }

/* 7.10: a best-case file.  If a subfile is laid out as  [L][R][v]  with
   every key of L < v < every key of R, Program 7.2 makes no exchange
   except the final one, which swaps v with the FIRST key of R, giving
   [L][v][R rotated left by one].  So: build best-case files for both
   halves recursively, rotate the right one RIGHT by one, and append the
   middle key as the pivot.  Both subfiles then come out best-case.     */
static void best(Item *a, int n, int lo)            /* keys lo .. lo+n-1 */
{
    if (n <= 0) return;
    int nl = (n - 1) / 2, nr = n - 1 - nl;
    best(a, nl, lo);                                  /* L = keys lo .. lo+nl-1 */
    best(a + nl, nr, lo + nl + 1);                    /* R = keys above the pivot */
    if (nr > 1) { Item t = a[nl + nr - 1]; memmove(a + nl + 1, a + nl, (nr - 1) * sizeof *a); a[nl] = t; }
    a[n - 1] = lo + nl;                               /* the pivot: median key */
}

/* 7.6: a file is "degenerate" if every partition leaves one side empty */
static int degenerate(const int *src, int n)
{
    int b[16]; memcpy(b, src, n * sizeof *b);
    int l = 0, r = n - 1;
    while (r > l) {
        int i = partition(b, l, r);
        if (i == l) l++; else if (i == r) r--; else return 0;
    }
    return 1;
}

int main(void)
{
    char s[] = "EASYQUESTION"; int n = 12; Item a[100000];
    for (int i = 0; i < n; i++) a[i] = s[i];
    printf("7.1 quicksort on EASYQUESTION ([x] = partitioning element in final place):\n");
    trace = 1; N0 = n; C = 0; quicksort(a, 0, n - 1); trace = 0;
    printf("   result: "); for (int i = 0; i < n; i++) printf("%c", a[i]); printf("   (%ld compares)\n", C);

    const char *z = "1001110000010100"; int m = (int)strlen(z);
    for (int i = 0; i < m; i++) a[i] = z[i];
    int p = partition(a, 0, m - 1);
    printf("7.2 partition of %s (pivot = last = %c): ", z, z[m - 1]);
    for (int i = 0; i < m; i++) printf("%c", a[i]);
    printf("   pivot at %d\n", p);

    /* 7.5: the largest moves at most ... measure the worst over all 8! files */
    {
        int N = 8, q[8], worst = 0, wq[8];
        for (int i = 0; i < N; i++) q[i] = i;
        int c[8] = { 0 }, k = 1;
        for (;;) {
            int b[8]; memcpy(b, q, sizeof b); BIG = N - 1; bigmoves = 0; quicksort(b, 0, N - 1);
            if (bigmoves > worst) { worst = bigmoves; memcpy(wq, q, sizeof wq); }
            while (k < N && c[k] >= k) { c[k] = 0; k++; }
            if (k >= N) break;
            int sw = (k % 2 == 0) ? 0 : c[k]; int t = q[sw]; q[sw] = q[k]; q[k] = t; c[k]++; k = 1;
        }
        printf("7.5 N=8: the largest key can be moved at most %d times, e.g. file", worst);
        for (int i = 0; i < N; i++) printf(" %d", wq[i]);
        printf("\n");
        BIG = -1;
    }

    /* 7.6: all files of 10 where every partition is degenerate (the
       quadratic worst case); with exact counting, files whose pivots are
       maxima cost one compare more per stage than files whose pivots are minima */
    {
        int N = 10, q[10], srt[10]; for (int i = 0; i < N; i++) srt[i] = q[i] = i;
        long worst = count_sort(srt, N), found = 0, lo = 1 << 30, hi = 0;
        int c[10] = { 0 }, k = 1;
        printf("7.6 sorted file of 10 costs %ld compares; some other degenerate files:\n", worst);
        for (;;) {
            if (degenerate(q, N)) {
                long cc = count_sort(q, N); if (cc < lo) lo = cc; if (cc > hi) hi = cc;
                found++;
                if (found >= 2 && found <= 7) { printf("    "); for (int i = 0; i < N; i++) printf("%d ", q[i]); printf("  %ld compares\n", cc); }
            }
            while (k < N && c[k] >= k) { c[k] = 0; k++; }
            if (k >= N) break;
            int sw = (k % 2 == 0) ? 0 : c[k]; int t = q[sw]; q[sw] = q[k]; q[k] = t; c[k]++; k = 1;
        }
        printf("    %ld degenerate files (= 2^(N-1) = %d), costing %ld..%ld compares (N^2/2 = %d)\n", found, 1 << (N - 1), lo, hi, N * N / 2);
    }

    /* 7.7: C_N = N + 1 + (2/N) sum_{k<N} C_k  (Program 7.1 recurrence), C_0 = C_1 = 0 */
    {
        static double Cn[100001]; double sum = 0; Cn[0] = Cn[1] = 0; sum = 0;
        for (int N = 2; N <= 100000; N++) { sum += Cn[N - 1]; Cn[N] = N + 1 + 2 * sum / N; }
        for (int N = 1000; N <= 100000; N *= 10)
            printf("7.7 N=%6d: exact C_N = %10.0f   2N ln N = %10.0f   ratio %.3f\n", N, Cn[N], 2 * N * log(N), Cn[N] / (2 * N * log(N)));
    }

    /* 7.8 / 7.9 */
    for (int N = 1000; N <= 100000; N *= 10) {
        for (int i = 0; i < N; i++) a[i] = 7;
        C = 0; quicksort(a, 0, N - 1);
        printf("7.8 N=%6d equal keys: %9ld compares  (N lg N = %9.0f)\n", N, C, N * log2(N));
    }
    for (int k = 1000; k <= 50000; k *= 7) {
        int N = 100000; unsigned long long rs = 1;
        for (int i = 0; i < N; i++) a[i] = 1;
        for (int t = 0; t < k; t++) { rs = rs * 6364136223846793005ULL + 1; a[(rs >> 33) % N] = 0; }
        int zeros = 0; for (int i = 0; i < N; i++) zeros += a[i] == 0;
        for (int i = N - 1; i > 0; i--) { rs = rs * 6364136223846793005ULL + 1; int j = (int)((rs >> 33) % (i + 1)); int t = a[i]; a[i] = a[j]; a[j] = t; }
        C = 0; quicksort(a, 0, N - 1);
        printf("7.9 N=100000, %5d zeros / %5d ones: %9ld compares  (N lg N = %.0f)\n",
               zeros, N - zeros, C, N * log2(N));
    }

    /* 7.10 */
    for (int N = 7; N <= 100000; N = N * 4 + 3) {
        best(a, N, 0);
        static int cnt[100001]; int ok = 1; memset(cnt, 0, sizeof cnt);
        for (int i = 0; i < N; i++) { if (a[i] < 0 || a[i] >= N || cnt[a[i]]++) ok = 0; }
        if (N == 7) { printf("7.10 best-case file N=7: "); for (int i = 0; i < N; i++) printf("%d ", a[i]); printf("\n"); }
        C = 0; quicksort(a, 0, N - 1);
        int sorted = 1; for (int i = 1; i < N; i++) if (a[i - 1] > a[i]) sorted = 0;
        printf("     N=%6d: permutation %s, sorted %s, %8ld compares (N lg N = %.0f)\n", N, ok ? "ok" : "BAD", sorted ? "yes" : "NO", C, N * log2(N));
    }
    return 0;
}
