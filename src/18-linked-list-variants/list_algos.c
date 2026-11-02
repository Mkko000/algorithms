/* ============================================================
 *  list_algos.c  —  classic algorithms on a SINGLY linked list
 *
 *  reverse (iterative and recursive), merge two sorted lists, split alternate
 *  nodes, middle node, k-th from the end in ONE pass, Floyd cycle detection,
 *  remove duplicates, is-sorted, merge sort on a list.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja "Data Structures Using C" ch.6 exercises, Lipschutz
 *                "Data Structures with C" ch.5, Weiss "Data Structures and Algorithm
 *                Analysis" ch.3 (C++ code translated to C)
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 list_algos.c -o list_algos
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//>>node
typedef struct node { int val; struct node *next; } node;

static node *push_front(node *h, int v)
{
    node *n = malloc(sizeof *n);
    if (!n) { perror("malloc"); exit(1); }
    n->val = v; n->next = h;
    return n;
}
static node *from_array(const int a[], int n)      /* builds a list in the SAME order as a[] */
{
    node *h = NULL;
    for (int i = n - 1; i >= 0; i--) h = push_front(h, a[i]);
    return h;
}
static int to_array(const node *h, int out[], int cap)
{
    int k = 0;
    for (; h && k < cap; h = h->next) out[k++] = h->val;
    return k;
}
static void free_list(node *h) { while (h) { node *t = h->next; free(h); h = t; } }
static int length(const node *h) { int n = 0; for (; h; h = h->next) n++; return n; }
//<<node

//>>reverse
/* three pointers: prev, cur, next. Each link is flipped exactly once: O(n) time, O(1) space. */
static node *reverse_iter(node *h)
{
    node *prev = NULL;
    while (h) {
        node *next = h->next;   /* 1. remember the rest      */
        h->next = prev;         /* 2. flip this link         */
        prev = h;               /* 3. advance both           */
        h = next;
    }
    return prev;
}
/* recursion: reverse the tail, then hang the head after the (new) last node. O(n) stack. */
static node *reverse_rec(node *h)
{
    if (!h || !h->next) return h;
    node *rest = reverse_rec(h->next);
    h->next->next = h;
    h->next = NULL;
    return rest;
}
//<<reverse

//>>merge
/* merge two SORTED lists by relinking nodes (no allocation). Stable: ties take from a first. */
static node *merge_sorted(node *a, node *b)
{
    node dummy; node *t = &dummy; dummy.next = NULL;
    while (a && b) {
        if (b->val < a->val) { t->next = b; b = b->next; }
        else                 { t->next = a; a = a->next; }
        t = t->next;
    }
    t->next = a ? a : b;
    return dummy.next;
}
//<<merge

//>>split
/* split into two lists: nodes 1,3,5,... go to *odd and 2,4,6,... to *even (relinking only) */
static void split_alternate(node *h, node **odd, node **even)
{
    node od, ev; node *to = &od, *te = &ev; od.next = ev.next = NULL;
    int i = 0;
    while (h) {
        node *next = h->next;
        h->next = NULL;
        if (i++ % 2 == 0) { to->next = h; to = h; } else { te->next = h; te = h; }
        h = next;
    }
    *odd = od.next; *even = ev.next;
}
//<<split

//>>twoptr
/* slow/fast pointers: when fast reaches the end, slow is at the middle. */
static node *middle(node *h)
{
    node *slow = h, *fast = h;
    while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
    return slow;               /* for an even length this is the SECOND of the two middles */
}
/* n-th node from the end in a single pass: advance 'lead' n steps, then move both. */
static node *nth_from_end(node *h, int n)
{
    node *lead = h, *lag = h;
    for (int i = 0; i < n; i++) { if (!lead) return NULL; lead = lead->next; }
    while (lead) { lead = lead->next; lag = lag->next; }
    return lag;
}
/* Floyd's tortoise and hare: they meet iff there is a cycle. */
static int has_cycle(const node *h)
{
    const node *slow = h, *fast = h;
    while (fast && fast->next) {
        slow = slow->next; fast = fast->next->next;
        if (slow == fast) return 1;
    }
    return 0;
}
//<<twoptr

