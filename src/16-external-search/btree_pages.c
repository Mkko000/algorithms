/* ============================================================
 *  btree_pages.c  —  B trees the way Sedgewick builds them:
 *                    items in EXTERNAL pages, keys + links in internal
 *                    pages, split-on-overflow bottom-up
 *  Chapter 16: External Searching (Sections 16.1, 16.3)
 *
 *  Every node is a "page" of up to M entries (one disk block). Internal
 *  entry j = (smallest key in subtree j, link to subtree j). External
 *  pages hold the items in order. (This variant, with all items at the
 *  bottom, is what many systems call a B+ tree.)
 *   - Program 16.1: page layout and initialization
 *   - Program 16.2: search: at height H>0 find the entry whose key range
 *     contains v and follow its link; at H=0 scan the page
 *   - Program 16.3: insert: put the entry in place; if the page is now
 *     full (M entries) split it and hand the new page UP to the parent
 *   - Program 16.4: split: move the upper M/2 entries to a new page.
 *     The tree grows only when the ROOT splits, so every external page
 *     is at the same depth (perfect balance, like 2-3-4 trees).
 *  Property 16.2: a search or insertion uses between log_M N and
 *  log_{M/2} N page accesses — 3 or 4 for billions of items when M is
 *  in the hundreds or thousands. We COUNT page accesses below.
 *  (btree.c holds the classic "keys in every node" textbook variant.)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 16.1-16.4
 *  Build:   gcc -std=c99 -Wall -O2 btree_pages.c -o btree_pages -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define M 64                                  /* entries per page (even) */
typedef int Key;
typedef int Item;                             /* item == key for the demo */
#define NULLitem (-1)

typedef struct STnode *link;
typedef struct { Key key; union { link next; Item item; } ref; } entry;
struct STnode { entry b[M]; int m; };         /* Program 16.1 */
static link head; static int H, N; static long pages, page_reads;

static link NEW(void) { link x = malloc(sizeof *x); x->m = 0; pages++; return x; }
void STinit(void) { head = NEW(); H = 0; N = 0; }

/* Program 16.2 */
static Item searchR(link h, Key v, int ht) {
    page_reads++;
    if (ht == 0) { for (int j = 0; j < h->m; j++) if (v == h->b[j].key) return h->b[j].ref.item; return NULLitem; }
    for (int j = 0; j < h->m; j++)
        if (j + 1 == h->m || v < h->b[j + 1].key) return searchR(h->b[j].ref.next, v, ht - 1);
    return NULLitem;
}
Item STsearch(Key v) { return searchR(head, v, H); }

/* Program 16.4 */
static link split(link h) {
    link t = NEW();
    for (int j = 0; j < M / 2; j++) t->b[j] = h->b[M / 2 + j];
    h->m = M / 2; t->m = M / 2;
    return t;
}
/* Program 16.3 */
static link insertR(link h, Item item, int ht) {
    int i, j; Key v = item; entry x; link u;
    x.key = v; x.ref.item = item;
    if (ht == 0) { for (j = 0; j < h->m; j++) if (v < h->b[j].key) break; }
    else
        for (j = 0; j < h->m; j++)
            if (j + 1 == h->m || v < h->b[j + 1].key) {
                link t = h->b[j++].ref.next;
                u = insertR(t, item, ht - 1);
                if (u == NULL) return NULL;          /* child did not split: done */
                x.key = u->b[0].key; x.ref.next = u; /* child split: add entry for new page */
                break;
            }
    for (i = (h->m)++; i > j; i--) h->b[i] = h->b[i - 1];
    h->b[j] = x;
    return h->m < M ? NULL : split(h);
}
void STinsert(Item item) {
    link u = insertR(head, item, H);
    N++;
    if (u == NULL) return;
    link t = NEW(); t->m = 2;                      /* root split: tree grows UP */
    t->b[0].key = head->b[0].key; t->b[0].ref.next = head;
    t->b[1].key = u->b[0].key;    t->b[1].ref.next = u;
    head = t; H++;
}

int main(void) {
    STinit(); srand(16);
    const int n = 1000000;
    for (int i = 0; i < n; i++) STinsert(rand());
    printf("N=%d random keys, M=%d entries/page: height H=%d, %ld pages (%.0f%% full on average)\n",
           N, M, H, pages, 100.0 * N / (pages * (double)M));
    printf("log_M N = %.2f, log_{M/2} N = %.2f  -> every search reads H+1 = %d pages\n",
           log((double)N) / log((double)M), log((double)N) / log((double)M / 2), H + 1);
    srand(16); int hits = 0; page_reads = 0;
    for (int i = 0; i < 1000; i++) hits += STsearch(rand()) != NULLitem;
    printf("1000 searches for inserted keys: %d hits, %.1f page reads each\n", hits, page_reads / 1000.0);
    printf("search for -5: %s\n", STsearch(-5) == NULLitem ? "miss" : "hit");
    return 0;
}
