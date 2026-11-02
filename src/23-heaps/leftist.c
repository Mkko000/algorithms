/* leftist.c — leftist heaps and skew heaps (Weiss 6.6 and 6.7), written in C.
 *   Fig 6.25-6.30 : declarations, merge (recursive, Fig 6.27) and the right-path version (Fig 6.28), insert, deleteMin
 *   Fig 6.21-6.24 : H1 and H2 and the merge worked out in the text
 *   Fig 6.31-6.33 : the same two heaps as skew heaps
 *   Exercises 6.19-6.29 and 6.40 are solved in exercises/ex23_02_weiss_heaps.c with the code below.
 * A tree is written in one line:  key(left,right)  with "." for an empty left child, a bare key for a leaf,
 * and the right child left out when it is empty:  21(23)  means 21 with a left child 23 only.
 * Author: Mohammed El Sisi. Compile: gcc -std=c99 -Wall -Wextra -O2 leftist.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#pragma GCC diagnostic ignored "-Wunused-function"   /* the exercise file uses the rest */

//>>lnode
typedef struct LNode { int key, npl; struct LNode *left, *right; } LNode;     /* npl = null path length; npl(NULL) = -1 */
static long lh_steps;                                  /* how many nodes the merges visited (cost counter) */
static int npl(const LNode *t) { return t ? t->npl : -1; }
static LNode *lh_node(int key) { LNode *t = malloc(sizeof *t); t->key = key; t->npl = 0; t->left = t->right = NULL; return t; }
//<<lnode

//>>lmerge
/* Weiss Fig 6.26/6.27. merge1 handles the case h1->key <= h2->key. */
static LNode *lh_merge(LNode *h1, LNode *h2);
static LNode *lh_merge1(LNode *h1, LNode *h2)
{
    if (h1->left == NULL)                              /* a single node: h2 simply becomes its left child */
        h1->left = h2;
    else {
        h1->right = lh_merge(h1->right, h2);           /* the recursion goes down the RIGHT path only */
        if (npl(h1->left) < npl(h1->right)) { LNode *t = h1->left; h1->left = h1->right; h1->right = t; }  /* leftist rule */
        h1->npl = npl(h1->right) + 1;
    }
    return h1;
}
static LNode *lh_merge(LNode *h1, LNode *h2)
{
    lh_steps++;
    if (!h1) return h2;
    if (!h2) return h1;
    return h1->key < h2->key ? lh_merge1(h1, h2) : lh_merge1(h2, h1);
}
static LNode *lh_insert(LNode *h, int x) { return lh_merge(lh_node(x), h); }
static LNode *lh_delete_min(LNode *h, int *out)        /* Fig 6.30: the root goes, its two subheaps are merged */
{
    LNode *l = h->left, *r = h->right; *out = h->key; free(h);
    return lh_merge(l, r);
}
//<<lmerge

//>>literative
/* Weiss Fig 6.28 and the text after it: do it in two passes over the right path, with no recursion.
   Pass 1 merges the two right paths into one sorted path; pass 2 walks back up fixing npl and swapping children. */
static LNode *lh_merge_iter(LNode *a, LNode *b)
{
    LNode *path[128]; int np = 0;
    while (a && b) {                                   /* pass 1: detach nodes from the right paths in key order */
        LNode **w = a->key < b->key ? &a : &b;
        LNode *n = *w; *w = n->right; n->right = NULL;
        path[np++] = n;
    }
    LNode *rest = a ? a : b;                           /* what is left of one path is already in order */
    while (rest) { LNode *n = rest; rest = n->right; n->right = NULL; path[np++] = n; }
    LNode *below = NULL;                               /* pass 2: from the bottom of the merged path up */
    for (int i = np - 1; i >= 0; i--) {
        LNode *n = path[i]; n->right = below;
        if (npl(n->left) < npl(n->right)) { LNode *t = n->left; n->left = n->right; n->right = t; }
        n->npl = npl(n->right) + 1;
        below = n;
    }
    return below;
}
//<<literative

