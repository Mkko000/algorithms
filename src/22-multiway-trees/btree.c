/* ============================================================
 *  btree.c  —  B tree of any order m (Thareja 11.2, Examples 11.1-11.4, Fig. 11.4; Q.8)
 *
 *  A B tree of order m is an M-way search tree (M = m) with four more rules:
 *    1. every node has at most m children (so at most m-1 keys);
 *    2. every node except the root and the leaves has at least ceil(m/2) children (so >= ceil(m/2)-1 keys);
 *    3. the root has at least 2 children unless it is a leaf;
 *    4. all leaves are on the same level.
 *  - insert  : always into a leaf; an overflowing node (m keys) is split at its median, the median goes UP
 *  - delete  : always from a leaf (an inner key is first replaced by its in-order successor); an underflowing
 *              node first borrows through the parent from the LEFT sibling, then from the RIGHT sibling,
 *              otherwise it merges with a sibling and the separator comes DOWN  (exactly Thareja's order)
 *  Every operation counts the nodes it touches — each one is a disk read in a real database.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Sec. 11.2
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 btree.c -o btree -lm
 * ============================================================ */
#include "mnode.h"
#include <math.h>

//>>btree
typedef struct { MNode *root; int m; } BTree;      /* m = order: max m children, max m-1 keys */

static int bt_min_keys(int m) { return (m + 1) / 2 - 1; }          /* ceil(m/2) - 1 */
static long bt_reads = 0;                                           /* nodes touched by the last operations */

static int bt_search(const BTree *t, int k)
{
    for (const MNode *x = t->root; x; ) {
        bt_reads++;
        int i = 0;
        while (i < x->n && x->key[i] < k) i++;
        if (i < x->n && x->key[i] == k) return 1;
        x = x->ch[i];                                   /* NULL at a leaf: not found */
    }
    return 0;
}

/* ---- insert ---- */
/* returns 0 = duplicate, 1 = inserted, 2 = inserted and x was split: *up (median) and *right (new sibling) */
static int bt_ins(MNode *x, int m, int k, int *up, MNode **right)
{
    bt_reads++;
    int i = 0;
    while (i < x->n && x->key[i] < k) i++;
    if (i < x->n && x->key[i] == k) return 0;
    MNode *newchild = NULL;
    if (x->ch[0]) {                                     /* inner node: insert below, maybe receive a split */
        int mk; MNode *mr = NULL;
        int r = bt_ins(x->ch[i], m, k, &mk, &mr);
        if (r != 2) return r;
        k = mk; newchild = mr;                          /* the median of the child goes into THIS node */
    }
    for (int j = x->n; j > i; j--) { x->key[j] = x->key[j - 1]; x->ch[j + 1] = x->ch[j]; }
    x->key[i] = k; x->ch[i + 1] = newchild; x->n++;
    if (x->n < m) return 1;                             /* at most m-1 keys: fits */

    int mid = (x->n - 1) / 2;                           /* overflow: m keys -> split at the median */
    MNode *r = mn_new();
    r->n = x->n - mid - 1;
    for (int j = 0; j < r->n; j++) r->key[j] = x->key[mid + 1 + j];
    for (int j = 0; j <= r->n; j++) { r->ch[j] = x->ch[mid + 1 + j]; x->ch[mid + 1 + j] = NULL; }
    *up = x->key[mid]; *right = r;
    x->n = mid;
    return 2;
}
static int bt_insert(BTree *t, int k)
{
    if (!t->root) { t->root = mn_new(); t->root->key[0] = k; t->root->n = 1; return 1; }
    int up; MNode *r = NULL;
    int res = bt_ins(t->root, t->m, k, &up, &r);
    if (res == 2) {                                     /* the root split: the tree grows by one level */
        MNode *nr = mn_new();
        nr->n = 1; nr->key[0] = up; nr->ch[0] = t->root; nr->ch[1] = r;
        t->root = nr;
    }
    return res != 0;
}

