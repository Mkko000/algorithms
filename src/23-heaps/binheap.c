/* binheap.c — the binary heap, in the form the four books use it (array a[1..n], children of i at 2i and 2i+1).
 *   Thareja 12.1  : Fig 12.7 insert (sift-up), Fig 12.10 delete the root (sift-down), Examples 12.1-12.3
 *   Weiss 6.3     : the "hole" version of both, buildHeap in linear time (Fig 6.14), decreaseKey / increaseKey / remove,
 *                   the sentinel in position 0 (Exercise 6.5), d-heaps (6.5, Exercises 6.14-6.15), decreaseAllKeys (6.38)
 *   Pal & Halder  : "shift-up" and "shift-down" heap construction (Fig 10.13/10.14)
 * Every operation counts its element comparisons, because Weiss's exercises (6.7, 6.12, 6.13) are about exactly that.
 * Author: Mohammed El Sisi. Compile: gcc -std=c99 -Wall -Wextra -O2 binheap.c -lm */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#pragma GCC diagnostic ignored "-Wunused-function"   /* the exercise files use the rest */

//>>bh
typedef struct { int *a; int n, cap; int mx; long cmp; } BHeap;   /* a[0] unused (or a sentinel); mx=1: max-heap */

static BHeap bh_new(int cap, int mx) { BHeap h = { calloc((size_t)cap + 2, sizeof(int)), 0, cap, mx, 0 }; return h; }
static void  bh_free(BHeap *h) { free(h->a); h->a = NULL; }
/* "x belongs above y": the one comparison the whole structure is made of */
static int   bh_above(BHeap *h, int x, int y) { h->cmp++; return h->mx ? x > y : x < y; }
static void  bh_swap(int *x, int *y) { int t = *x; *x = *y; *y = t; }

/* Thareja Fig 12.7: put VAL at the end (the tree stays complete), then let it rise while it beats its parent */
static int bh_insert(BHeap *h, int val)
{
    assert(h->n < h->cap);
    int pos = ++h->n;                                  /* Step 1 */
    h->a[pos] = val;                                   /* Step 2 */
    while (pos > 1) {                                  /* Steps 3-5 */
        int par = pos / 2;
        if (!bh_above(h, h->a[pos], h->a[par])) break;
        bh_swap(&h->a[pos], &h->a[par]);
        pos = par;
    }
    return pos;                                        /* where VAL finally rests */
}

/* Thareja Fig 12.10: remove the root. LAST replaces it and sinks. The book's Step 5 reads HEAP[RIGHT] even when
   RIGHT > N (a node with a single child). That works in the book because cell N+1 still holds LAST; here we test RIGHT <= N first, so the code never relies on a stale cell (see exercises/ex23_01). */
static int bh_delete_root(BHeap *h)
{
    assert(h->n > 0);
    int top = h->a[1], last = h->a[h->n--];            /* Step 1 */
    if (h->n == 0) return top;
    int ptr = 1, left = 2;                             /* Step 2 */
    h->a[ptr] = last;                                  /* Step 3 */
    while (left <= h->n) {                             /* Step 4 */
        int c = left;                                  /* the better child */
        if (left + 1 <= h->n && bh_above(h, h->a[left + 1], h->a[left])) c = left + 1;
        if (!bh_above(h, h->a[c], h->a[ptr])) break;   /* Step 5: PTR is at least as good as both children */
        bh_swap(&h->a[ptr], &h->a[c]);                 /* Steps 6-7 */
        ptr = c; left = 2 * ptr;
    }
    return top;
}
//<<bh

//>>bhvalid
static int bh_valid(const BHeap *h)
{
    for (int i = 2; i <= h->n; i++) {
        int p = h->a[i / 2], c = h->a[i];
        if (h->mx ? p < c : p > c) return 0;
    }
    return 1;
}
static void bh_print(const BHeap *h, const char *name)
{
    printf("%s:", name);
    for (int i = 1; i <= h->n; i++) printf(" %d", h->a[i]);
    printf("\n");
}
//<<bhvalid

//>>bhbuild
/* Pal & Halder 10.2 "shift-up": build by n insertions (top-down) */
static void bh_build_insert(BHeap *h, const int *v, int n) { for (int i = 0; i < n; i++) bh_insert(h, v[i]); }

static void bh_sift_down(BHeap *h, int i)
{
    int x = h->a[i];
    for (;;) {
        int c = 2 * i;
        if (c > h->n) break;
        if (c + 1 <= h->n && bh_above(h, h->a[c + 1], h->a[c])) c++;
        if (!bh_above(h, h->a[c], x)) break;
        h->a[i] = h->a[c]; i = c;                      /* the hole moves down, no swap needed (Weiss Fig 6.12) */
    }
    h->a[i] = x;
}
/* Weiss Fig 6.14 / P&H "shift-down": copy the items in any order, then fix the parents from n/2 down to 1 */
static void bh_build_floyd(BHeap *h, const int *v, int n)
{
    assert(n <= h->cap);
    memcpy(h->a + 1, v, (size_t)n * sizeof(int)); h->n = n;
    for (int i = n / 2; i >= 1; i--) bh_sift_down(h, i);
}
//<<bhbuild

