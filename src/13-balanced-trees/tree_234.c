/* ============================================================
 *  tree_234.c  —  Top-down 2-3-4 trees implemented DIRECTLY
 *                 (multi-key nodes), to see what red-black trees encode
 *  Chapter 13: Balanced Trees (Section 13.3)
 *
 *  Definition 13.1/13.2: nodes hold 1, 2 or 3 keys (2-, 3-, 4-nodes)
 *  and every external link is at the same distance from the root.
 *  Top-down insertion: on the way down, whenever we meet a 4-node, SPLIT
 *  it: its middle key moves up into the parent (which cannot be a
 *  4-node, because we would have split it already). When we reach the
 *  bottom, the node there is a 2- or 3-node, so the new key just fits.
 *  The tree grows only at the ROOT (when the root is a 4-node), which is
 *  why it stays perfectly balanced. Property 13.6: searches visit at
 *  most lg N + 1 nodes; Property 13.7: fewer than lg N + 1 splits per
 *  insertion in the worst case, < 1 on average.
 *  The book notes that coding this directly is awkward (three node
 *  types, many cases) — compare with red_black.c, which encodes the
 *  same tree as a BST with one "color" bit per node.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Section 13.3
 *                (Definitions 13.1-13.2, Figures 13.10-13.12)
 *  Build:   gcc -std=c99 -Wall -O2 tree_234.c -o tree_234 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct node *link;
struct node { int n; int key[3]; link c[4]; };    /* n keys, n+1 children (NULL at bottom) */
static link root; static long splits;

static link NEWnode(void) { link x = calloc(1, sizeof *x); return x; }

/* split full child p->c[i] (a 4-node) of non-full p */
static void split_child(link p, int i) {
    link y = p->c[i], r = NEWnode();
    r->n = 1; r->key[0] = y->key[2]; r->c[0] = y->c[2]; r->c[1] = y->c[3];
    y->n = 1;                                            /* y keeps key[0] */
    for (int j = p->n; j > i; j--) { p->key[j] = p->key[j - 1]; p->c[j + 1] = p->c[j]; }
    p->key[i] = y->key[1]; p->c[i + 1] = r; p->n++;
    splits++;
}

void insert(int v) {
    if (root == NULL) { root = NEWnode(); root->n = 1; root->key[0] = v; return; }
    if (root->n == 3) { link s = NEWnode(); s->c[0] = root; split_child(s, 0); root = s; }  /* grow at the top */
    link x = root;
    for (;;) {
        int i = 0; while (i < x->n && v > x->key[i]) i++;
        if (x->c[0] == NULL) {                           /* bottom: room guaranteed */
            for (int j = x->n; j > i; j--) x->key[j] = x->key[j - 1];
            x->key[i] = v; x->n++; return;
        }
        if (x->c[i]->n == 3) {                           /* split 4-nodes on the way DOWN */
            split_child(x, i);
            if (v > x->key[i]) i++;
        }
        x = x->c[i];
    }
}
int search(int v, int *visited) {
    *visited = 0;
    for (link x = root; x != NULL; ) {
        (*visited)++;
        int i = 0; while (i < x->n && v > x->key[i]) i++;
        if (i < x->n && v == x->key[i]) return 1;
        x = x->c[i];
    }
    return 0;
}
static int leafdepth(link x, int d, int *ok, int *want) {      /* all leaves at same depth? */
    if (x->c[0] == NULL) { if (*want < 0) *want = d; else if (*want != d) *ok = 0; return d; }
    for (int i = 0; i <= x->n; i++) leafdepth(x->c[i], d + 1, ok, want);
    return *want;
}
static void count_types(link x, int t[4]) { if (!x) return; t[x->n]++; for (int i = 0; i <= x->n; i++) count_types(x->c[i], t); }
static void freeT(link x) { if (!x) return; for (int i = 0; i <= x->n; i++) freeT(x->c[i]); free(x); }
static void show(link x, int d) {
    if (!x || d > 1) return;
    printf("%*s[", 2 * d, "");
    for (int i = 0; i < x->n; i++) printf(i ? " %c" : "%c", x->key[i]);
    printf("]\n");
    for (int i = 0; i <= x->n; i++) show(x->c[i], d + 1);
}

int main(void) {
    const char *s = "ASEARCHINGEXAMPLE";
    for (int i = 0; s[i]; i++) insert(s[i]);
    printf("2-3-4 tree for ASEARCHINGEXAMPLE (top two levels):\n"); show(root, 0);

    freeT(root); root = NULL; splits = 0;
    const int N = 100000;
    for (int i = 0; i < N; i++) insert(i);              /* sorted: the BST worst case */
    int ok = 1, want = -1, h = leafdepth(root, 0, &ok, &want), t[4] = {0}, vis, maxvis = 0;
    count_types(root, t);
    for (int i = 0; i < N; i += 97) { search(i, &vis); if (vis > maxvis) maxvis = vis; }
    printf("N=%d sorted inserts: all leaves at depth %d (%s), max nodes visited %d <= lg N + 1 = %.1f\n",
           N, h, ok ? "perfect balance" : "BROKEN", maxvis, log2(N) + 1);
    printf("node mix: 2-nodes %d, 3-nodes %d, 4-nodes %d; splits per insert %.2f\n",
           t[1], t[2], t[3], (double)splits / N);
    freeT(root); root = NULL; splits = 0; srand(1);
    for (int i = 0; i < N; i++) insert(rand());
    int t2[4] = {0}; count_types(root, t2);
    printf("N=%d random inserts: splits per insert %.2f (< 1 on average), node mix %d/%d/%d\n",
           N, (double)splits / N, t2[1], t2[2], t2[3]);
    freeT(root);
    return 0;
}
