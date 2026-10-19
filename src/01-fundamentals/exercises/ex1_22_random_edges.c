/* ============================================================
 *  ex1_22_random_edges.c  —  Exercises 1.22-1.24 (and 1.10, 1.11)
 *  Chapter 1: Introduction — solved exercises
 *
 *  Exercise 1.22 (restated): modify Program 1.4 to generate random pairs
 *  of integers between 0 and N-1 instead of reading them, looping until
 *  N-1 union operations have been performed; print the total number of
 *  edges generated for N = 10^3, 10^4, 10^5, 10^6.
 *  Exercise 1.24: give an approximate formula for that number as a
 *  function of N.  (Answer checked here: about (1/2) N ln N.)
 *  Also prints the back-of-envelope numbers for Exercises 1.10 / 1.11.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 1.4, Exercises 1.10, 1.11, 1.22-1.24
 *  Build:   gcc -std=c99 -Wall -O2 ex1_22_random_edges.c -o ex1_22 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static unsigned long long rs = 0x9E3779B97F4A7C15ULL;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }

static long edges_to_connect(int n)
{
    int *id = malloc(n * sizeof *id), *sz = malloc(n * sizeof *sz);
    for (int i = 0; i < n; i++) { id[i] = i; sz[i] = 1; }
    long edges = 0; int unions = 0;
    while (unions < n - 1) {                       /* Program 1.4, random input */
        int p = rnd() % n, q = rnd() % n, i, j;
        edges++;
        for (i = p; i != id[i]; i = id[i]) id[i] = id[id[i]];
        for (j = q; j != id[j]; j = id[j]) id[j] = id[id[j]];
        if (i == j) continue;
        if (sz[i] < sz[j]) { id[i] = j; sz[j] += sz[i]; }
        else               { id[j] = i; sz[i] += sz[j]; }
        unions++;
    }
    free(id); free(sz);
    return edges;
}

int main(void)
{
    printf("Exercise 1.22/1.24: random edges needed to connect N objects\n");
    printf("%9s %12s %14s %7s\n", "N", "edges (avg)", "(1/2) N ln N", "ratio");
    for (int n = 1000; n <= 1000000; n *= 10) {
        int trials = n >= 1000000 ? 3 : 10;
        double sum = 0;
        for (int t = 0; t < trials; t++) sum += edges_to_connect(n);
        double avg = sum / trials, f = 0.5 * n * log(n);
        printf("%9d %12.0f %14.0f %7.3f\n", n, avg, f, avg / f);
    }

    /* Exercise 1.10: quick find, N = 10^6, M = 10^9, >= 10 instr/iteration */
    double qf = 1e9 * 1e6 * 10 / 1e9;              /* seconds */
    printf("\nExercise 1.10: quick find >= M*N*10 = 1e16 instr -> %.0f s = %.0f days\n",
           qf, qf / 86400);
    /* Exercise 1.11: weighted, each find <= lg N links, <= 100 instr each */
    double wq = 1e9 * 2 * log2(1e6) * 100 / 1e9;
    printf("Exercise 1.11: weighted <= M*2lgN*100 = %.2e instr -> %.0f s (~%.0f min)\n",
           wq * 1e9, wq, wq / 60);
    return 0;
}
