/* ============================================================
 *  mway.c  —  M-way search tree (Thareja 11.1, Fig. 11.2-11.3, Review Q.13)
 *
 *  A node has up to M-1 keys and M children; a node is NOT forced to be full or balanced.
 *    - search : find the interval  key[i-1] < k < key[i]  and go down ch[i]
 *    - insert : go down while the interval's child exists; at the end put the key in the node when it has
 *               room, otherwise hang a brand-new one-key node as that child
 *    - delete : a key of a leaf is just removed; a key of an inner node is replaced by its in-order
 *               predecessor (or successor), which is then deleted the same way.  A node that loses its last
 *               key is spliced out (replaced by its only remaining child).
 *  Thareja gives the search and insert rules but no delete rule for the plain M-way tree — this is the usual one.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Sec. 11.1
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 mway.c -o mway
 * ============================================================ */
#include "mnode.h"

//>>mway
typedef struct { MNode *root; int M; } MWay;     /* M = order: up to M-1 keys and M children per node */

/* search: returns 1 if found; *visited = number of nodes looked at (= "disk reads") */
static int mw_search(const MWay *t, int k, int *visited)
{
    int v = 0;
    for (const MNode *x = t->root; x; ) {
        v++;
        int i = 0;
        while (i < x->n && x->key[i] < k) i++;           /* first key >= k */
        if (i < x->n && x->key[i] == k) { if (visited) *visited = v; return 1; }
        x = x->ch[i];                                    /* the interval k falls in */
    }
    if (visited) *visited = v;
    return 0;
}

/* insert: returns 0 if k is already there */
static int mw_insert(MWay *t, int k)
{
    if (!t->root) { t->root = mn_new(); t->root->key[0] = k; t->root->n = 1; return 1; }
    MNode *x = t->root;
    for (;;) {
        int i = 0;
        while (i < x->n && x->key[i] < k) i++;
        if (i < x->n && x->key[i] == k) return 0;        /* duplicate */
        if (x->ch[i]) { x = x->ch[i]; continue; }        /* the interval already has a subtree: go in */
        if (x->n < t->M - 1) {                           /* room: the empty interval is split in two empty ones */
            for (int j = x->n; j > i; j--) { x->key[j] = x->key[j - 1]; x->ch[j + 1] = x->ch[j]; }
            x->key[i] = k; x->ch[i + 1] = NULL; x->n++;
        } else {                                         /* full: the new key starts a new child node */
            MNode *c = mn_new(); c->key[0] = k; c->n = 1; x->ch[i] = c;
        }
        return 1;
    }
}

/* delete: returns the (possibly new) root of the subtree; *done = 1 if the key was found */
static MNode *mw_del(MNode *x, int k, int *done)
{
    if (!x) return NULL;
    int i = 0;
    while (i < x->n && x->key[i] < k) i++;
    if (i == x->n || x->key[i] != k) { x->ch[i] = mw_del(x->ch[i], k, done); return x; }
    *done = 1;
    if (x->ch[i]) {                                      /* inner key: take the predecessor from the left subtree */
        const MNode *p = x->ch[i];
        while (p->ch[p->n]) p = p->ch[p->n];
        int pk = p->key[p->n - 1];
        x->key[i] = pk;
        int d2 = 0; x->ch[i] = mw_del(x->ch[i], pk, &d2); assert(d2);
        return x;
    }
    if (x->ch[i + 1]) {                                  /* left interval empty: take the successor instead */
        const MNode *s = x->ch[i + 1];
        while (s->ch[0]) s = s->ch[0];
        int sk = s->key[0];
        x->key[i] = sk;
        int d2 = 0; x->ch[i + 1] = mw_del(x->ch[i + 1], sk, &d2); assert(d2);
        return x;
    }
    for (int j = i; j < x->n - 1; j++) x->key[j] = x->key[j + 1];     /* both neighbours empty: just remove */
    for (int j = i + 1; j < x->n; j++) x->ch[j] = x->ch[j + 1];
    x->ch[x->n] = NULL; x->n--;
    if (x->n == 0) { MNode *only = x->ch[0]; free(x); return only; } /* splice out an empty node */
    return x;
}
static int mw_delete(MWay *t, int k) { int d = 0; t->root = mw_del(t->root, k, &d); return d; }

