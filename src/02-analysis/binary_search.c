/* ============================================================
 *  binary_search.c  —  Sequential vs. binary search
 *  Chapter 2: Principles of Algorithm Analysis
 *
 *  Sequential search is O(N); binary search is O(log N) but needs
 *  a sorted array. The whole point of Chapter 2 is to make that
 *  difference feel real: doubling N adds ONE step to binary search.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 2.1-2.2
 *  Build:   gcc -std=c99 -Wall -O2 binary_search.c -o binary_search
 * ============================================================ */
#include <stdio.h>

/* Returns index of v in a[l..r], or -1 if absent. a[] must be sorted. */
int binary_search(int a[], int l, int r, int v) {
    while (r >= l) {
        int m = l + (r - l) / 2;        /* avoids (l+r) overflow */
        if (v == a[m]) return m;
        if (v <  a[m]) r = m - 1;
        else           l = m + 1;
    }
    return -1;
}

/* Recursive twin, mirroring the recursive definition. */
int binary_search_r(int a[], int l, int r, int v) {
    if (r < l) return -1;
    int m = l + (r - l) / 2;
    if (v == a[m]) return m;
    if (v <  a[m]) return binary_search_r(a, l, m - 1, v);
    return             binary_search_r(a, m + 1, r, v);
}

int sequential_search(int a[], int n, int v) {
    for (int i = 0; i < n; i++) if (a[i] == v) return i;
    return -1;
}

int main(void) {
    int a[] = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91};
    int n = sizeof(a)/sizeof(a[0]);

    int keys[] = {23, 91, 2, 17};
    for (int k = 0; k < 4; k++) {
        int v = keys[k];
        printf("binary_search(%2d) -> index %d   | sequential -> %d\n",
               v, binary_search(a, 0, n - 1, v),
               sequential_search(a, n, v));
    }
    printf("recursive binary_search(56) -> %d\n",
           binary_search_r(a, 0, n - 1, 56));
    return 0;
}
