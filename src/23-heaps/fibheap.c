/* fibheap.c — Fibonacci heaps (Thareja 12.3), in C.
 *   node = key, degree, mark, parent, one child, left/right (circular doubly linked lists)    (Fig 12.26/12.27)
 *   Insert (Fig 12.28)  Union (12.30)  Extract-Min (12.31)  Consolidate (12.32)  Link (12.33)
 *   Decrease-Val (12.35)  Cut (12.36)  Cascading-Cut (12.37)  Delete (12.39);  Examples 12.8-12.10, Exercise 30.
 * The forest is written  root(child,child(grandchild)) root ...  starting at min[H]; a marked node has a ' after the key.
 * Author: Mohammed El Sisi. Compile: gcc -std=c99 -Wall -Wextra -O2 fibheap.c -lm */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#pragma GCC diagnostic ignored "-Wunused-function"

//>>fnode
typedef struct FNode { int key, degree, mark; struct FNode *parent, *child, *left, *right; } FNode;
typedef struct { FNode *min; int n; long links, cuts; FNode *start; } FHeap;   /* min[H] and n[H]; start = where a hand-drawn root list begins */

static FNode *fn_new(int key) { FNode *x = calloc(1, sizeof *x); x->key = key; x->left = x->right = x; return x; }

/* put x (a whole circular list of one) next to y in y's list: O(1)  — "concatenate" in the book */
static void list_splice(FNode *y, FNode *x)           /* x's list is merged into y's list */
{
    FNode *yr = y->right, *xl = x->left;
    y->right = x; x->left = y; xl->right = yr; yr->left = xl;
}
static void list_remove(FNode *x) { x->left->right = x->right; x->right->left = x->left; x->left = x->right = x; }

/* Fig 12.28 */
static void fh_insert(FHeap *h, FNode *x)
{
    x->degree = 0; x->parent = x->child = NULL; x->mark = 0; x->left = x->right = x;     /* Steps 1-2 */
    if (h->min) list_splice(h->min, x);                                                  /* Step 3 */
    if (!h->min || x->key < h->min->key) h->min = x;                                     /* Step 4 */
    h->n++;                                                                              /* Step 5 */
}
/* Fig 12.30: the two root lists are joined into one in O(1); h2 is left empty */
static void fh_union(FHeap *h1, FHeap *h2)
{
    if (!h1->min) h1->min = h2->min;
    else if (h2->min) { list_splice(h1->min, h2->min); if (h2->min->key < h1->min->key) h1->min = h2->min; }
    h1->n += h2->n; h1->links += h2->links; h1->cuts += h2->cuts;
    h2->min = NULL; h2->n = 0;
}
//<<fnode

//>>fextract
/* Fig 12.33: remove y from the root list and make it a child of x */
static void fh_link(FHeap *h, FNode *y, FNode *x)
{
    list_remove(y);                                    /* Step 1 */
    y->parent = x;                                     /* Step 2 */
    if (!x->child) x->child = y; else list_splice(x->child, y);
    x->degree++;                                       /* Step 3 */
    y->mark = 0;                                       /* Step 4 */
    h->links++;
}
/* Fig 12.32: until every root has its own degree */
static void fh_consolidate(FHeap *h)
{
    FNode *A[64] = { 0 };                              /* A[d] = the root of degree d seen so far (D(n) <= log_phi n < 64) */
    FNode **roots = malloc(sizeof(FNode *) * (size_t)(h->n + 2)); int nr = 0;   /* at most n roots */
    /* the root list is circular and has no first element; the book simply walks it from the left of its picture.
       h->start remembers that left end when the heap was written down by hand, otherwise we begin at min[H]. */
    FNode *begin = (h->start && !h->start->parent) ? h->start : h->min, *x = begin;
    do { roots[nr++] = x; x = x->right; } while (x != begin);    /* take a snapshot: the list changes while we link */
    h->start = NULL;
    for (int i = 0; i < nr; i++) {
        FNode *ptr = roots[i]; int d = ptr->degree;
        while (A[d]) {
            FNode *temp = A[d];
            if (ptr->key > temp->key) { FNode *t = ptr; ptr = temp; temp = t; }    /* the smaller key becomes the parent */
            fh_link(h, temp, ptr);
            A[d] = NULL; d++;
        }
        A[d] = ptr;
    }
    free(roots);
    h->min = NULL;                                     /* Steps 13-18: rebuild the root list and min[H] from A[] */
    for (int i = 0; i < 64; i++)
        if (A[i]) {
            A[i]->left = A[i]->right = A[i];
            if (!h->min) h->min = A[i]; else { list_splice(h->min, A[i]); if (A[i]->key < h->min->key) h->min = A[i]; }
        }
}
/* Fig 12.31 */
static FNode *fh_extract_min(FHeap *h)
{
    FNode *x = h->min;
    if (!x) return NULL;
    if (x->child) {                                    /* every child becomes a root */
        FNode *c = x->child;
        do { c->parent = NULL; c = c->right; } while (c != x->child);
        list_splice(x, x->child);                      /* the child list joins the root list next to x */
        x->child = NULL; x->degree = 0;
    }
    FNode *r = x->right;
    if (h->start == x) h->start = (x->right == x) ? NULL : x->right;
    list_remove(x);                                    /* remove x from the root list */
    h->n--;
    if (r == x) h->min = NULL;                         /* x was the only node */
    else { h->min = r; fh_consolidate(h); }
    return x;
}
//<<fextract

