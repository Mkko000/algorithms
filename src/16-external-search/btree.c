/* ============================================================
 *  btree.c  —  Simplified B-tree (M-way search tree, in memory)
 *  Chapter 16: External Searching
 *
 *  A didactic B-tree of order M: each node holds up to M-1 keys.
 *  Splits on overflow. Good enough to see the idea of multiway
 *  external search (disk pages = nodes).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 16.1-16.3 (simplified)
 *  Build:   gcc -std=c99 -Wall -O2 btree.c -o btree
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

#define M 4   /* order: max M-1 keys per node */

typedef struct node *link;
struct node { int m; int key[M]; link child[M + 1]; int leaf; };
static link root;

link NEW(void) {
    link x = calloc(1, sizeof *x); x->leaf = 1; return x;
}
void STinit(void) { root = NEW(); }

/* Search: scan keys in node, descend. */
int searchR(link h, int v) {
    int i = 0;
    while (i < h->m && v > h->key[i]) i++;
    if (i < h->m && v == h->key[i]) return 1;
    if (h->leaf) return 0;
    return searchR(h->child[i], v);
}
int STsearch(int v) { return searchR(root, v); }

/* Split a full child of h at index i. */
void split_child(link h, int i) {
    link y = h->child[i];
    link z = NEW();
    int mid = M / 2 - 1;                 /* promote middle key */
    z->leaf = y->leaf;
    z->m = y->m - mid - 1;
    for (int j = 0; j < z->m; j++) z->key[j] = y->key[mid + 1 + j];
    if (!y->leaf)
        for (int j = 0; j <= z->m; j++) z->child[j] = y->child[mid + 1 + j];
    y->m = mid;
    for (int j = h->m; j >= i + 1; j--) h->child[j + 1] = h->child[j];
    h->child[i + 1] = z;
    for (int j = h->m - 1; j >= i; j--) h->key[j + 1] = h->key[j];
    h->key[i] = y->key[mid];
    h->m++;
}

void insert_nonfull(link h, int v) {
    int i = h->m - 1;
    if (h->leaf) {
        while (i >= 0 && v < h->key[i]) { h->key[i + 1] = h->key[i]; i--; }
        h->key[i + 1] = v; h->m++;
    } else {
        while (i >= 0 && v < h->key[i]) i--;
        i++;
        if (h->child[i]->m == M - 1) {
            split_child(h, i);
            if (v > h->key[i]) i++;
        }
        insert_nonfull(h->child[i], v);
    }
}

void STinsert(int v) {
    link r = root;
    if (r->m == M - 1) {
        link s = NEW(); s->leaf = 0; s->child[0] = r;
        root = s;
        split_child(s, 0);
        insert_nonfull(s, v);
    } else insert_nonfull(r, v);
}

void show(link h, int depth) {
    for (int d = 0; d < depth; d++) printf("  ");
    printf("[");
    for (int i = 0; i < h->m; i++) printf(" %d", h->key[i]);
    printf(" ]\n");
    if (!h->leaf) for (int i = 0; i <= h->m; i++) show(h->child[i], depth + 1);
}

int main(void) {
    int keys[] = {1,19,18,5,25,3,9,12,14,7,22,15,30,8};
    STinit();
    for (int i = 0; i < 14; i++) STinsert(keys[i]);
    puts("B-tree:");
    show(root, 0);
    printf("search 14 -> %s\n", STsearch(14) ? "hit" : "miss");
    printf("search 11 -> %s\n", STsearch(11) ? "hit" : "miss");
    return 0;
}
