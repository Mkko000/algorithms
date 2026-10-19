/* ============================================================
 *  coin_flip.c  —  Arrays as counters: coin-flipping simulation
 *  Chapter 3: Elementary Data Structures (Section 3.2)
 *
 *  Flip a fair coin N times, repeat the experiment M times, and use
 *  an array f[0..N] to count how many experiments produced exactly
 *  i heads. Print a histogram: the bell curve (normal approximation
 *  to the binomial) falls out of nothing but an array of counters.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 3.7
 *  Build:   gcc -std=c99 -Wall -O2 coin_flip.c -o coin_flip
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

int heads(void) { return rand() < RAND_MAX / 2; }

int main(int argc, char *argv[]) {
    int N = argc > 1 ? atoi(argv[1]) : 32;      /* flips per experiment */
    int M = argc > 2 ? atoi(argv[2]) : 1000;    /* experiments          */
    int *f = calloc(N + 1, sizeof *f);          /* f[i] = #experiments with i heads */
    srand(7);
    for (int j = 0; j < M; j++) {
        int cnt = 0;
        for (int i = 0; i < N; i++) if (heads()) cnt++;
        f[cnt]++;
    }
    for (int j = 0; j <= N; j++) {
        if (f[j] == 0) continue;
        printf("%2d %4d ", j, f[j]);
        for (int i = 0; i < f[j]; i += 10) putchar('*');
        putchar('\n');
    }
    free(f);
    return 0;
}
