/* ============================================================
 *  quicksort_3way.c  —  Dijkstra 3-way partitioning (many duplicates)
 *  Chapter 7: Quicksort
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 7.5
 *  Build:   gcc -std=c99 -Wall -O2 quicksort_3way.c -o quicksort_3way
 * ============================================================ */
#include <stdio.h>
#include "../common_item.h"

void quicksort(Item a[], int l, int r) {
    int i, j, k, p, q; Item v;
    if (r <= l) return;
    v = a[r]; i = l - 1; j = r; p = l - 1; q = r;
    for (;;) {
        while (less(a[++i], v)) ;
        while (less(v, a[--j])) if (j == l) break;
        if (i >= j) break;
        exch(a[i], a[j]);
        if (eq(a[i], v)) { p++; exch(a[p], a[i]); }
        if (eq(v, a[j])) { q--; exch(a[q], a[j]); }
    }
    exch(a[i], a[r]); j = i - 1; i = i + 1;
    for (k = l; k < p; k++, j--) exch(a[k], a[j]);
    for (k = r - 1; k > q; k--, i++) exch(a[k], a[i]);
    quicksort(a, l, j);
    quicksort(a, i, r);
}

int main(void) {
    Item a[] = {'B','A','B','A','B','A','A','B','B','A','B','A'};
    int n = sizeof(a)/sizeof(a[0]);
    quicksort(a, 0, n - 1);
    for (int i = 0; i < n; i++) printf("%c ", a[i]);
    putchar('\n');
    return 0;
}
