/* ============================================================
 *  key_indexed_counting.c  —  Sort when keys are small integers 0..R-1
 *  Chapter 6: Elementary Sorting Methods
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 6.17
 *  Build:   gcc -std=c99 -Wall -O2 key_indexed_counting.c -o key_indexed_counting
 * ============================================================ */
#include <stdio.h>
#include <string.h>

#define R 26   /* keys are 'a'..'z' mapped to 0..25 */

void key_indexed(char a[], int n) {
    int count[R + 1] = {0};
    char aux[64];
    for (int i = 0; i < n; i++) count[a[i] - 'a' + 1]++;
    for (int r = 0; r < R; r++) count[r + 1] += count[r];   /* prefix sums = starts */
    for (int i = 0; i < n; i++) aux[count[a[i] - 'a']++] = a[i];
    for (int i = 0; i < n; i++) a[i] = aux[i];
}

int main(void) {
    char a[] = "easysortingexample";
    int n = (int)strlen(a);
    key_indexed(a, n);
    printf("%s\n", a);
    return 0;
}
