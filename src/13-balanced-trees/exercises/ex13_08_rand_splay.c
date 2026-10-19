/* ============================================================
 *  ex13_08_rand_splay.c  —  Exercises 13.8, 13.25, 13.26
 *  Chapter 13: Balanced Trees — randomized BSTs and splay trees
 *
 *  13.8  The randomized BST from inserting E A S Y Q U T I O N, with a bad
 *        randomization function that takes the root-insertion option
 *        whenever the tree size is odd.
 *  13.25 The splay BST from inserting E A S Y Q U T I O N by splay insertion.
 *  13.26 How many links must be changed for a double rotation?  How many
 *        are actually changed for each double rotation in Program 13.5?
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 13.2, 13.5, Exercises 13.8-13.26
 *  Build:   gcc -std=c99 -Wall -O2 ex13_08_rand_splay.c -o ex13_08
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct node *link;
struct node { int key, N; link l, r; };
static link z;
static long writes, changed;                       /* link assignments / ones that change a link */
#define SET(P, V) do { link v_ = (V); writes++; if ((P) != v_) changed++; (P) = v_; } while (0)
static link NEW(int k, link l, link r, int N) { link x = malloc(sizeof *x); x->key = k; x->l = l; x->r = r; x->N = N; return x; }
static void fixN(link h) { if (h != z) h->N = h->l->N + h->r->N + 1; }
static link rotR(link h) { link x = h->l; SET(h->l, x->r); SET(x->r, h); fixN(h); fixN(x); return x; }
static link rotL(link h) { link x = h->r; SET(h->r, x->l); SET(x->l, h); fixN(h); fixN(x); return x; }

static link insertT(link h, int k)                  /* root insertion */
{
    if (h == z) return NEW(k, z, z, 1);
    if (k < h->key) { h->l = insertT(h->l, k); h = rotR(h); }
    else            { h->r = insertT(h->r, k); h = rotL(h); }
    return h;
}
static link insertR(link h, int k)                  /* Program 13.2 with the BAD coin: odd size -> root */
{
    if (h == z) return NEW(k, z, z, 1);
    if (h->N % 2 == 1) return insertT(h, k);
    if (k < h->key) h->l = insertR(h->l, k); else h->r = insertR(h->r, k);
    h->N++;
    return h;
}

static long dbl, dbl_changed, dbl_writes, zz[2], zzc[2];   /* [0] zig-zig, [1] zig-zag */
static link splay(link h, int v)                     /* Program 13.5 */
{
    if (h == z) return NEW(v, z, z, 1);
    if (v < h->key) {
        if (h->l == z) { h->l = NEW(v, z, z, 1); fixN(h); return rotR(h); }
        long w0 = writes, c0 = changed;
        int zag = !(v < h->l->key);
        if (!zag) { SET(h->l->l, splay(h->l->l, v)); fixN(h->l); w0 = writes; c0 = changed; h = rotR(h); }
        else      { SET(h->l->r, splay(h->l->r, v)); fixN(h->l); w0 = writes; c0 = changed; SET(h->l, rotL(h->l)); }
        h = rotR(h);
        dbl++; dbl_writes += writes - w0; dbl_changed += changed - c0; zz[zag]++; zzc[zag] += changed - c0;
        return h;
    } else {
        if (h->r == z) { h->r = NEW(v, z, z, 1); fixN(h); return rotL(h); }
        long w0 = writes, c0 = changed;
        int zag = !(h->r->key < v);
        if (!zag) { SET(h->r->r, splay(h->r->r, v)); fixN(h->r); w0 = writes; c0 = changed; h = rotL(h); }
        else      { SET(h->r->l, splay(h->r->l, v)); fixN(h->r); w0 = writes; c0 = changed; SET(h->r, rotR(h->r)); }
        h = rotL(h);
        dbl++; dbl_writes += writes - w0; dbl_changed += changed - c0; zz[zag]++; zzc[zag] += changed - c0;
        return h;
    }
}
static void show(link h) { if (h == z) { printf("-"); return; } printf("%c", h->key); if (h->l != z || h->r != z) { printf("("); show(h->l); printf(","); show(h->r); printf(")"); } }
static int height(link h) { if (h == z) return 0; int a = height(h->l), b = height(h->r); return 1 + (a > b ? a : b); }

int main(void)
{
    z = NEW(0, NULL, NULL, 0); z->l = z->r = z;
    const char *s = "EASYQUTION";
    link t = z;
    printf("13.8 randomized BST, root insertion whenever the (sub)tree size is odd:\n");
    for (int i = 0; s[i]; i++) { t = insertR(t, s[i]); printf("     +%c  ", s[i]); show(t); printf("\n"); }
    printf("     height %d\n", height(t));

    t = z;
    printf("13.25 splay insertion:\n");
    for (int i = 0; s[i]; i++) { t = splay(t, s[i]); printf("     +%c  ", s[i]); show(t); printf("\n"); }
    printf("     height %d\n", height(t));

    /* 13.26: a double rotation rebuilds 3 nodes: links that must end up different */
    printf("13.26 double rotations in this run: %ld; per double rotation: %.2f link writes, %.2f actually changed\n",
           dbl, (double)dbl_writes / dbl, (double)dbl_changed / dbl);
    {   /* larger random test */
        unsigned long long rs = 3; t = z; dbl = dbl_writes = dbl_changed = 0;
        for (int i = 0; i < 100000; i++) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; t = splay(t, (int)(rs >> 40)); }
        printf("     100000 random splay insertions: %ld double rotations, %.2f writes, %.2f changed each\n",
               dbl, (double)dbl_writes / dbl, (double)dbl_changed / dbl);
        printf("     zig-zig: %.2f links changed each   zig-zag: %.2f links changed each   (+1 for the parent's link)\n",
               (double)zzc[0] / zz[0], (double)zzc[1] / zz[1]);
    }
    return 0;
}
