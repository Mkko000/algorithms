/* ============================================================
 *  balanced_compare.c  —  Empirical study of balanced trees
 *  Chapter 13: Balanced Trees (Section 13.6, Table 13.1)
 *
 *  Builds a symbol table of N random 32-bit keys with each method and
 *  then does N search misses, timing both phases and measuring the
 *  average search path, like Table 13.1 in the book:
 *    B  standard BST (Program 12.9 style)
 *    R  randomized BST (Program 13.2)
 *    S  splay BST (Program 13.5; searches splay too)
 *    C  red-black BST (Program 13.6)
 *    L  skip list, t = 2 (Programs 13.7-13.9)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Section 13.6
 *  Build:   gcc -std=c99 -Wall -O2 balanced_compare.c -o balanced_compare
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned Key;
typedef struct node *link;
struct node { Key key; link l, r; int N; int red; };
static link z;
static long cmps;

static link NEW(Key k, link l, link r, int N) { link x = malloc(sizeof *x); x->key = k; x->l = l; x->r = r; x->N = N; x->red = 1; return x; }
static void fixN(link h) { h->N = h->l->N + h->r->N + 1; }
static link rotR(link h) { link x = h->l; h->l = x->r; x->r = h; fixN(h); fixN(x); return x; }
static link rotL(link h) { link x = h->r; h->r = x->l; x->l = h; fixN(h); fixN(x); return x; }
static void freeT(link h) { if (h == z) return; freeT(h->l); freeT(h->r); free(h); }
static int search(link h, Key v) { while (h != z) { cmps++; if (v == h->key) return 1; h = v < h->key ? h->l : h->r; } return 0; }

/* B: standard */
static link insB(link h, Key v) { if (h == z) return NEW(v, z, z, 1); if (v < h->key) h->l = insB(h->l, v); else h->r = insB(h->r, v); h->N++; return h; }
/* R: randomized */
static link insT(link h, Key v) { if (h == z) return NEW(v, z, z, 1);
    if (v < h->key) { h->l = insT(h->l, v); return rotR(h); } h->r = insT(h->r, v); return rotL(h); }
static link insR(link h, Key v) { if (h == z) return NEW(v, z, z, 1);
    if (rand() < RAND_MAX / (h->N + 1)) return insT(h, v);
    if (v < h->key) h->l = insR(h->l, v); else h->r = insR(h->r, v); h->N++; return h; }
/* S: splay (Program 13.5); search = splay then check the root */
static link splay(link h, Key v) {
    if (h == z) return NEW(v, z, z, 1);
    if (v < h->key) {
        if (h->l == z) return rotR((h->l = NEW(v, z, z, 1), fixN(h), h));
        if (v < h->l->key) { h->l->l = splay(h->l->l, v); h = rotR(h); }
        else { h->l->r = splay(h->l->r, v); h->l = rotL(h->l); }
        return rotR(h);
    } else {
        if (h->r == z) return rotL((h->r = NEW(v, z, z, 1), fixN(h), h));
        if (h->r->key < v) { h->r->r = splay(h->r->r, v); h = rotL(h); }
        else { h->r->l = splay(h->r->l, v); h->r = rotR(h->r); }
        return rotL(h);
    }
}
/* splay search: splay the last node on the search path to the root (top-down-free version) */
static link splaySearch(link h, Key v, int *hit) {
    if (h == z) { *hit = 0; return h; }
    cmps++;
    if (v == h->key) { *hit = 1; return h; }
    if (v < h->key) {
        if (h->l == z) { *hit = 0; return h; }
        cmps++;
        if (v == h->l->key) { *hit = 1; return rotR(h); }
        if (v < h->l->key) { h->l->l = splaySearch(h->l->l, v, hit); h = rotR(h); }
        else { h->l->r = splaySearch(h->l->r, v, hit); if (h->l->r != z) h->l = rotL(h->l); }
        return h->l == z ? h : rotR(h);
    } else {
        if (h->r == z) { *hit = 0; return h; }
        cmps++;
        if (v == h->r->key) { *hit = 1; return rotL(h); }
        if (h->r->key < v) { h->r->r = splaySearch(h->r->r, v, hit); h = rotL(h); }
        else { h->r->l = splaySearch(h->r->l, v, hit); if (h->r->l != z) h->r = rotR(h->r); }
        return h->r == z ? h : rotL(h);
    }
}
/* C: red-black (Program 13.6) */
static link RB(link h, Key v, int sw) {
    if (h == z) return NEW(v, z, z, 1);
    if (h->l->red && h->r->red) { h->red = 1; h->l->red = 0; h->r->red = 0; }
    if (v < h->key) { h->l = RB(h->l, v, 0);
        if (h->red && h->l->red && sw) h = rotR(h);
        if (h->l->red && h->l->l->red) { h = rotR(h); h->red = 0; h->r->red = 1; } }
    else { h->r = RB(h->r, v, 1);
        if (h->red && h->r->red && !sw) h = rotL(h);
        if (h->r->red && h->r->r->red) { h = rotL(h); h->red = 0; h->l->red = 1; } }
    fixN(h); return h;
}
/* L: skip list */
#define lgNmax 30
typedef struct snode *slink;
struct snode { Key key; slink *next; int sz; };
static slink shead; static int slg;
static slink SNEW(Key k, int n) { slink x = malloc(sizeof *x); x->next = malloc(n * sizeof(slink)); x->key = k; x->sz = n; for (int i = 0; i < n; i++) x->next[i] = NULL; return x; }
static int randX(void) { int i, j, t = rand(); for (i = 1, j = 2; i < lgNmax; i++, j += j) if (t > RAND_MAX / j) break; if (i > slg) slg = i; return i; }
static void sinsert(slink t, slink x, int k) {
    Key v = x->key; slink tk = t->next[k];
    if (tk == NULL || v < tk->key) { if (k < x->sz) { x->next[k] = tk; t->next[k] = x; } if (k == 0) return; sinsert(t, x, k - 1); return; }
    sinsert(tk, x, k);
}
static int ssearch(Key v) { slink t = shead; for (int k = slg - 1; k >= 0; k--) { while (t->next[k] && (cmps++, t->next[k]->key < v)) t = t->next[k]; } cmps++; return t->next[0] && t->next[0]->key == v; }
static void sfree(void) { slink t = shead; while (t) { slink n = t->next[0]; free(t->next); free(t); t = n; } }