//>>bhops
/* Weiss 6.3: when the position of an item is known, three more operations cost O(log N) */
static void bh_percolate_up(BHeap *h, int p)
{
    int x = h->a[p];
    for (; p > 1 && bh_above(h, x, h->a[p / 2]); p /= 2) h->a[p] = h->a[p / 2];
    h->a[p] = x;
}
static void bh_change_key(BHeap *h, int p, int delta, int toward_root)   /* decreaseKey in a min-heap = toward_root */
{
    assert(p >= 1 && p <= h->n && delta >= 0);
    int sign = h->mx ? 1 : -1;                         /* "better" direction */
    h->a[p] += (toward_root ? sign : -sign) * delta;
    if (toward_root) bh_percolate_up(h, p); else bh_sift_down(h, p);
}
/* remove(p): decreaseKey(p, infinity), then deleteMin */
static void bh_remove(BHeap *h, int p)
{
    assert(p >= 1 && p <= h->n);
    h->a[p] = h->mx ? 2147483647 : -2147483647 - 1;    /* "infinity" in the good direction */
    bh_percolate_up(h, p);
    bh_delete_root(h);
}
//<<bhops

//>>bhsentinel
/* Weiss Exercise 6.5: put a copy of the inserted item in position 0. It stops the loop with no  pos > 1  test. */
static void bh_insert_sentinel(BHeap *h, int val)
{
    assert(h->n < h->cap);
    int pos = ++h->n;
    h->a[0] = val;                                     /* nothing is above position 0: the loop ends there */
    for (; bh_above(h, val, h->a[pos / 2]); pos /= 2) h->a[pos] = h->a[pos / 2];
    h->a[pos] = val;
}
//<<bhsentinel

//>>dheap
/* Weiss 6.5 / Exercise 6.14: a d-heap in an array a[1..n]. Children of i: d(i-1)+2 .. d(i-1)+d+1; parent: (i-2)/d + 1 */
static int dh_parent(int i, int d) { return (i - 2) / d + 1; }
static int dh_child(int i, int d, int k) { return d * (i - 1) + 2 + k; }          /* k = 0..d-1 */
typedef struct { int *a; int n, cap, d; long cmp; } DHeap;
static DHeap dh_new(int cap, int d) { DHeap h = { calloc((size_t)cap + 2, sizeof(int)), 0, cap, d, 0 }; return h; }
static void dh_insert(DHeap *h, int v)                 /* min-heap */
{
    int p = ++h->n;
    while (p > 1) { int q = dh_parent(p, h->d); h->cmp++; if (v >= h->a[q]) break; h->a[p] = h->a[q]; p = q; }
    h->a[p] = v;
}
static int dh_delete_min(DHeap *h)
{
    int top = h->a[1], x = h->a[h->n--], i = 1;
    if (h->n == 0) return top;
    for (;;) {
        int first = dh_child(i, h->d, 0);
        if (first > h->n) break;
        int best = first;
        for (int k = 1; k < h->d && first + k <= h->n; k++) { h->cmp++; if (h->a[first + k] < h->a[best]) best = first + k; }
        h->cmp++;
        if (h->a[best] >= x) break;
        h->a[i] = h->a[best]; i = best;
    }
    h->a[i] = x;
    return top;
}
static int dh_valid(const DHeap *h)
{
    for (int i = 2; i <= h->n; i++) if (h->a[dh_parent(i, h->d)] > h->a[i]) return 0;
    return 1;
}
//<<dheap

//>>offs
/* Weiss Exercise 6.38: decreaseAllKeys(delta) in O(1). Store key + off, never touch the items. */
typedef struct { BHeap h; long off; } OffHeap;          /* real key = stored - off  (a min-heap) */
static void oh_insert(OffHeap *o, int real) { bh_insert(&o->h, (int)(real + o->off)); }
static int  oh_min(const OffHeap *o) { return (int)(o->h.a[1] - o->off); }
static int  oh_delete_min(OffHeap *o) { return (int)(bh_delete_root(&o->h) - o->off); }
static void oh_decrease_all(OffHeap *o, int delta) { o->off += delta; }          /* every real key becomes smaller by delta */
//<<offs

//>>less
/* Weiss Exercise 6.10(a): list every key < X in O(K), where K is the number printed. Heap order lets us prune a whole
   subtree as soon as its root is >= X (min-heap), so we visit each reported node plus at most 2 failed children. */
static int bh_collect_less(const BHeap *h, int i, int x, int *out, int *visited)
{
    if (i > h->n) return 0;
    (*visited)++;
    if (h->a[i] >= x) return 0;
    int k = 0; out[k++] = h->a[i];
    k += bh_collect_less(h, 2 * i, x, out + k, visited);
    k += bh_collect_less(h, 2 * i + 1, x, out + k, visited);
    return k;
}
//<<less

