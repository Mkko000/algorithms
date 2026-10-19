/* ============================================================
 *  binomial_queue.c  —  Binomial queues: a PQ where JOIN is O(lg N)
 *  Chapter 9: Priority Queues and Heapsort (Section 9.7)
 *
 *  A power-of-2 heap (Definition 9.5) is a left-heap-ordered tree: root
 *  with an empty right link and a complete left subtree; the root holds
 *  the largest key. A binomial queue (Definition 9.6) is an array bq[]
 *  where bq[i] is either NULL or a power-of-2 heap of size 2^i — its
 *  shape is the BINARY representation of N.
 *   - pair():   join two heaps of equal size in O(1)    (Program 9.13)
 *   - insert:   like adding 1 to a binary counter       (Program 9.14)
 *   - delmax:   scan roots, remove max, its left subtree splits into a
 *               queue of smaller heaps, join them back (Program 9.15)
 *   - join:     binary ADDITION with carries            (Program 9.16)
 *  All in O(lg N) (Property 9.7); N inserts cost O(N) total (9.8).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 9.13-9.16
 *  Build:   gcc -std=c99 -Wall -O2 binomial_queue.c -o binomial_queue
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

#define maxBQsize 20
typedef int Item;
typedef struct PQnode *PQlink;
struct PQnode { Item key; PQlink l, r; };
typedef struct pq { PQlink bq[maxBQsize]; } *PQ;
static PQlink z = NULL;

static PQlink NEWnode(Item v) { PQlink t = malloc(sizeof *t); t->key = v; t->l = z; t->r = z; return t; }
PQ PQinit(void) { PQ p = malloc(sizeof *p); for (int i = 0; i < maxBQsize; i++) p->bq[i] = z; return p; }
int PQempty(PQ p) { for (int i = 0; i < maxBQsize; i++) if (p->bq[i] != z) return 0; return 1; }

/* Program 9.13: join two power-of-2 heaps of the same size */
PQlink pair(PQlink p, PQlink q) {
    if (p->key < q->key) { p->r = q->l; q->l = p; return q; }
    else                 { q->r = p->l; p->l = q; return p; }
}

/* Program 9.14: insert = binary increment */
void PQinsert(PQ pq, Item v) {
    PQlink c = NEWnode(v);
    for (int i = 0; i < maxBQsize; i++) {
        if (c == z) break;
        if (pq->bq[i] == z) { pq->bq[i] = c; break; }
        c = pair(c, pq->bq[i]); pq->bq[i] = z;
    }
}

/* Program 9.16: join = binary addition; the 3-bit case table */
#define test(C, B, A) (4 * (C) + 2 * (B) + 1 * (A))
void BQjoin(PQlink *a, PQlink *b) {
    PQlink c = z;
    for (int i = 0; i < maxBQsize; i++)
        switch (test(c != z, b[i] != z, a[i] != z)) {
            case 2: a[i] = b[i]; break;
            case 3: c = pair(a[i], b[i]); a[i] = z; break;
            case 4: a[i] = c; c = z; break;
            case 5: c = pair(c, a[i]); a[i] = z; break;
            case 6:
            case 7: c = pair(c, b[i]); break;
        }
}
void PQjoin(PQ a, PQ b) { BQjoin(a->bq, b->bq); free(b); }

/* Program 9.15: delete the maximum */
Item PQdelmax(PQ pq) {
    int i, max = -1; Item v = 0;
    PQlink x, temp[maxBQsize];
    for (i = 0; i < maxBQsize; i++)
        if (pq->bq[i] != z && (max < 0 || pq->bq[i]->key > v)) { max = i; v = pq->bq[max]->key; }
    x = pq->bq[max]->l;
    for (i = max; i < maxBQsize; i++) temp[i] = z;
    for (i = max; i > 0; i--) { temp[i - 1] = x; x = x->r; temp[i - 1]->r = z; }
    free(pq->bq[max]); pq->bq[max] = z;
    BQjoin(pq->bq, temp);
    return v;
}

static void shape(PQ p, const char *m) {
    printf("%-26s sizes present:", m);
    for (int i = maxBQsize - 1; i >= 0; i--) if (p->bq[i] != z) printf(" 2^%d", i);
    putchar('\n');
}

int main(void) {
    PQ a = PQinit(), b = PQinit();
    int keys[] = {5, 13, 2, 21, 8, 3, 34, 1, 55, 89, 7};
    for (int i = 0; i < 11; i++) PQinsert(a, keys[i]);
    shape(a, "a after 11 inserts (1011b)");
    for (int i = 0; i < 5; i++) PQinsert(b, 100 + i);
    shape(b, "b after 5 inserts (101b)");
    PQjoin(a, b);
    shape(a, "a joined with b (16=10000b)");
    printf("delmax order:");
    while (!PQempty(a)) printf(" %d", PQdelmax(a));
    putchar('\n');
    free(a);
    return 0;
}
