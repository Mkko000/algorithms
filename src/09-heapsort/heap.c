/* ============================================================
 *  heap.c  —  Binary heap priority queue + heapsort
 *  Chapter 9: Priority Queues and Heapsort (Sections 9.2-9.4)
 *
 *  Programs 9.3/9.4: fixUp (bottom-up heapify) and fixDown (top-down).
 *  Program 9.5: heap-based PQ.  Program 9.6: sort with a PQ (N inserts
 *  then N delmax).  Program 9.7: heapsort (bottom-up construction).
 *  Measured, on N random keys:
 *   - Property 9.2: insert <= lg N compares, delmax <= 2 lg N.
 *   - Property 9.4: bottom-up heap construction < 2N compares
 *                   (vs ~N lg N worst for N successive inserts).
 *   - Property 9.5: heapsort < 2N lg N compares.
 *   - Property 9.6: k largest via heap: ~2N + 2k lg N.
 *   - Floyd's variant mentioned in the text: on delmax, sift the hole
 *     all the way down (1 compare per level), then fixUp — fewer compares.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 9.3-9.5, 9.7
 *  Build:   gcc -std=c99 -Wall -O2 heap.c -o heap -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef int Item;
static long C;                                   /* comparison counter */
#define less(A, B) (C++, (A) < (B))
#define exch(A, B) { Item t = A; A = B; B = t; }

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

/* Program 9.6: sort by inserting everything into a PQ, then delmax */
void PQsort(Item a[], int l, int r) {
    int k;
    PQinit(r - l + 1);
    for (k = l; k <= r; k++) PQinsert(a[k]);
    for (k = r; k >= l; k--) a[k] = PQdelmax();
    free(pq);
}

/* Floyd: move the larger child up all the way to the bottom, then fixUp */
void heapsort_floyd(Item a[], int l, int r) {
    int k, n = r - l + 1;
    Item *h = a + l - 1;
    for (k = n / 2; k >= 1; k--) fixDown(h, k, n);
    while (n > 1) {
        Item v = h[n]; h[n] = h[1]; n--;
        int j = 1;
        while (2 * j <= n) {                       /* hole goes to a leaf */
            int c = 2 * j;
            if (c < n && less(h[c], h[c + 1])) c++;
            h[j] = h[c]; j = c;
        }
        h[j] = v; fixUp(h, j);                     /* usually rises little */
    }
}

static void check(Item *a, int n) { for (int i = 1; i < n; i++) if (a[i-1] > a[i]) { puts("BUG"); exit(1); } }

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

    int M = 100000; double lg = log2((double)M);
    Item *b = malloc((M + 1) * sizeof *b), *c = malloc((M + 1) * sizeof *c);
    srand(5); for (int i = 0; i < M; i++) b[i] = rand();
    printf("\nN=%d, lg N=%.1f\n", M, lg);

    /* worst single insert / delmax */
    PQinit(M); long worst_ins = 0, worst_del = 0;
    for (int i = 0; i < M; i++) { C = 0; PQinsert(b[i]); if (C > worst_ins) worst_ins = C; }
    for (int i = 0; i < M; i++) { C = 0; PQdelmax(); if (C > worst_del) worst_del = C; }
    free(pq);
    printf("worst insert %ld compares (<= lg N), worst delmax %ld (<= 2 lg N)\n", worst_ins, worst_del);

    /* construction: N inserts vs bottom-up */
    for (int i = 0; i < M; i++) c[i + 1] = b[i];
    C = 0; for (int k = 2; k <= M; k++) fixUp(c, k);
    printf("build by %d inserts : %8ld compares (random keys)\n", M, C);
    for (int i = 1; i <= M; i++) c[i] = i;
    C = 0; for (int k = 2; k <= M; k++) fixUp(c, k);
    printf("build by %d inserts : %8ld compares (increasing keys: ~N lg N)\n", M, C);
    for (int i = 0; i < M; i++) c[i + 1] = b[i];
    C = 0; for (int k = M / 2; k >= 1; k--) fixDown(c, k, M);
    printf("build bottom-up    : %8ld compares (< 2N = %d)\n", C, 2 * M);

    for (int i = 0; i < M; i++) c[i] = b[i];
    C = 0; PQsort(c, 0, M - 1); check(c, M);
    printf("PQsort (9.6)       : %8ld compares\n", C);
    for (int i = 0; i < M; i++) c[i] = b[i];
    C = 0; heapsort(c, 0, M - 1); check(c, M);
    printf("heapsort (9.7)     : %8ld compares (< 2N lg N = %.0f)\n", C, 2 * M * lg);
    for (int i = 0; i < M; i++) c[i] = b[i];
    C = 0; heapsort_floyd(c, 0, M - 1); check(c, M);
    printf("heapsort, Floyd    : %8ld compares (about N lg N = %.0f)\n", C, M * lg);

    /* Property 9.6: 10 largest */
    int k = 10;
    for (int i = 0; i < M; i++) c[i + 1] = b[i];
    C = 0; for (int j = M / 2; j >= 1; j--) fixDown(c, j, M);
    int hn = M; printf("10 largest:");
    for (int i = 0; i < k; i++) { printf(" %d", c[1]); exch(c[1], c[hn]); fixDown(c, 1, --hn); }
    printf("\n  using %ld compares (2N + 2k lg N = %.0f)\n", C, 2.0 * M + 2 * k * lg);
    free(b); free(c);
    return 0;
}
