/* ============================================================
 *  ex12_70_root_ops.c  —  Exercises 12.70, 12.71, 12.74, 12.76, 12.77
 *  Chapter 12: Symbol Tables and BSTs — root insertion, delete, join
 *
 *  12.70 The BST from inserting E A S Y Q U E S T I O N by root insertion.
 *  12.71 A sequence of the 10 keys A..J that needs the maximum number of
 *        comparisons to build a tree by root insertion; the count.
 *  12.74 Search costs for the N/10 most recently inserted keys, standard
 *        insertion vs root insertion.
 *  12.76 Insert E A S Y Q U T I O N, then delete Q (Program 12.15).
 *  12.77 Build EASY and QUESTION in two trees, then join them (Program 12.16).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 12.11-12.16, Exercises 12.70-12.77
 *  Build:   gcc -std=c99 -Wall -O2 ex12_70_root_ops.c -o ex12_70 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct node *link;
struct node { int key; link l, r; };
static long C;
static struct node pool[2200000]; static int np;
static link NEW(int k) { link x = &pool[np++]; x->key = k; x->l = x->r = NULL; return x; }
static link rotR(link h) { link x = h->l; h->l = x->r; x->r = h; return x; }   /* Program 12.11 */
static link rotL(link h) { link x = h->r; h->r = x->l; x->l = h; return x; }
static link insertT(link h, int k)                                             /* Program 12.12 */
{
    if (!h) return NEW(k);
    C++;
    if (k < h->key) { h->l = insertT(h->l, k); h = rotR(h); }
    else            { h->r = insertT(h->r, k); h = rotL(h); }
    return h;
}
static link insert(link h, int k) { if (!h) return NEW(k); C++; if (k < h->key) h->l = insert(h->l, k); else h->r = insert(h->r, k); return h; }
static int size(link h) { return h ? 1 + size(h->l) + size(h->r) : 0; }
static link partR(link h, int k)                     /* bring the k-th smallest to the root */
{
    int t = size(h->l);
    if (t > k) { h->l = partR(h->l, k); h = rotR(h); }
    if (t < k) { h->r = partR(h->r, k - t - 1); h = rotL(h); }
    return h;
}
static link joinLR(link a, link b) { if (!b) return a; b = partR(b, 0); b->l = a; return b; }   /* Program 12.15 helper */
static link delete(link h, int k)
{
    if (!h) return NULL;
    if (k < h->key) h->l = delete(h->l, k);
    else if (h->key < k) h->r = delete(h->r, k);
    else h = joinLR(h->l, h->r);
    return h;
}
static link join(link a, link b)                                               /* Program 12.16 */
{
    if (!b) return a;
    if (!a) return b;
    link al = a->l, ar = a->r;
    b = insertT(b, a->key);
    b->l = join(al, b->l);
    b->r = join(ar, b->r);
    return b;
}
static void show(link h) { if (!h) { printf("-"); return; } printf("%c", h->key); if (h->l || h->r) { printf("("); show(h->l); printf(","); show(h->r); printf(")"); } }
static int depth_of(link h, int k) { int d = 1; while (h && h->key != k) { h = k < h->key ? h->l : h->r; d++; } return d; }
static unsigned long long rs = 777;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }

int main(void)
{
    const char *s = "EASYQUESTION"; link t = NULL; C = 0;
    for (int i = 0; s[i]; i++) t = insertT(t, s[i]);
    printf("12.70 root insertion of EASYQUESTION: "); show(t); printf("   (%ld compares)\n", C);

    /* 12.71: all 10! orders of A..J */
    {
        int p[10]; for (int i = 0; i < 10; i++) p[i] = 'A' + i;
        long best = -1; int bp[10]; long cntmax = 0;
        for (;;) {
            np = 0; t = NULL; C = 0;
            for (int i = 0; i < 10; i++) t = insertT(t, p[i]);
            if (C > best) { best = C; cntmax = 0; for (int i = 0; i < 10; i++) bp[i] = p[i]; }
            if (C == best) cntmax++;
            int i = 8; while (i >= 0 && p[i] > p[i + 1]) i--;
            if (i < 0) break;
            int j = 9; while (p[j] < p[i]) j--;
            int x = p[i]; p[i] = p[j]; p[j] = x;
            for (int a = i + 1, b = 9; a < b; a++, b--) { x = p[a]; p[a] = p[b]; p[b] = x; }
        }
        printf("12.71 max compares to build by root insertion: %ld, e.g. ", best);
        for (int i = 0; i < 10; i++) printf("%c", bp[i]);
        printf("; %ld of the 10! orders reach it\n", cntmax);
    }

    printf("12.74 searches for the N/10 most recently inserted keys (average compares):\n");
    for (int N = 1000; N <= 1000000; N *= 10) {
        static int k[1000000];
        for (int i = 0; i < N; i++) k[i] = (int)rnd();
        double avg[2], sd[2];
        for (int m = 0; m < 2; m++) {
            np = 0; t = NULL;
            for (int i = 0; i < N; i++) t = m ? insertT(t, k[i]) : insert(t, k[i]);
            double s1 = 0, s2 = 0;
            for (int q = 0; q < N; q++) { int key = k[N - 1 - (int)(rnd() % (N / 10))]; double d = depth_of(t, key); s1 += d; s2 += d * d; }
            avg[m] = s1 / N; sd[m] = sqrt(s2 / N - avg[m] * avg[m]);
        }
        printf("     N=%7d: standard %6.2f (sd %.2f)   root insertion %6.2f (sd %.2f)\n", N, avg[0], sd[0], avg[1], sd[1]);
    }

    np = 0; t = NULL; const char *s2 = "EASYQUTION";
    for (int i = 0; s2[i]; i++) t = insert(t, s2[i]);
    printf("12.76 BST of EASYQUTION:  "); show(t); printf("\n");
    t = delete(t, 'Q');
    printf("      after deleting Q:   "); show(t); printf("\n");

    np = 0; link a = NULL, b = NULL;
    for (const char *c = "EASY"; *c; c++) a = insert(a, *c);
    for (const char *c = "QUESTION"; *c; c++) b = insert(b, *c);
    printf("12.77 EASY: "); show(a); printf("   QUESTION: "); show(b); printf("\n");
    t = join(a, b);
    printf("      join: "); show(t); printf("\n");
    return 0;
}
