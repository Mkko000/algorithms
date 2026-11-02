/* ============================================================
 *  bplus.c  —  B+ tree (Thareja 11.3, Fig. 11.9-11.13, Examples 11.5-11.6; Review Q.10)
 *
 *  All the data (the keys) live in the LEAVES; the inner nodes only hold copies of keys used for routing,
 *  and the leaves are chained left-to-right (the `next` pointer of MNode) so a range query is a walk along the chain.
 *  A node of order m holds at most m-1 keys.
 *    - insert : into a leaf.  Overflow of a LEAF -> split, the first key of the right half is COPIED up.
 *               Overflow of an INDEX node -> split, the middle key MOVES up.   (Fig. 11.10)
 *    - delete : from a leaf.  A leaf that becomes empty is removed together with its separator; an index node
 *               left with no key is merged with a sibling (the separator comes DOWN), or — if the merge would be
 *               too big — it borrows a child from the sibling.   (Fig. 11.12, Example 11.6)
 *    - range  : search the first key, then follow `next`.
 *  Thareja's delete rule only talks about a node that "underflows"; her example shows underflow = empty.
 *  The textbook-complete version (minimum fill ceil((m-1)/2) with borrowing) is the B tree's bt_fix in btree.c.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Sec. 11.3
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 bplus.c -o bplus
 * ============================================================ */
#include "mnode.h"

//>>bplus
typedef struct { MNode *root; int m; } BPlus;

static long bp_reads = 0;
static int bp_upper(const MNode *x, int k) { int i = 0; while (i < x->n && x->key[i] <= k) i++; return i; }  /* number of keys <= k */

static const MNode *bp_find_leaf(const BPlus *t, int k)
{
    const MNode *x = t->root;
    while (x && !mn_leaf(x)) { bp_reads++; x = x->ch[bp_upper(x, k)]; }
    if (x) bp_reads++;
    return x;
}
static int bp_search(const BPlus *t, int k)
{
    const MNode *l = bp_find_leaf(t, k);
    if (!l) return 0;
    for (int i = 0; i < l->n; i++) if (l->key[i] == k) return 1;
    return 0;
}
/* all keys in [lo, hi] by walking the leaf chain; returns how many */
static int bp_range(const BPlus *t, int lo, int hi, int *out)
{
    int c = 0;
    const MNode *first = bp_find_leaf(t, lo);
    for (const MNode *l = first; l; l = l->next) {
        if (l != first) bp_reads++;                          /* following `next` = one more leaf read */
        for (int i = 0; i < l->n; i++) {
            if (l->key[i] > hi) return c;
            if (l->key[i] >= lo) out[c++] = l->key[i];
        }
    }
    return c;
}

/* ---- insert ---- */
static int bp_ins(MNode *x, int m, int k, int *up, MNode **right)
{
    int i = bp_upper(x, k);
    if (mn_leaf(x)) {
        for (int j = 0; j < x->n; j++) if (x->key[j] == k) return 0;
        for (int j = x->n; j > i; j--) x->key[j] = x->key[j - 1];
        x->key[i] = k; x->n++;
        if (x->n < m) return 1;
        int keep = x->n / 2;                                 /* leaf split: left keeps n/2, right gets the rest */
        MNode *r = mn_new();
        r->n = x->n - keep;
        for (int j = 0; j < r->n; j++) r->key[j] = x->key[keep + j];
        x->n = keep;
        r->next = x->next; x->next = r;                      /* keep the chain */
        *up = r->key[0]; *right = r;                         /* COPY the first key of the right leaf up */
        return 2;
    }
    int mk; MNode *mr = NULL;
    int res = bp_ins(x->ch[i], m, k, &mk, &mr);
    if (res != 2) return res;
    for (int j = x->n; j > i; j--) { x->key[j] = x->key[j - 1]; x->ch[j + 1] = x->ch[j]; }
    x->key[i] = mk; x->ch[i + 1] = mr; x->n++;
    if (x->n < m) return 1;
    int mid = (x->n - 1) / 2;                                /* index split: the middle key MOVES up */
    MNode *r = mn_new();
    r->n = x->n - mid - 1;
    for (int j = 0; j < r->n; j++) r->key[j] = x->key[mid + 1 + j];
    for (int j = 0; j <= r->n; j++) { r->ch[j] = x->ch[mid + 1 + j]; x->ch[mid + 1 + j] = NULL; }
    *up = x->key[mid]; *right = r; x->n = mid;
    return 2;
}
static int bp_insert(BPlus *t, int k)
{
    if (!t->root) { t->root = mn_new(); t->root->key[0] = k; t->root->n = 1; return 1; }
    int up; MNode *r = NULL;
    int res = bp_ins(t->root, t->m, k, &up, &r);
    if (res == 2) {
        MNode *nr = mn_new();
        nr->n = 1; nr->key[0] = up; nr->ch[0] = t->root; nr->ch[1] = r;
        t->root = nr;                                        /* the tree grows by one level */
    }
    return res != 0;
}

