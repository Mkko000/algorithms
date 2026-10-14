/* ============================================================
 *  msd_radix.c  —  MSD (most-significant-digit) radix sort
 *  Chapter 10: Radix Sorting
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 10.2
 *  Build:   gcc -std=c99 -Wall -O2 msd_radix.c -o msd_radix
 * ============================================================ */
#include <stdio.h>
#include <string.h>

#define R 256
#define maxN 64
static char aux[maxN][32];

static int digit(const char *s, int d) {
    return (d < (int)strlen(s)) ? (unsigned char)s[d] : -1;  /* -1 = end-of-string */
}

void msd_r(char a[][32], int l, int r, int d) {
    if (r <= l) return;
    int count[R + 2] = {0};                /* +1 for end-of-string bin */
    for (int i = l; i <= r; i++) count[digit(a[i], d) + 2]++;
    for (int i = 0; i < R + 1; i++) count[i + 1] += count[i];
    for (int i = l; i <= r; i++) strcpy(aux[count[digit(a[i], d) + 1]++], a[i]);
    for (int i = l; i <= r; i++) strcpy(a[i], aux[i - l]);
    for (int i = 0; i < R; i++)
        msd_r(a, l + count[i], l + count[i + 1] - 1, d + 1);
}

int main(void) {
    char a[][32] = {"she","sells","seashells","by","the","sea","shore"};
    int n = 7;
    msd_r(a, 0, n - 1, 0);
    for (int i = 0; i < n; i++) printf("%s ", a[i]);
    putchar('\n');
    return 0;
}
