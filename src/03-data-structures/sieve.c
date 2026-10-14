/* ============================================================
 *  sieve.c  —  Sieve of Eratosthenes (array as a set of flags)
 *  Chapter 3: Elementary Data Structures
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 3.5
 *  Build:   gcc -std=c99 -Wall -O2 sieve.c -o sieve
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int N = 1000;
    char *a = malloc(N + 1);
    for (int i = 2; i <= N; i++) a[i] = 1;      /* assume all prime */
    for (int i = 2; i <= N; i++)
        if (a[i])                                /* i is prime */
            for (int j = i; i * j <= N; j++)     /* cross off its multiples */
                a[i * j] = 0;
    int count = 0;
    for (int i = 2; i <= N; i++) if (a[i]) { printf("%d ", i); count++; }
    printf("\n%d primes up to %d\n", count, N);
    free(a);
    return 0;
}
