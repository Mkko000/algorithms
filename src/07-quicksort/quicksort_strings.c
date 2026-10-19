/* ============================================================
 *  quicksort_strings.c  —  Quicksort on strings (pointer sort)
 *  Chapter 7: Quicksort (Section 7.7)
 *
 *  With string keys we sort an array of POINTERS (Item = char*), so
 *  each exchange is cheap; the cost is in comparisons, and a string
 *  comparison is not O(1): it scans the common prefix. Keys with long
 *  shared prefixes (URLs, file paths) make every compare expensive —
 *  that observation motivates the radix methods of Chapter 10.
 *  Here we count CHARACTER comparisons, not just key comparisons.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 7.1 with the string Item of Program 6.11
 *  Build:   gcc -std=c99 -Wall -O2 quicksort_strings.c -o quicksort_strings
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char *Item;
static long keycmp, charcmp;
static int scmp(const char *a, const char *b) {
    keycmp++;
    while (*a && *a == *b) { a++; b++; charcmp++; }
    charcmp++;
    return (unsigned char)*a - (unsigned char)*b;
}
#define less(A, B) (scmp(A, B) < 0)
#define exch(A, B) { Item t = A; A = B; B = t; }

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
void quicksort(Item a[], int l, int r) {
    if (r <= l) return;
    int i = partition(a, l, r);
    quicksort(a, l, i - 1); quicksort(a, i + 1, r);
}

int main(void) {
    const int N = 5000;
    Item *a = malloc(N * sizeof *a);
    char (*buf)[48] = malloc(N * sizeof *buf);
    for (int round = 0; round < 2; round++) {
        srand(3);
        for (int i = 0; i < N; i++) {
            if (round == 0) sprintf(buf[i], "%c%c%c%c%c", 'a'+rand()%26, 'a'+rand()%26, 'a'+rand()%26, 'a'+rand()%26, 'a'+rand()%26);
            else sprintf(buf[i], "https://example.com/users/%06d", rand() % 1000000);
            a[i] = buf[i];
        }
        keycmp = charcmp = 0;
        quicksort(a, 0, N - 1);
        for (int i = 1; i < N; i++) if (strcmp(a[i-1], a[i]) > 0) { puts("BUG"); return 1; }
        printf("%-24s key compares=%ld  char compares=%ld  (%.1f chars/compare)\n",
               round ? "long common prefix:" : "random 5-letter words:",
               keycmp, charcmp, (double)charcmp / keycmp);
    }
    printf("first 3 sorted: %s %s %s\n", a[0], a[1], a[2]);
    free(a); free(buf);
    return 0;
}
