/* ============================================================
 *  isam.c  —  Indexed sequential access: a static two-level index
 *  Chapter 16: External Searching (Section 16.2)
 *
 *  Before B trees: sort the file once, store it in pages of M items,
 *  and build an INDEX page holding the first key of every data page
 *  (and, for huge files, an index of index pages). A search reads the
 *  index (binary search inside the page, in memory), then exactly one
 *  data page: 2 page reads here, a small constant in general
 *  (Property 16.1-style: log_M N levels).
 *  The weakness the book points out: the structure is STATIC. New keys
 *  must go into overflow pages chained off the data page, and as they
 *  pile up, searches degrade until the whole thing is rebuilt. That
 *  motivates B trees (dynamic, always balanced).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Section 16.2
 *  Build:   gcc -std=c99 -Wall -O2 isam.c -o isam
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

#define M 100                      /* items per page */
typedef struct page { int key[M]; int m; struct page *overflow; } page;
static page *data; static int npages; static int *index_; static long reads;

static int cmpint(const void *a, const void *b) { int x = *(const int *)a, y = *(const int *)b; return (x > y) - (x < y); }

void build(int *a, int N) {
    qsort(a, N, sizeof *a, cmpint);
    npages = (N + M - 1) / M;
    data = calloc(npages, sizeof *data); index_ = malloc(npages * sizeof *index_);
    for (int i = 0; i < N; i++) { page *p = &data[i / M]; p->key[p->m++] = a[i]; }
    for (int p = 0; p < npages; p++) index_[p] = data[p].key[0];
}
static int findpage(int v) {                      /* binary search in the index page */
    reads++;
    int l = 0, r = npages - 1;
    while (l < r) { int m = (l + r + 1) / 2; if (index_[m] <= v) l = m; else r = m - 1; }
    return l;
}
int search(int v) {
    for (page *p = &data[findpage(v)]; p; p = p->overflow) {
        reads++;
        for (int j = 0; j < p->m; j++) if (p->key[j] == v) return 1;
    }
    return 0;
}
void insert(int v) {                               /* static file: overflow chains */
    page *p = &data[findpage(v)];
    while (p->m == M) { if (!p->overflow) p->overflow = calloc(1, sizeof *p); p = p->overflow; }
    p->key[p->m++] = v;
}

int main(void) {
    const int N = 100000; int *a = malloc(N * sizeof *a);
    srand(3); for (int i = 0; i < N; i++) a[i] = rand();
    build(a, N);
    reads = 0; for (int i = 0; i < 1000; i++) search(rand());
    printf("fresh ISAM file: %d data pages, %.2f page reads per (miss) search\n", npages, reads / 1000.0);
    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < N / 2; i++) insert(rand());
        reads = 0; for (int i = 0; i < 1000; i++) search(rand());   /* random keys: mostly misses */
        printf("after %3d%% more inserts: %.2f page reads per (miss) search: overflow chains growing\n",
               50 * (k + 1), reads / 1000.0);
    }
    free(a);
    return 0;
}
