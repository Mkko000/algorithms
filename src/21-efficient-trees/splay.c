/* ============================================================
 *  splay.c  —  splay trees (self-adjusting BSTs)
 *
 *  Idea: every time a node is accessed (search / insert / delete) it is rotated up to the ROOT by a
 *  sequence of "splay steps". Recently used keys are near the top, so a skewed access pattern is fast,
 *  and although one operation can cost O(n) the amortised cost is O(log n).
 *
 *  Let N be the accessed node, P its parent, G its grandparent:
 *    ZIG       P is the root                 -> one rotation at P
 *    ZIG-ZIG   N and P are both left (or both right) children -> rotate G FIRST, then P
 *    ZIG-ZAG   N is a left child and P is a right child (or vice versa) -> rotate P, then G
 *  (Thareja calls the last two "zig-zig" and "zig-zag"; the single rotation case is "zig", done when
 *   the parent of N is the root.)
 *  Note the order in zig-zig: rotating P first would NOT give the amortised guarantee.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja 10.6; Weiss 4.5 (splay trees, top-down variant in the lesson);
 *                Bandyopadhyay & Dey (self-adjusting trees)
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 splay.c -o splay
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#pragma GCC diagnostic ignored "-Wunused-function"

typedef struct SN { int key; struct SN *l, *r, *p; } SN;
typedef struct { SN *root; char log[256]; } ST;

//>>splay
static void slog(ST *t, const char *s) { if (strlen(t->log) + strlen(s) + 2 < sizeof t->log) { if (t->log[0]) strcat(t->log, " "); strcat(t->log, s); } }

static void rotate_up(ST *t, SN *x)           /* rotate x above its parent (right rotation if x is a left child) */
{
    SN *p = x->p, *g = p->p;
    if (x == p->l) { p->l = x->r; if (x->r) x->r->p = p; x->r = p; }
    else           { p->r = x->l; if (x->l) x->l->p = p; x->l = p; }
    p->p = x; x->p = g;
    if (!g) t->root = x; else if (g->l == p) g->l = x; else g->r = x;
}
static void splay(ST *t, SN *x)
{
    while (x->p) {
        SN *p = x->p, *g = p->p;
        if (!g) { rotate_up(t, x); slog(t, "zig"); }                           /* P is the root */
        else if ((g->l == p) == (p->l == x)) { rotate_up(t, p); rotate_up(t, x); slog(t, "zigzig"); }   /* same side */
        else { rotate_up(t, x); rotate_up(t, x); slog(t, "zigzag"); }          /* opposite sides */
    }
}
//<<splay

//>>splayops
static SN *s_new(int k, SN *p) { SN *n = malloc(sizeof *n); n->key = k; n->l = n->r = NULL; n->p = p; return n; }

static int s_insert(ST *t, int k)            /* ordinary BST insert, then splay the new node */
{
    SN *y = NULL, *x = t->root;
    while (x) { y = x; if (k == x->key) { splay(t, x); return 0; } x = k < x->key ? x->l : x->r; }
    SN *z = s_new(k, y);
    if (!y) t->root = z; else if (k < y->key) y->l = z; else y->r = z;
    splay(t, z);
    return 1;
}
static SN *s_search(ST *t, int k)            /* a search ALSO restructures: found node (or last node on the path) goes to the root */
{
    SN *x = t->root, *last = NULL;
    while (x) { last = x; if (k == x->key) { splay(t, x); return x; } x = k < x->key ? x->l : x->r; }
    if (last) splay(t, last);
    return NULL;
}
static int s_delete(ST *t, int k)
{
    if (!s_search(t, k) || t->root->key != k) return 0;
    SN *z = t->root, *L = z->l, *R = z->r;
    if (L) L->p = NULL;
    if (R) R->p = NULL;
    free(z);
    if (!L) { t->root = R; return 1; }
    t->root = L;                              /* splay the MAXIMUM of the left tree to its root: it then has no right child */
    SN *m = L; while (m->r) m = m->r;
    splay(t, m);
    m->r = R; if (R) R->p = m;
    return 1;
}
//<<splayops

