/* ============================================================
 *  radix_quicksort.c  —  3-way radix quicksort (string keys)
 *  Chapter 10: Radix Sorting
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 10.3
 *  Build:   gcc -std=c99 -Wall -O2 radix_quicksort.c -o radix_quicksort
 * ============================================================ */
#include <stdio.h>
#include <string.h>

static int digit(const char *s, int d) {
    return (d < (int)strlen(s)) ? (unsigned char)s[d] : -1;
}
static void exch(char a[][32], int i, int j) {
    char t[32]; strcpy(t, a[i]); strcpy(a[i], a[j]); strcpy(a[j], t);
}

void rqsort(char a[][32], int l, int r, int d) {
    if (r <= l) return;
    int i = l - 1, j = r, p = l - 1, q = r;
    int v = digit(a[r], d);
    for (;;) {
        while (digit(a[++i], d) < v) ;
        while (v < digit(a[--j], d)) if (j == l) break;
        if (i >= j) break;
        exch(a, i, j);
        if (digit(a[i], d) == v) { p++; exch(a, p, i); }
        if (v == digit(a[j], d)) { q--; exch(a, q, j); }
    }
    if (v >= 0) {                        /* only recurse deeper if not end-of-string */
        exch(a, i, r); j = i - 1; i = i + 1;
        for (int k = l; k <= p; k++, j--) exch(a, k, j);
        for (int k = r - 1; k >= q; k--, i++) exch(a, k, i);
        rqsort(a, l, j, d);
        if (v >= 0) rqsort(a, j + 1, i - 1, d + 1);
        rqsort(a, i, r, d);
    } else {
        /* all remaining equal at this digit (end); done with this block */
    }
}

int main(void) {
    char a[][32] = {"she","sells","seashells","by","the","sea","shore","shells"};
    int n = 8;
    rqsort(a, 0, n - 1, 0);
    for (int i = 0; i < n; i++) printf("%s\n", a[i]);
    return 0;
}
