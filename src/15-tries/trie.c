/* ============================================================
 *  trie.c  —  Binary trie (bit-based) for integer keys
 *  Chapter 15: Radix Search
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 15.2
 *  Build:   gcc -std=c99 -Wall -O2 trie.c -o trie
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

/* Use the w-th bit of an int key (MSB first, 5 bits for demo 0..31). */
#define bitsword 5
int digit(int v, int w) { return (v >> (bitsword - 1 - w)) & 1; }

typedef struct node *link;
struct node { int key; link l, r; };
static link z, head;
static int NULLkey = -1;

link NEW(int k) {
    link x = malloc(sizeof *x); x->key = k; x->l = z; x->r = z; return x;
}
void STinit(void) { z = NEW(NULLkey); head = z; }

int searchR(link h, int v, int w) {
    if (h == z) return 0;
    if (h->l == z && h->r == z) return h->key == v;   /* leaf */
    if (digit(v, w) == 0) return searchR(h->l, v, w + 1);
    return searchR(h->r, v, w + 1);
}
int STsearch(int v) { return searchR(head, v, 0); }

link insertR(link h, int v, int w) {
    if (h == z) return NEW(v);
    if (h->l == z && h->r == z) {          /* split a leaf */
        link t = NEW(NULLkey);             /* new internal */
        if (digit(h->key, w) == digit(v, w)) {
            /* same bit: recurse deeper on that side */
            if (digit(v, w) == 0) t->l = insertR(h, v, w + 1);
            else                  t->r = insertR(h, v, w + 1);
        } else {
            if (digit(v, w) == 0) { t->l = NEW(v); t->r = h; }
            else                  { t->r = NEW(v); t->l = h; }
        }
        return t;
    }
    if (digit(v, w) == 0) h->l = insertR(h->l, v, w + 1);
    else                  h->r = insertR(h->r, v, w + 1);
    return h;
}
void STinsert(int v) { head = insertR(head, v, 0); }

int main(void) {
    int keys[] = {1, 19, 18, 5, 25, 3, 9};   /* Sedgewick's classic sample */
    STinit();
    for (int i = 0; i < 7; i++) STinsert(keys[i]);
    for (int i = 0; i < 7; i++)
        printf("search %2d -> %s\n", keys[i], STsearch(keys[i]) ? "hit" : "miss");
    printf("search  7 -> %s\n", STsearch(7) ? "hit" : "miss");
    return 0;
}
