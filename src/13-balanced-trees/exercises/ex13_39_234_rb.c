/* ============================================================
 *  ex13_39_234_rb.c  —  Exercises 13.39-13.42, 13.48-13.50
 *  Chapter 13: Balanced Trees — 2-3-4 trees and red-black trees
 *
 *  13.39 The balanced 2-3-4 tree from inserting E A S Y Q U T I O N with
 *        the top-down insertion method.
 *  13.40 The same with the bottom-up insertion method.
 *  13.41/13.42 Minimum and maximum heights of balanced 2-3-4 trees with N keys.
 *  13.48 The red-black BST from E A S Y Q U T I O N, top-down (Program 13.6).
 *  13.49 The same, bottom-up insertion.
 *  13.50 The red-black tree from inserting A through K in order; what
 *        happens in general for ascending keys?
 *
 *  2-3-4 trees print as [keys] with children in parentheses; red-black
 *  trees print as key(left,right) with a '*' on red nodes.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 13.6, Section 13.3, Exercises 13.39-13.50
 *  Build:   gcc -std=c99 -Wall -O2 ex13_39_234_rb.c -o ex13_39 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ---------------- 2-3-4 trees ---------------- */
typedef struct n4 *L4;
struct n4 { int n; int key[3]; L4 c[4]; };
static L4 NEW4(void) { return calloc(1, sizeof(struct n4)); }
static void split_child(L4 p, int i)                /* p->c[i] is a 4-node; p has room */
{
    L4 y = p->c[i], r = NEW4();
    r->n = 1; r->key[0] = y->key[2]; r->c[0] = y->c[2]; r->c[1] = y->c[3];
    y->n = 1; y->c[2] = y->c[3] = NULL;
    for (int j = p->n; j > i; j--) { p->key[j] = p->key[j - 1]; p->c[j + 1] = p->c[j]; }
    p->key[i] = y->key[1]; p->c[i + 1] = r; p->n++;
}
static L4 topdown(L4 root, int v)
{
    if (!root) { root = NEW4(); root->n = 1; root->key[0] = v; return root; }
    if (root->n == 3) { L4 s = NEW4(); s->c[0] = root; split_child(s, 0); root = s; }
    L4 x = root;
    for (;;) {
        int i = 0; while (i < x->n && v > x->key[i]) i++;
        if (!x->c[0]) { for (int j = x->n; j > i; j--) x->key[j] = x->key[j - 1]; x->key[i] = v; x->n++; return root; }
        if (x->c[i]->n == 3) { split_child(x, i); if (v > x->key[i]) i++; }
        x = x->c[i];
    }
}
/* bottom-up: insert at the bottom; a 4-node that receives a key splits,
   sending its middle key up (which may split the parent, and so on)     */
static int up_key; static L4 up_right;
static int bu(L4 x, int v)                          /* returns 1 if x split */
{
    int i = 0; while (i < x->n && v > x->key[i]) i++;
    int k = v; L4 rc = NULL;
    if (x->c[0]) { if (!bu(x->c[i], v)) return 0; k = up_key; rc = up_right; }
    if (x->n < 3) {
        for (int j = x->n; j > i; j--) { x->key[j] = x->key[j - 1]; x->c[j + 1] = x->c[j]; }
        x->key[i] = k; x->c[i + 1] = rc; x->n++;
        return 0;
    }
    /* x is a 4-node: split it around its middle key, then put k on its side */
    int K[4]; L4 Cc[5]; int m = 0;
    for (int j = 0; j < 3; j++) K[j] = x->key[j];
    for (int j = 0; j < 4; j++) Cc[j] = x->c[j];
    for (int j = 3; j > i; j--) { K[j] = K[j - 1]; }
    for (int j = 4; j > i + 1; j--) Cc[j] = Cc[j - 1];
    K[i] = k; Cc[i + 1] = rc; m = 4;
    /* middle of the ORIGINAL 4-node goes up: original key[1] */
    int mid = x->key[1], pos = 0; while (K[pos] != mid) pos++;
    L4 r = NEW4();
    x->n = 0; r->n = 0;
    for (int j = 0; j < pos; j++) { x->key[x->n] = K[j]; x->c[x->n] = Cc[j]; x->n++; }
    x->c[x->n] = Cc[pos]; for (int j = x->n + 1; j < 4; j++) x->c[j] = NULL;
    for (int j = pos + 1; j < m; j++) { r->key[r->n] = K[j]; r->c[r->n] = Cc[j]; r->n++; }
    r->c[r->n] = Cc[m];
    up_key = mid; up_right = r;
    return 1;
}
static L4 bottomup(L4 root, int v)
{
    if (!root) { root = NEW4(); root->n = 1; root->key[0] = v; return root; }
    if (bu(root, v)) { L4 s = NEW4(); s->n = 1; s->key[0] = up_key; s->c[0] = root; s->c[1] = up_right; root = s; }
    return root;
}
static void show4(L4 x)
{
    printf("[");
    for (int i = 0; i < x->n; i++) printf("%c", x->key[i]);
    printf("]");
    if (x->c[0]) { printf("("); for (int i = 0; i <= x->n; i++) { show4(x->c[i]); if (i < x->n) printf(" "); } printf(")"); }
}
static int same4(L4 a, L4 b)
{
    if (!a || !b) return a == b;
    if (a->n != b->n) return 0;
    for (int i = 0; i < a->n; i++) if (a->key[i] != b->key[i]) return 0;
    for (int i = 0; i <= a->n; i++) if (!same4(a->c[i], b->c[i])) return 0;
    return 1;
}
static int h4(L4 x) { return x ? 1 + h4(x->c[0]) : 0; }

