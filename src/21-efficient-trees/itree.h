/* itree.h — integer binary tree helpers shared by bst_ops.c, avl.c, threaded.c and the exercises.
 * Author: Mohammed El Sisi
 *   struct INode {key, h, l, r}     h = height (1 for a leaf, 0 for NULL) - only AVL needs it
 *   iparse("45(39(12,.),56)")       build a tree from a spec;  '.' = empty child
 *   ipre/iin/ipost/ilevel           traversals written into a string
 *   iheight, isize, ifree
 */
#ifndef ITREE_H
#define ITREE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma GCC diagnostic ignored "-Wunused-function"

typedef struct INode {
    int key, h;
    struct INode *l, *r;
} INode;

static INode *inode(int key)
{
    INode *n = malloc(sizeof *n);
    n->key = key; n->h = 1; n->l = n->r = NULL;
    return n;
}
static void ifree(INode *t) { if (t) { ifree(t->l); ifree(t->r); free(t); } }
static int iheight(const INode *t) { if (!t) return 0; int a = iheight(t->l), b = iheight(t->r); return 1 + (a > b ? a : b); }   /* each side computed ONCE: the ?: version is exponential */
static int isize(const INode *t) { return t ? 1 + isize(t->l) + isize(t->r) : 0; }
static void ifix_h(INode *t)                         /* recompute every h bottom-up */
{
    if (!t) return;
    ifix_h(t->l); ifix_h(t->r);
    int a = t->l ? t->l->h : 0, b = t->r ? t->r->h : 0;
    t->h = 1 + (a > b ? a : b);
}

static INode *iparse_rec(const char **p)
{
    if (**p == '.') { (*p)++; return NULL; }
    int sign = 1, v = 0;
    if (**p == '-') { sign = -1; (*p)++; }
    while (**p >= '0' && **p <= '9') v = v * 10 + (*(*p)++ - '0');
    INode *n = inode(sign * v);
    if (**p == '(') {
        (*p)++; n->l = iparse_rec(p);
        assert(**p == ','); (*p)++;
        n->r = iparse_rec(p);
        assert(**p == ')'); (*p)++;
    }
    return n;
}
static INode *iparse(const char *s) { const char *p = s; INode *t = iparse_rec(&p); assert(!*p); ifix_h(t); return t; }

typedef struct { char s[1024]; } IBuf;
static void iput(IBuf *b, int k) { char t[16]; snprintf(t, sizeof t, "%s%d", b->s[0] ? " " : "", k); strcat(b->s, t); }
static void ipre_(const INode *t, IBuf *b)  { if (t) { iput(b, t->key); ipre_(t->l, b); ipre_(t->r, b); } }
static void iin_(const INode *t, IBuf *b)   { if (t) { iin_(t->l, b); iput(b, t->key); iin_(t->r, b); } }
static void ipost_(const INode *t, IBuf *b) { if (t) { ipost_(t->l, b); ipost_(t->r, b); iput(b, t->key); } }
static const char *ipre(const INode *t, IBuf *b)  { b->s[0] = 0; ipre_(t, b);  return b->s; }
static const char *iin(const INode *t, IBuf *b)   { b->s[0] = 0; iin_(t, b);   return b->s; }
static const char *ipost(const INode *t, IBuf *b) { b->s[0] = 0; ipost_(t, b); return b->s; }
static const char *ilevel(const INode *t, IBuf *b)
{
    const INode *q[512]; int f = 0, r = 0; b->s[0] = 0;
    if (t) q[r++] = t;
    while (f < r) { const INode *x = q[f++]; iput(b, x->key); if (x->l) q[r++] = x->l; if (x->r) q[r++] = x->r; }
    return b->s;
}
/* the tree as a spec string again - handy for asserting a whole shape in one line */
static void ispec_(const INode *t, char *o)
{
    if (!t) { strcat(o, "."); return; }
    char k[16]; snprintf(k, sizeof k, "%d", t->key); strcat(o, k);
    if (t->l || t->r) { strcat(o, "("); ispec_(t->l, o); strcat(o, ","); ispec_(t->r, o); strcat(o, ")"); }
}
static const char *ispec(const INode *t, char *o) { o[0] = 0; ispec_(t, o); return o; }
#endif
