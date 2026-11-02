/* binomial.c — binomial heaps (Thareja 12.2) and binomial queues (Weiss 6.8) on the same node layout.
 *   node = value + parent + leftmost child + right sibling + degree          (Thareja Fig 12.14, Weiss Fig 6.51/6.52)
 *   Thareja : Min_Binomial-Heap (12.15), Link (12.17), Union (12.18), Insert (12.20), Min-Extract (12.21),
 *             Decrease_Val (12.23), Delete_Node (12.24); Examples 12.4-12.7
 *   Weiss   : merge as binary addition with a carry (Fig 6.55), deleteMin (6.56), Exercises 6.30-6.36
 * A forest is written  root(child,child(grandchild)) root ...  with the LEFTMOST child (the highest degree) first.
 * Author: Mohammed El Sisi. Compile: gcc -std=c99 -Wall -Wextra -O2 binomial.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#pragma GCC diagnostic ignored "-Wunused-function"

//>>bnode
typedef struct BNode { int val, degree; struct BNode *parent, *child, *sibling; } BNode;
static long bn_links;                                  /* number of Link calls = number of key comparisons that built trees */
static BNode *bn_node(int val) { BNode *x = calloc(1, sizeof *x); x->val = val; return x; }

/* Thareja Fig 12.15: the minimum is one of the roots, so look at all of them: O(log n) */
static BNode *bn_min(BNode *head)
{
    BNode *y = NULL, *x = head; int min = INT_MAX;     /* Step 1 */
    while (x) {                                        /* Steps 2-4 */
        if (x->val < min) { min = x->val; y = x; }
        x = x->sibling;
    }
    return y;                                          /* Step 5 */
}

/* Thareja Fig 12.17: make Y (a B(i-1)) the leftmost child of Z (another B(i-1)): the result is a B(i) */
static void bn_link(BNode *y, BNode *z)
{
    y->parent = z;                                     /* Step 1 */
    y->sibling = z->child;                             /* Step 2 */
    z->child = y;                                      /* Step 3 */
    z->degree++;                                       /* Step 4 */
    bn_links++;
}
//<<bnode

//>>bunion
/* the first half of Union: merge the two root lists into one list sorted by degree (like merging two sorted lists) */
static BNode *bn_merge_roots(BNode *a, BNode *b)
{
    BNode dummy = { 0 }, *t = &dummy;
    while (a && b) {
        if (a->degree <= b->degree) { t->sibling = a; a = a->sibling; } else { t->sibling = b; b = b->sibling; }
        t = t->sibling;
    }
    t->sibling = a ? a : b;
    return dummy.sibling;
}

/* Thareja Fig 12.18. PTR is the root being examined, NEXT the one after it, PREV the one before. */
static BNode *bn_union(BNode *h1, BNode *h2)
{
    BNode *head = bn_merge_roots(h1, h2);              /* Steps 1-3 */
    if (!head) return NULL;                            /* Step 4 */
    BNode *prev = NULL, *ptr = head, *next = ptr->sibling;   /* Step 5 */
    while (next) {                                     /* Step 6 */
        if (ptr->degree != next->degree ||
            (next->sibling && next->sibling->degree == ptr->degree)) {   /* Step 7: nothing to link here, move on */
            prev = ptr; ptr = next;
        } else if (ptr->val <= next->val) {            /* two equal degrees: the smaller key stays the root */
            ptr->sibling = next->sibling;
            bn_link(next, ptr);
        } else {
            if (!prev) head = next; else prev->sibling = next;
            bn_link(ptr, next);
            ptr = next;
        }
        next = ptr->sibling;
    }
    return head;                                       /* Step 8 */
}
static BNode *bn_insert(BNode *head, BNode *x)         /* Fig 12.20: a one-node heap united with H */
{
    x->parent = x->child = x->sibling = NULL; x->degree = 0;
    return bn_union(head, x);
}
//<<bunion

