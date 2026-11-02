/* ============================================================
 *  threaded.c  —  threaded binary trees
 *
 *  A binary tree with n nodes has n+1 NULL pointers. A threaded tree reuses them:
 *    right NULL  ->  points to the in-order successor     (one-way threading)
 *    left  NULL  ->  points to the in-order predecessor   (two-way / fully threaded)
 *  A flag per pointer says "child" or "thread", so we can walk the tree WITHOUT a stack or recursion.
 *
 *   Part A  array (memory) representation LEFT/RIGHT with thread flags, exactly like Thareja's tables:
 *           one-way in-order, one-way pre-order, two-way in-order threading of Exercise 16
 *   Part B  pointer-based threaded BST with a HEADER node (Lipschutz 7.6-7.7): insert, successor,
 *           predecessor, in-order and reverse in-order traversal in O(1) extra space
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja 10.3 + Exercise 16; Lipschutz 7.6-7.7 (header nodes, threads); Weiss 4.x;
 *                Bandyopadhyay & Dey 12.4; Pal & Halder 8.x
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 threaded.c -o threaded
 * ============================================================ */
#include "itree.h"

#define N 32
/* ---------------- Part A: the array representation ---------------- */
//>>arrays
typedef struct {
    int l[N], r[N];                 /* LEFT, RIGHT: node number, or -1 for NULL                    */
    int lth[N], rth[N];             /* 1 if that pointer is a thread, 0 if it is a real child       */
    int root, n;                    /* ROOT and number of nodes (node numbers are 1..n)             */
} TA;

static void ta_from(TA *a, const INode *t)       /* node number = key (the figures use 1..12) */
{
    if (!t) return;
    if (t->key > a->n) a->n = t->key;
    a->l[t->key] = t->l ? t->l->key : -1;
    a->r[t->key] = t->r ? t->r->key : -1;
    ta_from(a, t->l); ta_from(a, t->r);
}
static void ta_init(TA *a, const INode *t)
{
    memset(a, 0, sizeof *a);
    for (int i = 0; i < N; i++) a->l[i] = a->r[i] = -1;
    a->root = t->key; ta_from(a, t);
}
static void order_in(const TA *a, int x, int *o, int *k)  { if (x == -1) return; order_in(a, a->l[x], o, k); o[(*k)++] = x; order_in(a, a->r[x], o, k); }
static void order_pre(const TA *a, int x, int *o, int *k) { if (x == -1) return; o[(*k)++] = x; order_pre(a, a->l[x], o, k); order_pre(a, a->r[x], o, k); }

/* one-way in-order: every NULL right pointer becomes a thread to the in-order successor */
static void thread_one_way_in(TA *a)
{
    int o[N], k = 0; order_in(a, a->root, o, &k);
    for (int i = 0; i < k; i++)
        if (a->r[o[i]] == -1 && i + 1 < k) { a->r[o[i]] = o[i + 1]; a->rth[o[i]] = 1; }
}
/* one-way pre-order: NULL right pointers point to the PRE-order successor */
static void thread_one_way_pre(TA *a)
{
    int o[N], k = 0; order_pre(a, a->root, o, &k);
    for (int i = 0; i < k; i++)
        if (a->r[o[i]] == -1 && i + 1 < k) { a->r[o[i]] = o[i + 1]; a->rth[o[i]] = 1; }
}
/* two-way in-order: NULL left pointers point to the predecessor as well */
static void thread_two_way_in(TA *a)
{
    thread_one_way_in(a);
    int o[N], k = 0; TA tmp = *a; for (int i = 0; i < N; i++) if (tmp.rth[i]) tmp.r[i] = -1;
    order_in(&tmp, a->root, o, &k);
    for (int i = 0; i < k; i++)
        if (a->l[o[i]] == -1 && i > 0) { a->l[o[i]] = o[i - 1]; a->lth[o[i]] = 1; }
}
/* walk without a stack or recursion */
static int walk_in_one_way(const TA *a, int *out)
{
    int k = 0, x = a->root;
    while (a->l[x] != -1 && !a->lth[x]) x = a->l[x];              /* leftmost node */
    while (x != -1) {
        out[k++] = x;
        if (a->rth[x]) x = a->r[x];                               /* thread: successor in one step   */
        else {                                                     /* real child: leftmost of right   */
            x = a->r[x];
            while (x != -1 && a->l[x] != -1 && !a->lth[x]) x = a->l[x];
        }
    }
    return k;
}
static int walk_pre_one_way(const TA *a, int *out)
{
    int k = 0, x = a->root;
    while (x != -1) {
        out[k++] = x;
        x = (a->l[x] != -1 && !a->lth[x]) ? a->l[x] : a->r[x];    /* left child if any, else right/thread */
    }
    return k;
}
//<<arrays

/* ---------------- Part B: pointer based, header node ---------------- */
//>>pointer
/* Lipschutz's layout: a HEADER node H. H.left -> root, H.right -> H (itself). The root's tree is
   threaded so that the leftmost node's left thread and the rightmost node's right thread both point to H.
   An empty tree is H with H.left -> H (a thread). */
typedef struct TN {
    int key;
    struct TN *l, *r;
    int lth, rth;                   /* 1 = thread, 0 = child */
} TN;

