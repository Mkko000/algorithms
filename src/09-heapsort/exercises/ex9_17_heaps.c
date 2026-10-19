/* ============================================================
 *  ex9_17_heaps.c  —  Exercises 9.17, 9.18, 9.21, 9.22, 9.26-9.29, 9.38
 *  Chapter 9: Priority Queues and Heapsort — heaps
 *
 *  9.17 Is an array sorted in descending order a heap?
 *  9.18 In a heap of size 15 with distinct keys, where can (and cannot)
 *       the kth largest key appear, for k = 2, 3, 4?
 *  9.21 The heap that results when E A S Y Q U E S T I O N are inserted
 *       into an initially empty heap.
 *  9.22 The sequence of heaps for P R I O * R * * I * T * Y * * * Q U E * * * U * E
 *  9.26 Minimum number of keys moved by delete the maximum; a heap of size
 *       15 that achieves it.
 *  9.27 Show how heapsort (Program 9.7) sorts E A S Y Q U E S T I O N.
 *  9.28 Show that heapsort is not stable.
 *  9.29 Percentage of time heapsort spends in the construction phase.
 *  9.38 Which PQ would you use to find the 100 smallest of 10^6 numbers?
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 9.3-9.7, Exercises 9.17-9.38
 *  Build:   gcc -std=c99 -Wall -O2 ex9_17_heaps.c -o ex9_17
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static long C, X;                                   /* compares, exchanges */
static int less(int a, int b) { C++; return a < b; }
static void exch(int *a, int i, int j) { X++; int t = a[i]; a[i] = a[j]; a[j] = t; }

static void fixUp(int *a, int k) { while (k > 1 && less(a[k / 2], a[k])) { exch(a, k, k / 2); k = k / 2; } }
static void fixDown(int *a, int k, int N)
{
    while (2 * k <= N) {
        int j = 2 * k;
        if (j < N && less(a[j], a[j + 1])) j++;
        if (!less(a[k], a[j])) break;
        exch(a, k, j); k = j;
    }
}
static void showc(const char *lab, int *a, int N) { printf("   %-6s", lab); for (int i = 1; i <= N; i++) printf("%c", a[i]); printf("\n"); }
static int isheap(int *a, int N) { for (int k = 2; k <= N; k++) if (a[k / 2] < a[k]) return 0; return 1; }

static unsigned long long rs = 99991;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }
static void random_heap(int *a, int N)          /* heapify a random permutation of 1..N */
{
    for (int i = 1; i <= N; i++) a[i] = i;
    for (int i = N; i > 1; i--) { int j = 1 + rnd() % i; int t = a[i]; a[i] = a[j]; a[j] = t; }
    for (int k = N / 2; k >= 1; k--) fixDown(a, k, N);
}