//>>bextract
/* Thareja Fig 12.21: remove the smallest root R; R's children, reversed, become a heap of their own; unite. */
static BNode *bn_extract_min(BNode *head, BNode **out)
{
    BNode *r = bn_min(head);                           /* Step 1 */
    assert(r);
    if (head == r) head = r->sibling;                  /* Step 2: unlink R from the root list */
    else { BNode *p = head; while (p->sibling != r) p = p->sibling; p->sibling = r->sibling; }
    BNode *rev = NULL, *c = r->child;                  /* Steps 3-5: reverse the child list (degrees now rise) */
    while (c) { BNode *nx = c->sibling; c->sibling = rev; c->parent = NULL; rev = c; c = nx; }
    r->child = NULL; r->sibling = NULL; r->degree = 0;
    *out = r;
    return bn_union(head, rev);                        /* Step 6 */
}

/* Thareja Fig 12.23: lower a value and let it rise by swapping values with its parent.
   (Swapping the VALUES means a node pointer no longer names "the same item"; Weiss/CLRS note this too.) */
static void bn_decrease(BNode *x, int k)
{
    assert(k <= x->val);
    x->val = k;
    BNode *ptr = x, *par = ptr->parent;
    while (par && ptr->val < par->val) { int t = ptr->val; ptr->val = par->val; par->val = t; ptr = par; par = ptr->parent; }
}
/* Fig 12.24: delete = decrease to minus infinity, then extract the minimum */
static BNode *bn_delete(BNode *head, BNode *x)
{
    BNode *out;
    bn_decrease(x, INT_MIN);
    head = bn_extract_min(head, &out);
    free(out);
    return head;
}
//<<bextract

//>>butil
static int bn_count(const BNode *x) { int n = 0; for (; x; x = x->sibling) n += 1 + bn_count(x->child); return n; }
static BNode *bn_find(BNode *x, int v)
{
    for (; x; x = x->sibling) { if (x->val == v) return x; BNode *r = bn_find(x->child, v); if (r) return r; }
    return NULL;
}
/* a tree of degree k is binomial: its children have degrees k-1, k-2, ..., 0 (left to right), each is binomial, and it is heap ordered */
static int bn_tree_ok(const BNode *x, const BNode *parent)
{
    if (x->parent != parent) return 0;
    int d = x->degree;
    for (const BNode *c = x->child; c; c = c->sibling) {
        d--;
        if (c->degree != d || c->val < x->val || !bn_tree_ok(c, x)) return 0;
    }
    return d == 0;
}
/* a heap is valid when the root degrees strictly increase and every tree is binomial: n's binary representation */
static int bn_valid(const BNode *head)
{
    int last = -1, n = 0;
    for (const BNode *r = head; r; r = r->sibling) {
        if (r->degree <= last || !bn_tree_ok(r, NULL)) return 0;
        last = r->degree; n += 1 << r->degree;
    }
    return n == bn_count(head);
}
static void bn_free(BNode *x) { while (x) { BNode *s = x->sibling; bn_free(x->child); free(x); x = s; } }
static void bn_str1(const BNode *x, char *b)
{
    char k[16]; sprintf(k, "%d", x->val); strcat(b, k);
    if (!x->child) return;
    strcat(b, "(");
    for (const BNode *c = x->child; c; c = c->sibling) { bn_str1(c, b); if (c->sibling) strcat(b, ","); }
    strcat(b, ")");
}
static const char *bn_s(const BNode *head)
{
    static char buf[8][16384]; static int i; char *b = buf[i++ & 7]; b[0] = 0;
    for (const BNode *r = head; r; r = r->sibling) { bn_str1(r, b); if (r->sibling) strcat(b, " "); }
    return b;
}
/* parse a forest in the notation above; the tree must really be binomial (checked by bn_valid afterwards) */
static BNode *bn_parse_tree(const char **s)
{
    int k = 0; while (**s >= '0' && **s <= '9') k = k * 10 + *(*s)++ - '0';
    BNode *x = bn_node(k), **tail = &x->child;
    if (**s == '(') {
        (*s)++;
        for (;;) {
            BNode *c = bn_parse_tree(s); c->parent = x; *tail = c; tail = &c->sibling; x->degree++;
            if (**s == ',') (*s)++; else break;
        }
        assert(**s == ')'); (*s)++;
    }
    return x;
}
static BNode *bn_parse(const char *str)
{
    const char *s = str; BNode *head = NULL, **tail = &head;
    while (*s) { BNode *t = bn_parse_tree(&s); *tail = t; tail = &t->sibling; while (*s == ' ') s++; }
    return head;
}
//<<butil