/* ---- delete ---- */
/* make child x->ch[i] have at least min keys again */
static int bt_merge_right_first = 0;                    /* convention switch: P&H combine with the RIGHT neighbour when both could be used */
static int bt_use_pred = 0;                             /* convention switch: P&H replace an inner key by its in-order PREDECESSOR */
static char bt_fixlog[128];                             /* what the last delete had to repair, e.g. "borrow-right" or "merge-left merge-left" */
static void bt_note(const char *what) { if (bt_fixlog[0]) strcat(bt_fixlog, " "); strcat(bt_fixlog, what); }
static void bt_fix(MNode *x, int i, int m)
{
    int mn = bt_min_keys(m);
    MNode *c = x->ch[i];
    if (c->n >= mn) return;
    MNode *L = i > 0 ? x->ch[i - 1] : NULL, *R = i < x->n ? x->ch[i + 1] : NULL;
    if (L && L->n > mn) {                               /* borrow from the left sibling through the parent */
        for (int j = c->n; j > 0; j--) c->key[j] = c->key[j - 1];
        for (int j = c->n + 1; j > 0; j--) c->ch[j] = c->ch[j - 1];
        c->key[0] = x->key[i - 1];                      /* separator comes down */
        c->ch[0] = L->ch[L->n];
        x->key[i - 1] = L->key[L->n - 1];               /* largest key of the sibling goes up */
        L->ch[L->n] = NULL; L->n--; c->n++;
        bt_note("borrow-left");
    } else if (R && R->n > mn) {                        /* borrow from the right sibling */
        c->key[c->n] = x->key[i];
        c->ch[c->n + 1] = R->ch[0];
        x->key[i] = R->key[0];
        for (int j = 0; j < R->n - 1; j++) R->key[j] = R->key[j + 1];
        for (int j = 0; j < R->n; j++) R->ch[j] = R->ch[j + 1];
        R->ch[R->n] = NULL; R->n--; c->n++;
        bt_note("borrow-right");
    } else {                                            /* merge: left + separator + this (or this + separator + right) */
        int left = L && !(bt_merge_right_first && R);   /* which neighbour do we combine with? */
        int li = left ? i - 1 : i;                      /* index of the left node of the pair */
        MNode *a = x->ch[li], *b = x->ch[li + 1];
        a->key[a->n] = x->key[li];
        for (int j = 0; j < b->n; j++) a->key[a->n + 1 + j] = b->key[j];
        for (int j = 0; j <= b->n; j++) a->ch[a->n + 1 + j] = b->ch[j];
        a->n += 1 + b->n;
        free(b);
        for (int j = li; j < x->n - 1; j++) x->key[j] = x->key[j + 1];
        for (int j = li + 1; j < x->n; j++) x->ch[j] = x->ch[j + 1];
        x->ch[x->n] = NULL; x->n--;
        bt_note(left ? "merge-left" : "merge-right");
    }
}
static int bt_del(MNode *x, int m, int k)
{
    bt_reads++;
    int i = 0;
    while (i < x->n && x->key[i] < k) i++;
    int found = i < x->n && x->key[i] == k;
    if (!x->ch[0]) {                                    /* leaf */
        if (!found) return 0;
        for (int j = i; j < x->n - 1; j++) x->key[j] = x->key[j + 1];
        x->n--;
        return 1;
    }
    if (found && bt_use_pred) {                         /* inner key: replace by the in-order predecessor (a leaf key) */
        const MNode *p = x->ch[i];
        while (p->ch[0]) p = p->ch[p->n];
        int pk = p->key[p->n - 1];
        x->key[i] = pk;
        int r = bt_del(x->ch[i], m, pk); assert(r);
        bt_fix(x, i, m);
        return 1;
    }
    if (found) {                                        /* inner key: replace by the in-order successor (a leaf key) */
        const MNode *s = x->ch[i + 1];
        while (s->ch[0]) s = s->ch[0];
        int sk = s->key[0];
        x->key[i] = sk;
        int r = bt_del(x->ch[i + 1], m, sk); assert(r);
        bt_fix(x, i + 1, m);
        return 1;
    }
    int r = bt_del(x->ch[i], m, k);
    if (r) bt_fix(x, i, m);
    return r;
}
static int bt_delete(BTree *t, int k)
{
    bt_fixlog[0] = 0;
    if (!t->root) return 0;
    int r = bt_del(t->root, t->m, k);
    if (t->root->n == 0) {                              /* the root lost its last key: the tree shrinks */
        MNode *old = t->root;
        t->root = old->ch[0];                           /* NULL when the whole tree is now empty */
        free(old);
    }
    return r;
}

