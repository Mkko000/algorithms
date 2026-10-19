/* ============================================================
 *  ex14_17_chaining.c  —  Exercises 14.5, 14.16-14.19
 *  Chapter 14: Hashing — separate chaining
 *
 *  14.5  Compute the chi-square statistic for N hash values in a table of
 *        size M:  X^2 = (M/N) sum (f_i - N/M)^2, which should be M +- 2 sqrt(M).
 *  14.16 Worst-case time to insert N keys with separate chaining, (i) with
 *        unordered lists and (ii) with ordered lists.
 *  14.17 Contents of a table of M = 5 lists after inserting E A S Y Q U T I O N,
 *        separate chaining with unordered lists, hash 11k mod M (k = letter number).
 *  14.18 The same with ordered lists.  Does the insertion order matter?
 *  14.19 Insert N random integers into a table of size N/100; find the
 *        shortest and longest lists, N = 10^3 .. 10^6.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 14.3, Exercises 14.5-14.19
 *  Build:   gcc -std=c99 -Wall -O2 ex14_17_chaining.c -o ex14_17 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct node *link;
struct node { int key; link next; };
static link NEW(int k, link n) { link x = malloc(sizeof *x); x->key = k; x->next = n; return x; }
static long C;
static int hash(int c, int M) { return (11 * (c - 'A' + 1)) % M; }

static void show(link *h, int M, int letters)
{
    for (int i = 0; i < M; i++) {
        printf("       %d:", i);
        for (link x = h[i]; x; x = x->next) printf(letters ? " %c" : " %d", x->key);
        printf("\n");
    }
}
static link insert_ordered(link h, int k)
{
    if (!h || (C++, k < h->key)) return NEW(k, h);
    link p = h;
    while (p->next && (C++, p->next->key <= k)) p = p->next;
    p->next = NEW(k, p->next);
    return h;
}
static unsigned long long rs = 2718281;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }

int main(void)
{
    const char *s = "EASYQUTION"; int M = 5;
    link h[5] = { NULL };
    printf("14.17 hash values 11k mod 5:");
    for (int i = 0; s[i]; i++) printf(" %c=%d", s[i], hash(s[i], M));
    printf("\n     unordered lists (insert at the front, Program 14.3):\n");
    for (int i = 0; s[i]; i++) { int j = hash(s[i], M); h[j] = NEW(s[i], h[j]); }
    show(h, M, 1);
    printf("14.18 ordered lists:\n");
    link o[5] = { NULL }, r[5] = { NULL };
    for (int i = 0; s[i]; i++) { int j = hash(s[i], M); o[j] = insert_ordered(o[j], s[i]); }
    show(o, M, 1);
    for (int i = 9; i >= 0; i--) { int j = hash(s[i], M); r[j] = insert_ordered(r[j], s[i]); }
    int same = 1;
    for (int i = 0; i < M; i++) { link a = o[i], b = r[i]; while (a && b && a->key == b->key) { a = a->next; b = b->next; } if (a || b) same = 0; }
    printf("     inserting in reverse order gives the same table: %s\n", same ? "yes" : "no");

    printf("14.16 worst case (all N keys hash to the same list):\n");
    for (int N = 1000; N <= 8000; N *= 2) {
        link one = NULL; long cu, co;
        C = 0; for (int i = 0; i < N; i++) one = insert_ordered(one, i); co = C;     /* ascending: walk to the end */
        cu = 0;                                                                     /* unordered: insert at front, no compares; */
        long cu_check = 0; for (int i = 0; i < N; i++) cu_check += i;               /* with a duplicate check it would be ~N^2/2 */
        printf("     N=%5d: unordered %ld compares (front insert; %ld with a search-before-insert)   ordered %ld (N^2/2 = %ld)\n",
               N, cu, cu_check, co, (long)N * N / 2);
    }

    printf("14.19 N random keys, M = N/100 lists (average length 100):\n");
    for (int N = 1000; N <= 1000000; N *= 10) {
        int m = N / 100; int *cnt = calloc(m, sizeof *cnt);
        for (int i = 0; i < N; i++) cnt[rnd() % m]++;
        int mn = N, mx = 0; double chi = 0;
        for (int i = 0; i < m; i++) { if (cnt[i] < mn) mn = cnt[i]; if (cnt[i] > mx) mx = cnt[i]; chi += (cnt[i] - 100.0) * (cnt[i] - 100.0); }
        chi *= (double)m / N;
        printf("     N=%7d: M=%5d  shortest %3d  longest %3d   chi^2 = %8.1f (expect %d +- %.0f)  (14.5)\n", N, m, mn, mx, chi, m, 2 * sqrt(m));
        free(cnt);
    }
    {   /* 14.5 on a BAD hash: keys that are multiples of 10 into M = 100 */
        int m = 100, N = 100000; int cnt[100] = { 0 }; double chi = 0;
        for (int i = 0; i < N; i++) cnt[(10 * (int)(rnd() % 100000)) % m]++;
        for (int i = 0; i < m; i++) chi += (cnt[i] - (double)N / m) * (cnt[i] - (double)N / m);
        chi *= (double)m / N;
        printf("14.5 bad hash (keys multiples of 10, M = 100): chi^2 = %.0f, far outside %d +- 20\n", chi, m);
    }
    return 0;
}
