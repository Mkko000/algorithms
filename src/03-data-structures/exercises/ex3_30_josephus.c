/* ============================================================
 *  ex3_30_josephus.c  —  Exercises 3.28, 3.30, 3.33
 *  Chapter 3: Elementary Data Structures — solved exercises
 *
 *  3.28 Program 3.9 sets twice as many links as it needs while building
 *       the circular list; build it without the extra work.
 *  3.30 Use Program 3.9 to find the Josephus function for M = 2, 3, 5, 10
 *       and N = 10^3, 10^4, 10^5, 10^6.
 *  3.33 Do the same with an array of indices instead of pointers
 *       (Figure 3.6), and check both against the recurrence
 *       J(1) = 1, J(N) = (J(N-1) + M - 1) mod N + 1.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 3.9, Exercises 3.28-3.33
 *  Build:   gcc -std=c99 -Wall -O2 ex3_30_josephus.c -o ex3_30
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct node *link;
struct node { int item; link next; };

/* 3.28: build 1..N with each link set exactly once, then close the circle */
static int josephus_list(int N, int M)
{
    link nodes = malloc(N * sizeof *nodes), x, t;
    for (int i = 0; i < N; i++) { nodes[i].item = i + 1; nodes[i].next = &nodes[(i + 1) % N]; }
    x = &nodes[N - 1];                       /* node before 1 */
    while (x != x->next) {
        for (int i = 1; i < M; i++) x = x->next;
        t = x->next; x->next = t->next;      /* eliminate t */
    }
    int r = x->item;
    free(nodes);
    return r;
}

/* 3.33: same algorithm, list held in an array of indices */
static int josephus_array(int N, int M)
{
    int *next = malloc((N + 1) * sizeof *next), x = N;
    for (int i = 1; i <= N; i++) next[i] = i % N + 1;
    while (x != next[x]) {
        for (int i = 1; i < M; i++) x = next[x];
        next[x] = next[next[x]];
    }
    free(next);
    return x;
}

static int josephus_rec(int N, int M)
{
    int j = 1;                               /* J(1) */
    for (int n = 2; n <= N; n++) j = (int)(((long)j + M - 1) % n) + 1;
    return j;
}

int main(void)
{
    printf("small check, N=9 M=5 (Figure 3.5): %d\n", josephus_list(9, 5));
    printf("3.30  J(N,M)        N=1000     10000    100000   1000000\n");
    int ok = 1;
    int Ms[] = { 2, 3, 5, 10 };
    for (int k = 0; k < 4; k++) {
        printf("      M=%-2d        ", Ms[k]);
        for (int N = 1000; N <= 1000000; N *= 10) {
            int a = josephus_list(N, Ms[k]);
            if (a != josephus_array(N, Ms[k]) || a != josephus_rec(N, Ms[k])) ok = 0;
            printf("%9d ", a);
        }
        printf("\n");
    }
    printf("list, index-array and recurrence agree: %s\n", ok ? "yes" : "NO");
    printf("M=2 closed form 2L+1 (N = 2^m + L): N=1000 -> %d, N=10^6 -> %d\n",
           2 * (1000 - 512) + 1, 2 * (1000000 - 524288) + 1);
    return 0;
}
