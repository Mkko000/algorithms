/* ============================================================
 *  bst_ops.c  —  every binary-search-tree operation of Thareja ch.10.2
 *
 *   search, insert (duplicates go RIGHT, the book's convention), delete (3 cases, predecessor or successor),
 *   height, number of nodes / internal / external nodes, mirror image, smallest, largest,
 *   delete whole tree, and "how many different BSTs hold n keys" (Catalan again).
 *  The Sedgewick-style BST lessons (#/bst, #/bst-ops) cover the same ideas with symbol-table
 *  interfaces; here the focus is the textbook pointer code and its exercises.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja 10.1-10.2 + Exercises 10-12, 19 + Programming Exercises 1-2; Lipschutz 7.8-7.10;
 *                Weiss 4.3; Bandyopadhyay & Dey 12.5; Pal & Halder 8.x
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 bst_ops.c -o bst_ops
 * ============================================================ */
#include "itree.h"

//>>basic
static INode *bst_search(INode *t, int v)
{
    while (t && t->key != v) t = v < t->key ? t->l : t->r;
    return t;
}
/* equal keys go to the right subtree (Thareja: "else insert in the right sub-tree") */
static INode *bst_insert(INode *t, int v)
{
    if (!t) return inode(v);
    if (v < t->key) t->l = bst_insert(t->l, v);
    else            t->r = bst_insert(t->r, v);
    return t;
}
static INode *bst_min(INode *t) { while (t && t->l) t = t->l; return t; }   /* leftmost node  */
static INode *bst_max(INode *t) { while (t && t->r) t = t->r; return t; }   /* rightmost node */
//<<basic

//>>delete
/* Three cases (Thareja 10.2.3):
     1. leaf            -> just remove it
     2. one child       -> the child takes its place
     3. two children    -> copy the in-order PREDECESSOR (largest key of the left subtree) into the node,
                           then delete that predecessor node (which has at most one child: case 1 or 2).
   use_succ = 1 switches to the in-order SUCCESSOR (smallest key of the right subtree). */
static INode *bst_delete(INode *t, int v, int use_succ)
{
    if (!t) return NULL;                                    /* not found: nothing to do */
    if (v < t->key) t->l = bst_delete(t->l, v, use_succ);
    else if (v > t->key) t->r = bst_delete(t->r, v, use_succ);
    else if (t->l && t->r) {                                /* case 3 */
        if (use_succ) { INode *s = bst_min(t->r); t->key = s->key; t->r = bst_delete(t->r, s->key, use_succ); }
        else          { INode *p = bst_max(t->l); t->key = p->key; t->l = bst_delete(t->l, p->key, use_succ); }
    } else {                                                /* case 1 and 2 together */
        INode *child = t->l ? t->l : t->r;
        free(t);
        return child;
    }
    return t;
}
//<<delete

//>>measure
static int bst_height(const INode *t)  { return iheight(t); }                    /* empty tree: 0, one node: 1 */
static int bst_nodes(const INode *t)   { return isize(t); }
static int bst_leaves(const INode *t)  { return !t ? 0 : (!t->l && !t->r) ? 1 : bst_leaves(t->l) + bst_leaves(t->r); }
static int bst_internal(const INode *t){ return bst_nodes(t) - bst_leaves(t); }
/* external nodes = the NULL pointers = n + 1 for n nodes */
static int bst_external(const INode *t){ return !t ? 1 : bst_external(t->l) + bst_external(t->r); }
static INode *bst_mirror(INode *t)                       /* swap the children at every node */
{
    if (t) { INode *x = t->l; t->l = bst_mirror(t->r); t->r = bst_mirror(x); }
    return t;
}
//<<measure

static INode *build(const int *a, int n) { INode *t = NULL; for (int i = 0; i < n; i++) t = bst_insert(t, a[i]); return t; }

//>>allbst
/* all distinct BSTs on the keys sorted in k[lo..hi]:  choose each key as root, combine every left shape
   with every right shape.  Writes specs into out[]; returns how many. */
static int all_bsts(const int *k, int lo, int hi, char out[][96])
{
    if (lo > hi) { strcpy(out[0], "."); return 1; }
    int cnt = 0;
    for (int r = lo; r <= hi; r++) {
        char L[16][96], R[16][96];
        int a = all_bsts(k, lo, r - 1, L), b = all_bsts(k, r + 1, hi, R);
        for (int i = 0; i < a; i++)
            for (int j = 0; j < b; j++) {
                if (L[i][0] == '.' && R[j][0] == '.') snprintf(out[cnt], 96, "%d", k[r]);
                else { snprintf(out[cnt], 96, "%d(", k[r]); strcat(out[cnt], L[i]); strcat(out[cnt], ","); strcat(out[cnt], R[j]); strcat(out[cnt], ")"); }
                cnt++;
            }
    }
    return cnt;
}
//<<allbst

