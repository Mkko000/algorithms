/* ============================================================
 *  randomized_bst.c  —  Randomized BSTs (insert, join, delete) and
 *                       explicit rebalancing by partitioning
 *  Chapter 13: Balanced Trees (Introduction + Section 13.1)
 *
 *  - Program 13.1 (balance): partition the tree so the MEDIAN comes to
 *    the root (partR from Program 12.14), then balance both subtrees
 *    recursively. O(N) work makes a perfectly balanced tree — but we
 *    cannot afford it after every insertion.
 *  - Program 13.2 (randomized insert): a new key becomes the root of a
 *    subtree of size N with probability 1/(N+1) (root insertion),
 *    otherwise we recurse. Property 13.1: the tree is distributed like
 *    a BST built from a RANDOM permutation, whatever the input order:
 *    ~2N ln N compares to build, ~2 ln N per search.
 *  - Program 13.3 (randomized combination of two ARBITRARY BSTs): like
 *    Program 12.16, but the root of a or of b is chosen with probability
 *    proportional to the sizes, so every node is equally likely root.
 *  - Program 13.4 (randomized delete): delete = replace the node by the
 *    randomized joinLR of its two subtrees (all keys left < all right).
 *  The demo inserts keys in SORTED order (the worst case for a plain
 *  BST) and compares heights.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 13.1-13.4
 *  Build:   gcc -std=c99 -Wall -O2 randomized_bst.c -o randomized_bst -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef int Key;
typedef struct node *link;
struct node { Key key; link l, r; int N; };
static link z;

static link NEW(Key k, link l, link r, int N) { link x = malloc(sizeof *x); x->key = k; x->l = l; x->r = r; x->N = N; return x; }
static void fixN(link h) { if (h != z) h->N = h->l->N + h->r->N + 1; }
static link rotR(link h) { link x = h->l; h->l = x->r; x->r = h; fixN(h); fixN(x); return x; }
static link rotL(link h) { link x = h->r; h->r = x->l; x->l = h; fixN(h); fixN(x); return x; }

static link insertT(link h, Key v) {             /* root insertion (Program 12.12) */
    if (h == z) return NEW(v, z, z, 1);
    if (v < h->key) { h->l = insertT(h->l, v); h = rotR(h); }
    else            { h->r = insertT(h->r, v); h = rotL(h); }
    return h;
}
static link insert_plain(link h, Key v) {
    if (h == z) return NEW(v, z, z, 1);
    if (v < h->key) h->l = insert_plain(h->l, v); else h->r = insert_plain(h->r, v);
    h->N++; return h;
}
static link partR(link h, int k) {               /* Program 12.14 */
    int t = h->l->N;
    if (t > k) { h->l = partR(h->l, k); h = rotR(h); }
    if (t < k) { h->r = partR(h->r, k - t - 1); h = rotL(h); }
    return h;
}

/* Program 13.1 */
link balanceR(link h) {
    if (h->N < 2) return h;
    h = partR(h, h->N / 2);
    h->l = balanceR(h->l);
    h->r = balanceR(h->r);
    return h;
}

/* Program 13.2 */
link insertR(link h, Key v) {
    if (h == z) return NEW(v, z, z, 1);
    if (rand() < RAND_MAX / (h->N + 1)) return insertT(h, v);
    if (v < h->key) h->l = insertR(h->l, v);
    else            h->r = insertR(h->r, v);
    h->N++; return h;
}

/* Program 13.4 (helper): randomized join, all keys in a < all keys in b */
link joinLR(link a, link b) {
    if (a == z) return b;
    if (b == z) return a;
    if (rand() / (RAND_MAX / (a->N + b->N) + 1) < a->N) { a->r = joinLR(a->r, b); fixN(a); return a; }
    else                                               { b->l = joinLR(a, b->l); fixN(b); return b; }
}

/* Program 13.3: randomized combination of two arbitrary BSTs */
static link STjoin(link a, link b);
static link joinR(link a, link b) {
    if (a == z) return b;
    link al = a->l, ar = a->r; Key v = a->key; free(a);
    b = insertR(b, v);                 /* randomized insert of a's root into b */
    /* insertR may not have put v at the root; split b around v to be safe */
    int k = 0; for (link t = b; t != z; ) { if (v < t->key) t = t->l; else { k += t->l->N + 1; t = t->r; } }
    b = partR(b, k - 1);               /* v is the (k-1)th smallest: bring it to the root */
    b->l = STjoin(al, b->l);
    b->r = STjoin(ar, b->r);
    fixN(b);
    return b;
}
static link STjoin(link a, link b) {
    if (a == z) return b;
    if (b == z) return a;
    if (rand() / (RAND_MAX / (a->N + b->N) + 1) < a->N) return joinR(a, b);
    else return joinR(b, a);
}

/* Program 13.4 */
link deleteR(link h, Key v) {
    if (h == z) return z;
    if (v < h->key) h->l = deleteR(h->l, v);
    if (h->key < v) h->r = deleteR(h->r, v);
    if (h->key == v) { link t = h; h = joinLR(h->l, h->r); free(t); }
    fixN(h);
    return h;
}

static int height(link h) { if (h == z) return -1; int a = height(h->l), b = height(h->r); return 1 + (a > b ? a : b); }
static long ipl(link h, int d) { return h == z ? 0 : d + ipl(h->l, d + 1) + ipl(h->r, d + 1); }
static int is_bst(link h, long lo, long hi) { return h == z || (h->key > lo && h->key < hi && is_bst(h->l, lo, h->key) && is_bst(h->r, h->key, hi)); }
static void freeT(link h) { if (h == z) return; freeT(h->l); freeT(h->r); free(h); }

int main(void) {
    z = NEW(0, NULL, NULL, 0); z->l = z->r = z;
    const int N = 3000;
    srand(13);
    link plain = z, rnd = z;
    for (int i = 0; i < N; i++) { plain = insert_plain(plain, i); rnd = insertR(rnd, i); }
    printf("keys 0..%d inserted in ORDER\n", N - 1);
    printf("  plain BST     : height %4d, avg depth %7.1f\n", height(plain), (double)ipl(plain, 0) / N);
    printf("  randomized BST: height %4d, avg depth %7.1f   (2 ln N = %.1f)\n", height(rnd), (double)ipl(rnd, 0) / N, 2 * log(N));
    plain = balanceR(plain);
    printf("  plain after balanceR (Program 13.1): height %d (lg N = %.1f)\n", height(plain), log2(N));
    for (int i = 0; i < N; i += 2) rnd = deleteR(rnd, i);      /* delete all even keys */
    printf("  randomized BST after deleting the %d even keys: N=%d, height %d, still a BST: %s\n",
           N / 2, rnd->N, height(rnd), is_bst(rnd, -1, N) ? "yes" : "NO");
    /* Program 13.3: combine two arbitrary randomized BSTs with interleaved keys */
    link a = z, b = z;
    for (int i = 0; i < N; i++) { if (i % 3 == 0) a = insertR(a, 2 * i + 1); else b = insertR(b, 2 * i + 1); }
    int na = a->N, nb = b->N;
    link c = STjoin(a, b);
    printf("  STjoin of trees with %d and %d interleaved keys: N=%d, height %d, still a BST: %s\n",
           na, nb, c->N, height(c), is_bst(c, -1, 2 * N + 2) ? "yes" : "NO");
    freeT(plain); freeT(rnd); freeT(c); free(z);
    return 0;
}