/* ---- validity: the four B-tree rules + search-tree order.  Returns the height or -1 ---- */
static int bt_valid_(const MNode *x, int m, int is_root, long lo, long hi)
{
    if (x->n < (is_root ? 1 : bt_min_keys(m)) || x->n > m - 1) return -1;
    for (int i = 0; i < x->n; i++) {
        if (x->key[i] <= lo || x->key[i] >= hi) return -1;
        if (i && x->key[i] <= x->key[i - 1]) return -1;
    }
    if (mn_leaf(x)) return 1;
    int h = -2;
    for (int i = 0; i <= x->n; i++) {
        if (!x->ch[i]) return -1;                       /* inner node: all n+1 children must exist */
        int c = bt_valid_(x->ch[i], m, 0, i ? x->key[i - 1] : lo, i < x->n ? x->key[i] : hi);
        if (c < 0) return -1;
        if (h == -2) h = c; else if (h != c) return -1; /* all leaves on one level */
    }
    return h + 1;
}
static int bt_valid(const BTree *t)
{
    if (!t->root) return 1;
    if (t->root->n < 1) return 0;
    return bt_valid_(t->root, t->m, 1, -2147483647L - 1, 2147483647L) > 0;
}
//<<btree

//>>btlevels
/* print the tree level by level (what you draw on paper) */
static void bt_levels(const BTree *t)
{
    const MNode *cur[4096], *nxt[4096];
    int nc = 0;
    if (!t->root) { puts("  (empty)"); return; }
    cur[nc++] = t->root;
    for (int lvl = 0; nc; lvl++) {
        printf("  level %d:", lvl);
        int nn = 0;
        for (int q = 0; q < nc; q++) {
            printf(" [");
            for (int i = 0; i < cur[q]->n; i++) printf(i ? " %d" : "%d", cur[q]->key[i]);
            printf("]");
            if (!mn_leaf(cur[q])) for (int i = 0; i <= cur[q]->n; i++) nxt[nn++] = cur[q]->ch[i];
        }
        putchar('\n');
        for (int q = 0; q < nn; q++) cur[q] = nxt[q];
        nc = nn;
    }
}
//<<btlevels

