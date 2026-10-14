/* ============================================================
 *  union_find.c  —  The connectivity (dynamic equivalence) problem
 *  Chapter 1: Introduction  (Sedgewick, Algorithms in C, Parts 1-4)
 *
 *  Four algorithms on the same problem, from slow to fast:
 *     1) quick-find            find O(1), union O(N)
 *     2) quick-union           both ~O(tree height)
 *     3) weighted quick-union  both O(log N)  guaranteed
 *     4) weighted + path halving  ~O(1) amortized (near inverse-Ackermann)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 1.1-1.4
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
    (void)components; (void)qf_init; (void)qf_find;
    (void)qf_connected; (void)qf_union; (void)qu_union; (void)wqu_union;
    return 0;
}
