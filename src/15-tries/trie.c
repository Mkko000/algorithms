/* ============================================================
 *  trie.c  —  Binary tries: keys in the leaves, bits pick the way
 *  Chapter 15: Radix Search (Section 15.2)
 *
 *  Program 15.2 (search) and Program 15.3 (insert) for w-bit integer
 *  keys. Definition 15.1: the trie of one key is a leaf; the trie of
 *  several keys is an internal node whose left subtree holds the keys
 *  that start with 0 and whose right subtree holds those that start
 *  with 1 (with the leading bit removed). Consequences checked here:
 *   - Property 15.2: the shape does NOT depend on the insertion order
 *     (we build the same keys in two orders and compare the shapes).
 *   - Property 15.3: ~lg N bit inspections per search for random keys.
 *   - Property 15.4: about N / ln 2 = 1.44 N nodes (internal ones too).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 15.2-15.3
 *  Build:   gcc -std=c99 -Wall -O2 trie.c -o trie -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int bitsword = 5;                     /* 5 bits for the letter demo */
static int digit(unsigned v, int w) { return (v >> (bitsword - 1 - w)) & 1; }

typedef struct node *link;
struct node { unsigned key; int leaf; link l, r; };
static long nodes;

static link LEAF(unsigned k) { link x = malloc(sizeof *x); x->key = k; x->leaf = 1; x->l = x->r = NULL; nodes++; return x; }
static link INTERNAL(link l, link r) { link x = malloc(sizeof *x); x->key = 0; x->leaf = 0; x->l = l; x->r = r; nodes++; return x; }

/* Program 15.2 */
static int searchR(link h, unsigned v, int w, int *bits) {
    if (h == NULL) return 0;
    if (h->leaf) return h->key == v;
    (*bits)++;
    return searchR(digit(v, w) ? h->r : h->l, v, w + 1, bits);
}
/* split two different keys that agree on bits 0..w-1 (Program 15.3's "split") */
static link split(link p, link q, int w) {
    int a = digit(p->key, w), b = digit(q->key, w);
    if (a == b) return a == 0 ? INTERNAL(split(p, q, w + 1), NULL) : INTERNAL(NULL, split(p, q, w + 1));
    return a == 0 ? INTERNAL(p, q) : INTERNAL(q, p);
}
/* Program 15.3 */
static link insertR(link h, unsigned v, int w) {
    if (h == NULL) return LEAF(v);
    if (h->leaf) return h->key == v ? h : split(LEAF(v), h, w);
    if (digit(v, w) == 0) h->l = insertR(h->l, v, w + 1);
    else                  h->r = insertR(h->r, v, w + 1);
    return h;
}
static void freeT(link h) { if (!h) return; freeT(h->l); freeT(h->r); free(h); }
static int same(link a, link b) {
    if (!a || !b) return a == b;
    if (a->leaf || b->leaf) return a->leaf && b->leaf && a->key == b->key;
    return same(a->l, b->l) && same(a->r, b->r);
}
static void show(link h, int d) {
    if (!h) return;
    show(h->r, d + 1);
    printf("%*s%s\n", 3 * d, "", h->leaf ? (char[]){(char)('A' + h->key - 1), 0} : "o");
    show(h->l, d + 1);
}

int main(void) {
    const char *s = "ASERCHINGXMPL", *t = "LPMXGNIHCRESA";   /* same keys, reverse order */
    link a = NULL, b = NULL;
    for (int i = 0; s[i]; i++) a = insertR(a, s[i] - 'A' + 1, 0);
    for (int i = 0; t[i]; i++) b = insertR(b, t[i] - 'A' + 1, 0);
    printf("trie of A S E R C H I N G X M P L (5-bit codes), drawn sideways (root at left, 1-branch up):\n");
    show(a, 0);
    printf("same keys in reverse order give the same trie (Property 15.2): %s\n", same(a, b) ? "yes" : "NO");
    int bits = 0;
    printf("search P -> %s, ", searchR(a, 'P' - 'A' + 1, 0, &bits) ? "hit" : "miss");
    printf("search Q -> %s\n\n", searchR(a, 'Q' - 'A' + 1, 0, &bits) ? "hit" : "miss");
    freeT(a); freeT(b);

    bitsword = 31; srand(15);
    for (int N = 1000; N <= 1000000; N *= 10) {
        unsigned *k = malloc(N * sizeof *k); link h = NULL; nodes = 0;
        for (int i = 0; i < N; i++) { k[i] = (unsigned)rand() & 0x7fffffffu; h = insertR(h, k[i], 0); }
        long leaves = 0; for (int i = 0; i < N; i++) { int bb = 0; searchR(h, k[i], 0, &bb); leaves += bb; }
        printf("N=%7d random 31-bit keys: %.2f bits per search (lg N = %.2f), %.3f N nodes (1/ln 2 + 1 = %.3f incl. leaves)\n",
               N, (double)leaves / N, log2(N), (double)nodes / N, 1 / log(2) + 1);
        freeT(h); free(k);
    }
    return 0;
}
