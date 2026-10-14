/* ============================================================
 *  lsd_radix.c  —  LSD (least-significant-digit) radix sort for strings
 *  Chapter 10: Radix Sorting
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 10.4
 *  Build:   gcc -std=c99 -Wall -O2 lsd_radix.c -o lsd_radix
 * ============================================================ */
#include <stdio.h>
#include <string.h>

#define R 256
#define W 3   /* fixed key width for the demo */

void lsd_radix(char a[][W + 1], int n) {
    char aux[32][W + 1];
    for (int d = W - 1; d >= 0; d--) {
        int count[R + 1] = {0};
        for (int i = 0; i < n; i++) count[(unsigned char)a[i][d] + 1]++;
        for (int r = 0; r < R; r++) count[r + 1] += count[r];
        for (int i = 0; i < n; i++) strcpy(aux[count[(unsigned char)a[i][d]]++], a[i]);
        for (int i = 0; i < n; i++) strcpy(a[i], aux[i]);
    }
}

int main(void) {
    char a[][W + 1] = {"dab","add","cab","fad","fee","bad","dad","bee","fed","bed","ebb","ace"};
    int n = 12;
    lsd_radix(a, n);
    for (int i = 0; i < n; i++) printf("%s ", a[i]);
    putchar('\n');
    return 0;
}
