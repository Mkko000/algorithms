/* ============================================================
 *  ex9_54_binomial.c  —  Exercises 9.54, 9.56, 9.57, 9.60
 *  Chapter 9: Priority Queues and Heapsort — binomial queues
 *
 *  9.54 Draw a binomial queue of size 29 (binomial-tree representation).
 *  9.56 The binomial queue that results when E A S Y Q U E S T I O N are
 *       inserted into an initially empty binomial queue.
 *  9.57 Build EASY and QUESTION separately, delete the maximum from each,
 *       then join the two results.
 *  9.60 Prove that a binomial tree with 2^n nodes has C(n,i) nodes at
 *       level i, 0 <= i <= n.
 *
 *  Trees are printed as  key(child child ...)  with the children of a node
 *  in decreasing order of size (B_{k-1} first).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 9.13-9.16, Exercises 9.54-9.60
 *  Build:   gcc -std=c99 -Wall -O2 ex9_54_binomial.c -o ex9_54
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

#define MAXB 20
typedef struct node *link;
struct node { int key; link child, sib; };       /* child = largest subtree */
typedef struct { link t[MAXB]; } BQ;               /* t[k] = binomial tree B_k or NULL */

static link NEW(int k) { link x = malloc(sizeof *x); x->key = k; x->child = x->sib = NULL; return x; }
static link pair(link a, link b)                 /* two B_k -> one B_{k+1} */
{
    if (a->key < b->key) { link t = a; a = b; b = t; }
    b->sib = a->child; a->child = b;
    return a;
}
static void join(BQ *a, BQ *b)                    /* binary addition with carry */
{
    link c = NULL;
    for (int k = 0; k < MAXB; k++) {
        link x = a->t[k], y = b->t[k]; b->t[k] = NULL;
        int n = (x != NULL) + (y != NULL) + (c != NULL);
        if (n == 0) continue;
        if (n == 1) { a->t[k] = x ? x : y ? y : c; c = NULL; }
        else if (n == 2) { link p = x ? x : y, q = (x && y) ? y : c; a->t[k] = NULL; c = pair(p, q); }
        else { a->t[k] = c; c = pair(x, y); }
    }
}
static void insert(BQ *q, int key) { BQ one = { { NULL } }; one.t[0] = NEW(key); join(q, &one); }
static int delmax(BQ *q)
{
    int m = -1;
    for (int k = 0; k < MAXB; k++) if (q->t[k] && (m < 0 || q->t[k]->key > q->t[m]->key)) m = k;
    link r = q->t[m]; q->t[m] = NULL;
    BQ kids = { { NULL } }; int k = m - 1;
    for (link c = r->child; c; k--) { link nx = c->sib; c->sib = NULL; kids.t[k] = c; c = nx; }
    int key = r->key; free(r);
    join(q, &kids);
    return key;
}
static void pt(link x, int ch)
{
    if (ch) printf("%c", x->key); else printf("%d", x->key);
    if (!x->child) return;
    printf("(");
    for (link c = x->child; c; c = c->sib) { pt(c, ch); if (c->sib) printf(" "); }
    printf(")");
}
static void show(const char *lab, BQ *q, int ch)
{
    printf("   %s\n", lab);
    for (int k = MAXB - 1; k >= 0; k--) if (q->t[k]) { printf("      B%d: ", k); pt(q->t[k], ch); printf("\n"); }
}
static int levels[MAXB + 1];
static void count(link x, int d) { levels[d]++; for (link c = x->child; c; c = c->sib) count(c, d + 1); }
static long binom(int n, int i) { long r = 1; for (int j = 1; j <= i; j++) r = r * (n - i + j) / j; return r; }

int main(void)
{
    BQ q = { { NULL } };
    for (int i = 29; i >= 1; i--) insert(&q, i);
    printf("9.54 N = 29 = binary 11101:\n");
    for (int k = MAXB - 1; k >= 0; k--) if (q.t[k]) printf("      B%d (%d nodes)\n", k, 1 << k);
    show("with keys 1..29 inserted in decreasing order:", &q, 0);

    BQ e = { { NULL } }; const char *s = "EASYQUESTION";
    for (int i = 0; s[i]; i++) insert(&e, s[i]);
    show("9.56 after inserting EASYQUESTION (N = 12 = 1100):", &e, 1);

    BQ a = { { NULL } }, b = { { NULL } };
    for (const char *p = "EASY"; *p; p++) insert(&a, *p);
    for (const char *p = "QUESTION"; *p; p++) insert(&b, *p);
    show("9.57 EASY:", &a, 1); show("QUESTION:", &b, 1);
    int ma = delmax(&a), mb = delmax(&b);
    printf("   delete max: %c from EASY, %c from QUESTION\n", ma, mb);
    show("EASY after delete max (N=3 = 11):", &a, 1); show("QUESTION after delete max (N=7 = 111):", &b, 1);
    join(&a, &b); show("join of the two (N=10 = 1010):", &a, 1);

    printf("9.60 nodes per level of B_n vs C(n,i):\n");
    int ok = 1;
    for (int n = 0; n <= 12; n++) {
        BQ t = { { NULL } }; for (int i = 0; i < (1 << n); i++) insert(&t, i);
        for (int i = 0; i <= MAXB; i++) levels[i] = 0;
        count(t.t[n], 0);
        if (n == 4 || n == 6) { printf("     B%d:", n); for (int i = 0; i <= n; i++) printf(" %d", levels[i]); printf("   C(%d,i):", n); for (int i = 0; i <= n; i++) printf(" %ld", binom(n, i)); printf("\n"); }
        for (int i = 0; i <= n; i++) if (levels[i] != binom(n, i)) ok = 0;
    }
    printf("     equal for every n <= 12: %s\n", ok ? "yes" : "NO");
    return 0;
}
