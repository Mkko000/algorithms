/* ============================================================
 *  quicksort.c  —  Basic quicksort + partitioning (Sedgewick style)
 *  Chapter 7: Quicksort
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 7.1-7.2
 *  Build:   gcc -std=c99 -Wall -O2 quicksort.c -o quicksort
 * ============================================================ */
#include <stdio.h>
#include "../common_item.h"

int partition(Item a[], int l, int r) {
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

void quicksort(Item a[], int l, int r) {
    if (r <= l) return;
    int i = partition(a, l, r);
    quicksort(a, l, i - 1);
    quicksort(a, i + 1, r);
}

int main(void) {
    Item a[] = {'Q','U','I','C','K','S','O','R','T','E','X','A','M','P','L','E'};
    int n = sizeof(a)/sizeof(a[0]);
    quicksort(a, 0, n - 1);
    for (int i = 0; i < n; i++) printf("%c ", a[i]);
    putchar('\n');
    return 0;
}
