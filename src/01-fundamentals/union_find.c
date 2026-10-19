/* ============================================================
 *  union_find.c  —  The connectivity (dynamic equivalence) problem
 *  Chapter 1: Introduction  (Sedgewick, Algorithms in C, Parts 1-4)
 *
 *  Four algorithms on the same problem, from slow to fast:
 *     1) quick-find            find O(1), union O(N)
 *     2) quick-union           both ~O(tree height)
 *     3) weighted quick-union  both O(log N)  guaranteed
 *     4) weighted + path halving  ~O(1) amortized (near inverse-Ackermann)
 *     5) weighted + FULL path compression (two-pass: every node on the
 *        path is pointed straight at the root)
 *  main() runs all five on the same random pairs and counts array
 *  accesses, a small-scale version of the book's Table 1.1.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 1.1-1.4
 *                (+ full path compression, described in Section 1.3)
 *  Build:   gcc -std=c99 -Wall -O2 union_find.c -o union_find
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

#define N 10000

/* ---- 1) Quick-find: id[i] is the "name" of i's component ---- */
static int qf_id[N];
void qf_init(void)            { for (int i = 0; i < N; i++) qf_id[i] = i; }
int  qf_find(int p)           { return qf_id[p]; }
int  qf_connected(int p,int q){ return qf_id[p] == qf_id[q]; }
void qf_union(int p, int q) {
    int t = qf_id[p];
    if (t == qf_id[q]) return;
    for (int i = 0; i < N; i++)          /* O(N): the expensive part */
        if (qf_id[i] == t) qf_id[i] = qf_id[q];
}

/* ---- 2/3/4) Forest-of-trees variants share the same id[] (parent links) ---- */
static int id[N], sz[N];
void uf_init(void) { for (int i = 0; i < N; i++) { id[i] = i; sz[i] = 1; } }

/* plain quick-union root: climb parent links to the root */
int qu_root(int i) { while (i != id[i]) i = id[i]; return i; }
void qu_union(int p, int q) {
    int i = qu_root(p), j = qu_root(q);
    if (i != j) id[i] = j;               /* link one root under the other */
}

/* weighted quick-union: always hang the smaller tree under the larger */
void wqu_union(int p, int q) {
    int i = qu_root(p), j = qu_root(q);
    if (i == j) return;
    if (sz[i] < sz[j]) { id[i] = j; sz[j] += sz[i]; }
    else               { id[j] = i; sz[i] += sz[j]; }
}

/* weighted quick-union + path halving: flatten the tree while we climb */
int wpc_root(int i) {
    while (i != id[i]) { id[i] = id[id[i]]; i = id[i]; }
    return i;
}
void wpc_union(int p, int q) {
    int i = wpc_root(p), j = wpc_root(q);
    if (i == j) return;
    if (sz[i] < sz[j]) { id[i] = j; sz[j] += sz[i]; }
    else               { id[j] = i; sz[i] += sz[j]; }
}

/* weighted quick-union + FULL path compression (two passes) */
int wfc_root(int i) {
    int r = i;
    while (r != id[r]) r = id[r];          /* pass 1: find the root      */
    while (i != r) { int nx = id[i]; id[i] = r; i = nx; }  /* pass 2: repoint */
    return r;
}
void wfc_union(int p, int q) {
    int i = wfc_root(p), j = wfc_root(q);
    if (i == j) return;
    if (sz[i] < sz[j]) { id[i] = j; sz[j] += sz[i]; }
    else               { id[j] = i; sz[i] += sz[j]; }
}

/* ---- instrumented run: count how many parent links we follow ---- */
static long steps;
static int root_count(int i, int mode) {
    if (mode == 3) { while (i != id[i]) { id[i] = id[id[i]]; i = id[i]; steps++; } return i; }
    if (mode == 4) { int r = i; while (r != id[r]) { r = id[r]; steps++; }
                     while (i != r) { int nx = id[i]; id[i] = r; i = nx; steps++; } return r; }
    while (i != id[i]) { i = id[i]; steps++; }
    return i;
}
static long run(int mode, int n, int m, unsigned seed) {
    srand(seed); steps = 0;
    if (mode == 0) qf_init(); else uf_init();
    for (int k = 0; k < m; k++) {
        int p = rand() % n, q = rand() % n;
        if (mode == 0) {                      /* quick-find */
            steps++;
            if (qf_id[p] == qf_id[q]) continue;
            int t = qf_id[p];
            for (int i = 0; i < n; i++) { steps++; if (qf_id[i] == t) qf_id[i] = qf_id[q]; }
            continue;
        }
        int i = root_count(p, mode), j = root_count(q, mode);
        if (i == j) continue;
        if (mode == 1) id[i] = j;             /* plain quick-union */
        else if (sz[i] < sz[j]) { id[i] = j; sz[j] += sz[i]; }
        else                    { id[j] = i; sz[i] += sz[j]; }
    }
    return steps;
}

int main(void) {
    /* A small demo: process a list of pairs; print only new connections. */
    int pairs[][2] = {{3,4},{4,9},{8,0},{2,3},{5,6},{5,9},
                      {7,3},{4,8},{5,6},{0,2},{6,1}};
    int np = sizeof(pairs)/sizeof(pairs[0]);

    uf_init();
    printf("Weighted quick-union + path halving:\n");
    int components = N;
    for (int k = 0; k < np; k++) {
        int p = pairs[k][0], q = pairs[k][1];
        if (wpc_root(p) == wpc_root(q)) {
            printf("  %d-%d  already connected\n", p, q);
        } else {
            wpc_union(p, q);
            components--;
            printf("  %d-%d  connected\n", p, q);
        }
    }
    printf("Among the first 10 objects, distinct components left: ");
    int seen = 0;
    for (int i = 0; i < 10; i++) if (wpc_root(i) == i) seen++;
    printf("%d\n", seen);

    /* Table 1.1 in miniature: same random pairs, five algorithms. */
    const char *name[] = {"quick-find", "quick-union", "weighted QU",
                          "weighted + halving", "weighted + full compression"};
    printf("\nN=%d objects, M=%d random pairs -> links followed / entries touched:\n", N, 2*N);
    for (int mode = 0; mode < 5; mode++)
        printf("  %-28s %10ld\n", name[mode], run(mode, N, 2*N, 12345u));

    /* sanity: the two-pass variant gives the same answers */
    uf_init(); wfc_union(1, 2); wfc_union(2, 3); wqu_union(7, 8);
    printf("wfc: 1~3 %s, 1~7 %s\n", wfc_root(1) == wfc_root(3) ? "yes" : "no",
                                     wfc_root(1) == wfc_root(7) ? "yes" : "no");
    (void)components; (void)qf_find; (void)qf_connected; (void)qf_union; (void)qu_union;
    return 0;
}