/* ---------------- red-black trees ---------------- */
typedef struct rb *link;
struct rb { int key, red; link l, r, p; };
static link z;
static link NEWrb(int k, int red) { link x = malloc(sizeof *x); x->key = k; x->red = red; x->l = x->r = z; x->p = NULL; return x; }
static link rotR(link h) { link x = h->l; h->l = x->r; x->r = h; return x; }
static link rotL(link h) { link x = h->r; h->r = x->l; x->l = h; return x; }
static link RBinsert(link h, int k, int sw)           /* Program 13.6 (top-down) */
{
    if (h == z) return NEWrb(k, 1);
    if (h->l->red && h->r->red) { h->red = 1; h->l->red = 0; h->r->red = 0; }
    if (k < h->key) {
        h->l = RBinsert(h->l, k, 0);
        if (h->red && h->l->red && sw) h = rotR(h);
        if (h->l->red && h->l->l->red) { h = rotR(h); h->red = 0; h->r->red = 1; }
    } else {
        h->r = RBinsert(h->r, k, 1);
        if (h->red && h->r->red && !sw) h = rotL(h);
        if (h->r->red && h->r->r->red) { h = rotL(h); h->red = 0; h->l->red = 1; }
    }
    return h;
}
/* bottom-up (classic): insert a red leaf, then fix red-red going up */
static link broot;
static void setchild(link p, link old, link nw) { if (!p) broot = nw; else if (p->l == old) p->l = nw; else p->r = nw; nw->p = p; }
static void rotLp(link x) { link y = x->r; x->r = y->l; if (y->l != z) y->l->p = x; setchild(x->p, x, y); y->l = x; x->p = y; }
static void rotRp(link x) { link y = x->l; x->l = y->r; if (y->r != z) y->r->p = x; setchild(x->p, x, y); y->r = x; x->p = y; }
static void BUinsert(int k)
{
    link y = NULL, x = broot;
    while (x != z && x) { y = x; x = k < x->key ? x->l : x->r; }
    link n = NEWrb(k, 1); n->p = y;
    if (!y) broot = n; else if (k < y->key) y->l = n; else y->r = n;
    while (n->p && n->p->red) {
        link p = n->p, g = p->p;
        link u = (g->l == p) ? g->r : g->l;
        if (u != z && u->red) { p->red = u->red = 0; g->red = 1; n = g; continue; }       /* split = colour flip */
        if (g->l == p) { if (p->r == n) { rotLp(p); n = p; p = n->p; } p->red = 0; g->red = 1; rotRp(g); }
        else           { if (p->l == n) { rotRp(p); n = p; p = n->p; } p->red = 0; g->red = 1; rotLp(g); }
    }
    broot->red = 0;
}
static void showrb(link h) { if (h == z) { printf("-"); return; } printf("%c%s", h->key, h->red ? "*" : ""); if (h->l != z || h->r != z) { printf("("); showrb(h->l); printf(","); showrb(h->r); printf(")"); } }
static int hrb(link h) { if (h == z) return 0; int a = hrb(h->l), b = hrb(h->r); return 1 + (a > b ? a : b); }
static int bh(link h, int *ok) { if (h == z) return 1; if (h->red && (h->l->red || h->r->red)) *ok = 0; int a = bh(h->l, ok), b = bh(h->r, ok); if (a != b) *ok = 0; return a + !h->red; }

