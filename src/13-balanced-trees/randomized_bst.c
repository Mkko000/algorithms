/* ============================================================
 *  randomized_bst.c  —  Randomized BST (expected balance)
 *  Chapter 13: Balanced Trees
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 13.2
 *  Build:   gcc -std=c99 -Wall -O2 randomized_bst.c -o randomized_bst
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct node *link;
struct node { char key; link l, r; int N; };
static link z;

link NEW(char k, link l, link r, int N) {
    link x = malloc(sizeof *x); x->key = k; x->l = l; x->r = r; x->N = N; return x;
}
link rotR(link h) { link x = h->l; h->l = x->r; x->r = h;
                    h->N = h->l->N + h->r->N + 1; x->N = h->N + (x->l?x->l->N:0) /*fix later*/; return x; }
/* Simpler N-fix after rotation: */
static void fixN(link h) { if (h && h != z) h->N = h->l->N + h->r->N + 1; }
link rotR2(link h) { link x = h->l; h->l = x->r; x->r = h; fixN(h); fixN(x); return x; }
link rotL2(link h) { link x = h->r; h->r = x->l; x->l = h; fixN(h); fixN(x); return x; }

link insertT(link h, char k) {                /* root insertion */
    if (h == z) return NEW(k, z, z, 1);
    if (k < h->key) { h->l = insertT(h->l, k); h = rotR2(h); }
    else            { h->r = insertT(h->r, k); h = rotL2(h); }
    return h;
}
link insertR(link h, char k) {                /* ordinary insert */
    if (h == z) return NEW(k, z, z, 1);
    if (k < h->key) h->l = insertR(h->l, k);
    else            h->r = insertR(h->r, k);
    h->N++; return h;
}
/* With probability 1/(N+1) insert at root; else recurse. */
link insertRand(link h, char k) {
    if (h == z) return NEW(k, z, z, 1);
    if (rand() % (h->N + 1) == 0) return insertT(h, k);
    if (k < h->key) h->l = insertRand(h->l, k);
    else            h->r = insertRand(h->r, k);
    h->N++; return h;
}

void inorder(link h) { if (h == z) return; inorder(h->l); printf("%c ", h->key); inorder(h->r); }

int main(void) {
    srand(42);
    z = NEW('\0', NULL, NULL, 0); z->l = z->r = z;
    link head = z;
    const char *s = "ASEARCHINGEXAMPLE";
    for (int i = 0; s[i]; i++) head = insertRand(head, s[i]);
    printf("inorder: "); inorder(head); putchar('\n');
    printf("N = %d, root = %c\n", head->N, head->key);
    (void)rotR; (void)insertR;
    return 0;
}