/* validity: every node has 1..M-1 sorted keys, every key of ch[i] lies strictly between key[i-1] and key[i] */
static int mw_valid_(const MNode *x, int M, long lo, long hi)
{
    if (!x) return 1;
    if (x->n < 1 || x->n > M - 1) return 0;
    for (int i = 0; i < x->n; i++) {
        if (x->key[i] <= lo || x->key[i] >= hi) return 0;
        if (i && x->key[i] <= x->key[i - 1]) return 0;
    }
    for (int i = 0; i <= x->n; i++)
        if (!mw_valid_(x->ch[i], M, i ? x->key[i - 1] : lo, i < x->n ? x->key[i] : hi)) return 0;
    for (int i = x->n + 1; i <= MW_MAXM + 1; i++) if (x->ch[i]) return 0;
    return 1;
}
static int mw_valid(const MWay *t) { return mw_valid_(t->root, t->M, -2147483647L - 1, 2147483647L); }
//<<mway

#ifndef MWAY_NO_MAIN
int main(void)
{
    /* ---- Fig. 11.3: the 3-way search tree used in Review Q.13 ---- */
    //>>q13
    MWay t = { mn_parse("[18 45]{[9 11] [27 36]{- [29 30] -} [54 63]{- - [72 81]}}"), 3 };
    printf("Fig 11.3 (M = 3):\n  %s\n", mn_str(t.root));
    assert(mw_valid(&t));

    int v;
    assert(mw_search(&t, 30, &v) && v == 3);              /* root -> [27 36] -> [29 30] */
    assert(mw_search(&t, 81, &v) && v == 3);
    assert(!mw_search(&t, 40, &v) && v == 2);             /* root, [27 36]: ch[2] is NULL */
    printf("  search 30: %d nodes, search 40 (absent): %d nodes\n", 3, v);

    int ins[] = { 23, 45, 67, 87, 54, 32, 11 };
    for (int i = 0; i < 7; i++) {
        int ok = mw_insert(&t, ins[i]);
        printf("  insert %2d -> %s\n", ins[i], ok ? "inserted" : "already present (ignored)");
        assert(ok == (ins[i] != 45 && ins[i] != 54 && ins[i] != 11));   /* 45, 54, 11 are already stored */
        assert(mw_valid(&t));
    }
    printf("after the inserts:\n  %s\n", mn_str(t.root));
    assert(!strcmp(mn_str(t.root),
        "[18 45]{[9 11] [27 36]{[23] [29 30]{- - [32]} -} [54 63]{- - [72 81]{[67] - [87]}}}"));

    int del[] = { 9, 36, 54 };
    for (int i = 0; i < 3; i++) {
        assert(mw_delete(&t, del[i]));
        printf("  delete %2d -> %s\n", del[i], mn_str(t.root));
        assert(mw_valid(&t));
    }
    assert(!strcmp(mn_str(t.root),
        "[18 45]{[11] [27 32]{[23] [29 30] -} [63]{- [72 81]{[67] - [87]}}}"));
    //<<q13
    mn_free(t.root);

    /* ---- the same keys, different order -> a different tree: an M-way tree is NOT balanced ---- */
    //>>shape
    {
        MWay a = { NULL, 3 }, b = { NULL, 3 };
        for (int k = 1; k <= 12; k++) mw_insert(&a, k);                          /* sorted */
        int mix[] = { 6, 3, 9, 1, 5, 8, 11, 2, 4, 7, 10, 12 };
        for (int i = 0; i < 12; i++) mw_insert(&b, mix[i]);                      /* balanced-ish order */
        printf("12 keys, M = 3: sorted order -> height %d,  middle-first order -> height %d\n",
               mn_height(a.root), mn_height(b.root));
        assert(mw_valid(&a) && mw_valid(&b));
        assert(mn_height(a.root) == 6 && mn_height(b.root) < mn_height(a.root));
        mn_free(a.root); mn_free(b.root);
    }
    //<<shape

    /* ---- stress: random inserts/deletes against a plain array, M = 3..9 ---- */
    //>>stress
    srand(11);
    for (int M = 3; M <= 9; M++) {
        MWay s = { NULL, M };
        static char in[400]; memset(in, 0, sizeof in);
        int cnt = 0;
        for (int step = 0; step < 4000; step++) {
            int k = rand() % 400;
            if (rand() % 3) { int ok = mw_insert(&s, k); assert(ok == !in[k]); if (ok) { in[k] = 1; cnt++; } }
            else            { int ok = mw_delete(&s, k); assert(ok == in[k]);  if (ok) { in[k] = 0; cnt--; } }
            if (step % 97 == 0) assert(mw_valid(&s));
        }
        assert(mw_valid(&s) && mn_count_keys(s.root) == cnt);
        int a[400]; int c = mn_inorder(s.root, a);
        assert(c == cnt);
        for (int i = 1; i < c; i++) assert(a[i - 1] < a[i]);
        mn_free(s.root);
    }
    printf("stress: 7 orders x 4000 random insert/delete steps agree with a plain array\n");
    //<<stress
    puts("mway: all checks passed");
    return 0;
}
#endif
