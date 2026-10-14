/* ============================================================
 *  select.c  —  Quickselect: find the k-th smallest in average O(N)
 *  Chapter 7: Quicksort
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 7.6
 *  Build:   gcc -std=c99 -Wall -O2 select.c -o select
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

void select_k(Item a[], int l, int r, int k) {
    if (r <= l) return;
    int i = partition(a, l, r);
    if (i > k) select_k(a, l, i - 1, k);
    if (i < k) select_k(a, i + 1, r, k);
}

int main(void) {
    Item a[] = {15, 3, 9, 8, 22, 1, 7, 14, 5, 11};
    int n = sizeof(a)/sizeof(a[0]);
    int k = n / 2;                       /* median */
    select_k(a, 0, n - 1, k);
    printf("median (k=%d) = %d\n", k, a[k]);
    printf("partitioned: ");
    for (int i = 0; i < n; i++) printf("%d ", a[i]);
    putchar('\n');
    return 0;
}
