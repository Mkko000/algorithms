/* ============================================================
 *  doubly_list.c  —  circular doubly linked list WITH a header (sentinel) node
 *
 *  The sentinel removes every special case (empty list, first, last): head->next is
 *  the first node, head->prev the last, and an empty list is head->next == head.
 *  Operations: insert before/after, delete any node in O(1) given the node,
 *  find, reverse, concatenate, rotate-first-to-last, move a node to the front,
 *  swap neighbours by relinking, insertion-sort by relinking, copy.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja ch.6 (doubly / circular doubly lists), Lipschutz ch.5.10-5.11,
 *                Weiss ch.3 (header node idea)
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 doubly_list.c -o doubly_list
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//>>def
typedef struct dnode { int val; struct dnode *prev, *next; } dnode;
typedef struct { dnode head; int size; } dlist;     /* head is the sentinel (its val is unused) */

static void dl_init(dlist *l) { l->head.prev = l->head.next = &l->head; l->size = 0; }
static int  dl_empty(const dlist *l) { return l->head.next == &l->head; }

/* insert a new node holding v right AFTER node p (p may be the sentinel): 4 pointer writes */
static dnode *dl_insert_after(dlist *l, dnode *p, int v)
{
    dnode *n = malloc(sizeof *n);
    if (!n) { perror("malloc"); exit(1); }
    n->val = v;
    n->prev = p;  n->next = p->next;
    p->next->prev = n;  p->next = n;
    l->size++;
    return n;
}
static dnode *dl_push_front(dlist *l, int v) { return dl_insert_after(l, &l->head, v); }
static dnode *dl_push_back (dlist *l, int v) { return dl_insert_after(l, l->head.prev, v); }

/* unlink a node (not the sentinel!) and return it WITHOUT freeing: lets us "move" nodes */
static dnode *dl_unlink(dlist *l, dnode *p)
{
    assert(p != &l->head);
    p->prev->next = p->next;  p->next->prev = p->prev;
    l->size--;
    return p;
}
static void dl_erase(dlist *l, dnode *p) { free(dl_unlink(l, p)); }
//<<def

//>>ops
static dnode *dl_find(dlist *l, int v)
{
    for (dnode *p = l->head.next; p != &l->head; p = p->next) if (p->val == v) return p;
    return NULL;
}
static void dl_clear(dlist *l) { while (!dl_empty(l)) dl_erase(l, l->head.next); }

/* reverse: swap prev/next in EVERY node including the sentinel */
static void dl_reverse(dlist *l)
{
    dnode *p = &l->head;
    do { dnode *t = p->next; p->next = p->prev; p->prev = t; p = t; } while (p != &l->head);
}
/* concatenate b onto the end of a in O(1); b becomes empty (Thareja PE 12) */
static void dl_concat(dlist *a, dlist *b)
{
    if (dl_empty(b)) return;
    dnode *al = a->head.prev, *bf = b->head.next, *bl = b->head.prev;
    al->next = bf;  bf->prev = al;
    bl->next = &a->head;  a->head.prev = bl;
    a->size += b->size;
    dl_init(b);
}
/* delete the first node and add it as the last one: no allocation, just relinking (PE 13) */
static void dl_rotate_left(dlist *l)
{
    if (l->size < 2) return;
    dnode *f = dl_unlink(l, l->head.next);
    f->prev = l->head.prev;  f->next = &l->head;
    l->head.prev->next = f;  l->head.prev = f;
    l->size++;
}
/* move an existing node to the front (PE 32 moves the middle node) */
static void dl_move_to_front(dlist *l, dnode *p)
{
    dl_unlink(l, p);
    p->prev = &l->head;  p->next = l->head.next;
    l->head.next->prev = p;  l->head.next = p;
    l->size++;
}
//<<ops

//>>swap
/* swap node p with its successor by RELINKING (the data stays where it is): PE 36 */
static void dl_swap_with_next(dlist *l, dnode *p)
{
    dnode *q = p->next;
    assert(p != &l->head && q != &l->head);
    p->prev->next = q;   q->prev = p->prev;
    p->next = q->next;   q->next->prev = p;
    q->next = p;         p->prev = q;
    (void)l;
}
/* insertion sort by relinking: stable, O(n^2) worst case, O(n) on sorted input (PE 8) */
static void dl_sort(dlist *l)
{
    dnode *p = l->head.next;
    while (p != &l->head) {
        dnode *nxt = p->next;
        dnode *q = p->prev;
        while (q != &l->head && q->val > p->val) q = q->prev;      /* find p's place going left */
        if (q != p->prev) {                                         /* only move if necessary    */
            dl_unlink(l, p);
            p->prev = q;  p->next = q->next;
            q->next->prev = p;  q->next = p;
            l->size++;
        }
        p = nxt;
    }
}
//<<swap

