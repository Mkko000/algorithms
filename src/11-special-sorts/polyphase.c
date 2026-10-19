/*
 * polyphase.c - Polyphase merging on three "tapes" (Section 11.4).
 *
 * Author: Mohammed El Sisi
 * Adapted from: R. Sedgewick, "Algorithms in C, Parts 1-4", 3rd ed.,
 *               Section 11.4 (Polyphase merging, Property 11.6).
 *
 * Each tape is an in-memory queue of runs (a run is a sorted block of ints).
 * Runs are first distributed on tapes 0 and 1 according to consecutive
 * Fibonacci numbers (dummy, empty runs pad the distribution). Then we
 * repeatedly merge runs from the two non-empty tapes onto the empty one
 * until one tape is exhausted, which becomes the output of the next phase.
 * We count how many records are read in total, and compare with balanced
 * 2-way merging, which needs 4 tapes (or, with only 3 tapes, an extra
 * copy pass after every merge pass to redistribute the runs).
 *
 * Build: gcc -std=c99 -Wall -O2 polyphase.c -o polyphase
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct { int *a; int n; } Run;
typedef struct { Run *r; int head, tail, cap; } Tape;

static void tput(Tape *t, Run x)
{
    if (t->tail == t->cap) { t->cap = t->cap ? 2 * t->cap : 16; t->r = realloc(t->r, t->cap * sizeof(Run)); }
    t->r[t->tail++] = x;
}
static Run tget(Tape *t) { return t->r[t->head++]; }
static int tlen(const Tape *t) { return t->tail - t->head; }

static long recordsRead = 0;

static Run merge(Run x, Run y)
{
    Run z; int i = 0, j = 0, k = 0;
    z.n = x.n + y.n; z.a = malloc((z.n ? z.n : 1) * sizeof(int));
    while (i < x.n || j < y.n)
        if (j == y.n || (i < x.n && x.a[i] <= y.a[j])) z.a[k++] = x.a[i++];
        else z.a[k++] = y.a[j++];
    recordsRead += z.n;
    free(x.a); free(y.a);
    return z;
}

static int cmpint(const void *p, const void *q) { int a = *(const int *)p, b = *(const int *)q; return (a > b) - (a < b); }

/* polyphase on 3 tapes; returns number of phases and fills out[] */
static int polyphase(int *data, int N, int M, int verbose)
{
    Tape t[3] = {{0}};
    int runs = (N + M - 1) / M, f1 = 1, f2 = 1, phases = 0, i, k;
    while (f1 + f2 < runs) { int s = f1 + f2; f1 = f2; f2 = s; }   /* f1 <= f2, f1+f2 >= runs */
    if (runs <= 1) f1 = 0, f2 = runs;
    /* distribute: f2 runs on tape 0, f1 runs on tape 1, dummies (empty runs) where needed */
    for (k = 0, i = 0; k < f1 + f2; k++) {
        Run x; x.n = 0; x.a = malloc(sizeof(int) * (M ? M : 1));
        if (i < N) { int n = (N - i < M) ? N - i : M; for (int j = 0; j < n; j++) x.a[j] = data[i + j]; x.n = n; i += n; qsort(x.a, n, sizeof(int), cmpint); }
        tput(&t[k < f2 ? 0 : 1], x);           /* dummies end up on tape 1 */
    }
    if (verbose) printf("  %d runs (+%d dummy): tape0=%d tape1=%d tape2=0\n", runs, f1 + f2 - runs, tlen(&t[0]), tlen(&t[1]));
    int out = 2;
    while (tlen(&t[0]) + tlen(&t[1]) + tlen(&t[2]) > 1) {
        int a = (out + 1) % 3, b = (out + 2) % 3;
        while (tlen(&t[a]) > 0 && tlen(&t[b]) > 0) tput(&t[out], merge(tget(&t[a]), tget(&t[b])));
        phases++;
        if (verbose) printf("  phase %d: tapes = %d %d %d\n", phases, tlen(&t[0]), tlen(&t[1]), tlen(&t[2]));
        out = tlen(&t[a]) == 0 ? a : b;
    }
    Run res = t[0].tail > t[0].head ? tget(&t[0]) : t[1].tail > t[1].head ? tget(&t[1]) : tget(&t[2]);
    for (i = 1; i < res.n; i++) if (res.a[i - 1] > res.a[i]) { printf("NOT SORTED\n"); exit(1); }
    if (res.n != N) { printf("LOST RECORDS\n"); exit(1); }
    free(res.a); for (i = 0; i < 3; i++) free(t[i].r);
    return phases;
}

/* balanced 2-way merging: just count passes; each pass reads all N records */
static int balancedPasses(int runs) { int p = 0; while (runs > 1) { runs = (runs + 1) / 2; p++; } return p; }

int main(void)
{
    int N = 21000, M = 1000, i;
    int *a = malloc(N * sizeof(int));
    srand(11);
    for (i = 0; i < N; i++) a[i] = rand() % 1000000;
    printf("Polyphase, 3 tapes, N=%d, M=%d (21 runs = Fibonacci 8+13):\n", N, M);
    recordsRead = 0;
    int ph = polyphase(a, N, M, 1);
    printf("  phases=%d, records read while merging=%ld = %.2f effective passes\n", ph, recordsRead, (double)recordsRead / N);
    printf("Balanced 2-way, 4 tapes: %d passes = %d records read\n", balancedPasses(21), balancedPasses(21) * N);
    printf("Balanced 2-way, 3 tapes (merge + copy back): %d records read\n\n", 2 * balancedPasses(21) * N);

    printf("%8s %6s %8s %10s %12s %12s\n", "N", "runs", "phases", "poly(3)", "balanced(4)", "balanced(3)");
    for (int n = 10000; n <= 1000000; n *= 10) {
        int *b = malloc(n * sizeof(int));
        for (i = 0; i < n; i++) b[i] = rand();
        recordsRead = 0;
        int p = polyphase(b, n, M, 0);
        printf("%8d %6d %8d %10.2f %12d %12d\n", n, (n + M - 1) / M, p, (double)recordsRead / n, balancedPasses((n + M - 1) / M), 2 * balancedPasses((n + M - 1) / M));
        free(b);
    }
    free(a);
    return 0;
}
