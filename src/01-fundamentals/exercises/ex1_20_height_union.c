/* ============================================================
 *  ex1_20_height_union.c  —  Exercises 1.12, 1.20, 1.21
 *  Chapter 1: Introduction — solved exercises
 *
 *  Exercise 1.20 (restated): modify weighted quick union (Program 1.3)
 *  to decide which root becomes the child using the HEIGHT of the trees
 *  instead of their size, and compare it empirically with Program 1.3.
 *  Exercise 1.21: show that Property 1.3 (at most lg N links from any
 *  node to its root) holds for that variant too.
 *  Exercise 1.12: compute the average distance to the root in a
 *  worst-case weighted tree of 2^n nodes (answer: n/2).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 1.3, Exercises 1.12, 1.20, 1.21
 *  Build:   gcc -std=c99 -Wall -O2 ex1_20_height_union.c -o ex1_20 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int *id, *w;                       /* w = size or height */

static int find(int x) { while (x != id[x]) x = id[x]; return x; }

static void unite(int p, int q, int byHeight)
{
    int i = find(p), j = find(q);
    if (i == j) return;
    if (w[i] < w[j]) { int t = i; i = j; j = t; }   /* i = bigger/taller */
    id[j] = i;
    if (byHeight) { if (w[i] == w[j]) w[i]++; }       /* height grows only on ties */
    else          w[i] += w[j];
}

static void stats(int n, double *avg, int *maxd)
{
    long sum = 0; *maxd = 0;
    for (int x = 0; x < n; x++) {
        int d = 0, y = x;
        while (y != id[y]) { y = id[y]; d++; }
        sum += d; if (d > *maxd) *maxd = d;
    }
    *avg = (double)sum / n;
}

static unsigned long long rs = 88172645463325252ULL;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }

int main(void)
{
    printf("Exercise 1.20: size vs height, random pairs until connected\n");
    printf("%9s | %-18s | %-18s | lg N\n", "N", "by size avg/max", "by height avg/max");
    for (int n = 1000; n <= 1000000; n *= 10) {
        double a[2]; int mx[2];
        id = malloc(n * sizeof *id); w = malloc(n * sizeof *w);
        for (int h = 0; h < 2; h++) {
            rs = 12345 + n;                         /* same pairs for both */
            for (int i = 0; i < n; i++) { id[i] = i; w[i] = h ? 0 : 1; }
            int comps = n;
            while (comps > 1) {
                int p = rnd() % n, q = rnd() % n;
                if (find(p) != find(q)) comps--;
                unite(p, q, h);
            }
            stats(n, &a[h], &mx[h]);
        }
        printf("%9d | %7.2f / %-8d | %7.2f / %-8d | %.1f\n",
               n, a[0], mx[0], a[1], mx[1], log2(n));
        free(id); free(w);
    }

    printf("\nExercise 1.12: worst case = always merge equal-size trees\n");
    for (int n = 1; n <= 16; n *= 2) {
        int m = 1 << n;
        double avg; int mxd;
        id = malloc(m * sizeof *id); w = malloc(m * sizeof *w);
        for (int i = 0; i < m; i++) { id[i] = i; w[i] = 1; }
        for (int s = 1; s < m; s *= 2)               /* merge blocks of size s */
            for (int b = 0; b < m; b += 2 * s) unite(b, b + s, 0);
        stats(m, &avg, &mxd);
        printf("2^%-2d = %6d nodes: average distance %.3f (n/2 = %.1f), max %d\n",
               n, m, avg, n / 2.0, mxd);
        free(id); free(w);
    }
    return 0;
}
