/* ============================================================
 *  bst_ops.c  —  The rest of the BST symbol-table ADT:
 *                select, partition, delete, join (+ count fields)
 *  Chapter 12: Symbol Tables and BSTs (Sections 12.5, 12.6, 12.9)
 *
 *  Every node keeps N = size of its subtree, which makes rank-based
 *  operations O(height):
 *   - Program 12.9:  nonrecursive insert (walk down, hang the new node)
 *   - Program 12.13: select(k): k-th smallest using the counts
 *   - Program 12.14: partition(k): rotate the k-th smallest to the ROOT
 *   - Program 12.15: delete(v): remove v's node, replace it by the join
 *                    of its subtrees (partition right subtree on 0 so its
 *                    smallest key comes to the top, then hang left under it)
 *   - Program 12.16: join(a, b) of two arbitrary BSTs by root insertion
 *  Property 12.6 is also checked: random insertion order -> average
 *  search ~ 1.39 lg N compares; sorted order -> a path of length N.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 12.9, 12.13-12.16
 *  Build:   gcc -std=c99 -Wall -O2 bst_ops.c -o bst_ops -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef int Key;
typedef struct STnode *link;
struct STnode { Key key; link l, r; int N; };
static link z;                                  /* sentinel external node, N=0 */

static link NEW(Key k, link l, link r, int N) { link x = malloc(sizeof *x); x->key = k; x->l = l; x->r = r; x->N = N; return x; }
static void fixN(link h) { h->N = h->l->N + h->r->N + 1; }
static link rotR(link h) { link x = h->l; h->l = x->r; x->r = h; fixN(h); fixN(x); return x; }
static link rotL(link h) { link x = h->r; h->r = x->l; x->l = h; fixN(h); fixN(x); return x; }

/* Program 12.9: nonrecursive insertion (maintaining counts on the way down) */
link insert_nr(link head, Key v) {
    if (head == z) return NEW(v, z, z, 1);
    link p = head, x = head;
    while (x != z) { p = x; x->N++; x = (v < x->key) ? x->l : x->r; }
    x = NEW(v, z, z, 1);
    if (v < p->key) p->l = x; else p->r = x;
    return head;
}

/* Program 12.13 */
Key selectR(link h, int k) {
    if (h == z) return -1;
    int t = h->l->N;
    if (t > k) return selectR(h->l, k);
    if (t < k) return selectR(h->r, k - t - 1);
    return h->key;
}

/* Program 12.14 */
link partR(link h, int k) {
    int t = h->l->N;
    if (t > k) { h->l = partR(h->l, k); h = rotR(h); }
    if (t < k) { h->r = partR(h->r, k - t - 1); h = rotL(h); }
    return h;
}

/* Program 12.15 */
static link joinLR(link a, link b) {
    if (b == z) return a;
    b = partR(b, 0);           /* smallest key of b to the root: it has an empty left link */
    b->l = a; fixN(b);
    return b;
}
link deleteR(link h, Key v) {
    if (h == z) return z;
    if (v < h->key) h->l = deleteR(h->l, v);
    if (h->key < v) h->r = deleteR(h->r, v);
    if (v == h->key) { link t = h; h = joinLR(h->l, h->r); free(t); }
    if (h != z) fixN(h);
    return h;
}

/* Program 12.16: join two BSTs (keys may interleave) via root insertion */
static link insertT(link h, Key v) {
    if (h == z) return NEW(v, z, z, 1);
    if (v < h->key) { h->l = insertT(h->l, v); h = rotR(h); }
    else            { h->r = insertT(h->r, v); h = rotL(h); }
    return h;
}
link STjoin(link a, link b) {
    if (b == z) return a;
    if (a == z) return b;
    b = insertT(b, a->key);           /* a's root becomes b's root, splitting b */
    b->l = STjoin(a->l, b->l);
    b->r = STjoin(a->r, b->r);
    free(a); fixN(b);
    return b;
}

static void inorder(link h) { if (h == z) return; inorder(h->l); printf(" %d", h->key); inorder(h->r); }
static int height(link h) { if (h == z) return -1; int a = height(h->l), b = height(h->r); return 1 + (a > b ? a : b); }
static long ipl(link h, int d) { return h == z ? 0 : d + ipl(h->l, d + 1) + ipl(h->r, d + 1); }
static void freeT(link h) { if (h == z) return; freeT(h->l); freeT(h->r); free(h); }

int main(void) {
    z = NEW(0, NULL, NULL, 0); z->l = z->r = z;
    link t = z;
    int keys[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 65};
    for (int i = 0; i < 10; i++) t = insert_nr(t, keys[i]);
    printf("inorder:"); inorder(t); printf("   (N=%d, height=%d)\n", t->N, height(t));
    printf("select(0)=%d select(4)=%d select(9)=%d\n", selectR(t, 0), selectR(t, 4), selectR(t, 9));
    t = partR(t, 4); printf("after partition(4) root=%d (the 5th smallest)\n", t->key);
    t = deleteR(t, 30); t = deleteR(t, 50);
    printf("after delete 30, 50:"); inorder(t); printf("   N=%d\n", t->N);
    link u = z; int more[] = {33, 55, 99, 1};
    for (int i = 0; i < 4; i++) u = insert_nr(u, more[i]);
    t = STjoin(t, u);
    printf("after join with {33 55 99 1}:"); inorder(t); printf("   N=%d\n", t->N);
    freeT(t);

    const int N = 1 << 14; link r = z, s = z;
    srand(6);
    for (int i = 0; i < N; i++) r = insert_nr(r, rand());
    for (int i = 0; i < 2000; i++) s = insert_nr(s, i);
    printf("random keys N=%d: avg depth %.2f (about 2 ln N = 1.39 lg N = %.2f, minus a small constant), height %d\n",
           N, (double)ipl(r, 0) / N + 1, 1.39 * log2(N), height(r));
    printf("sorted keys N=2000: height %d  <- the worst case of Property 12.8\n", height(s));
    freeT(r); freeT(s); free(z);
    return 0;
}
