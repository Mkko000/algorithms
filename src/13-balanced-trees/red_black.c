/* ============================================================
 *  red_black.c  —  Red-black BST (2-3-4 tree representation)
 *  Chapter 13: Balanced Trees
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 13.6
 *  Build:   gcc -std=c99 -Wall -O2 red_black.c -o red_black
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct node *link;
struct node { char key; int red; link l, r; int N; };
static link z, head;

link NEW(char k, link l, link r, int N, int red) {
    link x = malloc(sizeof *x);
    x->key = k; x->l = l; x->r = r; x->N = N; x->red = red;
    return x;
}
void STinit(void) {
    z = NEW('\0', NULL, NULL, 0, 0); z->l = z->r = z;
    head = z;
}
static void fixN(link h) { h->N = h->l->N + h->r->N + 1; }

link rotR(link h) { link x = h->l; h->l = x->r; x->r = h; fixN(h); fixN(x); return x; }
link rotL(link h) { link x = h->r; h->r = x->l; x->l = h; fixN(h); fixN(x); return x; }

link RBinsert(link h, char k, int sw) {
    if (h == z) return NEW(k, z, z, 1, 1);
    /* split 4-nodes on the way down */
    if (h->l->red && h->r->red) { h->red = 1; h->l->red = 0; h->r->red = 0; }
    if (k < h->key) {
        h->l = RBinsert(h->l, k, 0);
        if (h->red && h->l->red && sw) h = rotR(h);
        if (h->l->red && h->l->l->red) {
            h = rotR(h); h->red = 0; h->r->red = 1;
        }
    } else {
        h->r = RBinsert(h->r, k, 1);
        if (h->red && h->r->red && !sw) h = rotL(h);
        if (h->r->red && h->r->r->red) {
            h = rotL(h); h->red = 0; h->l->red = 1;
        }
    }
    fixN(h); return h;
}
void STinsert(char k) { head = RBinsert(head, k, 0); head->red = 0; }

int searchR(link h, char k) {
    if (h == z) return 0;
    if (k == h->key) return 1;
    return (k < h->key) ? searchR(h->l, k) : searchR(h->r, k);
}

void inorder(link h) {
    if (h == z) return;
    inorder(h->l);
    printf("%c%s ", h->key, h->red ? "*" : "");
    inorder(h->r);
}

int main(void) {
    STinit();
    const char *s = "ASEARCHINGEXAMPLE";
    for (int i = 0; s[i]; i++) STinsert(s[i]);
    printf("inorder (* = red): "); inorder(head); putchar('\n');
    printf("count = %d\n", head->N);
    printf("search X -> %s\n", searchR(head, 'X') ? "hit" : "miss");
    printf("search Z -> %s\n", searchR(head, 'Z') ? "hit" : "miss");
    return 0;
}