/* ---- delete ---- */
static char bp_log[128];
static void bp_note(const char *s) { if (bp_log[0]) strcat(bp_log, " "); strcat(bp_log, s); }
static void bp_drop(MNode *x, int ci)                        /* remove child ci and one adjacent separator from x */
{
    int ki = ci > 0 ? ci - 1 : 0;
    for (int j = ki; j < x->n - 1; j++) x->key[j] = x->key[j + 1];
    for (int j = ci; j < x->n; j++) x->ch[j] = x->ch[j + 1];
    x->ch[x->n] = NULL; x->n--;
}
static void bp_unlink(BPlus *t, MNode *leaf)                 /* take a leaf out of the chain (walk from the leftmost leaf) */
{
    MNode *l = t->root;
    while (l && !mn_leaf(l)) l = l->ch[0];
    if (l == leaf) return;                                   /* the leftmost leaf has no predecessor */
    while (l && l->next != leaf) l = l->next;
    if (l) l->next = leaf->next;
}
static int bp_del(BPlus *t, MNode *x, int k)
{
    if (mn_leaf(x)) {
        for (int i = 0; i < x->n; i++) if (x->key[i] == k) {
            for (int j = i; j < x->n - 1; j++) x->key[j] = x->key[j + 1];
            x->n--; return 1;
        }
        return 0;
    }
    int i = bp_upper(x, k);
    MNode *c = x->ch[i];
    int r = bp_del(t, c, k);
    if (!r || c->n > 0) return r;
    if (mn_leaf(c)) {                                        /* an empty leaf: remove it and its separator */
        bp_unlink(t, c); free(c); bp_drop(x, i); bp_note("drop-leaf");
        return 1;
    }
    /* c is an index node with no key left (one child): merge with a sibling, or borrow when the merge would not fit */
    int m = t->m;
    MNode *L = i > 0 ? x->ch[i - 1] : NULL, *R = i < x->n ? x->ch[i + 1] : NULL;
    if (L && L->n + 1 <= m - 1) {                            /* merge into the left sibling: separator comes down */
        L->key[L->n] = x->key[i - 1]; L->ch[L->n + 1] = c->ch[0]; L->n++;
        free(c); bp_drop(x, i); bp_note("merge-left");
    } else if (!L && R && R->n + 1 <= m - 1) {               /* merge the right sibling into c */
        c->key[0] = x->key[i];
        for (int j = 0; j < R->n; j++) c->key[1 + j] = R->key[j];
        for (int j = 0; j <= R->n; j++) c->ch[1 + j] = R->ch[j];
        c->n = 1 + R->n;
        free(R); bp_drop(x, i + 1);                          /* removes ch[i+1] and key[i] */
        bp_note("merge-right");
    } else if (L) {                                          /* borrow the last child of L through the parent */
        c->ch[1] = c->ch[0]; c->ch[0] = L->ch[L->n];
        c->key[0] = x->key[i - 1]; c->n = 1;
        x->key[i - 1] = L->key[L->n - 1];
        L->ch[L->n] = NULL; L->n--; bp_note("borrow-left");
    } else {                                                 /* borrow the first child of R */
        c->key[0] = x->key[i]; c->ch[1] = R->ch[0]; c->n = 1;
        x->key[i] = R->key[0];
        for (int j = 0; j < R->n - 1; j++) R->key[j] = R->key[j + 1];
        for (int j = 0; j < R->n; j++) R->ch[j] = R->ch[j + 1];
        R->ch[R->n] = NULL; R->n--; bp_note("borrow-right");
    }
    return 1;
}
static int bp_delete(BPlus *t, int k)
{
    bp_log[0] = 0;
    if (!t->root) return 0;
    int r = bp_del(t, t->root, k);
    if (t->root->n == 0) {                                   /* empty root: an index root with one child collapses, an empty leaf means an empty tree */
        MNode *old = t->root;
        t->root = mn_leaf(old) ? NULL : old->ch[0];
        free(old);
    }
    return r;
}