#ifndef BTREE_NO_MAIN
#define LEN(a) ((int)(sizeof(a) / sizeof((a)[0])))
int main(void)
{
    /* ---- Fig. 11.4 and 11.2.1: searching a B tree of order 4 ---- */
    //>>fig114
    BTree f = { mn_parse("[45]{[29 32]{[18 27] [30 31] [36 39]} [49 63]{[46 47] [54 59 61] [67 72]}}"), 4 };
    assert(bt_valid(&f));
    bt_reads = 0; assert(bt_search(&f, 59)); printf("Fig 11.4: search 59 -> found after reading %ld nodes (root, [49 63], [54 59 61])\n", bt_reads);
    assert(bt_reads == 3);
    bt_reads = 0; assert(!bt_search(&f, 9));  printf("          search  9 -> not found after reading %ld nodes (root, [29 32], [18 27])\n", bt_reads);
    assert(bt_reads == 3);
    mn_free(f.root);
    //<<fig114

    /* ---- Example 11.1: order 5, insert 8, 9, 39, 4 ---- */
    //>>ex111
    {
        BTree t = { mn_parse("[18 45 72]{[7 11] [21 27 36 42] [54 63] [81 89 90]}"), 5 };
        assert(bt_valid(&t));
        printf("Example 11.1 (order 5), start:\n"); bt_levels(&t);
        bt_insert(&t, 8);  assert(!strcmp(mn_str(t.root), "[18 45 72]{[7 8 11] [21 27 36 42] [54 63] [81 89 90]}"));
        bt_insert(&t, 9);  assert(!strcmp(mn_str(t.root), "[18 45 72]{[7 8 9 11] [21 27 36 42] [54 63] [81 89 90]}"));
        bt_insert(&t, 39); assert(!strcmp(mn_str(t.root), "[18 36 45 72]{[7 8 9 11] [21 27] [39 42] [54 63] [81 89 90]}"));
        printf("after 8, 9, 39 (36 moved up when the leaf was split):\n"); bt_levels(&t);
        bt_insert(&t, 4);
        assert(!strcmp(mn_str(t.root), "[36]{[8 18]{[4 7] [9 11] [21 27]} [45 72]{[39 42] [54 63] [81 89 90]}}"));
        printf("after 4 (8 moved up, then 36 moved up, the tree grew a level):\n"); bt_levels(&t);
        assert(bt_valid(&t));
        mn_free(t.root);
    }
    //<<ex111

    /* ---- Example 11.2: order 5, delete 93, 201, 180, 72 ---- */
    //>>ex112
    {
        BTree t = { mn_parse("[108]{[63 81]{[36 45] [72 79] [90 93 101]} [117 201]{[111 114] [151 180] [243 256 333 450]}}"), 5 };
        assert(bt_valid(&t));
        bt_delete(&t, 93);  assert(!strcmp(mn_str(t.root), "[108]{[63 81]{[36 45] [72 79] [90 101]} [117 201]{[111 114] [151 180] [243 256 333 450]}}"));
        bt_delete(&t, 201); assert(!strcmp(mn_str(t.root), "[108]{[63 81]{[36 45] [72 79] [90 101]} [117 243]{[111 114] [151 180] [256 333 450]}}"));
        bt_delete(&t, 180); assert(!strcmp(mn_str(t.root), "[108]{[63 81]{[36 45] [72 79] [90 101]} [117 256]{[111 114] [151 243] [333 450]}}"));
        assert(!strcmp(bt_fixlog, "borrow-right"));             /* 243 came down, 256 went up */
        bt_delete(&t, 72);                                       /* leaf merges left, then the inner node merges too */
        assert(!strcmp(bt_fixlog, "merge-left merge-right"));
        assert(!strcmp(mn_str(t.root), "[81 108 117 256]{[36 45 63 79] [90 101] [111 114] [151 243] [333 450]}"));
        assert(bt_valid(&t) && mn_height(t.root) == 2);
        printf("Example 11.2 after deleting 93, 201, 180, 72:\n"); bt_levels(&t);
        mn_free(t.root);
    }
    //<<ex112

    /* ---- Example 11.3: order 3, insert 121, 87 then delete 36, 109 ---- */
    //>>ex113
    {
        BTree t = { mn_parse("[72]{[45]{[27 36] [54]} [108]{[81 90] [109 117]}}"), 3 };
        assert(bt_valid(&t));
        bt_insert(&t, 121); assert(!strcmp(mn_str(t.root), "[72]{[45]{[27 36] [54]} [108 117]{[81 90] [109] [121]}}"));
        bt_insert(&t, 87);  assert(!strcmp(mn_str(t.root), "[72 108]{[45]{[27 36] [54]} [87]{[81] [90]} [117]{[109] [121]}}"));
        bt_delete(&t, 36);  assert(!strcmp(mn_str(t.root), "[72 108]{[45]{[27] [54]} [87]{[81] [90]} [117]{[109] [121]}}"));
        bt_delete(&t, 109); assert(!strcmp(mn_str(t.root), "[72]{[45]{[27] [54]} [87 108]{[81] [90] [117 121]}}"));
        printf("Example 11.3 final tree: %s\n", mn_str(t.root));
        assert(bt_valid(&t));
        mn_free(t.root);
    }
    //<<ex113

    /* ---- Example 11.4: build an order-5 tree from 20 keys, printing the book's nine steps ---- */
    //>>ex114
    {
        BTree t = { NULL, 5 };
        struct { int keys[4]; int n; const char *expect; } S[] = {
            { { 3, 14, 7, 1 }, 4, "[1 3 7 14]" },
            { { 8 },           1, "[7]{[1 3] [8 14]}" },
            { { 5, 11, 17 },   3, "[7]{[1 3 5] [8 11 14 17]}" },
            { { 13 },          1, "[7 13]{[1 3 5] [8 11] [14 17]}" },
            { { 6, 23, 12, 20 }, 4, "[7 13]{[1 3 5 6] [8 11 12] [14 17 20 23]}" },
            { { 26 },          1, "[7 13 20]{[1 3 5 6] [8 11 12] [14 17] [23 26]}" },
            { { 4 },           1, "[4 7 13 20]{[1 3] [5 6] [8 11 12] [14 17] [23 26]}" },
            { { 16, 18, 24, 25 }, 4, "[4 7 13 20]{[1 3] [5 6] [8 11 12] [14 16 17 18] [23 24 25 26]}" },
            { { 19 },          1, "[13]{[4 7]{[1 3] [5 6] [8 11 12]} [17 20]{[14 16] [18 19] [23 24 25 26]}}" },
        };
        for (int s = 0; s < LEN(S); s++) {
            for (int i = 0; i < S[s].n; i++) { assert(bt_insert(&t, S[s].keys[i])); assert(bt_valid(&t)); }
            printf("  step %d: %s\n", s + 1, mn_str(t.root));
            assert(!strcmp(mn_str(t.root), S[s].expect));
        }
        mn_free(t.root);
    }
    //<<ex114

    /* ---- Review Q.8: the B tree of Fig. 11.4 (order 4): insert 1,5,7,11,13,15,17,19 then delete 30,59,67 ---- */
    //>>q8
    {
        BTree t = { mn_parse("[45]{[29 32]{[18 27] [30 31] [36 39]} [49 63]{[46 47] [54 59 61] [67 72]}}"), 4 };
        int add[] = { 1, 5, 7, 11, 13, 15, 17, 19 };
        for (int i = 0; i < LEN(add); i++) {
            assert(bt_insert(&t, add[i])); assert(bt_valid(&t));
            printf("  Q8(a) insert %2d: %s\n", add[i], mn_str(t.root));
        }
        printf("Q8(a) result:\n"); bt_levels(&t);
        int del[] = { 30, 59, 67 };
        for (int i = 0; i < LEN(del); i++) {
            assert(bt_delete(&t, del[i])); assert(bt_valid(&t));
            printf("  Q8(b) delete %2d: %s   (repair: %s)\n", del[i], mn_str(t.root), bt_fixlog[0] ? bt_fixlog : "none");
        }
        printf("Q8(b) result:\n"); bt_levels(&t);
        assert(mn_count_keys(t.root) == 18 + 8 - 3);      /* 18 keys at the start, +8, -3 */
        assert(!strcmp(mn_str(t.root), "[11 18 45]{[5]{[1] [7]} [15]{[13] [17]} [29 32]{[19 27] [31] [36 39]} [49 63]{[46 47] [54 61] [72]}}"));
        mn_free(t.root);
    }
    //<<q8

    /* ---- height: why a big m matters (RQ1).  Keys needed to force a given height with min fill ---- */
    //>>height
    printf("minimum keys in a B tree of height h   (h = 1 is just a root):\n");
    for (int m = 3; m <= 5; m++) {
        int t = (m + 1) / 2;                            /* min children of an inner node = ceil(m/2) */
        printf("  m=%d:", m);
        long keys = 1;
        for (int h = 1; h <= 6; h++) {
            if (h == 1) keys = 1; else { long p = 1; for (int i = 0; i < h - 2; i++) p *= t; keys = 2 * p * t - 1; }
            printf(" h=%d:%ld", h, keys);
        }
        printf("\n");
    }
    {
        /* build 10000 sorted keys with each order and report the real height */
        printf("height of a tree holding 10000 keys inserted in sorted order:\n");
        for (int m = 3; m <= 15; m += 4) {
            BTree t = { NULL, m };
            for (int k = 0; k < 10000; k++) bt_insert(&t, k);
            int h = mn_height(t.root);
            double lo = log(10001.0) / log(m), hi = 1 + log((10000 + 1) / 2.0) / log((m + 1) / 2);
            printf("  m=%2d: height %d   (bounds %.1f <= h <= %.1f)\n", m, h, lo, hi);
            assert(h >= lo - 1e-9 && h <= hi + 1e-9 && bt_valid(&t));
            mn_free(t.root);
        }
    }
    //<<height

    /* ---- RQ12: a B tree of order 2 is a perfect binary tree ---- */
    //>>order2
    {
        /* order 2: every node has exactly 2 children (min = ceil(2/2) = 1... but a node with 1 child has 0 keys, so only
           full nodes are possible) -> 1 key, 2 children, all leaves at one level = a perfect binary tree of 2^h - 1 keys */
        BTree t = { mn_parse("[4]{[2]{[1] [3]} [6]{[5] [7]}}"), 2 };
        assert(bt_valid(&t) && mn_count_keys(t.root) == 7 && mn_height(t.root) == 3);
        printf("order 2: %s  -> 7 keys = 2^3 - 1, height 3: a perfect binary tree\n", mn_str(t.root));
        mn_free(t.root);
    }
    //<<order2

    /* ---- stress: random insert/delete against an array, orders 3..15, and the validator after every step ---- */
    //>>stress
    srand(2024);
    for (int m = 3; m <= 15; m++) {
        BTree s = { NULL, m };
        static char in[1000]; memset(in, 0, sizeof in);
        int cnt = 0;
        for (int step = 0; step < 6000; step++) {
            int k = rand() % 1000;
            if (rand() % 5 < 3) { int ok = bt_insert(&s, k); assert(ok == !in[k]); if (ok) { in[k] = 1; cnt++; } }
            else                { int ok = bt_delete(&s, k); assert(ok == in[k]);  if (ok) { in[k] = 0; cnt--; } }
            assert(bt_valid(&s));
            assert(bt_search(&s, k) == in[k]);
        }
        assert(mn_count_keys(s.root) == cnt);
        int a[1000]; int c = mn_inorder(s.root, a); assert(c == cnt);
        for (int i = 1; i < c; i++) assert(a[i - 1] < a[i]);
        mn_free(s.root);
    }
    printf("stress: orders 3..15 x 6000 random steps, validator true after every step\n");
    //<<stress
    puts("btree: all checks passed");
    return 0;
}
#endif
