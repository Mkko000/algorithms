/* ============================================================
 *  index_heap.c  —  Index-heap priority queue (items stay put; the
 *                   heap holds INDICES into a client array)
 *  Chapter 9: Priority Queues and Heapsort (Section 9.6)
 *
 *  Program 9.11 interface / 9.12 implementation. The client owns
 *  data[] (here: task priorities). The PQ keeps pq[] (heap of indices)
 *  and qp[] (inverse: where index k sits in the heap) so that
 *  PQchange(k) can find k's heap position in O(1) and then fixUp or
 *  fixDown in O(lg N). This is exactly what Dijkstra/Prim (Part 5)
 *  need: "the priority of vertex k just changed".
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 9.11-9.12
 *  Build:   gcc -std=c99 -Wall -O2 index_heap.c -o index_heap
 * ============================================================ */
#include <stdio.h>

#define maxPQ 32
static int data[maxPQ];                     /* client's array of priorities */
static int pq[maxPQ + 1], qp[maxPQ], N;     /* pq[i]=index at heap pos i; qp[k]=heap pos of k */
#define less(i, j) (data[pq[i]] < data[pq[j]])

static void exch(int i, int j) {
    int t = pq[i]; pq[i] = pq[j]; pq[j] = t;
    qp[pq[i]] = i; qp[pq[j]] = j;          /* keep the inverse in sync */
}
static void fixUp(int k) { while (k > 1 && less(k / 2, k)) { exch(k, k / 2); k = k / 2; } }
static void fixDown(int k, int n) {
    while (2 * k <= n) {
        int j = 2 * k;
        if (j < n && less(j, j + 1)) j++;
        if (!less(k, j)) break;
        exch(k, j); k = j;
    }
}
void PQinit(void) { N = 0; }
int  PQempty(void) { return N == 0; }
void PQinsert(int k) { qp[k] = ++N; pq[N] = k; fixUp(N); }
int  PQdelmax(void) { exch(1, N); fixDown(1, --N); return pq[N + 1]; }
void PQchange(int k) { fixUp(qp[k]); fixDown(qp[k], N); }

int main(void) {
    const char *task[] = {"email", "build", "deploy", "review", "lunch", "tests"};
    int prio[]         = {  2,       5,       9,        4,        1,      6   };
    PQinit();
    for (int k = 0; k < 6; k++) { data[k] = prio[k]; PQinsert(k); }
    data[4] = 10; PQchange(4);          /* lunch suddenly urgent */
    data[2] = 0;  PQchange(2);          /* deploy postponed */
    printf("order of service:");
    while (!PQempty()) { int k = PQdelmax(); printf(" %s(%d)", task[k], data[k]); }
    putchar('\n');
    return 0;
}
