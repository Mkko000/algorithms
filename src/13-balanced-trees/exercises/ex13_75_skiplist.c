/* ============================================================
 *  ex13_75_skiplist.c  —  Exercises 13.75, 13.76, 13.81
 *  Chapter 13: Balanced Trees — skip lists
 *
 *  13.75 The skip list from inserting E A S Y Q U T I O N when randX()
 *        returns 1, 3, 1, 1, 2, 2, 1, 4, 1, 1.
 *  13.76 The same for A E I N O Q S T U Y (already in order).
 *  13.81 How many random numbers are needed, on the average, to build a
 *        skip list with parameter t, using randX() from Program 13.9?
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 13.7-13.9, Exercises 13.75-13.81
 *  Build:   gcc -std=c99 -Wall -O2 ex13_75_skiplist.c -o ex13_75
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

#define lgNmax 8
typedef struct STnode *link;
struct STnode { int key; link *next; int sz; };
static link head; static int lgN;
static long C;
static link NEW(int k, int n) { link x = malloc(sizeof *x); x->next = calloc(n, sizeof(link)); x->key = k; x->sz = n; return x; }
static void insertR(link t, link x, int k)               /* Program 13.9 */
{
    int v = x->key;
    link tk = t->next[k];
    if (tk == NULL || (C++, v < tk->key)) {
        if (k < x->sz) { x->next[k] = tk; t->next[k] = x; }
        if (k == 0) return;
        insertR(t, x, k - 1); return;
    }
    insertR(tk, x, k);
}
static void insert(int v, int lev) { link x = NEW(v, lev); if (lev > lgN) lgN = lev; insertR(head, x, lgN - 1); }
static void show(void)
{
    for (int k = lgN - 1; k >= 0; k--) {
        printf("       level %d: head", k + 1);
        /* align: walk level 0 and print a key only if it has level > k */
        for (link x = head->next[0]; x; x = x->next[0]) printf(x->sz > k ? " -> %c" : "     ", x->key);
        printf("\n");
    }
}
static void build(const char *s, const int *lev)
{
    head = NEW(0, lgNmax); lgN = 0; C = 0;
    for (int i = 0; s[i]; i++) insert(s[i], lev[i]);
    show();
    printf("       %ld key compares to build\n", C);
}
/* Program 13.9's randX: ONE random number decides the level */
static long calls;
static int randX_book(int t)
{
    int i, j; long r = rand(); calls++;
    for (i = 1, j = t; i < lgNmax; i++, j *= t) if (r > RAND_MAX / j) break;
    return i;
}
static int randX_coins(int t)                             /* alternative: flip a 1/t coin per level */
{
    int i = 1; calls++;
    while (i < lgNmax && rand() % t == 0) { i++; calls++; }
    return i;
}

int main(void)
{
    int lev[10] = { 1, 3, 1, 1, 2, 2, 1, 4, 1, 1 };
    printf("13.75 EASYQUTION with levels 1 3 1 1 2 2 1 4 1 1:\n");  build("EASYQUTION", lev);
    printf("13.76 AEINOQSTUY with the same levels:\n");            build("AEINOQSTUY", lev);

    printf("13.81 random numbers used per insertion (N = 100000):\n");
    for (int t = 2; t <= 8; t *= 2) {
        int N = 100000; long hist[lgNmax + 1] = { 0 };
        srand(1); calls = 0; for (int i = 0; i < N; i++) hist[randX_book(t)]++;
        double a = (double)calls / N;
        srand(1); calls = 0; for (int i = 0; i < N; i++) randX_coins(t);
        printf("     t=%d: Program 13.9 %.3f per insert;  coin flipping %.3f (t/(t-1) = %.3f);  levels 1/2/3: %.3f %.3f %.3f\n",
               t, a, (double)calls / N, t / (t - 1.0), hist[1] / (double)N, hist[2] / (double)N, hist[3] / (double)N);
    }
    return 0;
}
