/* ============================================================
 *  circular_header.c  —  circular singly linked list with a header node
 *
 *  last->next returns to the header, so "end of list" is  p == head  (never NULL), and the
 *  header makes insertion/deletion at the front, middle and back one identical piece of
 *  code. Operations: push front/back, insert after k-th, delete first / last / k-th /
 *  by value, delete all negative values, count non-zero, copy, Josephus elimination.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja ch.6 (circular linked lists, header linked lists, PE 6, 10, 25, 27, 37-39),
 *                Lipschutz ch.5.9-5.10 and 5.12 (Josephus problem)
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 circular_header.c -o circular_header
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//>>def
typedef struct cnode { int val; struct cnode *next; } cnode;

/* the header's val field is free: many books keep the NUMBER OF NODES there. We do too,
   so length is O(1)  (header->val == number of data nodes). */
static cnode *ch_new(void)
{
    cnode *h = malloc(sizeof *h);
    if (!h) { perror("malloc"); exit(1); }
    h->val = 0; h->next = h;                 /* empty: points at itself */
    return h;
}
static cnode *ch_insert_after(cnode *h, cnode *p, int v)   /* p is the header or any node */
{
    cnode *n = malloc(sizeof *n);
    if (!n) { perror("malloc"); exit(1); }
    n->val = v; n->next = p->next; p->next = n;
    h->val++;
    return n;
}
static cnode *ch_last(cnode *h) { cnode *p = h; while (p->next != h) p = p->next; return p; }
static void ch_push_front(cnode *h, int v) { ch_insert_after(h, h, v); }
static void ch_push_back(cnode *h, int v)  { ch_insert_after(h, ch_last(h), v); }

/* delete the node AFTER p (p = predecessor). The only deletion primitive we need. */
static int ch_delete_after(cnode *h, cnode *p)
{
    cnode *t = p->next;
    if (t == h) return 0;                    /* nothing to delete (UNDERFLOW) */
    p->next = t->next;
    free(t); h->val--;
    return 1;
}
//<<def

//>>ops
static int ch_delete_first(cnode *h) { return ch_delete_after(h, h); }
static int ch_delete_last(cnode *h)
{
    if (h->next == h) return 0;
    cnode *p = h;
    while (p->next->next != h) p = p->next;  /* p = predecessor of the last node */
    return ch_delete_after(h, p);
}
/* k-th node, k = 1,2,...  (Thareja PE 24: delete the k-th node) */
static int ch_delete_kth(cnode *h, int k)
{
    if (k < 1 || k > h->val) return 0;
    cnode *p = h;
    while (--k) p = p->next;
    return ch_delete_after(h, p);
}
/* delete all nodes with negative data (PE 39) in one pass with a trailing pointer */
static int ch_delete_negatives(cnode *h)
{
    int removed = 0;
    for (cnode *p = h; p->next != h; ) {
        if (p->next->val < 0) { ch_delete_after(h, p); removed++; }
        else p = p->next;                    /* only advance when we did NOT delete */
    }
    return removed;
}
static int ch_count_nonzero(const cnode *h)  /* PE 27 */
{
    int c = 0;
    for (const cnode *p = h->next; p != h; p = p->next) c += p->val != 0;
    return c;
}
static cnode *ch_copy(const cnode *h)        /* PE 6 */
{
    cnode *c = ch_new(), *tail = c;
    for (const cnode *p = h->next; p != h; p = p->next) tail = ch_insert_after(c, tail, p->val);
    return c;
}
static void ch_free(cnode *h) { while (ch_delete_first(h)) ; free(h); }
//<<ops

//>>josephus
/* n people stand in a circle; every m-th is eliminated until one is left.
   Returns the survivor's number (1-based). The circular list makes "skip m−1, remove m-th"
   natural; cost O(n·m). A closed form exists: J(1)=0, J(i)=(J(i−1)+m) mod i  (O(n)). */
static int josephus_list(int n, int m)
{
    cnode *h = ch_new();
    for (int i = 1; i <= n; i++) ch_push_back(h, i);
    cnode *p = h;                            /* invariant: p->next is the person being counted */
    while (h->val > 1) {
        for (int step = 1; step < m; step++) {
            p = p->next;                     /* count one person...                          */
            if (p->next == h) p = h;         /* ...and wrap: the successor of the last is the first */
        }
        ch_delete_after(h, p);               /* eliminate the m-th person                    */
        if (p->next == h) p = h;             /* the next count starts after the victim       */
    }
    int survivor = h->next->val;
    ch_free(h);
    return survivor;
}
static int josephus_formula(int n, int m)
{
    int j = 0;
    for (int i = 2; i <= n; i++) j = (j + m) % i;
    return j + 1;
}
//<<josephus

static void show(const char *tag, const cnode *h)
{
    printf("%-18s", tag);
    for (const cnode *p = h->next; p != h; p = p->next) printf("%d ", p->val);
    printf("(n=%d)\n", h->val);
}
static int check(const cnode *h, const int *e, int n)
{
    int i = 0;
    const cnode *p = h->next;
    for (; p != h; p = p->next, i++) if (i >= n || p->val != e[i]) return 0;
    return i == n && h->val == n;
}

int main(void)
{
    cnode *h = ch_new();
    assert(check(h, NULL, 0) && ch_count_nonzero(h) == 0);
    assert(!ch_delete_first(h) && !ch_delete_last(h) && !ch_delete_kth(h, 1));  /* underflow is safe */
    for (int i = 1; i <= 5; i++) ch_push_back(h, i);
    ch_push_front(h, 0);
    show("0..5:", h);                         assert(check(h, (int[]){0,1,2,3,4,5}, 6));
    ch_delete_first(h); ch_delete_last(h);
    show("no first/last:", h);                assert(check(h, (int[]){1,2,3,4}, 4));
    ch_delete_kth(h, 2);
    show("delete 2nd:", h);                   assert(check(h, (int[]){1,3,4}, 3));
    assert(!ch_delete_kth(h, 4) && !ch_delete_kth(h, 0));

    for (int v = -1; v >= -3; v--) { ch_push_back(h, v); ch_push_front(h, v * 10); }
    show("with negatives:", h);
    assert(ch_delete_negatives(h) == 6);      /* consecutive negatives must all go: -1 -2 -3 at the end */
    show("negatives gone:", h);               assert(check(h, (int[]){1,3,4}, 3));

    ch_push_back(h, 0); ch_push_back(h, 0);
    assert(ch_count_nonzero(h) == 3);
    cnode *c = ch_copy(h);
    assert(check(c, (int[]){1,3,4,0,0}, 5) && c->next != h->next);
    ch_free(c); ch_free(h);

    /* Josephus: known value J(41,3)=31 (the historical story), and list == formula for many n,m */
    printf("Josephus n=41 m=3 -> survivor %d (formula %d)\n", josephus_list(41, 3), josephus_formula(41, 3));
    assert(josephus_list(41, 3) == 31);
    for (int n = 1; n <= 40; n++) for (int m = 1; m <= 9; m++) assert(josephus_list(n, m) == josephus_formula(n, m));
    puts("Josephus list simulation == closed form for n<=40, m<=9");
    return 0;
}
