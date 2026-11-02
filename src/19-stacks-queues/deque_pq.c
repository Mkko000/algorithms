/* ============================================================
 *  deque_pq.c  —  circular queue, deque (+ input/output restricted) and priority queues
 *
 *   1. circular queue, three ways to tell "full" from "empty":
 *        (a) a count field   (b) keep one cell empty   (c) Thareja's FRONT/REAR = -1 convention
 *   2. deque on a circular array: push/pop at both ends; input- and output-restricted wrappers
 *   3. priority queue as a sorted linked list (equal priorities stay FIFO)
 *   4. priority queue as an array of queues, one per priority level
 *   Everything is model-checked against a trivially-correct (but slow) reference.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja ch.8.4 (circular queue, deque, priority queue), Lipschutz ch.6.12-6.14,
 *                Weiss ch.3.4, Bandyopadhyay & Dey ch.7
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 deque_pq.c -o deque_pq
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define N 8

//>>circ
/* (a) count field: front = index of the oldest element, n = how many.
       rear = (front + n) % N  — it is never stored, it is computed. Full: n == N. Empty: n == 0. */
typedef struct { int a[N]; int front, n; } cqueue;
static int cq_put(cqueue *q, int x)
{
    if (q->n == N) return 0;                           /* overflow */
    q->a[(q->front + q->n++) % N] = x; return 1;
}
static int cq_get(cqueue *q, int *x)
{
    if (q->n == 0) return 0;                           /* underflow */
    *x = q->a[q->front]; q->front = (q->front + 1) % N; q->n--; return 1;
}

/* (b) front/rear only, so N-1 usable cells: empty is front == rear, full is (rear+1)%N == front */
typedef struct { int a[N]; int front, rear; } cqueue_b;
static int cb_put(cqueue_b *q, int x)
{
    if ((q->rear + 1) % N == q->front) return 0;
    q->a[q->rear] = x; q->rear = (q->rear + 1) % N; return 1;
}
static int cb_get(cqueue_b *q, int *x)
{
    if (q->front == q->rear) return 0;
    *x = q->a[q->front]; q->front = (q->front + 1) % N; return 1;
}

/* (c) the book's convention: front = rear = -1 means empty.  N usable cells.
       full  <=>  (front == 0 && rear == N-1)  ||  (front == rear + 1)   — the two faces of the same state */
typedef struct { int a[N]; int front, rear; } cqueue_c;
static int cc_full(const cqueue_c *q) { return (q->front == 0 && q->rear == N - 1) || q->front == q->rear + 1; }
static int cc_put(cqueue_c *q, int x)
{
    if (cc_full(q)) return 0;
    if (q->front == -1) q->front = q->rear = 0;
    else q->rear = (q->rear + 1) % N;
    q->a[q->rear] = x; return 1;
}
static int cc_get(cqueue_c *q, int *x)
{
    if (q->front == -1) return 0;
    *x = q->a[q->front];
    if (q->front == q->rear) q->front = q->rear = -1;        /* took the last one: back to empty */
    else q->front = (q->front + 1) % N;
    return 1;
}
//<<circ

//>>deque
/* deque on a circular array.  left = index of the leftmost element, n = count.
   push_left moves left one step BACK (mod N) and stores; push_right stores at (left+n)%N. */
typedef struct { int a[N]; int left, n; } deque;
static int dq_push_left(deque *d, int x)
{
    if (d->n == N) return 0;
    d->left = (d->left + N - 1) % N; d->a[d->left] = x; d->n++; return 1;
}
static int dq_push_right(deque *d, int x)
{
    if (d->n == N) return 0;
    d->a[(d->left + d->n++) % N] = x; return 1;
}
static int dq_pop_left(deque *d, int *x)
{
    if (!d->n) return 0;
    *x = d->a[d->left]; d->left = (d->left + 1) % N; d->n--; return 1;
}
static int dq_pop_right(deque *d, int *x)
{
    if (!d->n) return 0;
    *x = d->a[(d->left + --d->n) % N]; return 1;
}
/* Input-restricted deque: insert only at the right; delete at both ends.
   Output-restricted deque: delete only at the left; insert at both ends.
   They are the same structure with two of the four operations simply not offered. */
