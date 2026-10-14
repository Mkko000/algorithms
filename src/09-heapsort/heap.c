/* ============================================================
 *  heap.c  —  Binary heap priority queue + heapsort
 *  Chapter 9: Priority Queues and Heapsort
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 9.3-9.5, 9.7
 *  Build:   gcc -std=c99 -Wall -O2 heap.c -o heap
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include "../common_item.h"

void fixUp(Item a[], int k) {
    while (k > 1 && less(a[k / 2], a[k])) {
        exch(a[k], a[k / 2]);
        k = k / 2;
    }
}

void fixDown(Item a[], int k, int N) {
    while (2 * k <= N) {
        int j = 2 * k;
        if (j < N && less(a[j], a[j + 1])) j++;
        if (!less(a[k], a[j])) break;
        exch(a[k], a[j]);
        k = j;
    }
}

/* Priority-queue ADT on a 1-indexed heap array. */
static Item *pq;
static int N;
void PQinit(int maxN) { pq = malloc((maxN + 1) * sizeof(Item)); N = 0; }
int  PQempty(void)    { return N == 0; }
void PQinsert(Item v) { pq[++N] = v; fixUp(pq, N); }
Item PQdelmax(void) {
    exch(pq[1], pq[N]);
    fixDown(pq, 1, --N);
    return pq[N + 1];
}

void heapsort(Item a[], int l, int r) {
    int k, n = r - l + 1;
    Item *h = a + l - 1;                  /* so h[1] is a[l] */
    for (k = n / 2; k >= 1; k--) fixDown(h, k, n);
    while (n > 1) { exch(h[1], h[n]); fixDown(h, 1, --n); }
}

int main(void) {
    Item vals[] = {30, 10, 50, 20, 40};
    PQinit(16);
    for (int i = 0; i < 5; i++) PQinsert(vals[i]);
    printf("PQ delmax order:");
    while (!PQempty()) printf(" %d", PQdelmax());
    putchar('\n');

    Item a[] = {'H','E','A','P','S','O','R','T','E','X','A','M','P','L','E'};
    int n = sizeof(a)/sizeof(a[0]);
    heapsort(a, 0, n - 1);
    printf("heapsort: ");
    for (int i = 0; i < n; i++) printf("%c ", a[i]);
    putchar('\n');
    free(pq);
    return 0;
}
