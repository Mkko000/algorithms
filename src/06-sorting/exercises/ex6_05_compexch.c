/* ============================================================
 *  ex6_05_compexch.c  —  Exercises 6.5, 6.6 (and the 0-1 principle)
 *  Chapter 6: Elementary Sorting Methods — solved exercises
 *
 *  6.5 Give all sequences of three compare-exchange operations that
 *      sort three elements (see Program 6.1).
 *  6.6 Give a sequence of five compare-exchange operations that sorts
 *      four elements.
 *  Method: try every sequence of operations compexch(a[i], a[j]), i < j,
 *  and keep those that sort ALL inputs (every permutation).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 6.1, Exercises 6.5-6.6
 *  Build:   gcc -std=c99 -Wall -O2 ex6_05_compexch.c -o ex6_05
 * ============================================================ */
#include <stdio.h>

static int P[6][2], np;

static int sorts_all(int n, const int *seq, int len)
{
    int a[4], idx[4] = { 0, 1, 2, 3 };
    /* iterate over all n! permutations (n <= 4) */
    int c[4] = { 0 }, k = 0, perm[4];
    for (int i = 0; i < n; i++) perm[i] = idx[i];
    for (;;) {
        for (int i = 0; i < n; i++) a[i] = perm[i];
        for (int s = 0; s < len; s++) { int i = P[seq[s]][0], j = P[seq[s]][1]; if (a[j] < a[i]) { int t = a[i]; a[i] = a[j]; a[j] = t; } }
        for (int i = 1; i < n; i++) if (a[i - 1] > a[i]) return 0;
        /* Heap's algorithm, iterative */
        while (k < n && c[k] >= k) { c[k] = 0; k++; }
        if (k >= n) return 1;
        int sw = (k % 2 == 0) ? 0 : c[k];
        int t = perm[sw]; perm[sw] = perm[k]; perm[k] = t;
        c[k]++; k = 1;
        if (n == 1) return 1;
    }
}

int main(void)
{
    for (int n = 3; n <= 4; n++) {
        np = 0;
        for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) { P[np][0] = i; P[np][1] = j; np++; }
        int len = n == 3 ? 3 : 5, seq[5], total = 1, found = 0;
        for (int i = 0; i < len; i++) total *= np;
        printf("%s: sequences of %d compare-exchanges that sort %d elements:\n", n == 3 ? "6.5" : "6.6", len, n);
        for (int code = 0; code < total; code++) {
            int x = code;
            for (int i = 0; i < len; i++) { seq[i] = x % np; x /= np; }
            if (sorts_all(n, seq, len)) {
                found++;
                if (n == 3 || found <= 6) {
                    printf("   ");
                    for (int i = 0; i < len; i++) printf(" %d:%d", P[seq[i]][0], P[seq[i]][1]);
                    printf("\n");
                }
            }
        }
        printf("   total: %d sequences\n", found);
    }
    /* fewer than 5 for four elements is impossible: check all 4-op sequences */
    np = 6; int seq[4], any = 0;
    for (int code = 0; code < 6 * 6 * 6 * 6; code++) { int x = code; for (int i = 0; i < 4; i++) { seq[i] = x % 6; x /= 6; } if (sorts_all(4, seq, 4)) any = 1; }
    printf("    four compare-exchanges suffice for 4 elements? %s\n", any ? "yes" : "no (5 is optimal)");
    return 0;
}