#define ir_insert(d, x)      dq_push_right((d), (x))
#define ir_delete_left(d, x) dq_pop_left((d), (x))
#define ir_delete_right(d,x) dq_pop_right((d), (x))
#define or_insert_left(d, x) dq_push_left((d), (x))
#define or_insert_right(d,x) dq_push_right((d), (x))
#define or_delete(d, x)      dq_pop_left((d), (x))
//<<deque

//>>pqlist
/* priority queue = sorted linked list; SMALLER number = HIGHER priority; ties are FCFS.
   insert: walk while the next node's priority is <= the new one's (so the new node goes AFTER equals).
   delete: remove the head.   insert O(n), delete O(1). */
typedef struct pnode { int data, pri; struct pnode *next; } pnode;
static pnode *pq_insert(pnode *head, int data, int pri)
{
    pnode *n = malloc(sizeof *n);
    if (!n) { perror("malloc"); exit(1); }
    n->data = data; n->pri = pri;
    if (!head || pri < head->pri) { n->next = head; return n; }
    pnode *p = head;
    while (p->next && p->next->pri <= pri) p = p->next;
    n->next = p->next; p->next = n;
    return head;
}
static pnode *pq_delete(pnode *head, int *data)
{
    assert(head);
    *data = head->data; pnode *rest = head->next; free(head); return rest;
}
//<<pqlist

//>>pqarray
/* priority queue as an array of circular queues, one per priority level 0..P-1 (0 = most urgent).
   insert: put at the rear of row p, O(1).   delete: scan rows from 0 for the first non-empty row, O(P). */
#define P 5
typedef struct { cqueue row[P]; } pq_array;
static int pa_insert(pq_array *q, int data, int pri) { return cq_put(&q->row[pri], data); }
static int pa_delete(pq_array *q, int *data, int *pri)
{
    for (int k = 0; k < P; k++) if (q->row[k].n) { *pri = k; return cq_get(&q->row[k], data); }
    return 0;
}
//<<pqarray