int main(void)
{
    int a[2000001];
    /* 9.17 */
    int d[16]; for (int i = 1; i <= 15; i++) d[i] = 100 - i;
    printf("9.17 descending array is a heap: %s\n", isheap(d, 15) ? "yes" : "no");

    /* 9.18: the kth largest at node p needs its depth(p) ancestors to be larger
       (depth <= k-1) and its whole subtree to be smaller (size <= N-k+1).    */
    printf("9.18 heap of size 15 (formula, then 2,000,000 random heaps to confirm):\n");
    int seen[5][16] = { { 0 } };
    for (int t = 0; t < 2000000; t++) {
        random_heap(a, 15);
        for (int p = 1; p <= 15; p++) { int k = 16 - a[p]; if (k <= 4) seen[k][p] = 1; }
    }
    for (int k = 2; k <= 4; k++) {
        printf("     k=%d can:", k);
        for (int p = 1; p <= 15; p++) {
            int dep = 0; for (int x = p; x > 1; x /= 2) dep++;
            int sz = 0; for (int lo = p, hi = p; lo <= 15; lo = 2 * lo, hi = 2 * hi + 1) sz += (hi < 15 ? hi : 15) - lo + 1;
            int can = dep <= k - 1 && dep >= 1 && sz <= 15 - k + 1;
            if (can) printf(" %d", p);
            if (can != seen[k][p]) printf("(!sample disagrees at %d)", p);
        }
        printf("   cannot: the rest\n");
    }

    /* 9.21 */
    const char *s = "EASYQUESTION"; int N = 0;
    for (int i = 0; s[i]; i++) { a[++N] = s[i]; fixUp(a, N); }
    showc("9.21", a, N);

    /* 9.22 */
    printf("9.22 heaps after each operation:\n");
    const char *ops = "PRIO*R**I*T*Y***QUE***U*E"; N = 0;
    for (int i = 0; ops[i]; i++) {
        char lab[8];
        if (ops[i] == '*') { int m = a[1]; exch(a, 1, N); N--; fixDown(a, 1, N); snprintf(lab, sizeof lab, "*%c", m); }
        else { a[++N] = ops[i]; fixUp(a, N); snprintf(lab, sizeof lab, "+%c", ops[i]); }
        showc(lab, a, N);
    }

    /* 9.26 */
    long best = 1 << 30; int ex[16];
    for (int t = 0; t < 200000; t++) {
        random_heap(a, 15); int b[16]; for (int i = 1; i <= 15; i++) b[i] = a[i];
        X = 0; exch(a, 1, 15); fixDown(a, 1, 14);
        if (X < best) { best = X; for (int i = 1; i <= 15; i++) ex[i] = b[i]; }
    }
    printf("9.26 fewest exchanges in delete-max (N=15, distinct keys): %ld (= %ld key moves with exch, %ld with insertion-style moves)\n", best, 2 * best, best + 1);
    printf("     example heap:"); for (int i = 1; i <= 15; i++) printf(" %d", ex[i]); printf("\n");

    /* 9.27 */
    printf("9.27 heapsort on EASYQUESTION (Program 9.7):\n");
    N = 12; for (int i = 0; i < 12; i++) a[i + 1] = s[i];
    for (int k = N / 2; k >= 1; k--) { fixDown(a, k, N); char lab[8]; snprintf(lab, sizeof lab, "k=%d", k); showc(lab, a, N); }
    while (N > 1) { exch(a, 1, N); fixDown(a, 1, --N); printf("   %-6s", ""); for (int i = 1; i <= 12; i++) printf(i <= N ? "%c" : "%c", a[i]); printf("   heap size %d\n", N); }

    /* 9.28: keys with tags; B1 B2 A */
    {
        int k[4] = { 0, 'B' * 4 + 1, 'B' * 4 + 2, 'A' * 4 + 3 }, M = 3;   /* tag in low bits, compare on key only */
        int b[4]; for (int i = 1; i <= 3; i++) b[i] = k[i];
        /* heapsort comparing key = v/4 */
        for (int pass = 0; pass < 2; pass++) {
            for (int kk = M / 2; kk >= 1; kk--) {
                int x = kk;
                while (2 * x <= M) { int j = 2 * x; if (j < M && b[j] / 4 < b[j + 1] / 4) j++; if (!(b[x] / 4 < b[j] / 4)) break; int t = b[x]; b[x] = b[j]; b[j] = t; x = j; }
            }
            break;
        }
        int n = M;
        while (n > 1) {
            int t = b[1]; b[1] = b[n]; b[n] = t; n--;
            int x = 1;
            while (2 * x <= n) { int j = 2 * x; if (j < n && b[j] / 4 < b[j + 1] / 4) j++; if (!(b[x] / 4 < b[j] / 4)) break; t = b[x]; b[x] = b[j]; b[j] = t; x = j; }
        }
        printf("9.28 heapsort of B1 B2 A -> ");
        for (int i = 1; i <= 3; i++) printf("%c%d ", b[i] / 4, b[i] % 4);
        printf(" (B2 before B1: not stable)\n");
    }

    /* 9.29 */
    printf("9.29 share of heapsort time spent building the heap:\n");
    for (int n = 1000; n <= 1000000; n *= 10) {
        int reps = 2000000 / n + 1; double tb = 0, tt = 0;
        for (int r = 0; r < reps; r++) {
            for (int i = 1; i <= n; i++) a[i] = (int)rnd();
            clock_t c0 = clock();
            for (int k = n / 2; k >= 1; k--) fixDown(a, k, n);
            clock_t c1 = clock();
            int m = n; while (m > 1) { exch(a, 1, m); fixDown(a, 1, --m); }
            clock_t c2 = clock();
            tb += c1 - c0; tt += c2 - c0;
        }
        printf("     N=%7d: %.1f%%\n", n, 100.0 * tb / tt);
    }

    /* 9.38: 100 smallest of 10^6 with a MAX-heap of size 100 */
    {
        int n = 1000000, K = 100; static int h[101]; int sz = 0;
        C = 0;
        for (int i = 0; i < n; i++) {
            int v = (int)rnd();
            if (sz < K) { h[++sz] = v; fixUp(h, sz); }
            else if (less(v, h[1])) { h[1] = v; fixDown(h, 1, K); }
        }
        printf("9.38 100 smallest of 10^6 with a max-heap of 100: %ld compares (%.2f per element; a full sort needs ~%d per element)\n",
               C, (double)C / n, 20);
    }
    return 0;
}
