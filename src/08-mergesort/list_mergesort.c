/* ============================================================
 *  list_mergesort.c  —  Mergesort for linked lists: top-down and
 *                       bottom-up (queue of runs)
 *  Chapter 8: Merging and Mergesort (Sections 8.7, 8.8)
 *
 *  Lists are where mergesort shines: merging needs NO extra array,
 *  only link changes, and the sort is stable.
 *   - Program 8.6: merge two sorted lists (dummy head trick)
 *   - Program 8.7: top-down: split the list in half by walking two
 *                  pointers (one 2x faster), sort halves, merge.
 *   - Program 8.8: bottom-up: put every node on a FIFO queue as a list
 *                  of length 1; repeatedly take two lists off the front,
 *                  merge, put the result at the back, until one remains.
 *  Section 8.8 ("recursion revisited") points out that these are the
 *  same divide-and-conquer computation organized two ways: top-down
 *  follows the recursion tree, bottom-up visits it level by level.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 8.6-8.8
 *  Build:   gcc -std=c99 -Wall -O2 list_mergesort.c -o list_mergesort
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct node *link;
struct node { int item; link next; };
static link NEW(int v, link n) { link x = malloc(sizeof *x); x->item = v; x->next = n; return x; }

/* Program 8.6 */
link merge(link a, link b) {
    struct node head; link c = &head;
    while (a != NULL && b != NULL)
        if (a->item <= b->item) { c->next = a; c = a; a = a->next; }   /* <= keeps it stable */
        else                    { c->next = b; c = b; b = b->next; }
    c->next = (a == NULL) ? b : a;
    return head.next;
}

/* Program 8.7: top-down */
link mergesort(link c) {
    if (c == NULL || c->next == NULL) return c;
    link a = c, b = c->next;
    while (b != NULL && b->next != NULL) { c = c->next; b = b->next->next; }
    b = c->next; c->next = NULL;               /* cut after the middle */
    return merge(mergesort(a), mergesort(b));
}

/* Program 8.8: bottom-up with a queue of lists */
#define QMAX 4096
static link Q[QMAX]; static int qh, qt, qn;
static void put(link x) { Q[qt] = x; qt = (qt + 1) % QMAX; qn++; }
static link get(void) { link x = Q[qh]; qh = (qh + 1) % QMAX; qn--; return x; }
link mergesortBU(link t) {
    qh = qt = qn = 0;
    if (t == NULL) return NULL;
    while (t != NULL) { link u = t->next; t->next = NULL; put(t); t = u; }
    while (qn > 1) { link a = get(), b = get(); put(merge(a, b)); }
    return get();
}

static link randlist(int n, unsigned seed) { srand(seed); link h = NULL; for (int i = 0; i < n; i++) h = NEW(rand() % 100, h); return h; }
static void show(const char *m, link h) { printf("%-10s", m); for (; h; h = h->next) printf(" %d", h->item); putchar('\n'); }
static void freel(link h) { while (h) { link t = h->next; free(h); h = t; } }

int main(void) {
    link a = randlist(15, 8);
    show("input:", a);
    a = mergesort(a);   show("top-down:", a); freel(a);
    link b = randlist(15, 8);
    b = mergesortBU(b); show("bottom-up:", b); freel(b);
    return 0;
}