int main(void) {
    z = NEW(0, NULL, NULL, 0); z->l = z->r = z; z->red = 0;
    printf("%8s | %-34s | %-34s\n", "", "construction time (s)", "N search misses: time (s) / avg compares");
    printf("%8s | %6s %6s %6s %6s %6s | %s\n", "N", "B", "R", "S", "C", "L", "B R S C L");
    for (int N = 50000; N <= 800000; N *= 4) {
        Key *a = malloc(N * sizeof(Key)), *m = malloc(N * sizeof(Key));
        srand(N);
        for (int i = 0; i < N; i++) { a[i] = (Key)rand() * 2u; m[i] = (Key)rand() * 2u + 1u; }   /* misses are odd */
        double tb[5], ts[5], ac[5];
        for (int meth = 0; meth < 5; meth++) {
            link h = z; clock_t c0 = clock();
            if (meth == 4) { shead = SNEW(0, lgNmax); slg = 0; for (int i = 0; i < N; i++) sinsert(shead, SNEW(a[i], randX()), lgNmax - 1); }
            else for (int i = 0; i < N; i++) {
                if (meth == 0) h = insB(h, a[i]);
                else if (meth == 1) h = insR(h, a[i]);
                else if (meth == 2) h = splay(h, a[i]);
                else { h = RB(h, a[i], 0); h->red = 0; }
            }
            clock_t c1 = clock(); cmps = 0; int found = 0;
            for (int i = 0; i < N; i++) {
                if (meth == 4) found += ssearch(m[i]);
                else if (meth == 2) { int hit; h = splaySearch(h, m[i], &hit); found += hit; }
                else found += search(h, m[i]);
            }
            clock_t c2 = clock();
            if (found) printf("unexpected hit\n");
            tb[meth] = (double)(c1 - c0) / CLOCKS_PER_SEC; ts[meth] = (double)(c2 - c1) / CLOCKS_PER_SEC; ac[meth] = (double)cmps / N;
            if (meth == 4) sfree(); else freeT(h);
        }
        printf("%8d | %6.3f %6.3f %6.3f %6.3f %6.3f | %.3f %.3f %.3f %.3f %.3f  /  %.1f %.1f %.1f %.1f %.1f\n", N,
               tb[0], tb[1], tb[2], tb[3], tb[4], ts[0], ts[1], ts[2], ts[3], ts[4], ac[0], ac[1], ac[2], ac[3], ac[4]);
        free(a); free(m);
    }
    free(z);
    return 0;
}
