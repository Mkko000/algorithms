/* ============================================================
 *  wbtree.c  —  Weiss's B-tree (Sec. 4.7, Fig. 4.62-4.67, Exercises 4.41-4.42): the "B+" flavour, with two parameters
 *
 *    M  = branching of the inner nodes   (each non-root inner node has ceil(M/2)..M children, so M-1 keys at most)
 *    L  = leaf capacity                  (each leaf holds ceil(L/2)..L data items)
 *    - the DATA lives only in the leaves, all leaves are at the same depth
 *    - an inner node with c children has c-1 keys; key i is the smallest key of subtree i+1
 *    - insert : add to the leaf; with L+1 items split in two (the first half gets floor((L+1)/2)); the inner node that
 *               receives a sixth child splits in two as well, and so on up to the root (a root split = new level)
 *               Exercise 4.41(c): with `adopt` = 1 a full leaf first gives an item to a sibling that has room
 *    - delete : remove from the leaf; below the minimum -> adopt from a neighbour that is above its minimum, else combine;
 *               the parent may then be short and does the same (a root left with one child disappears)
 *  Weiss reads in notation  ( ... ) = a leaf of data items,  [ ... ]{ ... } = an inner node with its children.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: M. A. Weiss, "Data Structures and Algorithm Analysis in C++" (3rd ed.), Sec. 4.7 (translated to C)
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 wbtree.c -o wbtree -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

//>>wb
#define WMAX 40
typedef struct WN {
    int leaf;
    int n;                       /* leaf: number of items   inner: number of CHILDREN */
    int key[WMAX + 2];           /* leaf: the items          inner: key[i] = smallest key of subtree i+1  (n-1 keys) */
    struct WN *ch[WMAX + 2];
} WN;
typedef struct { WN *root; int M, L; int adopt; } WTree;

static WN *wn_new(int leaf) { WN *x = calloc(1, sizeof *x); if (!x) exit(1); x->leaf = leaf; return x; }
static void wn_free(WN *x) { if (!x) return; if (!x->leaf) for (int i = 0; i < x->n; i++) wn_free(x->ch[i]); free(x); }
static int w_min_leaf(const WTree *t) { return (t->L + 1) / 2; }
static int w_min_children(const WTree *t) { return (t->M + 1) / 2; }
static long w_reads = 0;

static int w_child_index(const WN *x, int k) { int i = 0; while (i < x->n - 1 && x->key[i] <= k) i++; return i; }
static int w_search(const WTree *t, int k)
{
    const WN *x = t->root;
    while (x && !x->leaf) { w_reads++; x = x->ch[w_child_index(x, k)]; }
    if (!x) return 0;
    w_reads++;
    for (int i = 0; i < x->n; i++) if (x->key[i] == k) return 1;
    return 0;
}

