/* ============================================================
 *  hash_universal.c  —  Universal hash for strings (Program 14.2)
 *  Chapter 14: Hashing
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 14.2
 *  Build:   gcc -std=c99 -Wall -O2 hash_universal.c -o hash_universal
 * ============================================================ */
#include <stdio.h>

int hashU(const char *v, int M) {
    int h, a = 31415, b = 27183;
    for (h = 0; *v != '\0'; v++, a = a * b % (M - 1))
        h = (a * h + (unsigned char)*v) % M;
    return h;
}

int main(void) {
    const char *words[] = {"apple","banana","cherry","date","elderberry"};
    int M = 97;
    for (int i = 0; i < 5; i++)
        printf("%-12s -> %d\n", words[i], hashU(words[i], M));
    return 0;
}
