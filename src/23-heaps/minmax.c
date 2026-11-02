/* Min-max heap  (Weiss, Exercise 6.18 and Figure 6.58)
 *
 * The shape is that of a binary heap (array positions 1..n).  The ORDER is different:
 *   - a node at an even depth (0, 2, 4, ...) is a "min level" node: it is the smallest key in its own subtree;
 *   - a node at an odd depth (1, 3, 5, ...) is a "max level" node: it is the largest key in its own subtree.
 * Weiss states this as: an even-depth node is smaller than its parent and larger than its grandparent (and an
 * odd-depth node the other way round); that is what the insert and the percolations below actually maintain.
 *   findMin = a[1];   findMax = the larger of a[2], a[3] (the root if n == 1);   both O(1).
 * insert, deleteMin and deleteMax are O(log N) (Atkinson, Sack, Santoro, Strothotte 1986).
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int a[512]; int n; } MMHeap;                 /* a[0] unused, as in the book's binary heap */

static int mm_level_is_min(int i) { int d = 0; while (i > 1) { i >>= 1; d++; } return d % 2 == 0; }
static int mm_min(const MMHeap *h) { return h->a[1]; }
static int mm_max_index(const MMHeap *h) { return h->n == 1 ? 1 : (h->n == 2 || h->a[2] >= h->a[3]) ? 2 : 3; }
static int mm_max(const MMHeap *h) { return h->a[mm_max_index(h)]; }

static void swap(int *x, int *y) { int t = *x; *x = *y; *y = t; }

/* bubble up on the levels of the same kind: grandparent (i/4) */
static void mm_up_same(MMHeap *h, int i, int min_level)
{
    while (i >= 4 && (min_level ? h->a[i] < h->a[i / 4] : h->a[i] > h->a[i / 4])) { swap(&h->a[i], &h->a[i / 4]); i /= 4; }
}
static void mm_bubble_up(MMHeap *h, int i)
{
    if (i == 1) return;
    int p = i / 2;
    if (mm_level_is_min(i)) {
        if (h->a[i] > h->a[p]) { swap(&h->a[i], &h->a[p]); mm_up_same(h, p, 0); }   /* belongs on a max level */
        else mm_up_same(h, i, 1);
    } else {
        if (h->a[i] < h->a[p]) { swap(&h->a[i], &h->a[p]); mm_up_same(h, p, 1); }
        else mm_up_same(h, i, 0);
    }
}
static void mm_insert(MMHeap *h, int x) { assert(h->n < 511); h->a[++h->n] = x; mm_bubble_up(h, h->n); }

/* the smallest (min level) or largest (max level) key among the children and grandchildren of i; 0 if none */
static int mm_best_descendant(const MMHeap *h, int i, int want_min)
{
    int best = 0, c[6] = { 2 * i, 2 * i + 1, 4 * i, 4 * i + 1, 4 * i + 2, 4 * i + 3 };
    for (int k = 0; k < 6; k++) {
        if (c[k] > h->n) continue;
        if (!best || (want_min ? h->a[c[k]] < h->a[best] : h->a[c[k]] > h->a[best])) best = c[k];
    }
    return best;
}
static void mm_trickle_down(MMHeap *h, int i)
{
    int min_level = mm_level_is_min(i);
    for (;;) {
        int m = mm_best_descendant(h, i, min_level);
        if (!m) return;
        if (m >= 4 * i) {                                                  /* m is a grandchild */
            if (min_level ? h->a[m] < h->a[i] : h->a[m] > h->a[i]) {
                swap(&h->a[m], &h->a[i]);
                int p = m / 2;                                             /* fix the parent of m on the other kind of level */
                if (min_level ? h->a[m] > h->a[p] : h->a[m] < h->a[p]) swap(&h->a[m], &h->a[p]);
                i = m;
            } else return;
        } else {                                                           /* m is a child: the last possible move */
            if (min_level ? h->a[m] < h->a[i] : h->a[m] > h->a[i]) swap(&h->a[m], &h->a[i]);
            return;
        }
    }
}
static int mm_delete_min(MMHeap *h) { assert(h->n); int r = h->a[1]; h->a[1] = h->a[h->n--]; if (h->n) mm_trickle_down(h, 1); return r; }
static int mm_delete_max(MMHeap *h)
{
    assert(h->n); int i = mm_max_index(h), r = h->a[i];
    h->a[i] = h->a[h->n--]; if (i <= h->n) mm_trickle_down(h, i);
    return r;
}
/* the definition, checked literally: min-level keys <= everything below them, max-level keys >= everything below */
static int mm_subtree_ok(const MMHeap *h, int i, int j)                    /* is key i ordered against node j ? */
{
    return mm_level_is_min(i) ? h->a[i] <= h->a[j] : h->a[i] >= h->a[j];
}
static int mm_valid(const MMHeap *h)
{
    for (int i = 1; i <= h->n; i++) {
        int stack[512], sp = 0;
        if (2 * i <= h->n) stack[sp++] = 2 * i;
        if (2 * i + 1 <= h->n) stack[sp++] = 2 * i + 1;
        while (sp) { int j = stack[--sp]; if (!mm_subtree_ok(h, i, j)) return 0;
            if (2 * j <= h->n) stack[sp++] = 2 * j;
            if (2 * j + 1 <= h->n) stack[sp++] = 2 * j + 1; }
    }
    return 1;
}
/* linear-time build (Exercise 6.18d): trickle-down from n/2 to 1, exactly like buildHeap */
static void mm_build(MMHeap *h, const int *v, int n)
{
    h->n = n; memcpy(h->a + 1, v, sizeof(int) * (size_t)n);
    for (int i = n / 2; i >= 1; i--) mm_trickle_down(h, i);
}
static void mm_print(const MMHeap *h, const char *label)
{
    printf("%s:", label); for (int i = 1; i <= h->n; i++) printf(" %d", h->a[i]); printf("\n");
}

