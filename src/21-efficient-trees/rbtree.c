/* ============================================================
 *  rbtree.c  —  red-black trees: insertion (3 cases), deletion (4 cases), invariant checker
 *
 *  A red-black tree is a BST where every node is RED or BLACK and
 *    1. the root is black
 *    2. every NIL leaf (the "external" nodes) is black
 *    3. a red node never has a red child                       (no two reds in a row)
 *    4. every path from a node to its NIL leaves has the same number of black nodes ("black height")
 *  Consequence: the longest path is at most twice the shortest, so height <= 2 log2(n+1).
 *
 *  Insert: colour the new node RED, then repair a red-red violation by looking at the UNCLE:
 *     case 1  uncle RED          -> recolour parent, uncle, grandparent; continue at grandparent
 *     case 2  uncle BLACK, "triangle" (node is an inner child)  -> rotate parent so it becomes case 3
 *     case 3  uncle BLACK, "line"                              -> recolour + rotate grandparent
 *  Delete: when a BLACK node leaves, its place carries an "extra black" that is pushed up by 4 cases
 *     (sibling red / sibling black with two black children / far child red / near child red).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja 10.5; Weiss 12.2 (top-down variant discussed in the lesson);
 *                Bandyopadhyay & Dey (red-black trees); the case analysis follows the standard
 *                presentation (CLRS style) with a NIL sentinel.
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 rbtree.c -o rbtree
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#pragma GCC diagnostic ignored "-Wunused-function"

enum { RED = 0, BLACK = 1 };
typedef struct RB { int key, color; struct RB *l, *r, *p; } RB;

//>>rbcore
static RB NILNODE = { 0, BLACK, &NILNODE, &NILNODE, &NILNODE };   /* ONE shared black sentinel */
#define NIL (&NILNODE)

typedef struct { RB *root; char log[256]; } RBT;

static void rlog(RBT *t, const char *s)
{
    if (strlen(t->log) + strlen(s) + 2 < sizeof t->log) { if (t->log[0]) strcat(t->log, " "); strcat(t->log, s); }
}
static void rot_left(RBT *t, RB *x)
{
    RB *y = x->r;
    x->r = y->l; if (y->l != NIL) y->l->p = x;
    y->p = x->p;
    if (x->p == NIL) t->root = y; else if (x == x->p->l) x->p->l = y; else x->p->r = y;
    y->l = x; x->p = y;
}
static void rot_right(RBT *t, RB *y)
{
    RB *x = y->l;
    y->l = x->r; if (x->r != NIL) x->r->p = y;
    x->p = y->p;
    if (y->p == NIL) t->root = x; else if (y == y->p->l) y->p->l = x; else y->p->r = x;
    x->r = y; y->p = x;
}
//<<rbcore

//>>rbinsert
static void insert_fix(RBT *t, RB *z)
{
    while (z->p->color == RED) {                       /* parent red => grandparent exists and is black */
        RB *g = z->p->p;
        if (z->p == g->l) {
            RB *u = g->r;                              /* the uncle */
            if (u->color == RED) {                     /* case 1: recolour and move up */
                z->p->color = BLACK; u->color = BLACK; g->color = RED; rlog(t, "I1");
                z = g;
            } else {
                if (z == z->p->r) { z = z->p; rot_left(t, z); rlog(t, "I2"); }   /* case 2: triangle -> line */
                z->p->color = BLACK; g->color = RED; rot_right(t, g); rlog(t, "I3"); /* case 3 */
            }
        } else {                                       /* mirror image */
            RB *u = g->l;
            if (u->color == RED) {
                z->p->color = BLACK; u->color = BLACK; g->color = RED; rlog(t, "I1");
                z = g;
            } else {
                if (z == z->p->l) { z = z->p; rot_right(t, z); rlog(t, "I2"); }
                z->p->color = BLACK; g->color = RED; rot_left(t, g); rlog(t, "I3");
            }
        }
    }
    t->root->color = BLACK;                            /* rule 1 */
}
static int rb_insert(RBT *t, int key)
{
    RB *y = NIL, *x = t->root;
    while (x != NIL) { y = x; if (key == x->key) return 0; x = key < x->key ? x->l : x->r; }
    RB *z = malloc(sizeof *z);
    z->key = key; z->color = RED; z->l = z->r = NIL; z->p = y;
    if (y == NIL) t->root = z; else if (key < y->key) y->l = z; else y->r = z;
    insert_fix(t, z);
    return 1;
}
//<<rbinsert