//>>queue
/* ================= Weiss 6.8: a binomial queue as an array of roots, trees[k] = the B(k) or NULL =================
   merging two queues is adding two binary numbers: trees[k] of H1, trees[k] of H2 and the carry give 0..3 trees of size 2^k */
#define BQ_MAX 32
typedef struct { BNode *tree[BQ_MAX]; int n; } BQueue;
static long bq_steps;                                  /* loop iterations of the merge, for Exercise 6.35 */

static BNode *bq_combine(BNode *t1, BNode *t2)         /* Fig 6.54: two B(k) become one B(k+1) */
{
    if (t2->val < t1->val) { BNode *t = t1; t1 = t2; t2 = t; }
    t2->sibling = t1->child; t2->parent = t1; t1->child = t2; t1->degree++;
    bn_links++;
    return t1;
}
static void bq_merge(BQueue *h1, BQueue *h2, int early_stop)    /* Fig 6.55; h2 is emptied */
{
    BNode *carry = NULL;
    h1->n += h2->n; h2->n = 0;
    for (int k = 0; k < BQ_MAX; k++) {
        /* Exercise 6.35(a): once H2 has no trees left (above this k) and there is no carry, the rest of H1 is already right */
        if (early_stop && !carry) { int more = 0; for (int j = k; j < BQ_MAX; j++) if (h2->tree[j]) { more = 1; break; } if (!more) return; }
        bq_steps++;
        BNode *t1 = h1->tree[k], *t2 = h2->tree[k];
        int cases = (t1 ? 1 : 0) + (t2 ? 2 : 0) + (carry ? 4 : 0);
        switch (cases) {
        case 0: case 1: break;                         /* no trees, or only H1's: nothing changes */
        case 2: h1->tree[k] = t2; h2->tree[k] = NULL; break;
        case 4: h1->tree[k] = carry; carry = NULL; break;
        case 3: carry = bq_combine(t1, t2); h1->tree[k] = h2->tree[k] = NULL; break;
        case 5: carry = bq_combine(t1, carry); h1->tree[k] = NULL; break;
        case 6: carry = bq_combine(t2, carry); h2->tree[k] = NULL; break;
        case 7: h1->tree[k] = carry; carry = bq_combine(t1, t2); h2->tree[k] = NULL; break;
        }
    }
    assert(!carry);
}
static void bq_insert(BQueue *h, int x)                /* Fig 6.55 used as insert: merge with a one-node queue */
{
    BQueue one = { { 0 }, 1 }; one.tree[0] = bn_node(x);
    bq_merge(h, &one, 1);
}
/* Exercise 6.34: insert WITHOUT calling merge. Find the first empty slot j, combine the new node with trees 0..j-1. */
static void bq_insert_direct(BQueue *h, int x)
{
    BNode *carry = bn_node(x); int k = 0;
    while (h->tree[k]) { carry = bq_combine(h->tree[k], carry); h->tree[k] = NULL; k++; }
    h->tree[k] = carry; h->n++;
}
static int bq_find_min(const BQueue *h)
{
    int best = -1;
    for (int k = 0; k < BQ_MAX; k++) if (h->tree[k] && (best < 0 || h->tree[k]->val < h->tree[best]->val)) best = k;
    assert(best >= 0);
    return best;
}
static int bq_delete_min(BQueue *h)                    /* Fig 6.56 */
{
    int k = bq_find_min(h), min = h->tree[k]->val;
    BNode *old = h->tree[k], *c = old->child;
    BQueue del = { { 0 }, (1 << k) - 1 };              /* B(k) minus its root = B(0)..B(k-1) */
    for (int j = k - 1; j >= 0; j--) { BNode *nx = c->sibling; del.tree[j] = c; c->sibling = NULL; c->parent = NULL; c = nx; }
    free(old); h->tree[k] = NULL; h->n -= 1 << k;
    bq_merge(h, &del, 1);
    h->n += 0;
    return min;
}
static void bq_free(BQueue *h) { for (int k = 0; k < BQ_MAX; k++) { bn_free(h->tree[k]); h->tree[k] = NULL; } }
static int bq_valid(const BQueue *h)
{
    int n = 0;
    for (int k = 0; k < BQ_MAX; k++) if (h->tree[k]) { if (h->tree[k]->degree != k || !bn_tree_ok(h->tree[k], NULL) || h->tree[k]->sibling) return 0; n += 1 << k; }
    return n == h->n;
}
static const char *bq_s(const BQueue *h)
{
    static char buf[4][65536]; static int i; char *b = buf[i++ & 3]; b[0] = 0;
    for (int k = 0; k < BQ_MAX; k++) if (h->tree[k]) { if (b[0]) strcat(b, " "); bn_str1(h->tree[k], b); }
    return b;
}
//<<queue

