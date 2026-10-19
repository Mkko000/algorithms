/* ============================================================
 *  dst.c  —  Digital search trees (DSTs)
 *  Chapter 15: Radix Search (Section 15.1)
 *
 *  A DST looks like a BST (one key per node) but we branch on the BITS
 *  of the search key instead of comparing whole keys: at depth w go
 *  left if bit w of the key is 0, right if it is 1 (Program 15.1).
 *  Keys in a subtree are NOT in order (the root may be bigger than
 *  keys in its right subtree), so a DST is not a sorted structure; but
 *  the height is bounded by the number of bits in the key: no
 *  "sorted input" disaster. Property 15.1: ~lg N comparisons on average,
 *  ~2 lg N in the worst case, never more than the number of key bits.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 15.1
 *  Build:   gcc -std=c99 -Wall -O2 dst.c -o dst -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define bitsword 32
typedef unsigned Key;
#define digit(A, B) (((A) >> (bitsword - 1 - (B))) & 1u)

typedef struct STnode *link;
struct STnode { Key key; link l, r; };
static link head, z;

static link NEW(Key v) { link x = malloc(sizeof *x); x->key = v; x->l = z; x->r = z; return x; }
void STinit(void) { z = NEW(0); z->l = z->r = z; head = z; }

/* Program 15.1 */
static int searchR(link h, Key v, int w, int *cmp) {
    if (h == z) return 0;
    (*cmp)++;
    if (v == h->key) return 1;
    if (digit(v, w) == 0) return searchR(h->l, v, w + 1, cmp);
    else                  return searchR(h->r, v, w + 1, cmp);
}
int STsearch(Key v, int *cmp) { *cmp = 0; return searchR(head, v, 0, cmp); }
static link insertR(link h, Key v, int w) {
    if (h == z) return NEW(v);
    if (digit(v, w) == 0) h->l = insertR(h->l, v, w + 1);
    else                  h->r = insertR(h->r, v, w + 1);
    return h;
}
void STinsert(Key v) { head = insertR(head, v, 0); }
static int height(link h) { if (h == z) return -1; int a = height(h->l), b = height(h->r); return 1 + (a > b ? a : b); }

int main(void) {
    STinit();
    const int N = 1 << 16;
    for (Key i = 0; i < (Key)N; i++) STinsert(i << 16);       /* SORTED insertions */
    printf("DST, %d keys inserted in increasing order: height %d (BST would be %d)\n", N, height(head), N - 1);
    long tot = 0; int c, mx = 0;
    for (Key i = 0; i < (Key)N; i++) { STsearch(i << 16, &c); tot += c; if (c > mx) mx = c; }
    printf("search hits: avg %.1f, max %d compares (lg N = %.0f, key bits = %d)\n", (double)tot / N, mx, log2(N), bitsword);
    printf("search for absent key: %s\n", STsearch(12345u, &c) ? "hit" : "miss");
    printf("root key = %u = the FIRST key inserted (a BST-like shape, but branching on bits)\n", head->key);
    return 0;
}
