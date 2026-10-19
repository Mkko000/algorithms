/* ============================================================
 *  ex3_36_list_ops.c  —  Exercises 3.34-3.36, 3.38, 3.39, 3.53, 3.54
 *  Chapter 3: Elementary Data Structures — solved exercises
 *
 *  3.34 Move the largest item on a list to be the final node.
 *  3.35 Move the smallest item to be the first node.
 *  3.36 Put the nodes in even positions after the nodes in odd positions,
 *       keeping the relative order of both.
 *  3.38 Return a copy of a list.
 *  3.39 Remove every item for which a given function returns nonzero.
 *  3.53/3.54 In a set of nodes with no NULL links, following links from
 *       any node ends in a cycle; count the distinct nodes reached,
 *       without modifying nodes and with O(1) extra memory.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Section 3.4, Exercises 3.34-3.54
 *  Build:   gcc -std=c99 -Wall -O2 ex3_36_list_ops.c -o ex3_36
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct node *link;
struct node { int item; link next; };

static link NEW(int item, link next) { link x = malloc(sizeof *x); x->item = item; x->next = next; return x; }
static link build(const int *a, int n) { link h = NULL; for (int i = n - 1; i >= 0; i--) h = NEW(a[i], h); return h; }
static void show(const char *s, link h) { printf("%-24s", s); for (; h; h = h->next) printf("%d ", h->item); printf("\n"); }
static void freelist(link h) { while (h) { link t = h->next; free(h); h = t; } }

/* 3.34: dummy head so the first node is not a special case */
static link max_to_end(link h)
{
    struct node dummy = { 0, h };
    link pm = &dummy, p, last;
    for (p = &dummy; p->next; p = p->next) if (p->next->item > pm->next->item) pm = p;
    if (!h) return h;
    link m = pm->next;
    if (!m->next) return dummy.next;         /* already last */
    pm->next = m->next;
    for (last = pm; last->next; last = last->next) ;
    last->next = m; m->next = NULL;
    return dummy.next;
}

/* 3.35 */
static link min_to_front(link h)
{
    if (!h) return h;
    link pm = NULL, p;
    for (p = h; p->next; p = p->next) if (p->next->item < (pm ? pm->next->item : h->item)) pm = p;
    if (!pm) return h;                       /* smallest already first */
    link m = pm->next; pm->next = m->next; m->next = h;
    return m;
}

/* 3.36: two tails, one pass, no new nodes */
static link odd_then_even(link h)
{
    struct node od = { 0, NULL }, ev = { 0, NULL };
    link o = &od, e = &ev;
    for (int pos = 1; h; h = h->next, pos++)
        if (pos % 2) o = o->next = h; else e = e->next = h;
    e->next = NULL; o->next = ev.next;
    return od.next;
}

/* 3.38 */
static link copy(link h)
{
    struct node d = { 0, NULL }; link t = &d;
    for (; h; h = h->next) t = t->next = NEW(h->item, NULL);
    return d.next;
}

/* 3.39 */
static link remove_if(link h, int (*bad)(link))
{
    struct node d = { 0, h }; link p = &d;
    while (p->next)
        if (bad(p->next)) { link t = p->next; p->next = t->next; free(t); }
        else p = p->next;
    return d.next;
}
static int is_even(link x) { return x->item % 2 == 0; }

/* 3.54: Brent's method — find cycle length lam, then the tail length mu */
static long reached(link x0, long *mu_out, long *lam_out)
{
    long power = 1, lam = 1;
    link tort = x0, hare = x0->next;
    while (tort != hare) {
        if (power == lam) { tort = hare; power *= 2; lam = 0; }
        hare = hare->next; lam++;
    }
    long mu = 0; tort = hare = x0;
    for (long i = 0; i < lam; i++) hare = hare->next;
    while (tort != hare) { tort = tort->next; hare = hare->next; mu++; }
    *mu_out = mu; *lam_out = lam;
    return mu + lam;
}

int main(void)
{
    int a[] = { 5, 9, 2, 7, 1, 8, 3, 6, 4 };
    link h = build(a, 9);
    show("list:", h);
    h = max_to_end(h);      show("3.34 max to end:", h);
    h = min_to_front(h);    show("3.35 min to front:", h);
    link c = copy(h);       show("3.38 copy:", c);
    c = odd_then_even(c);   show("3.36 odd pos, even pos:", c);
    c = remove_if(c, is_even); show("3.39 remove evens:", c);
    show("original untouched:", h);
    freelist(h); freelist(c);

    /* 3.54: a rho-shaped structure: tail of 7 nodes into a cycle of 12 */
    enum { T = 7, C = 12 };
    struct node pool[T + C];
    for (int i = 0; i < T + C; i++) { pool[i].item = i; pool[i].next = &pool[i + 1]; }
    pool[T + C - 1].next = &pool[T];
    long mu, lam;
    long n = reached(&pool[0], &mu, &lam);
    printf("3.54 from node 0: tail %ld + cycle %ld = %ld distinct nodes (expected %d)\n", mu, lam, n, T + C);
    n = reached(&pool[T + 3], &mu, &lam);
    printf("     from a node on the cycle: tail %ld + cycle %ld = %ld\n", mu, lam, n);
    return 0;
}