int main(void)
{
    const char *s = "EASYQUTION";
    L4 t = NULL; printf("13.39 top-down 2-3-4:\n");
    for (int i = 0; s[i]; i++) { t = topdown(t, s[i]); printf("     +%c  ", s[i]); show4(t); printf("\n"); }
    L4 b = NULL; printf("13.40 bottom-up 2-3-4:\n");
    for (int i = 0; s[i]; i++) { b = bottomup(b, s[i]); printf("     +%c  ", s[i]); show4(b); printf("\n"); }

    {   /* when do the two methods differ?  shortest prefix of a random order */
        unsigned long long rs = 11; int best = 99; char bs[32];
        for (int trial = 0; trial < 20000; trial++) {
            char p[16]; for (int i = 0; i < 12; i++) p[i] = 'A' + i;
            for (int i = 11; i > 0; i--) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; int j = (int)(rs % (unsigned long long)(i + 1)); char c = p[i]; p[i] = p[j]; p[j] = c; }
            L4 x = NULL, y = NULL;
            for (int i = 0; i < 12; i++) { x = topdown(x, p[i]); y = bottomup(y, p[i]); if (!same4(x, y)) { if (i + 1 < best) { best = i + 1; for (int k = 0; k <= i; k++) bs[k] = p[k]; bs[i + 1] = 0; } break; } }
        }
        L4 x = NULL, y = NULL; for (int i = 0; bs[i]; i++) { x = topdown(x, bs[i]); y = bottomup(y, bs[i]); }
        printf("     the methods agree on EASYQUTION; the shortest disagreement found: %s\n       top-down  ", bs); show4(x);
        printf("\n       bottom-up "); show4(y); printf("\n");
    }
    printf("13.41/13.42 a 2-3-4 tree of height h holds 2^h - 1 .. 4^h - 1 keys, so for N keys\n");
    printf("     min height = ceil(log4(N+1)), max height = floor(lg(N+1)):\n");
    int Ns[] = { 10, 100, 1000, 1000000 };
    for (int i = 0; i < 4; i++) {
        int N = Ns[i];
        int hmin = 0; while ((long)pow(4, hmin) - 1 < N) hmin++;
        int hmax = 0; while ((1L << (hmax + 1)) - 1 <= N) hmax++;
        /* build: ascending keys top-down (tends to be tall-ish) and random (check bounds) */
        L4 x = NULL; unsigned long long rs = 9; int hr;
        for (int k = 0; k < N; k++) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; x = topdown(x, (int)(rs >> 34)); }
        hr = h4(x);
        printf("     N=%7d: min %2d  max %2d   (a random top-down tree: %d)\n", N, hmin, hmax, hr);
    }

    z = NEWrb(0, 0); z->l = z->r = z;
    link h = z; printf("13.48 red-black, top-down (Program 13.6):\n");
    for (int i = 0; s[i]; i++) { h = RBinsert(h, s[i], 0); h->red = 0; printf("     +%c  ", s[i]); showrb(h); printf("\n"); }
    int ok = 1; bh(h, &ok); printf("     red-black rules hold: %s, height %d\n", ok ? "yes" : "NO", hrb(h));
    broot = NULL; printf("13.49 red-black, bottom-up:\n");
    for (int i = 0; s[i]; i++) { if (!broot) { broot = NEWrb(s[i], 0); continue; } BUinsert(s[i]); }
    printf("     "); showrb(broot); printf("\n");
    ok = 1; bh(broot, &ok); printf("     red-black rules hold: %s, height %d\n", ok ? "yes" : "NO", hrb(broot));

    h = z; for (int c = 'A'; c <= 'K'; c++) { h = RBinsert(h, c, 0); h->red = 0; }
    printf("13.50 A..K ascending, top-down: "); showrb(h); printf("   height %d\n", hrb(h));
    for (int N = 1000; N <= 1000000; N *= 10) {
        h = z; for (int k = 0; k < N; k++) { h = RBinsert(h, k, 0); h->red = 0; }
        ok = 1; bh(h, &ok);
        printf("     N=%7d ascending: height %2d (lg N = %.1f), rules %s\n", N, hrb(h), log2(N), ok ? "ok" : "BROKEN");
    }
    return 0;
}