static dlist *dl_copy(const dlist *src)
{
    dlist *c = malloc(sizeof *c); dl_init(c);
    for (const dnode *p = src->head.next; p != &src->head; p = p->next) dl_push_back(c, p->val);
    return c;
}
static int check(const dlist *l, const int *e, int n)         /* verifies both directions + size */
{
    if (l->size != n) return 0;
    int i = 0;
    for (const dnode *p = l->head.next; p != &l->head; p = p->next, i++) {
        if (i >= n || p->val != e[i]) return 0;
        if (p->next->prev != p || p->prev->next != p) return 0;
    }
    if (i != n) return 0;
    i = n - 1;
    for (const dnode *p = l->head.prev; p != &l->head; p = p->prev) if (p->val != e[i--]) return 0;
    return i == -1;
}
static void show(const char *tag, const dlist *l)
{
    printf("%-20s", tag);
    for (const dnode *p = l->head.next; p != &l->head; p = p->next) printf("%d ", p->val);
    printf("(size %d)\n", l->size);
}

int main(void)
{
    dlist l; dl_init(&l);
    assert(dl_empty(&l) && check(&l, NULL, 0));
    for (int i = 1; i <= 5; i++) dl_push_back(&l, i);
    dl_push_front(&l, 0);
    show("0..5:", &l);                      assert(check(&l, (int[]){0,1,2,3,4,5}, 6));

    dl_insert_after(&l, dl_find(&l, 2), 99);
    show("99 after 2:", &l);                assert(check(&l, (int[]){0,1,2,99,3,4,5}, 7));
    dl_erase(&l, dl_find(&l, 99));
    dl_erase(&l, l.head.next);              dl_erase(&l, l.head.prev);
    show("erase ends:", &l);                assert(check(&l, (int[]){1,2,3,4}, 4));
    assert(dl_find(&l, 42) == NULL);

    dl_reverse(&l);
    show("reverse:", &l);                   assert(check(&l, (int[]){4,3,2,1}, 4));
    dl_rotate_left(&l);
    show("rotate_left:", &l);               assert(check(&l, (int[]){3,2,1,4}, 4));
    dl_move_to_front(&l, dl_find(&l, 1));
    show("1 to front:", &l);                assert(check(&l, (int[]){1,3,2,4}, 4));
    dl_swap_with_next(&l, dl_find(&l, 3));
    show("swap 3,2:", &l);                  assert(check(&l, (int[]){1,2,3,4}, 4));

    dlist b; dl_init(&b); dl_push_back(&b, 7); dl_push_back(&b, 8);
    dl_concat(&l, &b);
    show("concat:", &l);                    assert(check(&l, (int[]){1,2,3,4,7,8}, 6) && check(&b, NULL, 0));

    dlist s; dl_init(&s);
    int v[] = {5, 2, 8, 1, 9, 3, 2, 7};
    for (int i = 0; i < 8; i++) dl_push_back(&s, v[i]);
    dl_sort(&s);
    show("sorted:", &s);                    assert(check(&s, (int[]){1,2,2,3,5,7,8,9}, 8));
    dlist *c = dl_copy(&s);
    assert(check(c, (int[]){1,2,2,3,5,7,8,9}, 8));
    dl_clear(c); free(c);

    srand(5);                                /* random tests against a plain array model */
    for (int round = 0; round < 200; round++) {
        dlist r; dl_init(&r); int a[30], n = rand() % 30;
        for (int i = 0; i < n; i++) { a[i] = rand() % 15; dl_push_front(&r, a[i]); }
        dl_sort(&r);
        for (int i = 1; i < n; i++) assert(dl_find(&r, a[i]) != NULL);
        int prev = -1; for (dnode *p = r.head.next; p != &r.head; p = p->next) { assert(p->val >= prev); prev = p->val; }
        assert(r.size == n);
        dl_reverse(&r); dl_clear(&r); assert(dl_empty(&r));
    }
    puts("200 random sort / reverse / clear tests passed");
    dl_clear(&l); dl_clear(&s);
    return 0;
}