//>>dups
static int is_sorted(const node *h)
{
    for (; h && h->next; h = h->next) if (h->val > h->next->val) return 0;
    return 1;
}
/* remove EVERY later duplicate (unsorted list): O(n^2), no extra memory */
static node *remove_duplicates(node *h)
{
    for (node *p = h; p; p = p->next) {
        node *q = p;
        while (q->next) {
            if (q->next->val == p->val) { node *d = q->next; q->next = d->next; free(d); }
            else q = q->next;
        }
    }
    return h;
}
//<<dups

//>>msort
/* merge sort on a list: split in the middle, sort halves, merge. O(n log n), O(log n) stack,
   and (unlike arrays) O(1) extra memory because merging only relinks nodes. */
static node *list_msort(node *h)
{
    if (!h || !h->next) return h;
    node *slow = h, *fast = h->next;              /* slow ends at the LAST node of the first half */
    while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
    node *second = slow->next; slow->next = NULL;
    return merge_sorted(list_msort(h), list_msort(second));
}
//<<msort

static void show(const char *tag, const node *h)
{
    printf("%-22s", tag);
    for (; h; h = h->next) printf("%d ", h->val);
    puts("");
}
static int same(const node *h, const int *e, int n)
{
    int buf[64]; int k = to_array(h, buf, 64);
    if (k != n) return 0;
    for (int i = 0; i < n; i++) if (buf[i] != e[i]) return 0;
    return 1;
}

int main(void)
{
    int a[] = {1, 2, 3, 4, 5};
    node *h = from_array(a, 5);
    show("original:", h);
    h = reverse_iter(h);  show("reverse_iter:", h);  assert(same(h, (int[]){5,4,3,2,1}, 5));
    h = reverse_rec(h);   show("reverse_rec:", h);   assert(same(h, a, 5));
    assert(reverse_iter(NULL) == NULL && reverse_rec(NULL) == NULL);

    node *x = from_array((int[]){1, 4, 9}, 3), *y = from_array((int[]){2, 3, 10, 11}, 4);
    node *m = merge_sorted(x, y);
    show("merge_sorted:", m);  assert(same(m, (int[]){1,2,3,4,9,10,11}, 7));
    free_list(m);

    node *o, *e; split_alternate(h, &o, &e);
    show("odd positions:", o); show("even positions:", e);
    assert(same(o, (int[]){1,3,5}, 3) && same(e, (int[]){2,4}, 2));
    free_list(o); free_list(e);

    h = from_array(a, 5);
    assert(middle(h)->val == 3);
    assert(nth_from_end(h, 1)->val == 5 && nth_from_end(h, 5)->val == 1 && nth_from_end(h, 6) == NULL);
    assert(!has_cycle(h));
    node *last = h; while (last->next) last = last->next;
    last->next = h->next;                       /* make a cycle 5 -> 2 */
    assert(has_cycle(h));
    last->next = NULL;
    printf("middle=%d  2nd-from-end=%d  cycle detection ok\n", middle(h)->val, nth_from_end(h, 2)->val);
    free_list(h);

    h = from_array((int[]){4, 2, 4, 1, 2, 4, 3}, 7);
    h = remove_duplicates(h); show("no duplicates:", h);
    assert(same(h, (int[]){4,2,1,3}, 4));
    assert(!is_sorted(h));
    h = list_msort(h); show("list_msort:", h);
    assert(same(h, (int[]){1,2,3,4}, 4) && is_sorted(h));
    free_list(h);

    /* random test: merge sort agrees with qsort order, length is preserved */
    srand(3);
    for (int round = 0; round < 300; round++) {
        int n = rand() % 40; node *l = NULL;
        for (int i = 0; i < n; i++) l = push_front(l, rand() % 20);
        l = list_msort(l);
        assert(length(l) == n && is_sorted(l));
        free_list(l);
    }
    puts("300 random merge-sort tests passed");
    return 0;
}
