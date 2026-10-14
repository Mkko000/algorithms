/* ============================================================
 *  queue.c  —  FIFO queue ADT: linked-list + circular-array
 *  Chapter 4: Abstract Data Types
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 4.9-4.11
 *  Build:   gcc -std=c99 -Wall -O2 queue.c -o queue
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

/* ---- linked-list queue (head = front, tail = back) ---- */
typedef struct qnode *qlink;
struct qnode { int item; qlink next; };
static qlink qhead, qtail;
void LQinit(void) { qhead = qtail = NULL; }
int  LQempty(void){ return qhead == NULL; }
void LQput(int v) {
    qlink t = malloc(sizeof *t); t->item = v; t->next = NULL;
    if (qhead == NULL) { qhead = qtail = t; return; }
    qtail->next = t; qtail = t;
}
int  LQget(void) {
    int v = qhead->item; qlink t = qhead->next;
    free(qhead); qhead = t; if (!qhead) qtail = NULL; return v;
}

/* ---- circular array queue ---- */
#define QMAX 32
static int qa[QMAX], qN, qheadA, qtailA;
void AQinit(void) { qN = qheadA = qtailA = 0; }
int  AQempty(void){ return qN == 0; }
void AQput(int v) { qa[qtailA++] = v; qtailA %= QMAX; qN++; }
int  AQget(void)  { int v = qa[qheadA++]; qheadA %= QMAX; qN--; return v; }

int main(void) {
    LQinit();
    for (int i = 1; i <= 5; i++) LQput(i * 10);
    printf("list queue:");
    while (!LQempty()) printf(" %d", LQget());
    putchar('\n');

    AQinit();
    for (int i = 1; i <= 5; i++) AQput(i);
    AQget(); AQget();                 /* remove 1,2 */
    AQput(6); AQput(7);               /* wrap around */
    printf("array queue:");
    while (!AQempty()) printf(" %d", AQget());
    putchar('\n');
    return 0;
}
