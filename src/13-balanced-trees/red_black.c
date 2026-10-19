/* ============================================================
 *  red_black.c  —  Red-black BSTs: top-down 2-3-4 trees as BSTs
 *  Chapter 13: Balanced Trees (Sections 13.3-13.4)
 *
 *  Program 13.6: each node has a colour bit (red = "glued to its
 *  parent": together they form a 3-node or a 4-node of a 2-3-4 tree).
 *  On the way down, a node with two red children is a 4-node: flip the
 *  colours (that is the 2-3-4 split). On the way back up, two reds in a
 *  row are fixed by one rotation (same direction) or two (zig-zag: the
 *  'sw' flag tells which side we came from).
 *  The demo checks the red-black invariants (Definitions 13.3-13.4)
 *  and measures Property 13.8 (worst case < 2 lg N + 2) and Property
 *  13.9 (average ~ 1.002 lg N compares for random keys).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 13.6
 *  Build:   gcc -std=c99 -Wall -O2 red_black.c -o red_black -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef int Key;
typedef struct node *link;
struct node { Key key; int red; link l, r; int N; };
static link z, head;
static long rotations, flips;

static link NEW(Key k, link l, link r, int N, int red) {
    link x = malloc(sizeof *x);
    x->key = k; x->l = l; x->r = r; x->N = N; x->red = red;
    return x;
}
void STinit(void) { z = NEW(0, NULL, NULL, 0, 0); z->l = z->r = z; head = z; }
static void fixN(link h) { h->N = h->l->N + h->r->N + 1; }
static link rotR(link h) { link x = h->l; h->l = x->r; x->r = h; fixN(h); fixN(x); rotations++; return x; }
static link rotL(link h) { link x = h->r; h->r = x->l; x->l = h; fixN(h); fixN(x); rotations++; return x; }

static link RBinsert(link h, Key k, int sw) {
    if (h == z) return NEW(k, z, z, 1, 1);
    if (h->l->red && h->r->red) {                 /* 4-node: split it (colour flip) */
        h->red = 1; h->l->red = 0; h->r->red = 0; flips++;
    }
    if (k < h->key) {
        h->l = RBinsert(h->l, k, 0);
        if (h->red && h->l->red && sw) h = rotR(h);          /* zig-zag, first half */
        if (h->l->red && h->l->l->red) { h = rotR(h); h->red = 0; h->r->red = 1; }
    } else {
        h->r = RBinsert(h->r, k, 1);
        if (h->red && h->r->red && !sw) h = rotL(h);
        if (h->r->red && h->r->r->red) { h = rotL(h); h->red = 0; h->l->red = 1; }
    }
    fixN(h); return h;
}
void STinsert(Key k) { head = RBinsert(head, k, 0); head->red = 0; }

static int searchR(link h, Key k, int *cmp) {
    while (h != z) { (*cmp)++; if (k == h->key) return 1; h = k < h->key ? h->l : h->r; }
    return 0;
}

/* invariants: no red node has a red child; every path has the same number of black nodes */
static int blackHeight(link h, int *ok) {
    if (h == z) return 1;
    if (h->red && (h->l->red || h->r->red)) *ok = 0;
    int a = blackHeight(h->l, ok), b = blackHeight(h->r, ok);
    if (a != b) *ok = 0;
    return a + !h->red;
}
static int height(link h) { if (h == z) return 0; int a = height(h->l), b = height(h->r); return 1 + (a > b ? a : b); }
static long ipl(link h, int d) { return h == z ? 0 : d + ipl(h->l, d + 1) + ipl(h->r, d + 1); }
static void freeT(link h) { if (h == z) return; freeT(h->l); freeT(h->r); free(h); }
static void inorderC(link h) { if (h == z) return; inorderC(h->l); printf("%c%s ", h->key, h->red ? "*" : ""); inorderC(h->r); }

static void experiment(const char *name, int N, int sorted) {
    freeT(head); head = z; rotations = flips = 0;
    for (int i = 0; i < N; i++) STinsert(sorted ? i : rand());
    int ok = 1, bh = blackHeight(head, &ok);
    printf("%-7s N=%d: height %d (2 lg N + 2 = %.1f), avg compares for a hit %.2f (lg N = %.2f)\n",
           name, N, height(head), 2 * log2(N) + 2, (double)ipl(head, 1) / head->N, log2(N));
    printf("        black height %d, invariants hold: %s, rotations/insert %.2f, flips/insert %.2f\n",
           bh, ok ? "yes" : "NO", (double)rotations / N, (double)flips / N);
}

int main(void) {
    STinit();
    const char *s = "ASEARCHINGEXAMPLE";
    for (int i = 0; s[i]; i++) STinsert(s[i]);
    printf("inorder (* = red): "); inorderC(head); putchar('\n');
    int c = 0;
    printf("count = %d, search X -> %s", head->N, searchR(head, 'X', &c) ? "hit" : "miss");
    printf(", search Z -> %s\n\n", searchR(head, 'Z', &c) ? "hit" : "miss");
    srand(13);
    experiment("sorted", 100000, 1);
    experiment("random", 100000, 0);
    freeT(head); free(z);
    return 0;
}