#ifndef BN_NO_MAIN
int main(void)
{
    /* ---- Fig 12.13: the binomial heap used by Thareja's linked representation (Fig 12.14) */
    BNode *h = bn_parse("7(11(16),27) 9(10(12(19),18),14(21),36)");
    printf("Fig 12.13: %s\n", bn_s(h));
    assert(bn_valid(h) && bn_count(h) == 12 && h->degree == 2 && h->sibling->degree == 3);   /* 12 = 1100 in binary: B2 + B3 */
    assert(h->child->val == 11 && h->child->degree == 1 && h->child->sibling->val == 27);   /* Fig 12.14: 7 -> 11 -> 27 */
    assert(bn_min(h)->val == 7);                       /* Example 12.4 style: the minimum is a root */
    bn_free(h);

    /* ---- inserting 1..7 (Weiss Fig 6.39-6.45) with Thareja's heap: 1 B0, then B1, then B1+B0, ... */
    BNode *w = NULL; long l0 = bn_links;
    for (int k = 1; k <= 7; k++) { w = bn_insert(w, bn_node(k)); printf("insert %d: %s\n", k, bn_s(w)); assert(bn_valid(w)); }
    assert(!strcmp(bn_s(w), "7 5(6) 1(3(4),2)"));
    printf("7 inserts used %ld links (7 - popcount(7) = 4)\n", bn_links - l0);
    assert(bn_links - l0 == 4);
    /* ---- Thareja 12.2.2 extract-min, Example 12.6 style */
    BNode *out; w = bn_extract_min(w, &out); assert(out->val == 1); free(out);
    printf("after extract-min: %s\n", bn_s(w)); assert(bn_valid(w) && bn_count(w) == 6);
    /* ---- Example 12.7 style: delete a given node */
    w = bn_delete(w, bn_find(w, 5)); assert(bn_valid(w) && bn_count(w) == 5 && !bn_find(w, 5));
    printf("after delete 5   : %s\n", bn_s(w));
    bn_decrease(bn_find(w, 7), 0); assert(bn_min(w)->val == 0); assert(bn_valid(w));
    bn_free(w);

    /* ---- stress against a sorted array */
    srand(5);
    for (int round = 0; round < 200; round++) {
        BNode *H = NULL; int n = rand() % 70, v[80];
        for (int i = 0; i < n; i++) { v[i] = rand() % 1000; H = bn_insert(H, bn_node(v[i])); }
        assert(bn_valid(H) && bn_count(H) == n);
        int prev = -1; BNode *x;
        for (int i = 0; i < n; i++) { H = bn_extract_min(H, &x); assert(x->val >= prev); prev = x->val; free(x); assert(bn_valid(H)); }
        assert(!H);
    }

    /* ---- Weiss queue */
    BQueue q = { { 0 }, 0 };
    for (int k = 1; k <= 7; k++) { bq_insert(&q, k); assert(bq_valid(&q)); }
    printf("Weiss queue after inserting 1..7: %s\n", bq_s(&q));
    int prev = 0; for (int k = 0; k < 7; k++) { int m = bq_delete_min(&q); assert(m == prev + 1); prev = m; assert(bq_valid(&q)); }
    printf("binomial: all checks passed\n");
    return 0;
}
#endif
