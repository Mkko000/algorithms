/* ============================================================
 *  selection_sort.c
 *  Chapter 6: Elementary Sorting Methods
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 6.2
 *  Build:   gcc -std=c99 -Wall -O2 selection_sort.c -o selection_sort
 * ============================================================ */
#include <stdio.h>
#include "../common_item.h"

void selection(Item a[], int l, int r) {
    for (int i = l; i < r; i++) {
        int min = i;
        for (int j = i + 1; j <= r; j++)
            if (less(a[j], a[min])) min = j;
        exch(a[i], a[min]);
    }
}

int main(void) {
    Item a[] = {'A','S','O','R','T','I','N','G','E','X','A','M','P','L','E'};
    int n = sizeof(a)/sizeof(a[0]);
    selection(a, 0, n - 1);
    for (int i = 0; i < n; i++) printf("%c ", a[i]);
    putchar('\n');
    return 0;
}
