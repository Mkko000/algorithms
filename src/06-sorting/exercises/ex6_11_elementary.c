/* ============================================================
 *  ex6_11_elementary.c  —  Exercises 6.11-6.15, 6.18, 6.20-6.30
 *  Chapter 6: Elementary Sorting Methods — solved exercises
 *
 *  6.11/6.15/6.20 Trace selection, insertion and bubble sort on
 *        E A S Y Q U E S T I O N   (one line per pass).
 *  6.12 Max / average number of exchanges involving one element in
 *       selection sort.   6.13 A file that maximizes min-updates.
 *  6.14/6.18/6.22 Stability, checked on records with equal keys.
 *  6.24 How many passes does an early-exit test save in bubble sort?
 *  6.26/6.27 Which elementary method is fastest for all-equal keys, and
 *       for reverse order?  6.28 A file where bubble sort uses fewer
 *       comparisons than insertion sort.  6.30 Shaker sort.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 6.2-6.4, Exercises 6.11-6.30
 *  Build:   gcc -std=c99 -Wall -O2 ex6_11_elementary.c -o ex6_11
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int k; int tag; } rec;          /* tag = original position */
static long C, X;                                 /* compares, exchanges */
#define less(A, B) (C++, (A).k < (B).k)
#define exch(A, B) { rec t = A; A = B; B = t; X++; }
#define compexch(A, B) if (less(B, A)) exch(A, B)
static int trace;
static void show(rec *a, int n, const char *lab) { if (!trace) return; printf("   %-4s", lab); for (int i = 0; i < n; i++) printf("%c ", a[i].k); printf("\n"); }

static void selection(rec *a, int l, int r)       /* Program 6.2 */
{
    for (int i = l; i < r; i++) {
        int min = i;
        for (int j = i + 1; j <= r; j++) if (less(a[j], a[min])) min = j;
        exch(a[i], a[min]);
        char b[8]; snprintf(b, sizeof b, "%d", i); show(a, r + 1, b);
    }
}
static void insertion(rec *a, int l, int r)       /* Program 6.3 */
{
    int i;
    for (i = r; i > l; i--) compexch(a[i - 1], a[i]);
    show(a, r + 1, "min");
    for (i = l + 2; i <= r; i++) {
        int j = i; rec v = a[i];
        while (less(v, a[j - 1])) { a[j] = a[j - 1]; j--; }
        a[j] = v;
        char b[8]; snprintf(b, sizeof b, "%d", i); show(a, r + 1, b);
    }
}
static int passes;
static void bubble(rec *a, int l, int r, int early)   /* Program 6.4 (+ 6.24) */
{
    passes = 0;
    for (int i = l; i < r; i++) {
        long x0 = X; passes++;
        for (int j = r; j > i; j--) compexch(a[j - 1], a[j]);
        char b[8]; snprintf(b, sizeof b, "%d", i); show(a, r + 1, b);
        if (early && X == x0) break;
    }
}
static void shaker(rec *a, int l, int r)          /* 6.30, stops when a pass exchanges nothing */
{
    while (l < r) {
        long x0 = X;
        for (int j = r; j > l; j--) compexch(a[j - 1], a[j]);
        l++;
        for (int j = l; j < r; j++) compexch(a[j], a[j + 1]);
        r--;
        if (X == x0) break;
    }
}

static void load(rec *a, const char *s) { int n = (int)strlen(s); for (int i = 0; i < n; i++) { a[i].k = s[i]; a[i].tag = i; } }
static int stable(rec *a, int n) { for (int i = 1; i < n; i++) if (a[i].k == a[i - 1].k && a[i].tag < a[i - 1].tag) return 0; return 1; }

static unsigned long long rs = 99991;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }

int main(void)
{
    const char *E = "EASYQUESTION";
    int n = (int)strlen(E);
    rec a[2000];
    trace = 1;
    printf("6.11 selection:\n"); load(a, E); C = X = 0; selection(a, 0, n - 1); printf("     %ld compares, %ld exchanges, stable: %s\n", C, X, stable(a, n) ? "yes" : "no");
    printf("6.15 insertion:\n"); load(a, E); C = X = 0; insertion(a, 0, n - 1); printf("     %ld compares, stable: %s\n", C, stable(a, n) ? "yes" : "no");
    printf("6.20 bubble:\n");    load(a, E); C = X = 0; bubble(a, 0, n - 1, 0); printf("     %ld compares, %ld exchanges, stable: %s\n", C, X, stable(a, n) ? "yes" : "no");
    trace = 0;

    /* 6.12: count how often each original element takes part in an exchange */
    {
        int N = 1000, cnt[1000], mx = 0; long tot = 0;
        for (int t = 0; t < 20; t++) {
            for (int i = 0; i < N; i++) { a[i].k = (int)(rnd() % 120); a[i].tag = i; cnt[i] = 0; }
            for (int i = 0; i < N - 1; i++) {
                int min = i;
                for (int j = i + 1; j < N; j++) if (a[j].k < a[min].k) min = j;
                cnt[a[i].tag]++; cnt[a[min].tag]++;               /* both items are "involved" */
                rec tmp = a[i]; a[i] = a[min]; a[min] = tmp;
            }
            for (int i = 0; i < N; i++) { tot += cnt[i]; if (cnt[i] > mx) mx = cnt[i]; }
        }
        printf("6.12 selection N=1000 random: max exchanges for one element %d, average %.2f\n", mx, (double)tot / (20.0 * N));
    }
    /* 6.12 worst case: N 1 2 ... N-1 -- the big key is swapped one step right in every pass */
    {
        int N = 10, x[10], inv = 0; x[0] = N; for (int i = 1; i < N; i++) x[i] = i;
        for (int i = 0; i < N - 1; i++) { int min = i; for (int j = i + 1; j < N; j++) if (x[j] < x[min]) min = j; if (x[i] == N || x[min] == N) inv++; int t = x[i]; x[i] = x[min]; x[min] = t; }
        printf("6.12 file 10 1 2 ... 9: key 10 takes part in %d of the 9 exchanges (= N-1)\n", inv);
    }
    /* 6.13: exhaustive search over all permutations of N = 8 and 9 */
    for (int N = 8; N <= 9; N++) {
        int p[9], best = -1, bp[9];
        for (int i = 0; i < N; i++) p[i] = i + 1;
        int c[9] = { 0 }, k = 1;
        for (;;) {
            int x[9], upd = 0; memcpy(x, p, sizeof x);
            for (int i = 0; i < N - 1; i++) { int min = i; for (int j = i + 1; j < N; j++) if (x[j] < x[min]) { min = j; upd++; } int t = x[i]; x[i] = x[min]; x[min] = t; }
            if (upd > best) { best = upd; memcpy(bp, p, sizeof bp); }
            while (k < N && c[k] >= k) { c[k] = 0; k++; }
            if (k >= N) break;
            int sw = (k % 2 == 0) ? 0 : c[k]; int t = p[sw]; p[sw] = p[k]; p[k] = t; c[k]++; k = 1;
        }
        printf("6.13 N=%d: max min-updates %d (reverse order gives %d), e.g.", N, best, (N / 2) * ((N + 1) / 2));
        for (int i = 0; i < N; i++) printf(" %d", bp[i]);
        printf("\n");
    }
    /* 6.24 */
    for (int N = 100; N <= 1000; N *= 10) {
        double sv = 0;
        for (int t = 0; t < 50; t++) { for (int i = 0; i < N; i++) a[i].k = (int)(rnd() % 120), a[i].tag = rnd(); bubble(a, 0, N - 1, 1); sv += (N - 1) - passes; }
        printf("6.24 bubble N=%d: early exit saves %.1f of %d passes on average\n", N, sv / 50, N - 1);
    }
    /* 6.26 / 6.27 / 6.30: all-equal keys and reverse order */
    {
        int N = 1000;
        const char *nm[4] = { "selection", "insertion", "bubble", "shaker" };
        for (int kind = 0; kind < 2; kind++) {
            printf("6.%d %s, N=1000 (compares/exchanges-or-moves):\n", kind ? 27 : 26, kind ? "reverse order" : "all keys equal");
            for (int m = 0; m < 4; m++) {
                for (int i = 0; i < N; i++) { a[i].k = kind ? N - i : 7; a[i].tag = i; }
                C = X = 0;
                if (m == 0) selection(a, 0, N - 1); else if (m == 1) insertion(a, 0, N - 1);
                else if (m == 2) bubble(a, 0, N - 1, 0); else shaker(a, 0, N - 1);
                printf("     %-9s %7ld compares %7ld exchanges\n", nm[m], C, X);
            }
        }
    }
    /* 6.28: reverse order J..A, N = 10: bubble (Program 6.4) always uses
       N(N-1)/2 = 45 compares; insertion (Program 6.3) uses more here.  */
    {
        load(a, "JIHGFEDCBA"); C = 0; bubble(a, 0, 9, 0); long cb = C;
        load(a, "JIHGFEDCBA"); C = 0; insertion(a, 0, 9); long ci = C;
        printf("6.28 file J I H G F E D C B A: bubble %ld compares, insertion %ld compares\n", cb, ci);
    }
    /* 6.30: shaker on random input vs bubble */
    {
        int N = 1000; long cb = 0, cs = 0, xb = 0, xs = 0;
        for (int t = 0; t < 10; t++) {
            rec b2[1000];
            for (int i = 0; i < N; i++) { a[i].k = (int)rnd(); a[i].tag = i; b2[i] = a[i]; }
            C = X = 0; bubble(a, 0, N - 1, 1); cb += C; xb += X;
            C = X = 0; shaker(b2, 0, N - 1); cs += C; xs += X;
        }
        printf("6.30 random N=1000, both with early exit: bubble %ld compares/%ld exch, shaker %ld compares/%ld exch (avg)\n", cb / 10, xb / 10, cs / 10, xs / 10);
    }
    return 0;
}