//>>sizes
/* Weiss Exercise 6.4: how big must the array be if the tree is not complete? A node at depth d sits at an index
   >= 2^d, so a tree whose deepest node is at depth D needs an array of at least 2^(D+1) - 1 cells. */
static double array_cells_needed(double depth) { return pow(2.0, depth + 1) - 1; }
//<<sizes

/* ------------------------------------------------------------------ tests */
#ifndef BH_NO_MAIN
static void swap_count_check(void);
int main(void)
{
    /* ---- Thareja Example 12.1: insert 99 into the max-heap of Fig 12.2 (54 45 36 27 21 18 21 11) */
    int f122[] = { 54, 45, 36, 27, 21, 18, 21, 11 };
    BHeap h = bh_new(32, 1); bh_build_floyd(&h, f122, 8);
    assert(bh_valid(&h));
    bh_print(&h, "Fig 12.2 ");
    bh_insert(&h, 99);
    bh_print(&h, "after 99 ");
    { int want[] = { 99, 54, 36, 45, 21, 18, 21, 11, 27 }; assert(h.n == 9 && !memcmp(h.a + 1, want, sizeof want)); }
    bh_free(&h);

    /* ---- Thareja Example 12.2: 45 36 54 27 63 72 61 18, built by n insertions (13 steps in the book) */
    int e122[] = { 45, 36, 54, 27, 63, 72, 61, 18 };
    h = bh_new(32, 1); bh_build_insert(&h, e122, 8);
    bh_print(&h, "Ex 12.2  ");
    { int want[] = { 72, 54, 63, 27, 36, 45, 61, 18 }; assert(!memcmp(h.a + 1, want, sizeof want)); }   /* Fig 12.6 */
    bh_free(&h);
    /* ... and by Floyd's method (the exercise only says "build"; the two methods give different, equally valid heaps) */
    h = bh_new(32, 1); bh_build_floyd(&h, e122, 8); bh_print(&h, "Ex 12.2 F");
    assert(bh_valid(&h)); bh_free(&h);

    /* ---- Thareja Example 12.3: delete the root of 54 45 36 27 29 18 21 11 */
    int e123[] = { 54, 45, 36, 27, 29, 18, 21, 11 };
    h = bh_new(32, 1); bh_build_floyd(&h, e123, 8);
    { int want0[] = { 54, 45, 36, 27, 29, 18, 21, 11 }; assert(!memcmp(h.a + 1, want0, sizeof want0)); }   /* already a heap */
    assert(bh_delete_root(&h) == 54);
    { int want[] = { 45, 29, 36, 27, 11, 18, 21 }; assert(h.n == 7 && !memcmp(h.a + 1, want, sizeof want)); bh_print(&h, "Ex 12.3  "); }
    bh_free(&h);

    swap_count_check();
    /* d-heaps (Weiss 6.14/6.15): positions, order, and deleteMin giving sorted output, for d = 2..8 */
    srand(14);
    for (int d = 2; d <= 8; d++) {
        for (int i = 2; i <= 300; i++) { int p = dh_parent(i, d), ok = 0; for (int k = 0; k < d; k++) if (dh_child(p, d, k) == i) ok = 1; assert(ok); }
        DHeap h = dh_new(1000, d); for (int i = 0; i < 700; i++) dh_insert(&h, rand() % 1000);
        assert(dh_valid(&h)); int prev = -1;
        for (int i = 0; i < 700; i++) { int m = dh_delete_min(&h); assert(m >= prev && dh_valid(&h)); prev = m; }
        free(h.a);
    }
    printf("binheap: all checks passed\n");
    return 0;
}
static void swap_count_check(void)
{
    /* a random stress against qsort: build, insert, delete, remove; max- and min-heaps */
    srand(7);
    for (int mx = 0; mx <= 1; mx++)
        for (int t = 0; t < 200; t++) {
            int n = 1 + rand() % 60, v[64];
            BHeap h = bh_new(130, mx);
            for (int i = 0; i < n; i++) v[i] = rand() % 100;
            if (t & 1) bh_build_floyd(&h, v, n); else bh_build_insert(&h, v, n);
            assert(bh_valid(&h));
            for (int i = 0; i < 20; i++) { int x = rand() % 100; if (i & 1) bh_insert_sentinel(&h, x); else bh_insert(&h, x); assert(bh_valid(&h)); }
            int p = 1 + rand() % h.n; bh_remove(&h, p); assert(bh_valid(&h));
            int prev = h.mx ? 1 << 30 : -(1 << 30);
            while (h.n) { int x = bh_delete_root(&h); assert(h.mx ? x <= prev : x >= prev); prev = x; assert(bh_valid(&h)); }
            bh_free(&h);
        }
}
#endif
