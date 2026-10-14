/* ============================================================
 *  bst_rotations.c  —  Left/right rotations + root insertion
 *  Chapter 12: Symbol Tables and BSTs
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 12.11-12.12
 *  Build:   gcc -std=c99 -Wall -O2 bst_rotations.c -o bst_rotations
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct node *link;
struct node { char key; link l, r; };

link NEW(char k) { link x = malloc(sizeof *x); x->key = k; x->l = x->r = NULL; return x; }

link rotR(link h) { link x = h->l; h->l = x->r; x->r = h; return x; }
link rotL(link h) { link x = h->r; h->r = x->l; x->l = h; return x; }

/* Insert at the root by recursive insert-then-rotate. */
link insertT(link h, char k) {
    if (!h) return NEW(k);
    if (k < h->key) { h->l = insertT(h->l, k); h = rotR(h); }
    else            { h->r = insertT(h->r, k); h = rotL(h); }
    return h;
}

void inorder(link h) { if (!h) return; inorder(h->l); printf("%c ", h->key); inorder(h->r); }
void preorder(link h){ if (!h) return; printf("%c ", h->key); preorder(h->l); preorder(h->r); }

int main(void) {
    link root = NULL;
    const char *s = "ASEARCHING";
    for (int i = 0; s[i]; i++) root = insertT(root, s[i]);
    printf("root = %c\n", root->key);          /* last inserted = G */
    printf("inorder:  "); inorder(root);  putchar('\n');
    printf("preorder: "); preorder(root); putchar('\n');
    return 0;
}