int main(void)
{
    srand(5);
    /* ---- the three circular queues, each against a model = log of accepted values + read position ---- */
    cqueue a = {{0}, 0, 0}; cqueue_b b = {{0}, 0, 0}; cqueue_c c = {{0}, -1, -1};
    static int log3[3][20000]; int head[3] = {0, 0, 0}, tail[3] = {0, 0, 0}, over[3] = {0, 0, 0}, x = 0;
    const int cap[3] = {N, N - 1, N};
    for (int step = 0; step < 20000; step++) {
        if (rand() % 100 < 55) {                                /* enqueue more often than dequeue, so "full" is reached */
            int r[3] = { cq_put(&a, step), cb_put(&b, step), cc_put(&c, step) };
            for (int i = 0; i < 3; i++) {
                assert(r[i] == (tail[i] - head[i] < cap[i]));   /* accepted exactly when the model says there is room */
                if (r[i]) log3[i][tail[i]++] = step; else over[i]++;
            }
        } else {
            int r[3] = { cq_get(&a, &x), 0, 0 }; int xs[3] = { x, 0, 0 };
            r[1] = cb_get(&b, &x); xs[1] = x;
            r[2] = cc_get(&c, &x); xs[2] = x;
            for (int i = 0; i < 3; i++) {
                assert(r[i] == (tail[i] > head[i]));
                if (r[i]) assert(xs[i] == log3[i][head[i]++]);  /* FIFO: oldest accepted value comes out */
            }
        }
    }
    printf("circular queues: 20000 random steps, 3 conventions match their models (overflows caught: %d / %d / %d)\n", over[0], over[1], over[2]);
    assert(over[0] > 0 && over[1] > 0 && over[2] > 0);
    /* the (b) convention really wastes one cell: */
    cqueue_b bb = {{0}, 0, 0}; int k = 0; while (cb_put(&bb, k)) k++;
    cqueue_c cc = {{0}, -1, -1}; int k2 = 0; while (cc_put(&cc, k2)) k2++;
    assert(k == N - 1 && k2 == N);
    printf("  convention (b) holds %d items, (a)/(c) hold %d\n", k, k2);

    /* ---- deque against a plain array model ---- */
    deque d = {{0}, 0, 0}; int model[64], lo = 32, hi = 32;        /* model is a window in a bigger array */
    for (int step = 0; step < 20000; step++) {
        int op = rand() % 4, x, y = step, size = hi - lo;
        if (op == 0) { int r = dq_push_left(&d, y);  assert(r == (size < N)); if (r) model[--lo] = y; }
        if (op == 1) { int r = dq_push_right(&d, y); assert(r == (size < N)); if (r) model[hi++] = y; }
        if (op == 2) { int r = dq_pop_left(&d, &x);  assert(r == (size > 0)); if (r) assert(x == model[lo++]); }
        if (op == 3) { int r = dq_pop_right(&d, &x); assert(r == (size > 0)); if (r) assert(x == model[--hi]); }
        if (lo < 4 || hi > 60) { /* re-centre the window */
            int cnt = hi - lo; memmove(&model[32 - cnt / 2], &model[lo], cnt * sizeof(int)); lo = 32 - cnt / 2; hi = lo + cnt;
        }
        assert(d.n == hi - lo);
    }
    puts("deque: 20000 random operations at both ends match the model");

    /* ---- priority queue (list): equal priorities FIFO ---- */
    pnode *h = NULL; int v;
    h = pq_insert(h, 1, 3); h = pq_insert(h, 2, 1); h = pq_insert(h, 3, 3); h = pq_insert(h, 4, 2);
    h = pq_insert(h, 5, 1); h = pq_insert(h, 6, 3);
    int order[6], oi = 0; while (h) { h = pq_delete(h, &v); order[oi++] = v; }
    int want[6] = {2, 5, 4, 1, 3, 6};               /* pri1: 2,5   pri2: 4   pri3: 1,3,6  (arrival order inside a level) */
    for (int i = 0; i < 6; i++) assert(order[i] == want[i]);
    /* Thareja Fig 8.26-8.28 */
    h = NULL; int ids[] = {'A','B','C','D','E'}, pr[] = {1,2,3,5,6};
    for (int i = 0; i < 5; i++) h = pq_insert(h, ids[i], pr[i]);
    h = pq_insert(h, 'F', 4);
    char s[32]; int w = 0; for (pnode *p = h; p; p = p->next) s[w++] = (char)p->data; s[w] = 0;
    assert(strcmp(s, "ABCFDE") == 0);
    while (h) h = pq_delete(h, &v);
    h = NULL; for (int i = 0; i < 5; i++) h = pq_insert(h, ids[i], pr[i]);
    h = pq_insert(h, 'F', 2);
    w = 0; for (pnode *p = h; p; p = p->next) s[w++] = (char)p->data; s[w] = 0;
    assert(strcmp(s, "ABFCDE") == 0);
    while (h) h = pq_delete(h, &v);
    puts("priority list: ABCDE(1,2,3,5,6) + F(pri 4) -> ABCFDE ;  + F(pri 2) -> ABFCDE  (Thareja Fig 8.26-8.28)");

    /* ---- priority queue (array of queues) vs list, random ---- */
    pq_array pa; memset(&pa, 0, sizeof pa); pnode *pl = NULL; int cnt[P] = {0};
    for (int step = 0; step < 20000; step++) {
        if (rand() % 100 < 55) {
            int pri = rand() % P, data = step;
            if (pa_insert(&pa, data, pri)) { pl = pq_insert(pl, data, pri); cnt[pri]++; }
        } else {
            int d1, d2, p1 = -1; int r1 = pa_delete(&pa, &d1, &p1);
            if (!pl) { assert(!r1); continue; }
            assert(r1); pl = pq_delete(pl, &d2); assert(d1 == d2); cnt[p1]--;
        }
    }
    while (pl) pl = pq_delete(pl, &v);
    puts("priority queue: array-of-queues and sorted list give identical output order over 20000 random steps");
    puts("deque_pq ok");
    return 0;
}