/* ---- insert ---- */
/* returns 0 dup, 1 ok, 2 split (*sep = smallest key of *right) */
static int w_ins(WTree *t, WN *x, int k, int *sep, WN **right, WN *left_sib, WN *right_sib, WN *parent, int pi)
{
    (void)left_sib; (void)right_sib;
    if (x->leaf) {
        int i = 0; while (i < x->n && x->key[i] < k) i++;
        if (i < x->n && x->key[i] == k) return 0;
        if (x->n == t->L && t->adopt && parent) {                 /* Exercise 4.41(c): look for a sibling with room before splitting */
            WN *L = pi > 0 ? parent->ch[pi - 1] : NULL, *R = pi < parent->n - 1 ? parent->ch[pi + 1] : NULL;
            if (L && L->n < t->L) {                               /* give our smallest item to the left sibling */
                int tmp[WMAX + 2], c = 0;
                for (int j = 0; j < i; j++) tmp[c++] = x->key[j];
                tmp[c++] = k;
                for (int j = i; j < x->n; j++) tmp[c++] = x->key[j];
                L->key[L->n++] = tmp[0];
                x->n = 0; for (int j = 1; j < c; j++) x->key[x->n++] = tmp[j];
                parent->key[pi - 1] = x->key[0];
                return 1;
            }
            if (R && R->n < t->L) {                               /* give our largest item to the right sibling */
                int tmp[WMAX + 2], c = 0;
                for (int j = 0; j < i; j++) tmp[c++] = x->key[j];
                tmp[c++] = k;
                for (int j = i; j < x->n; j++) tmp[c++] = x->key[j];
                for (int j = R->n; j > 0; j--) R->key[j] = R->key[j - 1];
                R->key[0] = tmp[c - 1]; R->n++;
                x->n = 0; for (int j = 0; j < c - 1; j++) x->key[x->n++] = tmp[j];
                parent->key[pi] = R->key[0];
                return 1;
            }
        }
        for (int j = x->n; j > i; j--) x->key[j] = x->key[j - 1];
        x->key[i] = k; x->n++;
        if (x->n <= t->L) return 1;
        int left = x->n / 2;                                      /* L+1 items -> floor((L+1)/2) and the rest */
        WN *r = wn_new(1);
        for (int j = left; j < x->n; j++) r->key[r->n++] = x->key[j];
        x->n = left;
        *sep = r->key[0]; *right = r;
        return 2;
    }
    int i = w_child_index(x, k);
    int s; WN *r = NULL;
    int res = w_ins(t, x->ch[i], k, &s, &r, NULL, NULL, x, i);
    if (res != 2) return res;
    for (int j = x->n - 1; j > i; j--) x->key[j] = x->key[j - 1];
    for (int j = x->n; j > i + 1; j--) x->ch[j] = x->ch[j - 1];
    x->key[i] = s; x->ch[i + 1] = r; x->n++;
    if (x->n <= t->M) return 1;
    int left = x->n / 2;                                          /* M+1 children -> floor((M+1)/2) and the rest */
    WN *nr = wn_new(0);
    nr->n = x->n - left;
    for (int j = 0; j < nr->n; j++) nr->ch[j] = x->ch[left + j];
    for (int j = 0; j < nr->n - 1; j++) nr->key[j] = x->key[left + j];
    *sep = x->key[left - 1];                                      /* the key between the halves goes up */
    for (int j = left; j < x->n; j++) x->ch[j] = NULL;
    x->n = left;
    *right = nr;
    return 2;
}
static int w_insert(WTree *t, int k)
{
    if (!t->root) { t->root = wn_new(1); t->root->key[0] = k; t->root->n = 1; return 1; }
    int s; WN *r = NULL;
    int res = w_ins(t, t->root, k, &s, &r, NULL, NULL, NULL, 0);
    if (res == 2) {
        WN *nr = wn_new(0);
        nr->n = 2; nr->ch[0] = t->root; nr->ch[1] = r; nr->key[0] = s;
        t->root = nr;
    }
    return res != 0;
}

/* ---- delete ---- */
static char w_log[128];
static void w_note(const char *s) { if (w_log[0]) strcat(w_log, " "); strcat(w_log, s); }
static void w_fix(WTree *t, WN *x, int i)
{
    WN *c = x->ch[i];
    int need = c->leaf ? w_min_leaf(t) : w_min_children(t);
    if (c->n >= need) return;
    WN *L = i > 0 ? x->ch[i - 1] : NULL, *R = i < x->n - 1 ? x->ch[i + 1] : NULL;
    if (L && L->n > need) {                                       /* adopt from the left neighbour */
        if (c->leaf) {
            for (int j = c->n; j > 0; j--) c->key[j] = c->key[j - 1];
            c->key[0] = L->key[--L->n]; c->n++;
            x->key[i - 1] = c->key[0];
        } else {
            for (int j = c->n - 1; j > 0; j--) c->key[j] = c->key[j - 1];
            for (int j = c->n; j > 0; j--) c->ch[j] = c->ch[j - 1];
            c->key[0] = x->key[i - 1]; c->ch[0] = L->ch[L->n - 1]; c->n++;
            x->key[i - 1] = L->key[L->n - 2];
            L->ch[L->n - 1] = NULL; L->n--;
        }
        w_note("adopt-left");
    } else if (R && R->n > need) {                                /* adopt from the right neighbour */
        if (c->leaf) {
            c->key[c->n++] = R->key[0];
            for (int j = 0; j < R->n - 1; j++) R->key[j] = R->key[j + 1];
            R->n--;
            x->key[i] = R->key[0];
        } else {
            c->key[c->n - 1] = x->key[i]; c->ch[c->n] = R->ch[0]; c->n++;
            x->key[i] = R->key[0];
            for (int j = 0; j < R->n - 2; j++) R->key[j] = R->key[j + 1];
            for (int j = 0; j < R->n - 1; j++) R->ch[j] = R->ch[j + 1];
            R->ch[R->n - 1] = NULL; R->n--;
        }
        w_note("adopt-right");
    } else {                                                      /* combine with a neighbour (the left one when there is one) */
        int li = L ? i - 1 : i;
        WN *a = x->ch[li], *b = x->ch[li + 1];
        if (a->leaf) { for (int j = 0; j < b->n; j++) a->key[a->n + j] = b->key[j]; a->n += b->n; }
        else {
            a->key[a->n - 1] = x->key[li];                        /* the separator comes down */
            for (int j = 0; j < b->n - 1; j++) a->key[a->n + j] = b->key[j];
            for (int j = 0; j < b->n; j++) a->ch[a->n + j] = b->ch[j];
            a->n += b->n;
        }
        free(b);
        for (int j = li; j < x->n - 2; j++) x->key[j] = x->key[j + 1];
        for (int j = li + 1; j < x->n - 1; j++) x->ch[j] = x->ch[j + 1];
        x->ch[x->n - 1] = NULL; x->n--;
        w_note(L ? "combine-left" : "combine-right");
    }
}
static int w_del(WTree *t, WN *x, int k)
{
    if (x->leaf) {
        for (int i = 0; i < x->n; i++) if (x->key[i] == k) { for (int j = i; j < x->n - 1; j++) x->key[j] = x->key[j + 1]; x->n--; return 1; }
        return 0;
    }
    int i = w_child_index(x, k);
    int r = w_del(t, x->ch[i], k);
    if (r) w_fix(t, x, i);
    return r;
}
static int w_delete(WTree *t, int k)
{
    w_log[0] = 0;
    if (!t->root) return 0;
    int r = w_del(t, t->root, k);
    if (!t->root->leaf && t->root->n == 1) { WN *old = t->root; t->root = old->ch[0]; free(old); }   /* a root with one child disappears */
    else if (t->root->leaf && t->root->n == 0) { free(t->root); t->root = NULL; }
    return r;
}