//>>skew
/* Weiss 6.7: no npl at all. After merging into the right subtree, ALWAYS swap the two children
   (the book swaps every node on the right path except the last one; this recursive form is the same thing). */
static LNode *sk_merge(LNode *h1, LNode *h2)
{
    lh_steps++;
    if (!h1) return h2;
    if (!h2) return h1;
    if (h2->key < h1->key) { LNode *t = h1; h1 = h2; h2 = t; }
    LNode *r = sk_merge(h1->right, h2);
    h1->right = h1->left; h1->left = r;                /* swap, with the merged heap landing on the left */
    return h1;
}
static LNode *sk_insert(LNode *h, int x) { return sk_merge(lh_node(x), h); }
static LNode *sk_delete_min(LNode *h, int *out)
{
    LNode *l = h->left, *r = h->right; *out = h->key; free(h);
    return sk_merge(l, r);
}
//<<skew

//>>lutil
static int lh_size(const LNode *t) { return t ? 1 + lh_size(t->left) + lh_size(t->right) : 0; }
static int lh_height(const LNode *t)
{
    if (!t) return 0;
    int l = lh_height(t->left), r = lh_height(t->right);
    return 1 + (l > r ? l : r);
}
static int lh_heap_ordered(const LNode *t)
{
    if (!t) return 1;
    if (t->left && t->left->key < t->key) return 0;
    if (t->right && t->right->key < t->key) return 0;
    return lh_heap_ordered(t->left) && lh_heap_ordered(t->right);
}
/* the leftist property: npl(left) >= npl(right) and npl is stored correctly */
static int lh_is_leftist(const LNode *t)
{
    if (!t) return 1;
    return npl(t->left) >= npl(t->right) && t->npl == npl(t->right) + 1 && lh_is_leftist(t->left) && lh_is_leftist(t->right);
}
/* length of the right path: for a leftist heap of N nodes it is at most floor(log2(N+1)) */
static int lh_right_path(const LNode *t) { int k = 0; for (; t; t = t->right) k++; return k; }
static int lh_perfect(const LNode *t)                  /* every level full */
{
    if (!t) return 1;
    return lh_size(t->left) == lh_size(t->right) && lh_perfect(t->left) && lh_perfect(t->right);
}
static void lh_free(LNode *t) { if (t) { lh_free(t->left); lh_free(t->right); free(t); } }
static void lh_str(const LNode *t, char *buf)
{
    if (!t) { strcat(buf, "."); return; }
    char k[16]; sprintf(k, "%d", t->key); strcat(buf, k);
    if (!t->left && !t->right) return;
    strcat(buf, "("); lh_str(t->left, buf);
    if (t->right) { strcat(buf, ","); lh_str(t->right, buf); }       /* a missing right child is simply left out */
    strcat(buf, ")");
}
static const char *lh_s(const LNode *t) { static char b[8][16384]; static int i; char *p = b[i++ & 7]; p[0] = 0; lh_str(t, p); return p; }
/* parse the same notation; npl is recomputed so the tree can be used as input to merge */
static LNode *lh_parse_(const char **s)
{
    if (**s == '.') { (*s)++; return NULL; }
    int k = 0; while (**s >= '0' && **s <= '9') k = k * 10 + *(*s)++ - '0';
    LNode *t = lh_node(k);
    if (**s == '(') {
        (*s)++; t->left = lh_parse_(s);
        if (**s == ',') { (*s)++; t->right = lh_parse_(s); }
        assert(**s == ')'); (*s)++;
        t->npl = npl(t->right) + 1;
    }
    return t;
}
static LNode *lh_parse(const char *s) { const char *p = s; LNode *t = lh_parse_(&p); assert(!*p); return t; }
//<<lutil

