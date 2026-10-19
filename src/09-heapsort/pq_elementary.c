/* ============================================================
 *  pq_elementary.c  —  Elementary priority queues, and a first-class
 *                      PQ with HANDLES (delete / change any item)
 *  Chapter 9: Priority Queues and Heapsort (Sections 9.1, 9.5)
 *
 *  Section 9.1 / Table 9.1: the cost of the PQ operations depends on
 *  the representation:
 *                    insert   delmax   delete   change   join
 *    ordered array     N        1        N        N       N
 *    ordered list      N        1        1        N       N
 *    unordered array   1        N        1        1       N
 *    unordered list    1        N        1        1       1
 *    heap            lg N     lg N     lg N     lg N      N
 *    binomial queue  lg N     lg N     lg N     lg N     lg N
 *  Program 9.2: unordered array (lazy: insert O(1), delmax scans).
 *  Programs 9.8-9.10: a FIRST-CLASS PQ whose insert returns a handle,
 *  implemented as an unordered DOUBLY-linked list, so PQdelete(h) and
 *  PQchange are O(1) and PQjoin is O(1) (splice two lists).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 9.1, 9.2, 9.8, 9.9, 9.10
 *  Build:   gcc -std=c99 -Wall -O2 pq_elementary.c -o pq_elementary
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef int Item;

/* ---------- Program 9.2: unordered array PQ ---------- */
static Item *pq; static int N;
void PQinit(int maxN) { pq = malloc(maxN * sizeof(Item)); N = 0; }
int  PQempty(void) { return N == 0; }
void PQinsert(Item v) { pq[N++] = v; }
Item PQdelmax(void) {
    int j, max = 0;
    for (j = 1; j < N; j++) if (pq[max] < pq[j]) max = j;
    Item t = pq[max]; pq[max] = pq[N - 1]; pq[N - 1] = t;   /* swap to the end */
    return pq[--N];
}

/* ---------- Programs 9.8-9.10: first-class PQ with handles ---------- */
typedef struct PQnode *PQlink;
struct PQnode { Item key; PQlink prev, next; };
typedef struct pq *PQ;
struct pq { PQlink head, tail; };
typedef PQlink PQhandle;

PQ PQnew(void) {
    PQ p = malloc(sizeof *p);
    p->head = malloc(sizeof *p->head); p->tail = malloc(sizeof *p->tail);
    p->head->prev = p->tail; p->head->next = p->tail;
    p->tail->prev = p->head; p->tail->next = p->head;
    return p;
}
int PQisempty(PQ p) { return p->head->next->next == p->head; }
PQhandle PQput(PQ p, Item v) {
    PQlink t = malloc(sizeof *t);
    t->key = v; t->next = p->head->next; t->next->prev = t;
    t->prev = p->head; p->head->next = t;
    return t;
}
Item PQget(PQ p) {                                   /* delete the maximum */
    PQlink x = p->head->next, t;
    for (t = x; t->next != p->head; t = t->next) if (t->key > x->key) x = t;
    Item v = x->key;
    x->next->prev = x->prev; x->prev->next = x->next; free(x);
    return v;
}
void PQchange(PQ p, PQhandle x, Item v) { (void)p; x->key = v; }
void PQdelete(PQ p, PQhandle x) { (void)p; x->next->prev = x->prev; x->prev->next = x->next; free(x); }
void PQjoin(PQ a, PQ b) {                           /* move all of b into a, O(1) */
    if (PQisempty(b)) { free(b->head); free(b->tail); free(b); return; }
    PQlink bf = b->head->next, bl = b->tail->prev;
    /* splice b's real nodes right after a->head */
    bl->next = a->head->next; a->head->next->prev = bl;
    a->head->next = bf; bf->prev = a->head;
    free(b->head); free(b->tail); free(b);
}

int main(void) {
    PQinit(10);
    int v[] = {5, 9, 1, 7, 3};
    for (int i = 0; i < 5; i++) PQinsert(v[i]);
    printf("unordered-array PQ:"); while (!PQempty()) printf(" %d", PQdelmax()); putchar('\n');
    free(pq);

    PQ a = PQnew(), b = PQnew();
    PQhandle h3 = PQput(a, 3); PQput(a, 8); PQhandle h1 = PQput(a, 1);
    PQput(b, 6); PQput(b, 10);
    PQchange(a, h1, 12);      /* 1 becomes 12 */
    PQdelete(a, h3);          /* remove the 3 by handle, no search */
    PQjoin(a, b);             /* a := a U b */
    printf("first-class PQ after change/delete/join:");
    while (!PQisempty(a)) printf(" %d", PQget(a));
    putchar('\n');
    free(a->head); free(a->tail); free(a);
    return 0;
}
