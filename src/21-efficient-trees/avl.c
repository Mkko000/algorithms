/* ============================================================
 *  avl.c  —  AVL trees: balance factors, the four rotations, insert and delete
 *
 *  Balance factor  BF(node) = height(left) - height(right)   (Thareja's definition; legal values -1, 0, +1).
 *  Heights are stored in the node, so BF costs O(1).
 *  After an insert or delete we walk back up, and at the first node with |BF| = 2 ("critical node"):
 *
 *     BF = +2, left child BF >= 0   :  LL case  ->  one RIGHT rotation
 *     BF = +2, left child BF  = -1  :  LR case  ->  LEFT rotation on the child, then RIGHT on the node
 *     BF = -2, right child BF <= 0  :  RR case  ->  one LEFT rotation
 *     BF = -2, right child BF  = +1 :  RL case  ->  RIGHT rotation on the child, then LEFT on the node
 *
 *  For DELETION Thareja names the cases by the side we deleted from and by BF of the SIBLING B:
 *     R0, R1, R-1  (deleted from the right subtree of the critical node A, B = A's left child)
 *     L0, L1, L-1  (deleted from the left subtree,  B = A's right child)
 *  R0 and R1 are single rotations (R0 is the one that does NOT shrink the height), R-1 is a double rotation.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja 10.4 + Exercises 13, 17, 18; Lipschutz 7.11-7.14; Weiss 4.4;
 *                Bandyopadhyay & Dey 12.6; Pal & Halder 8.x
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 avl.c -o avl
 * ============================================================ */
#include "itree.h"

static char LOG[512];                         /* records every rotation: "LL@72 RR@27 ..."  */
static void logr(const char *kind, int key)       /* bounded: silently stops when the buffer is full */
{
    char t[24]; snprintf(t, sizeof t, "%s%s@%d", LOG[0] ? " " : "", kind, key);
    if (strlen(LOG) + strlen(t) < sizeof LOG) strcat(LOG, t);
}

//>>avlcore
static int H(const INode *n) { return n ? n->h : 0; }
static int BF(const INode *n) { return n ? H(n->l) - H(n->r) : 0; }
static void upd(INode *n) { int a = H(n->l), b = H(n->r); n->h = 1 + (a > b ? a : b); }

/*        y                x
         / \              / \
        x   C    -->     A   y        rotate RIGHT at y  (x = y->l)
       / \                  / \
      A   B                B   C                                                          */
static INode *rot_right(INode *y)
{
    INode *x = y->l;
    y->l = x->r; x->r = y;
    upd(y); upd(x);
    return x;
}
static INode *rot_left(INode *x)              /* mirror image */
{
    INode *y = x->r;
    x->r = y->l; y->l = x;
    upd(x); upd(y);
    return y;
}
/* restore the AVL property at n (its subtrees are already AVL). `del` = 0 insert, 1 = deleted from
   the left subtree, 2 = deleted from the right subtree (only used for the log names). */
static INode *rebalance(INode *n, int del)
{
    upd(n);
    int bf = BF(n);
    if (bf > 1) {                                           /* left heavy */
        int cb = BF(n->l);
        if (del == 2) logr(cb == 0 ? "R0" : cb == 1 ? "R1" : "R-1", n->key);
        else if (!del) logr(cb >= 0 ? "LL" : "LR", n->key);
        if (cb < 0) n->l = rot_left(n->l);
        return rot_right(n);
    }
    if (bf < -1) {                                          /* right heavy */
        int cb = BF(n->r);
        if (del == 1) logr(cb == 0 ? "L0" : cb == -1 ? "L-1" : "L1", n->key);
        else if (!del) logr(cb <= 0 ? "RR" : "RL", n->key);
        if (cb > 0) n->r = rot_right(n->r);
        return rot_left(n);
    }
    return n;
}
//<<avlcore

//>>avlinsert
static INode *avl_insert(INode *t, int v)
{
    if (!t) return inode(v);
    if (v < t->key) t->l = avl_insert(t->l, v);
    else if (v > t->key) t->r = avl_insert(t->r, v);
    else return t;                                          /* duplicates ignored */
    return rebalance(t, 0);
}
//<<avlinsert

//>>avldelete
static INode *avl_delete(INode *t, int v)
{
    if (!t) return NULL;
    int side = 0;
    if (v < t->key) { t->l = avl_delete(t->l, v); side = 1; }
    else if (v > t->key) { t->r = avl_delete(t->r, v); side = 2; }
    else if (t->l && t->r) {                                /* two children: predecessor replaces it */
        INode *p = t->l; while (p->r) p = p->r;
        t->key = p->key;
        t->l = avl_delete(t->l, p->key); side = 1;
    } else {
        INode *c = t->l ? t->l : t->r;
        free(t);
        return c;
    }
    return rebalance(t, side);
}
//<<avldelete

