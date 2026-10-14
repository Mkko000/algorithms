/* ============================================================
 *  quicksort_improved.c  —  Median-of-three + cutoff to insertion
 *  Chapter 7: Quicksort
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 7.4
 *  Build:   gcc -std=c99 -Wall -O2 quicksort_improved.c -o quicksort_improved
 * ============================================================ */
#include <stdio.h>
#include "../common_item.h"

#define M 10

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

void insertion(Item a[], int l, int r) {
    for (int i = l + 1; i <= r; i++) {
        Item v = a[i]; int j = i;
        while (j > l && less(v, a[j - 1])) { a[j] = a[j - 1]; j--; }
        a[j] = v;
    }
}

void quicksort(Item a[], int l, int r) {
    if (r - l <= M) return;
    exch(a[(l + r) / 2], a[r - 1]);
    compexch(a[l], a[r - 1]);
    compexch(a[l], a[r]);
    compexch(a[r - 1], a[r]);
    int i = partition(a, l + 1, r - 1);
    quicksort(a, l, i - 1);
    quicksort(a, i + 1, r);
}

void sort(Item a[], int l, int r) {
    quicksort(a, l, r);
    insertion(a, l, r);
}

int main(void) {
    Item a[] = {'Q','U','I','C','K','S','O','R','T','E','X','A','M','P','L','E'};
    int n = sizeof(a)/sizeof(a[0]);
    sort(a, 0, n - 1);
    for (int i = 0; i < n; i++) printf("%c ", a[i]);
    putchar('\n');
    return 0;
}