/* ---- relink the leaf chain of a tree built by hand (mn_parse) ---- */
static void bp_link_(MNode *x, MNode **prev)
{
    if (mn_leaf(x)) { if (*prev) (*prev)->next = x; *prev = x; return; }
    for (int i = 0; i <= x->n; i++) bp_link_(x->ch[i], prev);
}
static void bp_relink(MNode *root) { MNode *prev = NULL; if (root) bp_link_(root, &prev); }

/* ---- validity ---- */
static int bp_valid_(const MNode *x, int m, int is_root, long lo, long hi, int depth, int *leafdepth, const MNode **prevleaf)
{
    if (x->n < 1 || x->n > m - 1) return 0;
    (void)is_root;
    for (int i = 0; i < x->n; i++) {
        if (x->key[i] < lo || x->key[i] >= hi) return 0;     /* leaf keys: lo <= key < hi (separator copies are lo) */
        if (i && x->key[i] <= x->key[i - 1]) return 0;
    }
    if (mn_leaf(x)) {
        if (*leafdepth < 0) *leafdepth = depth; else if (*leafdepth != depth) return 0;
        if (*prevleaf && (*prevleaf)->next != x) return 0;   /* the chain must visit the leaves in order */
        if (*prevleaf && (*prevleaf)->key[(*prevleaf)->n - 1] >= x->key[0]) return 0;
        *prevleaf = x;
        return 1;
    }
    for (int i = 0; i <= x->n; i++) {
        if (!x->ch[i]) return 0;
        if (!bp_valid_(x->ch[i], m, 0, i ? x->key[i - 1] : lo, i < x->n ? x->key[i] : hi, depth + 1, leafdepth, prevleaf)) return 0;
    }
    return 1;
}
static int bp_valid(const BPlus *t)
{
    if (!t->root) return 1;
    int ld = -1; const MNode *pl = NULL;
    if (!bp_valid_(t->root, t->m, 1, -2147483647L - 1, 2147483647L, 0, &ld, &pl)) return 0;
    return pl == NULL || pl->next == NULL;                   /* the last leaf ends the chain */
}
//<<bplus

//>>bpprint
/* the keys stored in the leaves, left to right, by following the chain only */
static int bp_leaf_keys(const BPlus *t, int *out)
{
    const MNode *l = t->root; int c = 0;
    while (l && !mn_leaf(l)) l = l->ch[0];
    for (; l; l = l->next) for (int i = 0; i < l->n; i++) out[c++] = l->key[i];
    return c;
}
static void bp_chain(const BPlus *t)
{
    const MNode *l = t->root;
    while (l && !mn_leaf(l)) l = l->ch[0];
    printf("  leaf chain:");
    for (; l; l = l->next) { printf(" ["); for (int i = 0; i < l->n; i++) printf(i ? " %d" : "%d", l->key[i]); printf("]"); }
    printf("\n");
}
//<<bpprint

