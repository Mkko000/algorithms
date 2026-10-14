/* ============================================================
 *  batcher_merge.c  —  Batcher's odd-even mergesort network (demo)
 *  Chapter 11: Special-Purpose Sorting Methods
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 11.3
 *  Build:   gcc -std=c99 -Wall -O2 batcher_merge.c -o batcher_merge
 * ============================================================ */
#include <stdio.h>
#include "../common_item.h"

/* Nonrecursive Batcher odd-even mergesort for N = power of 2. */
void batcher_sort(Item a[], int N) {
    for (int p = 1; p < N; p += p)
        for (int k = p; k > 0; k /= 2)
            for (int j = k % p; j + k < N; j += k + k)
                for (int i = 0; i < k; i++)
                    if ((i + j) / (p + p) == (i + j + k) / (p + p))
                        compexch(a[i + j], a[i + j + k]);
}

int main(void) {
    Item a[] = {15, 8, 3, 12, 1, 9, 7, 4, 11, 6, 14, 2, 10, 5, 13, 0};
    int n = 16;
    batcher_sort(a, n);
    for (int i = 0; i < n; i++) printf("%d ", a[i]);
    putchar('\n');
    return 0;
}