#ifndef MM_NO_MAIN
int main(void)
{
    /* Figure 6.58 */
    int fig[] = { 6, 81, 87, 14, 17, 12, 28, 71, 25, 80, 20, 52, 78, 31, 42, 31, 59, 16, 24, 79, 63, 18, 19, 32, 13, 15, 48 };
    MMHeap h; h.n = 27; memcpy(h.a + 1, fig, sizeof fig);
    assert(mm_valid(&h) && mm_min(&h) == 6 && mm_max(&h) == 87);                  /* 6.18a */
    /* insert the keys 1..15 in order and delete from both ends */
    MMHeap g = { { 0 }, 0 };
    for (int i = 1; i <= 15; i++) { mm_insert(&g, i); assert(mm_valid(&g)); }
    mm_print(&g, "insert 1..15      ");
    assert(mm_min(&g) == 1 && mm_max(&g) == 15);
    assert(mm_delete_min(&g) == 1 && mm_delete_max(&g) == 15 && mm_valid(&g));
    mm_print(&g, "after min and max  ");
    /* Fig 6.58 itself: deleteMin then deleteMax */
    assert(mm_delete_min(&h) == 6 && mm_valid(&h) && mm_min(&h) == 12);
    assert(mm_delete_max(&h) == 87 && mm_valid(&h) && mm_max(&h) == 81);
    mm_print(&h, "Fig 6.58 after dMin, dMax");
    /* random: compare with a sorted array, both ends, plus linear build */
    srand(61);
    for (int round = 0; round < 300; round++) {
        MMHeap r = { { 0 }, 0 }; int ref[300], m = 0;
        for (int step = 0; step < 300; step++) {
            int op = rand() % 4;
            if (op < 2 && m < 300) { int x = rand() % 500; mm_insert(&r, x); ref[m++] = x; }
            else if (m) {
                int lo = 0, hi = 0; for (int i = 1; i < m; i++) { if (ref[i] < ref[lo]) lo = i; if (ref[i] > ref[hi]) hi = i; }
                int idx = op == 2 ? lo : hi; int got = op == 2 ? mm_delete_min(&r) : mm_delete_max(&r);
                assert(got == ref[idx]); ref[idx] = ref[--m];
            }
            assert(mm_valid(&r) && r.n == m);
            if (m) { int lo = ref[0], hi = ref[0]; for (int i = 1; i < m; i++) { if (ref[i] < lo) lo = ref[i]; if (ref[i] > hi) hi = ref[i]; }
                     assert(mm_min(&r) == lo && mm_max(&r) == hi); }
        }
    }
    for (int round = 0; round < 200; round++) {
        int v[200], n = 1 + rand() % 200; for (int i = 0; i < n; i++) v[i] = rand() % 1000;
        MMHeap b; mm_build(&b, v, n); assert(mm_valid(&b));
    }
    printf("minmax: all checks passed\n");
    return 0;
}
#endif