//>>rbdelete
static void transplant(RBT *t, RB *u, RB *v)
{
    if (u->p == NIL) t->root = v; else if (u == u->p->l) u->p->l = v; else u->p->r = v;
    v->p = u->p;                                       /* legal even when v is NIL: we need NIL->p below */
}
static void delete_fix(RBT *t, RB *x)                  /* x carries an "extra black" */
{
    while (x != t->root && x->color == BLACK) {
        if (x == x->p->l) {
            RB *w = x->p->r;                           /* sibling */
            if (w->color == RED) {                     /* case 1: red sibling -> make it black */
                w->color = BLACK; x->p->color = RED; rot_left(t, x->p); w = x->p->r; rlog(t, "D1");
            }
            if (w->l->color == BLACK && w->r->color == BLACK) {   /* case 2: push the extra black up */
                w->color = RED; x = x->p; rlog(t, "D2");
            } else {
                if (w->r->color == BLACK) {            /* case 3: near child red -> rotate to case 4 */
                    w->l->color = BLACK; w->color = RED; rot_right(t, w); w = x->p->r; rlog(t, "D3");
                }
                w->color = x->p->color; x->p->color = BLACK; w->r->color = BLACK;   /* case 4: far child red */
                rot_left(t, x->p); rlog(t, "D4");
                x = t->root;
            }
        } else {                                       /* mirror image */
            RB *w = x->p->l;
            if (w->color == RED) { w->color = BLACK; x->p->color = RED; rot_right(t, x->p); w = x->p->l; rlog(t, "D1"); }
            if (w->r->color == BLACK && w->l->color == BLACK) { w->color = RED; x = x->p; rlog(t, "D2"); }
            else {
                if (w->l->color == BLACK) { w->r->color = BLACK; w->color = RED; rot_left(t, w); w = x->p->l; rlog(t, "D3"); }
                w->color = x->p->color; x->p->color = BLACK; w->l->color = BLACK;
                rot_right(t, x->p); rlog(t, "D4");
                x = t->root;
            }
        }
    }
    x->color = BLACK;
}
static RB *rb_find(RB *x, int key) { while (x != NIL && x->key != key) x = key < x->key ? x->l : x->r; return x; }
static int rb_delete(RBT *t, int key)
{
    RB *z = rb_find(t->root, key);
    if (z == NIL) return 0;
    RB *y = z, *x; int ycol = y->color;
    if (z->l == NIL) { x = z->r; transplant(t, z, z->r); }
    else if (z->r == NIL) { x = z->l; transplant(t, z, z->l); }
    else {                                             /* two children: use the in-order successor */
        y = z->r; while (y->l != NIL) y = y->l;
        ycol = y->color; x = y->r;
        if (y->p == z) x->p = y;
        else { transplant(t, y, y->r); y->r = z->r; y->r->p = y; }
        transplant(t, z, y);
        y->l = z->l; y->l->p = y; y->color = z->color;
    }
    free(z);
    if (ycol == BLACK) delete_fix(t, x);               /* only a removed BLACK node breaks rule 4 */
    return 1;
}
//<<rbdelete

static void rb_free(RB *x) { if (x != NIL) { rb_free(x->l); rb_free(x->r); free(x); } }