#ifndef LH_NO_MAIN
int main(void)
{
    /* ---- Figures 6.21 - 6.24: leftist merge of H1 and H2 */
    LNode *h1 = lh_parse("3(10(21(23),14),8(17(26)))");
    LNode *h2 = lh_parse("6(12(18(33),24),7(37,18))");
    assert(lh_is_leftist(h1) && lh_is_leftist(h2) && lh_heap_ordered(h1) && lh_heap_ordered(h2));
    printf("H1 = %s\nH2 = %s\n", lh_s(h1), lh_s(h2));
    /* Fig 6.22: H2 merged with H1's right subheap (the recursive step of the book's explanation) */
    LNode *h1r = h1->right; h1->right = NULL;
    LNode *m = lh_merge(h2, h1r);
    printf("Fig 6.22 merge(H2, right of H1) = %s\n", lh_s(m));
    assert(!strcmp(lh_s(m), "6(12(18(33),24),7(8(17(26),18),37))"));
    h1->right = m;                                     /* Fig 6.23: made the right child of H1's root */
    printf("Fig 6.23 before the swap       = %s\n", lh_s(h1));
    h1->npl = 1;
    if (npl(h1->left) < npl(h1->right)) { LNode *t = h1->left; h1->left = h1->right; h1->right = t; }   /* Fig 6.24 */
    printf("Fig 6.24 after the swap        = %s\n", lh_s(h1));
    assert(lh_heap_ordered(h1) && lh_size(h1) == 16);
    lh_free(h1);

    /* the whole merge in one call, recursive and iterative, must agree */
    LNode *a = lh_parse("3(10(21(23),14),8(17(26)))"), *b = lh_parse("6(12(18(33),24),7(37,18))");
    LNode *c = lh_parse("3(10(21(23),14),8(17(26)))"), *d = lh_parse("6(12(18(33),24),7(37,18))");
    LNode *r1 = lh_merge(a, b), *r2 = lh_merge_iter(c, d);
    printf("leftist merge, recursive = %s\n", lh_s(r1));
    assert(!strcmp(lh_s(r1), lh_s(r2)) && lh_is_leftist(r1));
    lh_free(r1); lh_free(r2);

    /* ---- Figures 6.31 - 6.33: the same two heaps as skew heaps */
    LNode *s1 = lh_parse("3(10(21(23),14),8(17(26)))"), *s2 = lh_parse("6(12(18(33),24),7(37,18))");
    LNode *s1r = s1->right; s1->right = NULL;
    LNode *sm = sk_merge(s2, s1r);                     /* Fig 6.32 */
    printf("Fig 6.32 skew merge(H2, right of H1) = %s\n", lh_s(sm));
    s1->right = sm;
    LNode *t = s1->left; s1->left = s1->right; s1->right = t;     /* Fig 6.33: swap at the root too */
    printf("Fig 6.33 skew merge result           = %s\n", lh_s(s1));
    LNode *s3 = lh_parse("3(10(21(23),14),8(17(26)))"), *s4 = lh_parse("6(12(18(33),24),7(37,18))");
    LNode *sr = sk_merge(s3, s4);
    assert(!strcmp(lh_s(sr), lh_s(s1)) && lh_heap_ordered(sr) && lh_size(sr) == 16);
    lh_free(s1); lh_free(sr);

    /* ---- a stress test against sorting, for both structures and both merge routines */
    srand(11);
    for (int round = 0; round < 300; round++) {
        LNode *L = NULL, *S = NULL; int n = rand() % 80, v[80];
        for (int i = 0; i < n; i++) { v[i] = rand() % 50; L = lh_insert(L, v[i]); S = sk_insert(S, v[i]); }
        assert(lh_is_leftist(L) && lh_heap_ordered(L) && lh_size(L) == n);
        assert(lh_heap_ordered(S) && lh_size(S) == n);
        assert(lh_right_path(L) <= (int)(n ? 1 + (int)__builtin_log2(n + 1.0) : 0));
        int prevL = -1, prevS = -1, x;
        for (int i = 0; i < n; i++) {
            L = lh_delete_min(L, &x); assert(x >= prevL); prevL = x;
            S = sk_delete_min(S, &x); assert(x >= prevS); prevS = x;
        }
        assert(!L && !S);
    }
    printf("leftist: all checks passed\n");
    return 0;
}
#endif
