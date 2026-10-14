/* ============================================================
 *  shellsort.c  —  Shellsort with Knuth's h = 3h+1 sequence
 *  Chapter 6: Elementary Sorting Methods
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 6.5
 *  Build:   gcc -std=c99 -Wall -O2 shellsort.c -o shellsort
 * ============================================================ */
#include <stdio.h>
#include "../common_item.h"

void shellsort(Item a[], int l, int r) {
    int h;
    for (h = 1; h <= (r - l) / 9; h = 3 * h + 1) ;   /* grow h */
    for (; h > 0; h /= 3)
        for (int i = l + h; i <= r; i++) {
            int j = i; Item v = a[i];
            while (j >= l + h && less(v, a[j - h])) {
                a[j] = a[j - h]; j -= h;
            }
            a[j] = v;
        }
}

int main(void) {
    Item a[] = {'A','S','O','R','T','I','N','G','E','X','A','M','P','L','E'};
    int n = sizeof(a)/sizeof(a[0]);
    shellsort(a, 0, n - 1);
    for (int i = 0; i < n; i++) printf("%c ", a[i]);
    putchar('\n');
    return 0;
}