static TN *th_new(void)
{
    TN *h = malloc(sizeof *h);
    h->key = 0; h->l = h; h->r = h; h->lth = 1; h->rth = 0;
    return h;
}
static TN *th_succ(const TN *p)                 /* in-order successor */
{
    TN *s = p->r;
    if (!p->rth) while (!s->lth) s = s->l;     /* real right child: go to its leftmost */
    return s;
}
static TN *th_pred(const TN *p)                 /* in-order predecessor (mirror image) */
{
    TN *s = p->l;
    if (!p->lth) while (!s->rth) s = s->r;
    return s;
}
/* BST insert (duplicates ignored). New leaf inherits the thread it is replacing. */
static int th_insert(TN *h, int key)
{
    TN *n = malloc(sizeof *n); n->key = key; n->lth = n->rth = 1;
    if (h->lth) {                               /* empty tree */
        n->l = h; n->r = h; h->l = n; h->lth = 0;
        return 1;
    }
    TN *p = h->l;
    for (;;) {
        if (key == p->key) { free(n); return 0; }
        if (key < p->key) {
            if (p->lth) { n->l = p->l; n->r = p; p->l = n; p->lth = 0; return 1; }
            p = p->l;
        } else {
            if (p->rth) { n->r = p->r; n->l = p; p->r = n; p->rth = 0; return 1; }
            p = p->r;
        }
    }
}
static void th_inorder(const TN *h, int *out, int *k)
{
    *k = 0;
    for (const TN *p = th_succ(h); p != h; p = th_succ(p)) out[(*k)++] = p->key;   /* H's "successor" is the first node */
}
static void th_reverse(const TN *h, int *out, int *k)
{
    *k = 0;
    for (const TN *p = th_pred(h); p != h; p = th_pred(p)) out[(*k)++] = p->key;
}
static void th_free(TN *h)
{
    TN *p = th_succ(h);
    while (p != h) { TN *nx = th_succ(p); free(p); p = nx; }
    free(h);
}
//<<pointer

#ifndef TH_NO_MAIN
static void show(const char *title, const TA *a)
{
    printf("%s\n  node LEFT RIGHT\n", title);
    for (int i = 1; i <= a->n; i++) {
        char L[8], R[8];
        snprintf(L, sizeof L, "%d%s", a->l[i], a->lth[i] ? "T" : "");
        snprintf(R, sizeof R, "%d%s", a->r[i], a->rth[i] ? "T" : "");
        printf("  %4d %4s %5s\n", i, L, R);
    }
}
int main(void)
{
    /* Exercise 16: the 12-node tree, node numbers are the labels */
    INode *t = iparse("1(2(4(8,9),5),3(6(10,11),7(.,12)))");
    int out[256], k;
    TA a;

    ta_init(&a, t); thread_one_way_in(&a);
    show("one-way in-order threading (T = thread)", &a);
    k = walk_in_one_way(&a, out);
    int in_exp[] = { 8, 4, 9, 2, 5, 1, 10, 6, 11, 3, 7, 12 };
    assert(k == 12 && !memcmp(out, in_exp, sizeof in_exp));
    /* threads: 8->4, 9->2, 5->1, 10->6, 11->3, 12 stays NULL */
    assert(a.r[8] == 4 && a.rth[8] && a.r[9] == 2 && a.r[5] == 1 && a.r[10] == 6 && a.r[11] == 3 && a.r[12] == -1 && !a.rth[12]);

    ta_init(&a, t); thread_one_way_pre(&a);
    show("one-way pre-order threading", &a);
    k = walk_pre_one_way(&a, out);
    int pre_exp[] = { 1, 2, 4, 8, 9, 5, 3, 6, 10, 11, 7, 12 };
    assert(k == 12 && !memcmp(out, pre_exp, sizeof pre_exp));
    assert(a.r[8] == 9 && a.r[9] == 5 && a.r[5] == 3 && a.r[10] == 11 && a.r[11] == 7);

    ta_init(&a, t); thread_two_way_in(&a);
    show("two-way in-order threading", &a);
    /* left threads: 8 has none (first), 9->4, 5->2, 10->1, 11->6, 7->3, 12->7 ... */
    assert(a.l[8] == -1 && a.l[9] == 4 && a.lth[9] && a.l[5] == 2 && a.l[10] == 1 && a.l[11] == 6 && a.l[7] == 3 && a.l[12] == 7);
    /* count: n+1 = 13 NULL pointers in the plain tree; two-way threading removes all but 2 */
    int nulls = 0; for (int i = 1; i <= a.n; i++) { if (a.l[i] == -1) nulls++; if (a.r[i] == -1) nulls++; }
    assert(nulls == 2);

    /* Part B */
    TN *h = th_new();
    int keys[] = { 50, 30, 70, 20, 40, 60, 80, 35, 45, 65, 30 };
    for (int i = 0; i < 11; i++) th_insert(h, keys[i]);
    th_inorder(h, out, &k);
    int sorted_exp[] = { 20, 30, 35, 40, 45, 50, 60, 65, 70, 80 };
    assert(k == 10 && !memcmp(out, sorted_exp, sizeof sorted_exp));
    th_reverse(h, out, &k);
    assert(k == 10 && out[0] == 80 && out[9] == 20);
    printf("threaded BST in-order, no stack:");
    th_inorder(h, out, &k); for (int i = 0; i < k; i++) printf(" %d", out[i]); printf("\n");
    th_free(h);

    srand(11);
    for (int round = 0; round < 200; round++) {
        TN *x = th_new(); int cnt = 0;
        for (int i = 0; i < 60; i++) { int v = rand() % 100; if (th_insert(x, v)) cnt++; }
        th_inorder(x, out, &k);
        assert(k == cnt);
        for (int i = 1; i < k; i++) assert(out[i - 1] < out[i]);
        th_free(x);
    }
    ifree(t);
    printf("threaded: all tests passed\n");
    return 0;
}
#endif
