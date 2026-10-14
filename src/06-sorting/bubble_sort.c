/* ============================================================
 *  bubble_sort.c
 *  Chapter 6: Elementary Sorting Methods
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 6.4
 *  Build:   gcc -std=c99 -Wall -O2 bubble_sort.c -o bubble_sort
 * ============================================================ */
#include <stdio.h>
#include "../common_item.h"

void bubble(Item a[], int l, int r) {
    for (int i = l; i < r; i++)
        for (int j = r; j > i; j--)
            compexch(a[j - 1], a[j]);
}

int main(void) {
    Item a[] = {'A','S','O','R','T','I','N','G','E','X','A','M','P','L','E'};
    int n = sizeof(a)/sizeof(a[0]);
    bubble(a, 0, n - 1);
    for (int i = 0; i < n; i++) printf("%c ", a[i]);
    putchar('\n');
    return 0;
}
