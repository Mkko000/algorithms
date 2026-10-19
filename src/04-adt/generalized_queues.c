/* ============================================================
 *  generalized_queues.c  —  Beyond LIFO/FIFO: random queue, deque,
 *                           and the duplicate-item policies
 *  Chapter 4: Abstract Data Types (Sections 4.6, 4.7)
 *
 *  Section 4.6: a "generalized queue" is any ADT with insert + delete,
 *  where the RULE for which item leaves defines the ADT:
 *     stack = newest, FIFO queue = oldest, random queue = random,
 *     priority queue = largest (Chapter 9), symbol table = by key.
 *  Here: a random queue (O(1) get: take a random slot and fill the
 *  hole with the last item) and a deque (double-ended queue).
 *
 *  Section 4.7: what if a client inserts an item that is already in the
 *  structure? Two policies: "ignore the new item" or "forget the old
 *  item". With INDEX items (keys are small ints 0..maxN-1) we can test
 *  membership in O(1) with an array t[] — Program 4.12 does this for
 *  a stack. We implement both policies.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Sections 4.6-4.7, Program 4.12
 *  Build:   gcc -std=c99 -Wall -O2 generalized_queues.c -o generalized_queues
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

/* ---------------- random queue ---------------- */
#define RQMAX 100
static int rq[RQMAX], rqN;
void RQput(int x) { rq[rqN++] = x; }
int  RQget(void) {                      /* pick a random slot, fill the hole with the last */
    int i = rand() % rqN, x = rq[i];
    rq[i] = rq[--rqN];
    return x;
}

/* ---------------- deque on a circular array ---------------- */
#define DQMAX 16
static int dq[DQMAX], dqh = 0, dqn = 0;
void DQpushFront(int x) { dqh = (dqh - 1 + DQMAX) % DQMAX; dq[dqh] = x; dqn++; }
void DQpushBack(int x)  { dq[(dqh + dqn) % DQMAX] = x; dqn++; }
int  DQpopFront(void)   { int x = dq[dqh]; dqh = (dqh + 1) % DQMAX; dqn--; return x; }
int  DQpopBack(void)    { dqn--; return dq[(dqh + dqn) % DQMAX]; }

/* ------- Program 4.12: index-item stack, "ignore the new item" ------- */
#define maxN 10
static int s[maxN], t[maxN], N;             /* t[x] == 1  <=>  x is on the stack */
void STACKinit(void) { N = 0; for (int i = 0; i < maxN; i++) t[i] = 0; }
void STACKpush_ignore(int item) {
    if (t[item] == 1) return;               /* already there: ignore the new one */
    s[N++] = item; t[item] = 1;
}
int STACKpop(void) { N--; t[s[N]] = 0; return s[N]; }

/* ------- "forget the old item": move it to the top ------- */
void STACKpush_forget(int item) {
    if (t[item] == 1) {                      /* remove the old copy (O(N) shift) */
        int k = 0; while (s[k] != item) k++;
        for (; k < N - 1; k++) s[k] = s[k + 1];
        N--;
    }
    s[N++] = item; t[item] = 1;
}
static void show(const char *m) { printf("%-28s bottom->top:", m); for (int i = 0; i < N; i++) printf(" %d", s[i]); putchar('\n'); }

int main(void) {
    srand(3);
    for (int i = 1; i <= 8; i++) RQput(i * 11);
    printf("random queue gets:"); while (rqN) printf(" %d", RQget()); putchar('\n');

    DQpushBack(1); DQpushBack(2); DQpushFront(0); DQpushFront(-1);
    int a = DQpopFront(), b = DQpopBack();      /* separate statements: C does not */
    int c = DQpopFront(), d = DQpopBack();      /* fix argument evaluation order   */
    printf("deque (-1 0 1 2): popFront=%d popBack=%d popFront=%d popBack=%d\n", a, b, c, d);

    int seq[] = {3, 5, 3, 7, 5, 1};
    STACKinit(); for (int i = 0; i < 6; i++) STACKpush_ignore(seq[i]); show("ignore-new  (3 5 3 7 5 1)");
    printf("  pop -> %d\n", STACKpop());
    STACKinit(); for (int i = 0; i < 6; i++) STACKpush_forget(seq[i]); show("forget-old  (3 5 3 7 5 1)");
    return 0;
}
