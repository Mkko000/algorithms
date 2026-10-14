/* ============================================================
 *  mergesort.c  —  Top-down mergesort (with abstract in-place merge)
 *  Chapter 8: Merging and Mergesort
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 8.2-8.3
 *  Build:   gcc -std=c99 -Wall -O2 mergesort.c -o mergesort
 * ============================================================ */
#include <stdio.h>
#include <limits.h>
#include "../common_item.h"

#define maxN 1000
static Item aux[maxN];

/* Abstract in-place merge: copy into aux (with sentinels), then merge back. */
void merge(Item a[], int l, int m, int r) {
    int i, j, k;
    for (i = m + 1; i > l; i--) aux[i - 1] = a[i - 1];
    for (j = m; j < r; j++)     aux[r + m - j] = a[j + 1];   /* reverse right half */
    for (k = l; k <= r; k++)
        a[k] = less(aux[i], aux[j]) ? aux[i++] : aux[j--];
}

void mergesort(Item a[], int l, int r) {
    if (r <= l) return;
    int m = (r + l) / 2;
    mergesort(a, l, m);
    mergesort(a, m + 1, r);
    merge(a, l, m, r);
}

int main(void) {
    Item a[] = {'M','E','R','G','E','S','O','R','T','E','X','A','M','P','L','E'};
    int n = sizeof(a)/sizeof(a[0]);
    mergesort(a, 0, n - 1);
    for (int i = 0; i < n; i++) printf("%c ", a[i]);
    putchar('\n');
    return 0;
}