/* checker: returns black height, or -1 on a violated rule */
static int rb_check(const RB *x, long lo, long hi)
{
    if (x == NIL) return 1;
    if (x->key <= lo || x->key >= hi) return -1;
    if (x->color == RED && (x->l->color == RED || x->r->color == RED)) return -1;
    if (x->l != NIL && x->l->p != x) return -1;
    if (x->r != NIL && x->r->p != x) return -1;
    int a = rb_check(x->l, lo, x->key), b = rb_check(x->r, x->key, hi);
    if (a < 0 || b < 0 || a != b) return -1;
    return a + (x->color == BLACK);
}
static int rb_valid(const RBT *t) { return t->root == NIL || (t->root->color == BLACK && t->root->p == NIL && rb_check(t->root, -1000000000L, 1000000000L) > 0); }
static int rb_height(const RB *x) { if (x == NIL) return 0; int a = rb_height(x->l), b = rb_height(x->r); return 1 + (a > b ? a : b); }
static void rb_spec(const RB *x, char *o)              /* "50b(30r,70r)" */
{
    if (x == NIL) { strcat(o, "."); return; }
    char k[16]; snprintf(k, sizeof k, "%d%c", x->key, x->color == RED ? 'r' : 'b'); strcat(o, k);
    if (x->l != NIL || x->r != NIL) { strcat(o, "("); rb_spec(x->l, o); strcat(o, ","); rb_spec(x->r, o); strcat(o, ")"); }
}
static const char *rb_str(const RBT *t, char *o) { o[0] = 0; rb_spec(t->root, o); return o; }

#ifndef RB_NO_MAIN
int main(void)
{
    char sp[2048];
    RBT t = { NIL, "" };

    /* insert 1..7 in order: a plain BST would be a chain; a red-black tree stays bushy */
    for (int i = 1; i <= 7; i++) { rb_insert(&t, i); assert(rb_valid(&t)); }
    assert(!strcmp(rb_str(&t, sp), "2b(1b,4r(3b,6b(5r,7r)))"));
    printf("insert 1..7   : %s\n  cases       : %s\n", sp, t.log);

    /* the classic textbook sequence */
    RBT u = { NIL, "" };
    int seq[] = { 10, 18, 7, 15, 16, 30, 25, 40, 60, 2, 1, 70 };
    for (int i = 0; i < 12; i++) { rb_insert(&u, seq[i]); assert(rb_valid(&u)); }
    printf("insert 10 18 7 15 16 30 25 40 60 2 1 70:\n  %s\n  cases: %s\n", rb_str(&u, sp), u.log);
    u.log[0] = 0;
    int del[] = { 18, 7, 1, 60 };
    for (int i = 0; i < 4; i++) { assert(rb_delete(&u, del[i])); assert(rb_valid(&u)); }
    printf("delete 18 7 1 60:\n  %s\n  cases: %s\n", rb_str(&u, sp), u.log);
    rb_free(u.root); rb_free(t.root);

    /* random: stress with a model, check all 4 rules after every operation, and height <= 2 log2(n+1) */
    srand(7);
    int worst_ratio_ok = 1;
    for (int round = 0; round < 200; round++) {
        RBT r = { NIL, "" }; char in[300] = {0}; int cnt = 0;
        for (int op = 0; op < 300; op++) {
            int v = rand() % 250;
            if (rand() % 5 < 3) { if (rb_insert(&r, v)) { assert(!in[v]); in[v] = 1; cnt++; } else assert(in[v]); }
            else { if (rb_delete(&r, v)) { assert(in[v]); in[v] = 0; cnt--; } else assert(!in[v]); }
            r.log[0] = 0;
            assert(rb_valid(&r));
            int h = rb_height(r.root), lim = 0;
            while ((1 << lim) < cnt + 1) lim++;          /* ceil(log2(n+1)) */
            if (h > 2 * lim) worst_ratio_ok = 0;
        }
        rb_free(r.root);
    }
    assert(worst_ratio_ok);
    /* sorted insertion of 1..1000: height stays tiny */
    RBT s = { NIL, "" };
    for (int i = 1; i <= 1000; i++) rb_insert(&s, i);
    printf("1000 sorted inserts: height = %d (bound 2*log2(1001) = 20)\n", rb_height(s.root));
    assert(rb_height(s.root) <= 20 && rb_valid(&s));
    rb_free(s.root);
    printf("rbtree: all tests passed\n");
    return 0;
}
#endif
