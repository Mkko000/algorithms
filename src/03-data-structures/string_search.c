/* ============================================================
 *  string_search.c  —  Brute-force substring search
 *  Chapter 3: Elementary Data Structures (strings)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 3.15
 *  Build:   gcc -std=c99 -Wall -O2 string_search.c -o string_search
 * ============================================================ */
#include <stdio.h>

/* Returns the index of the first occurrence of p in a, or -1. */
int str_search(const char *p, const char *a) {
    int i, j;
    for (i = 0; a[i] != '\0'; i++) {
        for (j = 0; p[j] != '\0'; j++)
            if (a[i + j] != p[j]) break;
        if (p[j] == '\0') return i;    /* matched the whole pattern */
    }
    return -1;
}

int main(void) {
    const char *text = "the quick brown fox jumps over the lazy dog";
    const char *pats[] = {"brown", "fox", "cat", "dog"};
    for (int k = 0; k < 4; k++)
        printf("find \"%s\" -> %d\n", pats[k], str_search(pats[k], text));
    return 0;
}
