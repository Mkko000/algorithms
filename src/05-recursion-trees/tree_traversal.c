/* ============================================================
 *  tree_traversal.c  —  Binary tree: preorder / inorder / postorder
 *                       + nonrecursive preorder with an explicit stack
 *  Chapter 5: Recursion and Trees
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 5.14-5.15
 *  Build:   gcc -std=c99 -Wall -O2 tree_traversal.c -o tree_traversal
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct node *link;
struct node { char item; link l, r; };

link NEW(char c, link l, link r) {
    link x = malloc(sizeof *x); x->item = c; x->l = l; x->r = r; return x;
}

void preorder (link h){ if(!h)return; printf("%c ",h->item); preorder(h->l);  preorder(h->r); }
void inorder  (link h){ if(!h)return; inorder(h->l);  printf("%c ",h->item); inorder(h->r); }
void postorder(link h){ if(!h)return; postorder(h->l); postorder(h->r); printf("%c ",h->item); }

/* Nonrecursive preorder: push right child first so left is processed next. */
void preorder_nr(link h) {
    link st[64]; int top = 0;
    if (h) st[top++] = h;
    while (top) {
        link x = st[--top];
        printf("%c ", x->item);
        if (x->r) st[top++] = x->r;
        if (x->l) st[top++] = x->l;
    }
}

int main(void) {
    /*         E
              / \
             D   H
            /   / \
           B   F   I
          / \   \
         A   C   G                          */
    link t = NEW('E',
                 NEW('D', NEW('B', NEW('A',0,0), NEW('C',0,0)), 0),
                 NEW('H', NEW('F', 0, NEW('G',0,0)), NEW('I',0,0)));
    printf("preorder:    "); preorder(t);    putchar('\n');
    printf("inorder:     "); inorder(t);     putchar('\n');
    printf("postorder:   "); postorder(t);   putchar('\n');
    printf("preorder_nr: "); preorder_nr(t); putchar('\n');
    return 0;
}
