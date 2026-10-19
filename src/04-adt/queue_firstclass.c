/* ============================================================
 *  queue_firstclass.c  —  First-class FIFO queue ADT: many queues
 *                         at once, each one a handle
 *  Chapter 4: Abstract Data Types (Section 4.8)
 *
 *  Program 4.18: interface where QUEUEinit RETURNS a Q handle and
 *  every operation takes the handle. Program 4.19: a client that
 *  simulates M queues (e.g. customers choosing a random line).
 *  Program 4.20: linked-list implementation. Contrast with the
 *  single, global queue in queue.c: there you could only have one.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 4.18-4.20
 *  Build:   gcc -std=c99 -Wall -O2 queue_firstclass.c -o queue_firstclass
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

/* ---------- QUEUE.h (Program 4.18) ---------- */
typedef int Item;
typedef struct queue *Q;
Q    QUEUEinit(int maxN);
int  QUEUEempty(Q);
void QUEUEput(Q, Item);
Item QUEUEget(Q);
void QUEUEdestroy(Q);

/* ---------- QUEUE.c (Program 4.20) ---------- */
typedef struct QUEUEnode *link;
struct QUEUEnode { Item item; link next; };
struct queue { link head; link tail; };
static link NEW(Item item, link next) { link x = malloc(sizeof *x); x->item = item; x->next = next; return x; }
Q QUEUEinit(int maxN) { (void)maxN; Q q = malloc(sizeof *q); q->head = NULL; q->tail = NULL; return q; }
int QUEUEempty(Q q) { return q->head == NULL; }
void QUEUEput(Q q, Item item) {
    if (q->head == NULL) { q->tail = NEW(item, q->head); q->head = q->tail; return; }
    q->tail->next = NEW(item, q->tail->next);
    q->tail = q->tail->next;
}
Item QUEUEget(Q q) {
    Item item = q->head->item; link t = q->head->next;
    free(q->head); q->head = t; return item;
}
void QUEUEdestroy(Q q) { while (!QUEUEempty(q)) QUEUEget(q); free(q); }

/* ---------- client (Program 4.19 style) ---------- */
#define M 4
int main(void) {
    Q queues[M];
    for (int i = 0; i < M; i++) queues[i] = QUEUEinit(100);
    srand(11);
    for (int i = 0; i < 20; i++) {                    /* customer i joins a random line */
        QUEUEput(queues[rand() % M], i);
        if (rand() % 3 == 0) {                        /* sometimes a line gets served */
            int k = rand() % M;
            if (!QUEUEempty(queues[k])) printf("served %2d from line %d\n", QUEUEget(queues[k]), k);
        }
    }
    for (int k = 0; k < M; k++) {
        printf("line %d:", k);
        while (!QUEUEempty(queues[k])) printf(" %d", QUEUEget(queues[k]));
        putchar('\n');
        QUEUEdestroy(queues[k]);
    }
    return 0;
}