/* ---- notation, parse, validate ---- */
static void w_str_(const WN *x, char *o, size_t cap)
{
    size_t l = strlen(o);
    if (x->leaf) { snprintf(o + l, cap - l, "("); for (int i = 0; i < x->n; i++) { l = strlen(o); snprintf(o + l, cap - l, i ? " %d" : "%d", x->key[i]); } l = strlen(o); snprintf(o + l, cap - l, ")"); return; }
    snprintf(o + l, cap - l, "[");
    for (int i = 0; i < x->n - 1; i++) { l = strlen(o); snprintf(o + l, cap - l, i ? " %d" : "%d", x->key[i]); }
    l = strlen(o); snprintf(o + l, cap - l, "]{");
    for (int i = 0; i < x->n; i++) { if (i) { l = strlen(o); snprintf(o + l, cap - l, " "); } w_str_(x->ch[i], o, cap); }
    l = strlen(o); snprintf(o + l, cap - l, "}");
}
static const char *w_str(const WN *root) { static char b[16384]; b[0] = 0; if (root) w_str_(root, b, sizeof b); else strcpy(b, "(empty)"); return b; }
static WN *w_parse_(const char **p)
{
    while (isspace((unsigned char)**p)) (*p)++;
    char *e;
    if (**p == '(') {
        (*p)++; WN *x = wn_new(1);
        for (;;) { while (isspace((unsigned char)**p)) (*p)++; if (**p == ')') { (*p)++; break; } x->key[x->n++] = (int)strtol(*p, &e, 10); *p = e; }
        return x;
    }
    assert(**p == '['); (*p)++;
    WN *x = wn_new(0);
    for (;;) { while (isspace((unsigned char)**p)) (*p)++; if (**p == ']') { (*p)++; break; } x->key[x->n++] = (int)strtol(*p, &e, 10); *p = e; }
    int nk = x->n; assert(**p == '{'); (*p)++;
    x->n = nk + 1;
    for (int i = 0; i <= nk; i++) x->ch[i] = w_parse_(p);
    while (isspace((unsigned char)**p)) (*p)++;
    assert(**p == '}'); (*p)++;
    return x;
}
static WN *w_parse(const char *s) { const char *p = s; return w_parse_(&p); }
static int w_height(const WN *x) { return !x ? 0 : x->leaf ? 1 : 1 + w_height(x->ch[0]); }
static int w_count(const WN *x) { if (!x) return 0; if (x->leaf) return x->n; int c = 0; for (int i = 0; i < x->n; i++) c += w_count(x->ch[i]); return c; }
static int w_leaves(const WN *x) { if (!x) return 0; if (x->leaf) return 1; int c = 0; for (int i = 0; i < x->n; i++) c += w_leaves(x->ch[i]); return c; }
/* returns the height, or -1 if a rule is broken */
static int w_valid_(const WTree *t, const WN *x, int root, long lo, long hi)
{
    if (x->leaf) {
        if (x->n > t->L || (!root && x->n < w_min_leaf(t)) || x->n < 1) return -1;
        for (int i = 0; i < x->n; i++) { if (x->key[i] < lo || x->key[i] >= hi) return -1; if (i && x->key[i] <= x->key[i - 1]) return -1; }
        return 1;
    }
    if (x->n > t->M || x->n < (root ? 2 : w_min_children(t))) return -1;
    int h = -2;
    for (int i = 0; i < x->n; i++) {
        long a = i ? x->key[i - 1] : lo, b = i < x->n - 1 ? x->key[i] : hi;
        int c = w_valid_(t, x->ch[i], 0, a, b);
        if (c < 0) return -1;
        if (h == -2) h = c; else if (h != c) return -1;
    }
    return h + 1;
}
static int w_valid(const WTree *t) { return !t->root || w_valid_(t, t->root, 1, -2147483647L - 1, 2147483647L) > 0; }
//<<wb