//>>fdecrease
/* Fig 12.36 and 12.37 */
static void fh_cut(FHeap *h, FNode *x, FNode *par)
{
    if (par->child == x) par->child = (x->right == x) ? NULL : x->right;     /* Step 1 */
    list_remove(x);
    par->degree--;                                     /* Step 2 */
    list_splice(h->min, x);                            /* Step 3 */
    x->parent = NULL;                                  /* Step 4 */
    x->mark = 0;                                       /* Step 5 */
    h->cuts++;
}
static void fh_cascading_cut(FHeap *h, FNode *x)
{
    FNode *par = x->parent;
    if (par) {
        if (!x->mark) x->mark = 1;                     /* the first child lost: just remember it */
        else { fh_cut(h, x, par); fh_cascading_cut(h, par); }    /* the second one: x leaves too, and so on upwards */
    }
}
/* Fig 12.35 */
static void fh_decrease(FHeap *h, FNode *x, int v)
{
    assert(v <= x->key);
    x->key = v;
    FNode *par = x->parent;
    if (par && x->key < par->key) { fh_cut(h, x, par); fh_cascading_cut(h, par); }
    if (x->key < h->min->key) h->min = x;
}
/* Fig 12.39 */
static void fh_delete(FHeap *h, FNode *x)
{
    fh_decrease(h, x, INT_MIN);
    FNode *r = fh_extract_min(h);
    assert(r == x);
    free(r);
}
//<<fdecrease

//>>futil
static int fn_count(const FNode *first)
{
    if (!first) return 0;
    int n = 0; const FNode *x = first;
    do { n += 1 + fn_count(x->child); x = x->right; } while (x != first);
    return n;
}
static int fn_ok(const FNode *first, const FNode *parent)
{
    if (!first) return 1;
    const FNode *x = first; int cnt = 0;
    do {
        if (x->right->left != x || x->left->right != x || x->parent != parent) return 0;
        if (parent && x->key < parent->key) return 0;                       /* heap order */
        int d = 0; if (x->child) { const FNode *c = x->child; do { d++; c = c->right; } while (c != x->child); }
        if (d != x->degree || !fn_ok(x->child, x)) return 0;
        cnt++; x = x->right;
    } while (x != first);
    return cnt > 0;
}
static int fh_valid(const FHeap *h)
{
    if (!h->min) return h->n == 0;
    if (!fn_ok(h->min, NULL) || fn_count(h->min) != h->n) return 0;
    const FNode *x = h->min;
    do { if (x->key < h->min->key) return 0; x = x->right; } while (x != h->min);   /* min[H] really is the smallest root */
    return 1;
}
static int fh_roots(const FHeap *h) { int k = 0; if (h->min) { const FNode *x = h->min; do { k++; x = x->right; } while (x != h->min); } return k; }
static int fh_distinct_degrees(const FHeap *h)       /* true right after a consolidation */
{
    int seen[64] = { 0 }; if (!h->min) return 1;
    const FNode *x = h->min; do { if (seen[x->degree]++) return 0; x = x->right; } while (x != h->min);
    return 1;
}
static void fn_str1(const FNode *x, char *b)
{
    char k[24]; sprintf(k, "%d%s", x->key, x->mark ? "'" : ""); strcat(b, k);
    if (!x->child) return;
    strcat(b, "("); const FNode *c = x->child;
    do { fn_str1(c, b); c = c->right; if (c != x->child) strcat(b, ","); } while (c != x->child);
    strcat(b, ")");
}
static const char *fh_s(const FHeap *h)
{
    static char buf[8][16384]; static int i; char *b = buf[i++ & 7]; b[0] = 0;
    if (!h->min) return b;
    const FNode *x = h->min;
    do { fn_str1(x, b); x = x->right; if (x != h->min) strcat(b, " "); } while (x != h->min);
    return b;
}
static FNode *fn_parse_tree(const char **s)
{
    int k = 0; while (**s >= '0' && **s <= '9') k = k * 10 + *(*s)++ - '0';
    FNode *x = fn_new(k);
    if (**s == '\'') { x->mark = 1; (*s)++; }
    if (**s == '(') {
        (*s)++;
        for (;;) {
            FNode *c = fn_parse_tree(s); c->parent = x; x->degree++;
            if (!x->child) x->child = c; else list_splice(x->child->left, c);   /* append at the end of the circular list */
            if (**s == ',') (*s)++; else break;
        }
        assert(**s == ')'); (*s)++;
    }
    return x;
}
/* roots are given in cyclic order; min[H] is then the smallest root (the first one if there is a tie) */
static FHeap fh_parse(const char *str)
{
    FHeap h = { NULL, 0, 0, 0, NULL }; const char *s = str; FNode *first = NULL;
    while (*s) {
        FNode *t = fn_parse_tree(&s);
        if (!first) first = t; else list_splice(first->left, t);
        while (*s == ' ') s++;
    }
    h.min = first; h.start = first;
    for (FNode *x = first->right; x != first; x = x->right) if (x->key < h.min->key) h.min = x;
    h.n = fn_count(first);
    return h;
}
static FNode *fh_find(FNode *first, int key)
{
    if (!first) return NULL;
    FNode *x = first;
    do { if (x->key == key) return x; FNode *r = fh_find(x->child, key); if (r) return r; x = x->right; } while (x != first);
    return NULL;
}
static void fn_free(FNode *first)
{
    if (!first) return;
    FNode *x = first;
    do { FNode *nx = x->right; fn_free(x->child); free(x); x = nx; } while (x != first);
}
static void fh_free(FHeap *h) { fn_free(h->min); h->min = NULL; h->n = 0; }
//<<futil