#ifndef BST_NO_MAIN
#include <stdlib.h>
static int sorted(const INode *t, long *prev, int *started)
{
    if (!t) return 1;
    if (!sorted(t->l, prev, started)) return 0;
    if (*started && t->key < *prev) return 0;
    *prev = t->key; *started = 1;
    return sorted(t->r, prev, started);
}
static int is_bst(const INode *t) { long p = 0; int s = 0; return sorted(t, &p, &s); }

int main(void)
{
    char sp[256]; IBuf b;
    /* Exercise 11: 98, 2, 48, 12, 56, 32, 4, 67, 23, 87, 23, 55, 46 */
    int in11[] = { 98, 2, 48, 12, 56, 32, 4, 67, 23, 87, 23, 55, 46 };
    INode *t = build(in11, 13);
    assert(!strcmp(ispec(t, sp), "98(2(.,48(12(4,32(23(.,23),46)),56(55,67(.,87)))),.)"));
    assert(!strcmp(iin(t, &b), "2 4 12 23 23 32 46 48 55 56 67 87 98"));
    printf("Q11 tree: %s\n", sp);
    /* (a) insert 21, 39, 45, 54, 63 */
    int more[] = { 21, 39, 45, 54, 63 };
    for (int i = 0; i < 5; i++) t = bst_insert(t, more[i]);
    assert(!strcmp(iin(t, &b), "2 4 12 21 23 23 32 39 45 46 48 54 55 56 63 67 87 98"));
    printf("after inserts: %s\n", ispec(t, sp));
    /* (b) delete 23, 56, 2, 45 */
    int del[] = { 23, 56, 2, 45 };
    for (int i = 0; i < 4; i++) { t = bst_delete(t, del[i], 0); assert(is_bst(t)); }
    assert(!strcmp(iin(t, &b), "4 12 21 23 32 39 46 48 54 55 63 67 87 98"));
    printf("after deletes: %s\n", ispec(t, sp));
    ifree(t);

    /* Exercise 12 */
    INode *q12 = iparse("45(39(12(10,34(32,.)),.),56(54,78(67,89(81,.))))");
    assert(is_bst(q12));
    assert(!strcmp(iin(q12, &b), "10 12 32 34 39 45 54 56 67 78 81 89"));
    assert(!strcmp(ipre(q12, &b), "45 39 12 10 34 32 56 54 78 67 89 81"));
    assert(!strcmp(ipost(q12, &b), "10 32 34 12 39 54 67 81 89 78 56 45"));
    q12 = bst_delete(q12, 45, 0);                                    /* the root: predecessor 39 replaces it */
    assert(q12->key == 39);
    printf("Q12 after deleting the root (predecessor): %s\n", ispec(q12, sp));
    int ins[] = { 11, 22, 33, 44, 55, 66, 77 };
    for (int i = 0; i < 7; i++) q12 = bst_insert(q12, ins[i]);
    assert(is_bst(q12) && bst_nodes(q12) == 18);                       /* 12 - 1 + 7 */
    printf("Q12 after the 7 inserts: %s\n", ispec(q12, sp));
    ifree(q12);

    /* the same root deletion with the successor */
    INode *q12b = iparse("45(39(12(10,34(32,.)),.),56(54,78(67,89(81,.))))");
    q12b = bst_delete(q12b, 45, 1);
    assert(q12b->key == 54);
    ifree(q12b);

    /* measures and mirror */
    INode *m = iparse("5(3(2,4),8(.,9))");
    assert(bst_height(m) == 3 && bst_nodes(m) == 6 && bst_leaves(m) == 3 && bst_internal(m) == 3 && bst_external(m) == 7);
    assert(bst_min(m)->key == 2 && bst_max(m)->key == 9);
    bst_mirror(m);
    assert(!strcmp(ispec(m, sp), "5(8(9,.),3(4,2))"));
    ifree(m);

    /* Exercise 19: all BSTs of 7, 9, 11 */
    int k3[] = { 7, 9, 11 }; char all[16][96];
    int c = all_bsts(k3, 0, 2, all);
    assert(c == 5);
    for (int i = 0; i < c; i++) printf("  %s\n", all[i]);
    /* 4 keys: Catalan(4) = 14 */
    int k4[] = { 1, 2, 3, 4 }; char all4[16][96];
    assert(all_bsts(k4, 0, 3, all4) == 14);

    /* random test against a plain sorted-array model */
    srand(5);
    for (int round = 0; round < 300; round++) {
        INode *r = NULL; int model[64], n = 0;
        for (int op = 0; op < 120; op++) {
            int v = rand() % 40;
            if (rand() % 3) { r = bst_insert(r, v); model[n++] = v; }
            else {
                int found = 0, idx = -1;
                for (int i = 0; i < n; i++) if (model[i] == v) { found = 1; idx = i; break; }
                int before = bst_nodes(r);
                r = bst_delete(r, v, rand() & 1);
                if (found) { model[idx] = model[--n]; assert(bst_nodes(r) == before - 1); }
                else assert(bst_nodes(r) == before);
            }
            assert(is_bst(r) && bst_nodes(r) == n);
            if (n >= 64) break;
        }
        ifree(r);
    }
    printf("bst_ops: all tests passed\n");
    return 0;
}
#endif
