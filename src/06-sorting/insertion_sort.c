/* ============================================================
 *  insertion_sort.c
 *  Chapter 6: Elementary Sorting Methods
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 6.3
 *  Build:   gcc -std=c99 -Wall -O2 insertion_sort.c -o insertion_sort
 * ============================================================ */
#include <stdio.h>
#include "../common_item.h"

void insertion(Item a[], int l, int r) {
    int i;
    for (i = r; i > l; i--) compexch(a[i - 1], a[i]); /* put smallest in a[l] as sentinel */
    for (i = l + 2; i <= r; i++) {
        int j = i; Item v = a[i];
        while (less(v, a[j - 1])) { a[j] = a[j - 1]; j--; }
        a[j] = v;
    }
}

int main(void) {
    Item a[] = {'A','S','O','R','T','I','N','G','E','X','A','M','P','L','E'};
    int n = sizeof(a)/sizeof(a[0]);
    insertion(a, 0, n - 1);
    for (int i = 0; i < n; i++) printf("%c ", a[i]);
    putchar('\n');
    return 0;
}