#ifndef FH_NO_MAIN
int main(void)
{
    /* ---- Fig 12.26: a small heap; structure checks */
    FHeap h = fh_parse("36 18(19(33)) 4(54,36(41)) 18(32,27(39)) 24(44)");
    printf("Fig 12.29 (before) : %s\n", fh_s(&h));
    assert(fh_valid(&h) && h.min->key == 4 && h.n == 14);
    /* ---- Example 12.8: insert 16 */
    fh_insert(&h, fn_new(16));
    printf("Example 12.8 after : %s\n", fh_s(&h));
    assert(fh_valid(&h) && h.n == 15 && h.min->key == 4);
    fh_free(&h);

    /* ---- Example 12.9: extract the minimum (4), the consolidation of Fig 12.34.
       The root list is read from the book's figure: 36 17 16 | 19 54 36 (the children of 4) | 18 24.
       The text of Fig 12.34 in the copy we have is too scrambled to compare the final picture node by node, so we
       check what the algorithm guarantees: 14 nodes, heap order, and distinct root degrees. */
    h = fh_parse("36 17 16 4(19(33),54,36(41)) 18(32,27(39)) 24(44)");
    FNode *m = fh_extract_min(&h);
    assert(m->key == 4); free(m);
    printf("Example 12.9 result: %s\n", fh_s(&h));
    assert(fh_valid(&h) && h.n == 14 && fh_distinct_degrees(&h) && h.min->key == 16);
    assert(h.links == 8 - fh_roots(&h));                              /* 8 roots, every link removes exactly one of them */
    fh_free(&h);

    /* ---- Example 12.10: decrease 39 to 9. 27 and 24 were already marked, so the cut cascades up to the root 19. */
    h = fh_parse("19(24'(27'(39))) 17(18(32),36(38)) 16(19,18) 36(41)");
    assert(fh_valid(&h) && h.min->key == 16 && h.n == 14);
    fh_decrease(&h, fh_find(h.min, 39), 9);
    printf("Example 12.10 after : %s   (cuts: %ld)\n", fh_s(&h), h.cuts);
    assert(fh_valid(&h) && h.cuts == 3 && h.min->key == 9);           /* 39 -> root, then 27, then 24 */
    { const FNode *x = h.min; int roots[16], k = 0; do { roots[k++] = x->key; x = x->right; } while (x != h.min);
      int has[200] = { 0 }; for (int i = 0; i < k; i++) has[roots[i]] = 1;
      assert(k == 7 && has[9] && has[27] && has[24] && has[19] && has[17] && has[16] && has[36]); }   /* the book's step 5 */
    fh_free(&h);

    /* ---- Exercise 30: decrease 33 to 9, insert 5, delete 19 */
    h = fh_parse("17(24(27(39),44),18) 16(36,19(33),18) 36(41)");
    printf("Exercise 30 start   : %s\n", fh_s(&h));
    fh_decrease(&h, fh_find(h.min, 33), 9);
    printf("  decrease 33 -> 9  : %s\n", fh_s(&h));
    assert(fh_valid(&h) && h.min->key == 9);
    fh_insert(&h, fn_new(5));
    printf("  insert 5          : %s\n", fh_s(&h));
    assert(fh_valid(&h) && h.min->key == 5);
    fh_delete(&h, fh_find(h.min, 19));
    printf("  delete 19         : %s\n", fh_s(&h));
    assert(fh_valid(&h) && !fh_find(h.min, 19) && h.n == 13);        /* 13 nodes, +1 inserted, -1 deleted */
    fh_free(&h);

    /* ---- union (Fig 12.30) in O(1): the root lists are simply joined */
    { FHeap p = fh_parse("12 7(9) 15"), q = fh_parse("3 20(22)");
      long before = p.links; fh_union(&p, &q);
      assert(fh_valid(&p) && p.n == 7 && p.min->key == 3 && !q.min && p.links == before);   /* no link was made */
      FNode *x = fh_extract_min(&p); assert(x->key == 3); free(x);
      assert(fh_valid(&p) && fh_distinct_degrees(&p)); fh_free(&p); }

    /* ---- stress: random operations against a plain multiset kept in an array */
    srand(23);
    for (int round = 0; round < 100; round++) {
        FHeap H = { NULL, 0, 0, 0, NULL }; FNode *nodes[300]; int live[300], cnt = 0; int val[300];
        for (int step = 0; step < 400; step++) {
            int op = rand() % 10;
            if (op < 4 || cnt == 0) {                  /* insert */
                if (cnt < 300) { FNode *x = fn_new(rand() % 1000); nodes[cnt] = x; val[cnt] = x->key; live[cnt] = 1; fh_insert(&H, x); cnt++; }
            } else if (op < 7) {                       /* decrease a random live key */
                int i = rand() % cnt; if (!live[i]) continue;
                int nv = nodes[i]->key - rand() % 100; fh_decrease(&H, nodes[i], nv); val[i] = nv;
            } else if (op < 8) {                       /* delete a random live node */
                int i = rand() % cnt; if (!live[i]) continue;
                FNode *x = nodes[i]; live[i] = 0; fh_decrease(&H, x, INT_MIN); FNode *r = fh_extract_min(&H); assert(r == x); free(r);
            } else {                                   /* extract-min must be the smallest live value */
                int best = INT_MAX; for (int i = 0; i < cnt; i++) if (live[i] && val[i] < best) best = val[i];
                FNode *r = fh_extract_min(&H); if (best == INT_MAX) { assert(!r); continue; }
                assert(r && r->key == best);
                for (int i = 0; i < cnt; i++) if (nodes[i] == r) live[i] = 0;
                free(r);
                assert(fh_distinct_degrees(&H));
            }
            assert(fh_valid(&H));
        }
        fh_free(&H);
    }
    /* ---- the Fibonacci bound: a node of degree k roots a subtree of at least phi^k nodes, so D(n) <= log_phi n */
    { FHeap H = { NULL, 0, 0, 0, NULL }; FNode *v[2000];
      for (int i = 0; i < 2000; i++) { v[i] = fn_new(rand() % 100000); fh_insert(&H, v[i]); }
      for (int i = 0; i < 1000; i++) { FNode *r = fh_extract_min(&H); free(r); for (int j = 0; j < 2000; j++) if (v[j] == r) v[j] = NULL; }
      int maxdeg = 0;
      for (int j = 0; j < 2000; j++) if (v[j]) { int sz = 1 + fn_count(v[j]->child); if (v[j]->degree > maxdeg) maxdeg = v[j]->degree;
          assert(sz >= (int)pow(1.6180339887, v[j]->degree) - 1); }
      printf("2000 inserts, 1000 extract-min: %d roots, largest degree %d (log_phi 1000 = %.1f)\n", fh_roots(&H), maxdeg, log(1000.0) / log(1.6180339887));
      assert(maxdeg <= (int)(log(1000.0) / log(1.6180339887)) + 1);
      fh_free(&H); }
    printf("fibheap: all checks passed\n");
    return 0;
}
#endif