static void s_free(SN *x) { if (x) { s_free(x->l); s_free(x->r); free(x); } }
static void s_spec(const SN *x, char *o)
{
    if (!x) { strcat(o, "."); return; }
    char k[16]; snprintf(k, sizeof k, "%d", x->key); strcat(o, k);
    if (x->l || x->r) { strcat(o, "("); s_spec(x->l, o); strcat(o, ","); s_spec(x->r, o); strcat(o, ")"); }
}
static const char *s_str(const ST *t, char *o) { o[0] = 0; s_spec(t->root, o); return o; }
static int s_ok(const SN *x, const SN *par, long lo, long hi)
{
    if (!x) return 1;
    return x->p == par && x->key > lo && x->key < hi && s_ok(x->l, x, lo, x->key) && s_ok(x->r, x, x->key, hi);
}
static int s_count(const SN *x) { return x ? 1 + s_count(x->l) + s_count(x->r) : 0; }
static int s_height(const SN *x) { if (!x) return 0; int a = s_height(x->l), b = s_height(x->r); return 1 + (a > b ? a : b); }

#ifndef SPLAY_NO_MAIN
int main(void)
{
    char sp[1024];
    ST t = { NULL, "" };

    /* inserting 1..7 in increasing order: every new key becomes the root -> a left-leaning chain */
    for (int i = 1; i <= 7; i++) { s_insert(&t, i); assert(t.root->key == i); }
    assert(!strcmp(s_str(&t, sp), "7(6(5(4(3(2(1,.),.),.),.),.),.)"));
    printf("after inserting 1..7  : %s\n", sp);
    /* now access 1, the deepest node: the long path is HALVED (the key splay-tree property) */
    t.log[0] = 0; s_search(&t, 1);
    printf("after search(1)       : %s\n  steps: %s\n", s_str(&t, sp), t.log);
    assert(t.root->key == 1 && s_height(t.root) == 5);   /* 7 levels -> 5 levels */
    t.log[0] = 0; s_search(&t, 2);
    printf("after search(2)       : %s\n  steps: %s\n", s_str(&t, sp), t.log);
    assert(t.root->key == 2);

    /* a small hand example: root 10, both zig-zig and zig-zag appear */
    ST u = { NULL, "" };
    int ks[] = { 50, 30, 70, 20, 40, 60, 80 };            /* insertion order gives a specific shape; rebuild with spec */
    for (int i = 0; i < 7; i++) s_insert(&u, ks[i]);
    printf("tree after 7 inserts   : %s\n", s_str(&u, sp));
    u.log[0] = 0;
    s_search(&u, 40);
    printf("search(40) -> root=%d, steps: %s\n  %s\n", u.root->key, u.log, s_str(&u, sp));
    assert(u.root->key == 40);
    assert(s_ok(u.root, NULL, -1, 1000) && s_count(u.root) == 7);
    s_search(&u, 55);                                          /* unsuccessful: last node on the path is splayed */
    assert(u.root->key == 50 || u.root->key == 60);
    assert(s_delete(&u, 40) && !s_search(&u, 40) && s_count(u.root) == 6);
    s_free(u.root); s_free(t.root);

    /* random vs model */
    srand(5);
    for (int round = 0; round < 300; round++) {
        ST r = { NULL, "" }; char in[200] = {0}; int cnt = 0;
        for (int op = 0; op < 300; op++) {
            int v = rand() % 150, c = rand() % 3;
            if (c == 0) { int ins = s_insert(&r, v); assert(ins == !in[v]); if (ins) { in[v] = 1; cnt++; } }
            else if (c == 1) { int d = s_delete(&r, v); assert(d == in[v]); if (d) { in[v] = 0; cnt--; } }
            else { SN *f = s_search(&r, v); assert((f != NULL) == in[v]); if (f) assert(r.root == f); }
            r.log[0] = 0;
            assert(s_ok(r.root, NULL, -1, 1000) && s_count(r.root) == cnt);
        }
        s_free(r.root);
    }
    /* locality: repeatedly accessing the same key costs O(1) after the first time */
    ST w = { NULL, "" };
    for (int i = 0; i < 1000; i++) s_insert(&w, (i * 37) % 1000);
    s_search(&w, 123); w.log[0] = 0; s_search(&w, 123);
    assert(w.log[0] == 0);                                      /* already at the root: zero steps */
    s_free(w.root);
    printf("splay: all tests passed\n");
    return 0;
}
#endif
