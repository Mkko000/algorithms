/* ============================================================
 *  ex5_86_trees.c  —  Exercises 5.68, 5.73, 5.75, 5.78, 5.86-5.90
 *  Chapter 5: Recursion and Trees — solved exercises
 *
 *  5.68 External nodes in an M-ary tree with N internal nodes.
 *  5.73 Height and external path length of a Fibonacci tree.
 *  5.75 Path length of a divide-and-conquer tree is between N lg N and
 *       N lg N + N.  Read literally the definition does not add up (a
 *       root plus subtrees of floor(N/2) and ceil(N/2) nodes is N+1
 *       nodes), so N must count the size-1 calls, the LEAVES of the
 *       tree of Figure 5.6; the bound is then on the sum of leaf depths
 *       (= total size of all subproblems).  Checked for N <= 5000.
 *  5.86 Count the leaves of a binary tree.
 *  5.87 Count nodes with one external and one internal child.
 *  5.88-5.90 Internal path length, recursively, in linear time.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Sections 5.4-5.7, Definition 5.6
 *  Build:   gcc -std=c99 -Wall -O2 ex5_86_trees.c -o ex5_86 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct node *link;
struct node { int n; link l, r; };
static link NEW(int n, link l, link r) { link x = malloc(sizeof *x); x->n = n; x->l = l; x->r = r; return x; }

static link fibtree(int h)                 /* h=0: external (NULL); h=1: one node */
{
    if (h == 0) return NULL;
    if (h == 1) return NEW(1, NULL, NULL);
    return NEW(h, fibtree(h - 1), fibtree(h - 2));
}
static link dctree(int n)                  /* root N, subtrees floor(N/2), ceil(N/2) */
{
    if (n == 1) return NEW(1, NULL, NULL);
    return NEW(n, dctree(n / 2), dctree((n + 1) / 2));
}

static int count(link h) { return h ? 1 + count(h->l) + count(h->r) : 0; }
static int height(link h) { if (!h) return -1; int a = height(h->l), b = height(h->r); return 1 + (a > b ? a : b); }
static int leaves(link h) { if (!h) return 0; if (!h->l && !h->r) return 1; return leaves(h->l) + leaves(h->r); }    /* 5.86 */
static int onechild(link h) { if (!h) return 0; return ((!h->l) != (!h->r)) + onechild(h->l) + onechild(h->r); }   /* 5.87 */
static long calls;
/* 5.90: one pass; returns node count and accumulates depths */
static long ipl(link h, int d) { calls++; if (!h) return 0; return d + ipl(h->l, d + 1) + ipl(h->r, d + 1); }
static long leafdepth(link h, int d) { if (!h) return 0; if (!h->l && !h->r) return d; return leafdepth(h->l, d + 1) + leafdepth(h->r, d + 1); }
static long epl(link h, int d) { if (!h) return d; return epl(h->l, d + 1) + epl(h->r, d + 1); }

int main(void)
{
    printf("5.68 M-ary tree, N internal nodes: external = (M-1)N + 1; memory = N items + M N links\n");
    printf("     e.g. binary N=10: %d external; 3-ary N=10: %d external\n", 1 * 10 + 1, 2 * 10 + 1);

    printf("5.73 Fibonacci trees:   h   N(nodes)  height* ext.path  N+1=F_{h+2}\n");
    long F[40] = { 0, 1 }; for (int i = 2; i < 40; i++) F[i] = F[i - 1] + F[i - 2];
    printf("     (*height counts the level of the external nodes, as in the book)\n");
    for (int h = 2; h <= 20; h += 3) {
        link t = fibtree(h);
        int N = count(t); long e = epl(t, 0);
        printf("                      %3d %9d %7d %9ld %9ld   EPL/((N+1) lg N) = %.3f\n", h, N, height(t) + 1, e, F[h + 2], e / ((N + 1) * log2(N + 1.0)));
    }

    printf("5.75 divide-and-conquer trees (N leaves): N, sum of leaf depths, N lg N, N lg N + N\n");
    int ok = 1;
    for (int n = 1; n <= 5000; n++) {
        link t = dctree(n); double lo = n * log2(n), v = leafdepth(t, 0);
        if (v < lo - 1e-9 || v > lo + n + 1e-9) ok = 0;
        if (n == 11 || n == 15 || n == 16 || n == 23 || n == 5000) printf("     %5d %8.0f %10.1f %10.1f\n", n, v, lo, lo + n);
    }
    printf("     bound holds for all N <= 5000: %s\n", ok ? "yes" : "NO");

    link t = dctree(11);
    calls = 0; long v = ipl(t, 0);
    printf("5.86 leaves(dc tree 11) = %d   5.87 one-child nodes = %d\n", leaves(t), onechild(t));
    printf("5.88/5.89 IPL(dc tree 11) = %ld using %ld calls = 2N+1 (N=%d): linear\n", v, calls, count(t));
    link f = fibtree(6);
    printf("     fib tree h=6: %d nodes, %d leaves, %d one-child nodes\n", count(f), leaves(f), onechild(f));
    return 0;
}
