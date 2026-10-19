/* ============================================================
 *  list_memory.c  —  A list-processing interface with our OWN
 *                    memory allocator (free list in arrays)
 *  Chapter 3: Elementary Data Structures (Sections 3.4, 3.5)
 *
 *  Program 3.12 declares black-box list primitives; Program 3.14
 *  implements them on top of preallocated arrays, keeping unused
 *  nodes on a "free list" so that freeNode/newNode are O(1) and
 *  never call malloc. Program 3.13 (Josephus) is the client.
 *  We also implement the same interface with array INDICES instead
 *  of pointers (the "cursor" representation, Exercise 3.52).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 3.12, 3.13, 3.14
 *  Build:   gcc -std=c99 -Wall -O2 list_memory.c -o list_memory
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

/* ---------- Program 3.12: interface ---------- */
typedef struct node *link;
struct node { int item; link next; };
typedef link Node;
void initNodes(int);
link newNode(int);
void freeNode(link);
void insertNext(link, link);
link deleteNext(link);
link Next(link);
int  Item_(link);

/* ---------- Program 3.14: implementation with a free list ---------- */
static link freelist;
void initNodes(int N) {
    freelist = malloc((N + 1) * sizeof *freelist);   /* one block, carved up */
    for (int i = 0; i < N + 1; i++) freelist[i].next = &freelist[i + 1];
    freelist[N].next = NULL;
}
link newNode(int i) { link x = deleteNext(freelist); x->item = i; x->next = x; return x; }
void freeNode(link x) { insertNext(freelist, x); }
void insertNext(link x, link t) { t->next = x->next; x->next = t; }
link deleteNext(link x) { link t = x->next; x->next = t->next; return t; }
link Next(link x) { return x->next; }
int  Item_(link x) { return x->item; }

/* ---------- Program 3.13: Josephus client ---------- */
int josephus(int N, int M) {
    initNodes(N);
    link x = newNode(1), t = x;
    for (int i = 2; i <= N; i++) { link y = newNode(i); insertNext(t, y); t = y; }
    while (t != Next(t)) {
        for (int i = 1; i < M; i++) t = Next(t);
        freeNode(deleteNext(t));            /* node goes back to the free list */
    }
    return Item_(t);
}

/* ---------- Cursor (array-index) version of the same idea ---------- */
#define MAXC 1000
static int citem[MAXC], cnext[MAXC], cfree;
int josephus_cursor(int N, int M) {
    for (int i = 0; i < MAXC - 1; i++) cnext[i] = i + 1;   /* all nodes free */
    cnext[MAXC - 1] = -1; cfree = 0;
    int first = cfree; cfree = cnext[cfree]; citem[first] = 1; cnext[first] = first;
    int t = first;
    for (int i = 2; i <= N; i++) {
        int y = cfree; cfree = cnext[cfree];
        citem[y] = i; cnext[y] = cnext[t]; cnext[t] = y; t = y;
    }
    while (t != cnext[t]) {
        for (int i = 1; i < M; i++) t = cnext[t];
        int d = cnext[t]; cnext[t] = cnext[d];
        cnext[d] = cfree; cfree = d;               /* free the index */
    }
    return citem[t];
}

int main(void) {
    printf("Josephus(9,5) with pointer free list: %d\n", josephus(9, 5));
    printf("Josephus(9,5) with index  free list: %d\n", josephus_cursor(9, 5));
    printf("Josephus(41,3) (the classic story)   : %d\n", josephus_cursor(41, 3));
    return 0;
}
