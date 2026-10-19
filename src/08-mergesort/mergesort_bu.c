/* ============================================================
 *  mergesort_bu.c  —  Bottom-up mergesort (no recursion)
 *  Chapter 8: Merging and Mergesort (Section 8.5)
 *
 *  Program 8.5: pass through the file merging adjacent subfiles of
 *  size m = 1, 2, 4, 8, ... using the abstract in-place merge
 *  merge(a, l, m, r) of Program 8.2.
 *  Prints the merge sizes of each pass to show:
 *   Property 8.5: in each pass every merge involves subfiles whose size
 *                 is a power of 2, except possibly the last one;
 *   Property 8.6: the number of passes is the number of bits in the
 *                 binary representation of N. Measured: it is ceil(lg N),
 *                 i.e. the bits of N-1; the two agree except when N is an
 *                 exact power of 2 (N=1024: 10 passes, but 1024 has 11 bits).
 *  Also counts comparisons (about N lg N, like top-down).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 8.2, 8.5
 *  Build:   gcc -std=c99 -Wall -O2 mergesort_bu.c -o mergesort_bu
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef int Item;
static long C;
#define less(A, B) (C++, (A) < (B))

#define maxN 100000
static Item aux[maxN];
static int verbose;

/* Program 8.2: copy the second half in reverse (bitonic), no sentinels needed */
static void merge(Item a[], int l, int m, int r) {
    int i, j, k;
    for (i = m + 1; i > l; i--) aux[i - 1] = a[i - 1];
    for (j = m; j < r; j++)     aux[r + m - j] = a[j + 1];
    for (k = l; k <= r; k++)
        if (less(aux[j], aux[i])) a[k] = aux[j--]; else a[k] = aux[i++];
}

#define min(A, B) ((A) < (B) ? (A) : (B))
/* Program 8.5 */
static void mergesortBU(Item a[], int l, int r) {
    int i, m;
    for (m = 1; m <= r - l; m = m + m) {
        if (verbose) printf("  m=%-3d merges:", m);
        for (i = l; i <= r - m; i += m + m) {
            int hi = min(i + m + m - 1, r);
            if (verbose) printf(" %d+%d", m, hi - (i + m) + 1);
            merge(a, i, i + m - 1, hi);
        }
        if (verbose) putchar('\n');
    }
}

static int bits(int n) { int b = 0; while (n) { b++; n >>= 1; } return b; }

int main(void) {
    char s[] = "ASORTINGEXAMPLE";
    int n = sizeof s - 1;
    Item a[32];
    for (int i = 0; i < n; i++) a[i] = s[i];
    printf("N=%d (binary has %d bits):\n", n, bits(n));
    verbose = 1; mergesortBU(a, 0, n - 1); verbose = 0;
    printf("sorted: ");
    for (int i = 0; i < n; i++) putchar(a[i]);
    printf("\n\n");

    /* Property 8.6 on a few sizes; comparisons vs N lg N */
    int sizes[] = {1000, 1024, 1025, 50000, 65536};
    for (int t = 0; t < 5; t++) {
        int N = sizes[t], passes = 0;
        Item *b = malloc(N * sizeof *b);
        srand(3); for (int i = 0; i < N; i++) b[i] = rand();
        for (int m = 1; m <= N - 1; m += m) passes++;
        C = 0; mergesortBU(b, 0, N - 1);
        for (int i = 1; i < N; i++) if (b[i-1] > b[i]) { puts("BUG"); return 1; }
        printf("N=%-6d passes=%-2d bits(N)=%-2d bits(N-1)=%-2d compares=%ld\n", N, passes, bits(N), bits(N - 1), C);
        free(b);
    }
    return 0;
}
