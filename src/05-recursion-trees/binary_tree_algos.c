/* ============================================================
 *  binary_tree_algos.c  —  Recursive binary-tree algorithms
 *  Chapter 5: Recursion and Trees (Sections 5.6, 5.7)
 *
 *   - Program 5.16: level-order traversal (preorder's stack -> a queue)
 *   - Program 5.17: count nodes and compute height recursively
 *   - Program 5.18: quick tree-print (rotated 90 degrees, indented)
 *   - Program 5.19: build a TOURNAMENT (each node = max of children)
 *                   from an array by divide and conquer
 *   - Program 5.20: build a PARSE TREE from a prefix expression
 *   Also checks Properties 5.5-5.7 on the built trees:
 *   N internal nodes -> N+1 external nodes, and
 *   external path length = internal path length + 2N.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 5.16-5.20
 *  Build:   gcc -std=c99 -Wall -O2 binary_tree_algos.c -o binary_tree_algos
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct node *link;
struct node { int item; link l, r; };
static link NEW(int item, link l, link r) { link x = malloc(sizeof *x); x->item = item; x->l = l; x->r = r; return x; }

/* Program 5.16: level order with a simple array queue */
void levelorder(link h, int ischar) {
    link q[256]; int head = 0, tail = 0;
    q[tail++] = h;
    while (head < tail) {
        link x = q[head++];
        if (ischar) printf("%c ", x->item); else printf("%d ", x->item);
        if (x->l) q[tail++] = x->l;
        if (x->r) q[tail++] = x->r;
    }
    putchar('\n');
}

/* Program 5.17: tree parameters */
int count(link h) { if (h == NULL) return 0; return count(h->l) + count(h->r) + 1; }
int height(link h) {
    if (h == NULL) return -1;
    int u = height(h->l), v = height(h->r);
    return (u > v ? u : v) + 1;
}
static void pathlen(link h, int depth, long *ipl, long *epl) {
    if (h == NULL) { *epl += depth; return; }
    *ipl += depth;
    pathlen(h->l, depth + 1, ipl, epl); pathlen(h->r, depth + 1, ipl, epl);
}

/* Program 5.18: print a tree sideways (reverse inorder) */
static void printnode(int c, int h, int ischar) {
    for (int i = 0; i < h; i++) printf("   ");
    if (ischar) printf("%c\n", c); else printf("%d\n", c);
}
void show(link x, int h, int ischar) {
    if (x == NULL) { printnode('*', h, 1); return; }
    show(x->r, h + 1, ischar);
    printnode(x->item, h, ischar);
    show(x->l, h + 1, ischar);
}

/* Program 5.19: tournament construction */
link max_tree(int a[], int l, int r) {
    int m = (l + r) / 2;
    link x = NEW(a[m], NULL, NULL);
    if (l == r) return x;
    x->l = max_tree(a, l, m);
    x->r = max_tree(a, m + 1, r);
    int u = x->l->item, v = x->r->item;
    x->item = u > v ? u : v;
    return x;
}

/* Program 5.20: parse tree from prefix expression */
static const char *pa; static int pi;
link parse(void) {
    char t = pa[pi++];
    link x = NEW(t, NULL, NULL);
    if (t == '+' || t == '*') { x->l = parse(); x->r = parse(); }
    return x;
}
int evaltree(link x) {
    if (x->item == '+') return evaltree(x->l) + evaltree(x->r);
    if (x->item == '*') return evaltree(x->l) * evaltree(x->r);
    return x->item - '0';
}
void freetree(link h) { if (!h) return; freetree(h->l); freetree(h->r); free(h); }

int main(void) {
    int a[] = {1, 7, 3, 9, 4, 2, 8, 5};
    link t = max_tree(a, 0, 7);
    printf("tournament on {1 7 3 9 4 2 8 5}: winner=%d\n", t->item);
    show(t, 0, 0);
    printf("level order: "); levelorder(t, 0);
    long ipl = 0, epl = 0; pathlen(t, 0, &ipl, &epl);
    int N = count(t);
    printf("N=%d height=%d  internal path=%ld external path=%ld  (ext-int=%ld = 2N)\n",
           N, height(t), ipl, epl, epl - ipl);
    freetree(t);

    pa = "*+a*bc+de"; pi = 0;           /* (a + b*c) * (d + e) with letters */
    link p = parse();
    printf("parse tree of prefix %s:\n", pa); show(p, 0, 1);
    printf("level order: "); levelorder(p, 1);
    freetree(p);
    pa = "*+2*34+15"; pi = 0; p = parse();
    printf("value of %s = %d\n", pa, evaltree(p));
    freetree(p);
    return 0;
}
