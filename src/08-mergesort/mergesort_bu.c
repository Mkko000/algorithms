/* ============================================================
 *  mergesort_bu.c  —  Bottom-up mergesort (no recursion)
 *  Chapter 8: Merging and Mergesort
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 8.5
 *  Build:   gcc -std=c99 -Wall -O2 mergesort_bu.c -o mergesort_bu
 * ============================================================ */
#include <stdio.h>
#include "../common_item.h"

#define maxN 1000
static Item aux[maxN];

void mergeAB(Item c[], Item a[], int n, Item b[], int m) {
    int i = 0, j = 0, k = 0;
    for (; k < n + m; k++) {
        if (i == n)      { c[k] = b[j++]; continue; }
        if (j == m)      { c[k] = a[i++]; continue; }
        c[k] = less(a[i], b[j]) ? a[i++] : b[j++];
    }
}

void mergesortBU(Item a[], int l, int r) {
    for (int m = 1; m <= r - l; m = m + m)
        for (int i = l; i <= r - m; i += m + m)
            mergeAB(aux + i, a + i, m,
                    a + i + m, (m < r - i - m + 1) ? m : r - i - m + 1),
            /* copy the merged block back */
            ({ for (int t = 0; t < ((m < r-i-m+1)? m+m : r-i+1); t++)
                   a[i+t] = aux[i+t]; });
}

int main(void) {
    Item a[] = {'M','E','R','G','E','S','O','R','T','E','X','A','M','P','L','E'};
    int n = sizeof(a)/sizeof(a[0]);
    /* cleaner bottom-up with a plain merge */
    for (int sz = 1; sz < n; sz += sz)
        for (int lo = 0; lo < n - sz; lo += sz + sz) {
            int mid = lo + sz - 1;
            int hi  = (lo + sz + sz - 1 < n - 1) ? lo + sz + sz - 1 : n - 1;
            /* merge a[lo..mid] with a[mid+1..hi] into aux then copy back */
            int i = lo, j = mid + 1, k = lo;
            while (i <= mid && j <= hi)
                aux[k++] = less(a[i], a[j]) ? a[i++] : a[j++];
            while (i <= mid) aux[k++] = a[i++];
            while (j <= hi)  aux[k++] = a[j++];
            for (k = lo; k <= hi; k++) a[k] = aux[k];
        }
    for (int i = 0; i < n; i++) printf("%c ", a[i]);
    putchar('\n');
    (void)mergesortBU; (void)mergeAB;
    return 0;
}