/* checks: BST order, stored heights correct, |BF| <= 1 everywhere. returns height or -1 on violation */
static int avl_check(const INode *t, long lo, long hi)
{
    if (!t) return 0;
    if (t->key <= lo || t->key >= hi) return -1;
    int a = avl_check(t->l, lo, t->key), b = avl_check(t->r, t->key, hi);
    if (a < 0 || b < 0) return -1;
    if (a - b > 1 || b - a > 1) return -1;
    if (t->h != 1 + (a > b ? a : b)) return -1;
    return t->h;
}
static void bfs_show(const INode *t, char *o)               /* "key(bf)" in level order */
{
    const INode *q[256]; int f = 0, r = 0; o[0] = 0;
    if (t) q[r++] = t;
    while (f < r) { const INode *x = q[f++]; char s[24]; snprintf(s, sizeof s, "%s%d(%+d)", f > 1 ? " " : "", x->key, BF(x)); strcat(o, s); if (x->l) q[r++] = x->l; if (x->r) q[r++] = x->r; }
}

#ifndef AVL_NO_MAIN
static int minimum_nodes(int h) { return h == 0 ? 0 : h == 1 ? 1 : 1 + minimum_nodes(h - 1) + minimum_nodes(h - 2); }
int main(void)
{
    char sp[512], bs[512]; IBuf b;

    /* Exercise 18: 16, 27, 9, 11, 36, 54, 81, 63, 72 */
    INode *t = NULL; LOG[0] = 0;
    int seq[] = { 16, 27, 9, 11, 36, 54, 81, 63, 72 };
    for (int i = 0; i < 9; i++) { t = avl_insert(t, seq[i]); assert(avl_check(t, -1000000, 1000000) > 0); }
    assert(!strcmp(ispec(t, sp), "16(9(.,11),63(36(27,54),81(72,.)))"));
    assert(!strcmp(LOG, "RR@27 RL@54 RR@36"));
    printf("Q18 final tree: %s\n  rotations: %s\n", sp, LOG);
    ifree(t);

    /* Exercise 17: balance the two trees by the right rotation(s) */
    INode *a1 = iparse("72(54(45(36,.),63),81)");
    LOG[0] = 0; a1 = rebalance(a1, 0);
    assert(!strcmp(ispec(a1, sp), "54(45(36,.),72(63,81))") && !strcmp(LOG, "LL@72"));
    INode *a2 = iparse("72(54(45,63(.,67)),81)");
    LOG[0] = 0; a2 = rebalance(a2, 0);
    assert(!strcmp(ispec(a2, sp), "63(54(45,.),72(67,81))") && !strcmp(LOG, "LR@72"));
    ifree(a1); ifree(a2);

    /* Exercise 13: the given AVL tree, then 18 81 29 15 19 25 26 1 inserted, then 39 63 15 1 deleted */
    INode *q13 = iparse("45(36(27,39),63(54,72(70,.)))");
    assert(avl_check(q13, -1000000, 1000000) > 0);
    bfs_show(q13, bs);
    printf("Q13 start (key(BF)): %s\n", bs);
    assert(!strcmp(bs, "45(-1) 36(+0) 63(-1) 27(+0) 39(+0) 54(+0) 72(+1) 70(+0)"));
    int ins[] = { 18, 81, 29, 15, 19, 25, 26, 1 };
    LOG[0] = 0;
    for (int i = 0; i < 8; i++) { q13 = avl_insert(q13, ins[i]); assert(avl_check(q13, -1000000, 1000000) > 0); }
    printf("  after inserts : %s\n  rotations     : %s\n", ispec(q13, sp), LOG);
    assert(!strcmp(iin(q13, &b), "1 15 18 19 25 26 27 29 36 39 45 54 63 70 72 81"));
    int del[] = { 39, 63, 15, 1 };
    LOG[0] = 0;
    for (int i = 0; i < 4; i++) { q13 = avl_delete(q13, del[i]); assert(avl_check(q13, -1000000, 1000000) > 0); }
    printf("  after deletes : %s\n  rotations     : %s\n", ispec(q13, sp), LOG);
    assert(!strcmp(iin(q13, &b), "18 19 25 26 27 29 36 45 54 70 72 81"));
    ifree(q13);

    /* worst-case height: the sparsest AVL tree of height h has N(h) = N(h-1) + N(h-2) + 1 nodes
       (Fibonacci-like), so  height <= 1.44 log2(n+2).  Build the sparsest one by sequential inserts and compare */
    for (int n = 1; n <= 1000; n++) {
        INode *s = NULL;
        for (int i = 1; i <= n; i++) s = avl_insert(s, i);          /* sorted input = worst case for a plain BST */
        int hh = s->h; assert(avl_check(s, 0, 1000000) == hh);
        assert(n >= minimum_nodes(hh));
        ifree(s);
    }
    printf("minimum nodes for AVL height 1..8: ");
    for (int h = 1; h <= 8; h++) printf("%d ", minimum_nodes(h));
    printf("\n");

    /* random test with insert + delete against a boolean model; every step re-checks the invariants */
    srand(3);
    for (int round = 0; round < 300; round++) {
        INode *r = NULL; char in[200] = {0}; int cnt = 0;
        for (int op = 0; op < 200; op++) {
            int v = rand() % 150;
            if (rand() % 5 < 3) { r = avl_insert(r, v); if (!in[v]) { in[v] = 1; cnt++; } }
            else { r = avl_delete(r, v); if (in[v]) { in[v] = 0; cnt--; } }
            assert(avl_check(r, -1, 100000) >= 0 && isize(r) == cnt);
        }
        ifree(r);
    }
    printf("avl: all tests passed\n");
    return 0;
}
#endif
