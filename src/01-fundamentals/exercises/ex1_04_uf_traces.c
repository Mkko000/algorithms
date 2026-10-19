/* ============================================================
 *  ex1_04_uf_traces.c  —  Exercises 1.4, 1.5, 1.7, 1.8, 1.17, 1.19
 *  Chapter 1: Introduction — solved exercises
 *
 *  Exercise 1.4 (restated): show the contents of the id array after
 *  each union when quick find (Program 1.1) processes the pairs
 *  0-2 1-4 2-5 3-6 0-4 6-0 1-3, and count how many times the program
 *  accesses the id array for each pair.
 *  1.5 / 1.7 / 1.8 / 1.17: the same for quick union, weighted quick
 *  union, weighted with halving, weighted with full path compression.
 *  1.19: show that quick union + full path compression (no weighting)
 *  can still build a long path.
 *
 *  Every read or write of id[] / sz[] goes through the macros RD/WR,
 *  so the counts follow the book's code literally.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 1.1-1.4, Exercises 1.4-1.8, 1.16-1.19
 *  Build:   gcc -std=c99 -Wall -O2 ex1_04_uf_traces.c -o ex1_04
 * ============================================================ */
#include <stdio.h>

#define N 10
static int id[N], sz[N];
static long acc;                         /* array accesses */
#define RD(a, i)    (acc++, (a)[i])
#define WR(a, i, v) (acc++, (a)[i] = (v))

enum { QF, QU, WQU, HALF, FULL, QUFULL };
static const char *name[] = {
    "quick find (Program 1.1)", "quick union (Program 1.2)",
    "weighted quick union (Program 1.3)",
    "weighted + halving (Program 1.4)",
    "weighted + full path compression (Exercise 1.16)",
    "quick union + full path compression, NO weighting (Exercise 1.19)"
};

static int root(int x, int mode)
{
    int i = x;
    if (mode == HALF) {                  /* Program 1.4 */
        for (; i != RD(id, i); i = RD(id, i))
            WR(id, i, RD(id, RD(id, i)));
        return i;
    }
    for (; i != RD(id, i); i = RD(id, i)) ;
    if (mode == FULL || mode == QUFULL)  /* second pass: point all at root */
        while (x != i) { int nx = RD(id, x); WR(id, x, i); x = nx; }
    return i;
}

static void pair(int p, int q, int mode)
{
    int i, j, t;
    if (mode == QF) {                    /* Program 1.1 */
        if (RD(id, p) == RD(id, q)) return;
        for (t = RD(id, p), i = 0; i < N; i++)
            if (RD(id, i) == t) WR(id, i, RD(id, q));
        return;
    }
    i = root(p, mode); j = root(q, mode);
    if (i == j) return;
    if (mode == QU || mode == QUFULL) { WR(id, i, j); return; }
    if (RD(sz, i) < RD(sz, j)) { WR(id, i, j); WR(sz, j, RD(sz, j) + RD(sz, i)); }
    else                       { WR(id, j, i); WR(sz, i, RD(sz, i) + RD(sz, j)); }
}

static void run(int mode, const int (*pr)[2], int np, int n)
{
    long total = 0;
    printf("\n== %s ==\n pair  ", name[mode]);
    for (int i = 0; i < n; i++) printf("%d ", i);
    printf("  accesses\n");
    for (int i = 0; i < N; i++) { id[i] = i; sz[i] = 1; }
    for (int k = 0; k < np; k++) {
        acc = 0;
        pair(pr[k][0], pr[k][1], mode);
        total += acc;
        printf(" %d-%d   ", pr[k][0], pr[k][1]);
        for (int i = 0; i < n; i++) printf("%d ", id[i]);
        printf("  %ld\n", acc);
    }
    printf(" total accesses: %ld\n", total);
}

static int depth(int x) { int d = 0; while (x != id[x]) { x = id[x]; d++; } return d; }

int main(void)
{
    static const int ex[][2] = { {0,2},{1,4},{2,5},{3,6},{0,4},{6,0},{1,3} };
    for (int m = QF; m <= FULL; m++) run(m, ex, 7, N);

    /* Exercise 1.19: pairs 0-1 1-2 2-3 ... always hand a ROOT to the
       left-hand find, so full path compression has nothing to compress. */
    static int chain[N - 1][2];
    for (int k = 0; k < N - 1; k++) { chain[k][0] = k; chain[k][1] = k + 1; }
    run(QUFULL, (const int (*)[2])chain, N - 1, N);
    printf(" depth of node 0 after the chain: %d (= N-1)\n", depth(0));
    return 0;
}
