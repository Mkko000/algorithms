/* ============================================================
 *  bst_analysis.c  —  how deep is a BST, really?  (Weiss 4.3.6) + lazy deletion (Weiss ex. 4.16)
 *
 *  Part A  average case.  If the n! insertion orders are equally likely, the AVERAGE node depth is O(log n)
 *          (Weiss proves D(n) = O(log n) with the recurrence  D(N) = (2/N) * sum D(i) + N-1  for the
 *          internal path length). The constant is 2 ln 2 ~= 1.39 depth levels per doubling:  avg depth ~ 1.39 log2 n.
 *          We MEASURE it, and compare with sorted input (depth ~ n/2) and with the AVL bound.
 *  Part B  lazy deletion: mark a node deleted instead of unlinking it. Cheap, keeps the shape stable,
 *          and re-inserting a deleted key just clears the mark. The tree must be rebuilt (purged) now and then.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Weiss 4.3.6 (average-case analysis) and Exercise 4.16 (lazy deletion)
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 bst_analysis.c -o bst_analysis -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

typedef struct LN { int key, dead; struct LN *l, *r; } LN;

//>>depth
static LN *ins(LN *t, int k, long *depth_acc, int d)     /* returns new root; adds the depth of a NEW node to *depth_acc */
{
    if (!t) { LN *n = calloc(1, sizeof *n); n->key = k; if (depth_acc) *depth_acc += d; return n; }
    if (k < t->key) t->l = ins(t->l, k, depth_acc, d + 1);
    else if (k > t->key) t->r = ins(t->r, k, depth_acc, d + 1);
    else t->dead = 0;                                    /* lazy deletion: inserting a dead key revives it */
    return t;
}
static void lfree(LN *t) { if (t) { lfree(t->l); lfree(t->r); free(t); } }
static long internal_path(const LN *t, int d) { return t ? d + internal_path(t->l, d + 1) + internal_path(t->r, d + 1) : 0; }
static int lcount(const LN *t) { return t ? 1 + lcount(t->l) + lcount(t->r) : 0; }
static int lheight(const LN *t) { if (!t) return 0; int a = lheight(t->l), b = lheight(t->r); return 1 + (a > b ? a : b); }
//<<depth

//>>lazy
static int lcontains(const LN *t, int k)
{
    while (t && t->key != k) t = k < t->key ? t->l : t->r;
    return t && !t->dead;                                /* found AND not marked deleted */
}
static int ldelete(LN *t, int k)                         /* O(depth), no relinking, no free */
{
    while (t && t->key != k) t = k < t->key ? t->l : t->r;
    if (!t || t->dead) return 0;
    t->dead = 1; return 1;
}
static int llive(const LN *t) { return t ? (!t->dead) + llive(t->l) + llive(t->r) : 0; }
static void collect(const LN *t, int *a, int *n) { if (t) { collect(t->l, a, n); if (!t->dead) a[(*n)++] = t->key; collect(t->r, a, n); } }
static LN *build_balanced(const int *a, int lo, int hi)  /* rebuild from the sorted live keys: purge the dead ones, height ~ log n */
{
    if (lo > hi) return NULL;
    int m = (lo + hi) / 2; LN *n = calloc(1, sizeof *n); n->key = a[m];
    n->l = build_balanced(a, lo, m - 1); n->r = build_balanced(a, m + 1, hi);
    return n;
}
//<<lazy

int main(void)
{
    srand(1);
    printf("   n   avg depth   1.39*log2(n)   sorted avg depth\n");
    for (int n = 256; n <= 65536; n *= 4) {
        double sum = 0;
        int trials = 20;
        for (int t = 0; t < trials; t++) {
            LN *r = NULL; int *p = malloc(n * sizeof *p);
            for (int i = 0; i < n; i++) p[i] = i;
            for (int i = n - 1; i > 0; i--) { int j = rand() % (i + 1), x = p[i]; p[i] = p[j]; p[j] = x; }   /* Fisher-Yates */
            for (int i = 0; i < n; i++) r = ins(r, p[i], NULL, 0);
            sum += (double)internal_path(r, 0) / n;
            lfree(r); free(p);
        }
        double avg = sum / trials;
        double model = 1.39 * log2((double)n);
        printf("%6d   %8.2f   %12.2f   %14d\n", n, avg, model, (n - 1) / 2);
        assert(avg < 1.39 * log2((double)n) + 1);        /* a bit under 1.39 log2 n - the formula is 2 ln n - O(1) */
        assert(avg > 0.8 * log2((double)n));
    }
    /* sorted input: a chain, average depth (n-1)/2 */
    LN *chain = NULL; for (int i = 0; i < 1000; i++) chain = ins(chain, i, NULL, 0);
    assert(lheight(chain) == 1000 && internal_path(chain, 0) == 999L * 1000 / 2);
    lfree(chain);

    /* lazy deletion */
    LN *t = NULL; int keys[] = { 50, 30, 70, 20, 40, 60, 80 };
    for (int i = 0; i < 7; i++) t = ins(t, keys[i], NULL, 0);
    assert(ldelete(t, 30) && !lcontains(t, 30) && lcontains(t, 20) && lcontains(t, 40));   /* children still reachable */
    assert(lcount(t) == 7 && llive(t) == 6);                                                /* node still there, just dead */
    assert(!ldelete(t, 30));                                                                /* already dead */
    t = ins(t, 30, NULL, 0); assert(lcontains(t, 30) && llive(t) == 7);                     /* revival costs nothing */
    for (int i = 0; i < 7; i++) if (keys[i] != 50) ldelete(t, keys[i]);
    assert(llive(t) == 1 && lcount(t) == 7);
    int live[16], n = 0; collect(t, live, &n);
    LN *fresh = build_balanced(live, 0, n - 1);                                              /* purge */
    assert(lcount(fresh) == 1 && lcontains(fresh, 50));
    lfree(fresh); lfree(t);
    printf("bst_analysis: all tests passed\n");
    return 0;
}
