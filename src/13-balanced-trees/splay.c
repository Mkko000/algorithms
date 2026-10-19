/* ============================================================
 *  splay.c  —  Splay BSTs: self-adjusting via double rotations
 *  Chapter 13: Balanced Trees (Section 13.2)
 *
 *  Program 13.5: splay insertion. Like root insertion, but when the
 *  search path goes two steps in the SAME direction (zig-zig) we rotate
 *  the GRANDPARENT first, then the parent. That small change halves
 *  the depth of every node on the search path, so even after the worst
 *  sequence the tree recovers. Property 13.4/13.5: any M operations on
 *  an N-node splay tree cost O((N+M) lg(N+M)) — an AMORTIZED guarantee
 *  (single operations may be slow; the total never is).
 *  Searches also splay: frequently accessed keys float to the top.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 13.5
 *  Build:   gcc -std=c99 -Wall -O2 splay.c -o splay
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef int Key;
typedef struct node *link;
struct node { Key key; link l, r; int N; };
static link z;
static link NEW(Key k, link l, link r, int N) { link x = malloc(sizeof *x); x->key = k; x->l = l; x->r = r; x->N = N; return x; }
static void fixN(link h) { if (h != z) h->N = h->l->N + h->r->N + 1; }
static link rotR(link h) { link x = h->l; h->l = x->r; x->r = h; fixN(h); fixN(x); return x; }
static link rotL(link h) { link x = h->r; h->r = x->l; x->l = h; fixN(h); fixN(x); return x; }

/* Program 13.5 */
link splay(link h, Key v) {
    if (h == z) return NEW(v, z, z, 1);
    if (v < h->key) {
        if (h->l == z) return rotR((h->l = NEW(v, z, z, 1), fixN(h), h));
        if (v < h->l->key) { h->l->l = splay(h->l->l, v); fixN(h->l); h = rotR(h); }   /* zig-zig */
        else               { h->l->r = splay(h->l->r, v); fixN(h->l); h->l = rotL(h->l); } /* zig-zag */
        return rotR(h);
    } else {
        if (h->r == z) return rotL((h->r = NEW(v, z, z, 1), fixN(h), h));
        if (h->r->key < v) { h->r->r = splay(h->r->r, v); fixN(h->r); h = rotL(h); }
        else               { h->r->l = splay(h->r->l, v); fixN(h->r); h->r = rotR(h->r); }
        return rotL(h);
    }
}

/* splay search: bring v (or the last node on its path) to the root */
link splay_search(link h, Key v) {
    if (h == z || h->key == v) return h;
    if (v < h->key) {
        if (h->l == z) return h;
        if (v < h->l->key)      { h->l->l = splay_search(h->l->l, v); fixN(h->l); h = rotR(h); }
        else if (h->l->key < v) { h->l->r = splay_search(h->l->r, v); fixN(h->l); if (h->l->r != z) h->l = rotL(h->l); }
        return h->l == z ? h : rotR(h);
    } else {
        if (h->r == z) return h;
        if (h->r->key < v)      { h->r->r = splay_search(h->r->r, v); fixN(h->r); h = rotL(h); }
        else if (v < h->r->key) { h->r->l = splay_search(h->r->l, v); fixN(h->r); if (h->r->l != z) h->r = rotR(h->r); }
        return h->r == z ? h : rotL(h);
    }
}

static int height(link h) { if (h == z) return -1; int a = height(h->l), b = height(h->r); return 1 + (a > b ? a : b); }
static int is_bst(link h, long lo, long hi) { return h == z || (h->key > lo && h->key < hi && is_bst(h->l, lo, h->key) && is_bst(h->r, h->key, hi)); }
static int depth(link h, Key v) { int d = 0; while (h != z && h->key != v) { h = v < h->key ? h->l : h->r; d++; } return d; }
static void freeT(link h) { if (h == z) return; freeT(h->l); freeT(h->r); free(h); }

int main(void) {
    z = NEW(0, NULL, NULL, 0); z->l = z->r = z;
    const int N = 1024; link h = z;
    for (int i = 0; i < N; i++) h = splay(h, i);          /* sorted insertions */
    printf("after %d sorted splay insertions: height %d (a path! but the cost was only ~2N)\n", N, height(h));
    printf("depth of key 0 = %d\n", depth(h, 0));
    h = splay_search(h, 0);
    printf("after ONE splay search for 0: root=%d, height %d (halved)\n", h->key, height(h));
    for (int k = 0; k < 5; k++) h = splay_search(h, rand() % N);
    printf("after 5 more random searches: height %d, BST ok: %s, N=%d\n", height(h), is_bst(h, -1, N) ? "yes" : "NO", h->N);
    freeT(h); free(z);
    return 0;
}