#ifndef WB_NO_MAIN
#define LEN(a) ((int)(sizeof(a) / sizeof((a)[0])))
int main(void)
{
    /* ---- the Florida example (Sec. 4.7): choose M and L from the block size ---- */
    //>>florida
    {
        int block = 8192, key = 32, branch = 4, record = 256;
        int M = 0; for (int m = 2; (long)(m - 1) * key + (long)m * branch <= block; m++) M = m;   /* (M-1) keys + M branches must fit */
        int L = block / record;
        long N = 10000000, leaves = (N + L / 2 - 1) / (L / 2);                                /* worst case: every leaf half full */
        printf("block %d bytes: M = %d (36M - 32 <= 8192), L = %d records per leaf\n", block, M, L);
        printf("worst case %ld leaves; every inner node branches at least %d ways\n", leaves, M / 2);
        assert(M == 228 && L == 32 && leaves == 625000);
        int level = 1; long width = 1;
        while (width < leaves) { width *= M / 2; level++; }                                    /* root = level 1 ... leaves at `level + ...` */
        printf("so the leaves are on level %d at worst (root = level 1, %d inner levels above the leaves)\n", level, level - 2);
        assert(level <= 4);
        /* a 5-ary tree of 31 nodes has three levels; a perfect binary tree of 31 nodes has five (Fig. 4.62) */
        assert(1 + 5 + 25 == 31 && (1 << 5) - 1 == 31);
    }
    //<<florida

    /* ---- Fig. 4.63 and the three insertions + one deletion of Sec. 4.7 ---- */
    //>>fig463
    {
        const char *f463 =
            "[41 66 87]{[8 18 26 35]{(2 4 6) (8 10 12 14 16) (18 20 22 24) (26 28 30 31 32) (35 36 37 38 39)} "
            "[48 51 54]{(41 42 44 46) (48 49 50) (51 52 53) (54 56 58 59)} "
            "[72 78 83]{(66 68 69 70) (72 73 74 76) (78 79 81) (83 84 85)} "
            "[92 97]{(87 89 90) (92 93 95) (97 98 99)}}";
        WTree t = { w_parse(f463), 5, 5, 0 };
        assert(w_valid(&t) && w_height(t.root) == 3);
        printf("Fig 4.63: %d items in %d leaves, %d levels\n", w_count(t.root), w_leaves(t.root), w_height(t.root));
        assert(w_count(t.root) == 59 && w_leaves(t.root) == 16);
        assert(!strcmp(w_str(t.root), f463));                              /* parse and print are inverses */

        assert(w_insert(&t, 57));                                          /* Fig 4.64: the leaf had room */
        assert(strstr(w_str(t.root), "(54 56 57 58 59)") && w_valid(&t));
        assert(w_insert(&t, 55));                                          /* Fig 4.65: the leaf splits 3 + 3 */
        assert(strstr(w_str(t.root), "[48 51 54 57]{(41 42 44 46) (48 49 50) (51 52 53) (54 55 56) (57 58 59)}") && w_valid(&t));
        printf("after 57 and 55: ...%s...\n", "[48 51 54 57]{... (54 55 56) (57 58 59)}");
        assert(w_insert(&t, 40));                                          /* Fig 4.66: leaf split, then the parent splits */
        assert(!strcmp(w_str(t.root),
            "[26 41 66 87]{[8 18]{(2 4 6) (8 10 12 14 16) (18 20 22 24)} [35 38]{(26 28 30 31 32) (35 36 37) (38 39 40)} "
            "[48 51 54 57]{(41 42 44 46) (48 49 50) (51 52 53) (54 55 56) (57 58 59)} "
            "[72 78 83]{(66 68 69 70) (72 73 74 76) (78 79 81) (83 84 85)} [92 97]{(87 89 90) (92 93 95) (97 98 99)}}"));
        assert(w_valid(&t));
        printf("after 40 (Fig 4.66): the root now has 5 children\n");
        assert(w_delete(&t, 99));                                          /* Fig 4.67: combine two leaves, then adopt a child */
        printf("delete 99 -> %s\n", w_log);
        assert(!strcmp(w_log, "combine-left adopt-left"));
        assert(!strcmp(w_str(t.root),
            "[26 41 66 83]{[8 18]{(2 4 6) (8 10 12 14 16) (18 20 22 24)} [35 38]{(26 28 30 31 32) (35 36 37) (38 39 40)} "
            "[48 51 54 57]{(41 42 44 46) (48 49 50) (51 52 53) (54 55 56) (57 58 59)} "
            "[72 78]{(66 68 69 70) (72 73 74 76) (78 79 81)} [87 92]{(83 84 85) (87 89 90) (92 93 95 97 98)}}"));
        assert(w_valid(&t));
        printf("Fig 4.67: %s\n", w_str(t.root));
        wn_free(t.root);
    }
    //<<fig463

    /* ---- Exercise 4.41(c): adopt before splitting.  Same keys, with and without: how many leaves, how full? ---- */
    //>>adopt
    {
        srand(9);
        enum { N = 4000 };
        static int ks[N]; for (int i = 0; i < N; i++) ks[i] = i;
        for (int i = N - 1; i > 0; i--) { int j = rand() % (i + 1), t = ks[i]; ks[i] = ks[j]; ks[j] = t; }
        for (int adopt = 0; adopt <= 1; adopt++) {
            WTree t = { NULL, 9, 8, adopt };
            for (int i = 0; i < N; i++) { assert(w_insert(&t, ks[i])); assert(w_valid(&t)); }
            int leaves = w_leaves(t.root);
            printf("L = 8, %d random keys, adopt=%d: %d leaves, leaf fill %.0f%%, height %d\n", N, adopt, leaves, 100.0 * N / (leaves * 8.0), w_height(t.root));
            wn_free(t.root);
        }
        WTree a = { NULL, 9, 8, 0 }, b = { NULL, 9, 8, 1 };
        for (int i = 0; i < N; i++) { w_insert(&a, ks[i]); w_insert(&b, ks[i]); }
        assert(w_leaves(b.root) <= w_leaves(a.root));              /* adoption never makes the tree bigger */
        wn_free(a.root); wn_free(b.root);
    }
    //<<adopt

    /* ---- stress: random insert/delete against an array, several (M, L), adopt on and off ---- */
    //>>stress
    srand(31);
    for (int cfg = 0; cfg < 8; cfg++) {
        int Ms[] = { 3, 4, 5, 6, 7, 9, 10, 13 }, Ls[] = { 2, 3, 5, 4, 6, 8, 3, 7 };
        WTree s = { NULL, Ms[cfg], Ls[cfg], cfg & 1 };
        static char in[700]; memset(in, 0, sizeof in);
        int cnt = 0;
        for (int step = 0; step < 6000; step++) {
            int k = rand() % 700;
            if (rand() % 5 < 3) { int ok = w_insert(&s, k); assert(ok == !in[k]); if (ok) { in[k] = 1; cnt++; } }
            else                { int ok = w_delete(&s, k); assert(ok == in[k]); if (ok) { in[k] = 0; cnt--; } }
            assert(w_valid(&s));
            assert(w_search(&s, k) == in[k]);
        }
        assert(w_count(s.root) == cnt);
        wn_free(s.root);
    }
    printf("stress: 8 (M, L) settings x 6000 random steps, all Weiss rules checked after every step\n");
    //<<stress
    puts("wbtree: all checks passed");
    return 0;
}
#endif