#ifndef BPLUS_NO_MAIN
#define LEN(a) ((int)(sizeof(a) / sizeof((a)[0])))
int main(void)
{
    /* ---- Fig 11.9: a B+ tree of order 3 — search, and a range query along the leaf chain ---- */
    //>>fig119
    {
        BPlus t = { mn_parse("[13]{[4 7]{[1 3] [5 6] [8 11]} [17 20]{[14 16] [18 19] [23 24]}}"), 3 };
        bp_relink(t.root);
        assert(bp_valid(&t));
        printf("Fig 11.9 (order 3):  %s\n", mn_str(t.root));
        bp_chain(&t);
        bp_reads = 0; assert(bp_search(&t, 16));  printf("  search 16: %ld nodes read (root, [17 20], leaf) \n", bp_reads);
        assert(bp_reads == 3);
        bp_reads = 0; assert(!bp_search(&t, 12)); assert(bp_reads == 3);
        int out[32];
        bp_reads = 0;
        int c = bp_range(&t, 5, 18, out);
        printf("  keys in [5, 18] (descend once, then follow the chain): ");
        for (int i = 0; i < c; i++) printf("%d ", out[i]);
        printf("  (%ld nodes read)\n", bp_reads);
        int want[] = { 5, 6, 8, 11, 14, 16, 18 };
        assert(c == LEN(want) && !memcmp(out, want, sizeof want));
        mn_free(t.root);
    }
    //<<fig119

    /* ---- Example 11.5: order 4, insert 33 (a leaf split, then an index split) ---- */
    //>>ex115
    {
        BPlus t = { mn_parse("[10 20 30]{[3 6 9] [15] [27] [32 34 48]}"), 4 };
        bp_relink(t.root);
        assert(bp_valid(&t));
        printf("Example 11.5 start:      %s\n", mn_str(t.root));
        assert(bp_insert(&t, 33));
        printf("after inserting 33:      %s\n", mn_str(t.root));
        assert(!strcmp(mn_str(t.root), "[20]{[10]{[3 6 9] [15]} [30 34]{[27] [32 33] [34 48]}}"));
        assert(bp_valid(&t));
        bp_chain(&t);
        mn_free(t.root);
    }
    //<<ex115

    /* ---- Example 11.6: delete 15 (the leaf empties, then the index node empties) ---- */
    //>>ex116
    {
        BPlus t = { mn_parse("[20]{[15]{[3 6 9] [15]} [30 34]{[27] [32 33] [34 48]}}"), 4 };
        bp_relink(t.root);
        assert(bp_valid(&t));
        printf("Example 11.6 start:      %s\n", mn_str(t.root));
        assert(bp_delete(&t, 15));
        printf("after deleting 15:       %s   (%s)\n", mn_str(t.root), bp_log);
        assert(!strcmp(mn_str(t.root), "[20 30 34]{[3 6 9] [27] [32 33] [34 48]}"));
        assert(!strcmp(bp_log, "drop-leaf merge-right"));
        assert(bp_valid(&t) && mn_height(t.root) == 2);
        bp_chain(&t);
        mn_free(t.root);
    }
    //<<ex116

    /* ---- Review Q.10: order 5, insert 90, 27, 7, 9, 18, 21, 3, 4, 16, 11, 21, 72 ---- */
    //>>q10
    {
        BPlus t = { NULL, 5 };
        int seq[] = { 90, 27, 7, 9, 18, 21, 3, 4, 16, 11, 21, 72 };
        for (int i = 0; i < LEN(seq); i++) {
            int ok = bp_insert(&t, seq[i]);
            assert(bp_valid(&t));
            printf("  insert %2d%s: %s\n", seq[i], ok ? "  " : " (dup, ignored)", mn_str(t.root));
            assert(ok == (i != 10));                         /* the second 21 is a duplicate */
        }
        assert(!strcmp(mn_str(t.root), "[7 18 27]{[3 4] [7 9 11 16] [18 21] [27 72 90]}"));
        bp_chain(&t);
        mn_free(t.root);
    }
    //<<q10

    /* ---- B tree vs B+ tree: a range query reads fewer nodes in the B+ tree (Table 11.1) ---- */
    //>>range
    {
        BPlus t = { NULL, 9 };
        for (int k = 0; k < 5000; k++) bp_insert(&t, k * 2);          /* even numbers 0..9998 */
        int out[4096];
        bp_reads = 0; int c = bp_range(&t, 1000, 1199, out);
        printf("B+ tree, m = 9, 5000 keys, height %d: range of %d keys read %ld nodes (the descent + the chain)\n", mn_height(t.root), c, bp_reads);
        assert(c == 100 && bp_reads < 100);
        mn_free(t.root);
    }
    //<<range

    /* ---- stress: random insert/delete against an array; chain keys == sorted array; validator after every step ---- */
    //>>stress
    srand(77);
    for (int m = 3; m <= 12; m++) {
        BPlus s = { NULL, m };
        static char in[600]; memset(in, 0, sizeof in);
        int cnt = 0;
        for (int step = 0; step < 5000; step++) {
            int k = rand() % 600;
            if (rand() % 5 < 3) { int ok = bp_insert(&s, k); assert(ok == !in[k]); if (ok) { in[k] = 1; cnt++; } }
            else                { int ok = bp_delete(&s, k); assert(ok == in[k]);  if (ok) { in[k] = 0; cnt--; } }
            assert(bp_valid(&s));
            assert(bp_search(&s, k) == in[k]);
        }
        int a[600], c = bp_leaf_keys(&s, a);
        assert(c == cnt);
        for (int i = 1; i < c; i++) assert(a[i - 1] < a[i]);
        for (int i = 0; i < c; i++) assert(in[a[i]]);
        mn_free(s.root);
    }
    printf("stress: orders 3..12 x 5000 random steps, tree valid and leaf chain sorted after every step\n");
    //<<stress
    puts("bplus: all checks passed");
    return 0;
}
#endif
