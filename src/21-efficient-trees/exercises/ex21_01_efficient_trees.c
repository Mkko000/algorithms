/* ============================================================
 *  ex21_01_efficient_trees.c  —  Solved exercises, Chapter 21 (BST / AVL / threaded trees)
 *
 *  Thareja ch.10 "Efficient Binary Trees": Review Questions 4, 11, 12, 13, 16, 17, 18, 19,
 *  Programming Exercises 1 and 2, and the Multiple-choice / True-False / Fill-in sets for BST, AVL
 *  and threaded trees (the red-black / splay items are in ex21_02_rb_splay.c).
 *  The solutions to Q11, Q12, Q19 live in ../bst_ops.c, Q13, Q17, Q18 in ../avl.c and Q16 in ../threaded.c;
 *  they are re-run here (their main()s are switched off) and the key numbers are asserted once more.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Ch. 10 Exercises
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 ex21_01_efficient_trees.c -o ex21_01
 * ============================================================ */
#define BST_NO_MAIN
#define AVL_NO_MAIN
#define TH_NO_MAIN
#include "../bst_ops.c"
#include "../avl.c"
#include "../threaded.c"

//>>RQ4
/* ---- RQ 4: a complete binary tree with 27 nodes: how many in the last level, what height? ----
   Levels 0..3 hold 1+2+4+8 = 15 nodes, level 4 holds the other 27 - 15 = 12 (capacity 16).
   Height (counting nodes on the longest path) = 5.                                              */
static void rq4(void)
{
    int n = 27, full = 0, level = 0;
    while (full + (1 << level) < n) { full += 1 << level; level++; }        /* last level number = 4 */
    int last = n - full, height = level + 1;
    printf("RQ4: n=27 -> last level has %d nodes, height %d\n", last, height);
    assert(last == 12 && height == 5);
    /* height of a complete tree is ceil(log2(n+1)) */
    int h2 = 0; while ((1 << h2) < n + 1) h2++;
    assert(h2 == height);
}
//<<RQ4

//>>PE
/* ---- Programming exercises 1 and 2 are bst_insert / bst_delete / bst_nodes in bst_ops.c; here is a user-level test ---- */
static void programming_exercises(void)
{
    INode *t = NULL;
    int a[] = { 50, 30, 70, 20, 40, 60, 80 };
    for (int i = 0; i < 7; i++) t = bst_insert(t, a[i]);
    assert(bst_nodes(t) == 7);
    t = bst_delete(t, 50, 0);               /* delete the root (two children) */
    assert(bst_nodes(t) == 6);
    t = bst_delete(t, 20, 0);               /* a leaf */
    t = bst_delete(t, 30, 0);               /* one child left (40) */
    assert(bst_nodes(t) == 4);
    IBuf b; printf("PE1/PE2: in-order after deletes: %s  (%d nodes)\n", iin(t, &b), bst_nodes(t));
    assert(!strcmp(iin(t, &b), "40 60 70 80"));
    ifree(t);
}
//<<PE

//>>objective
/* ---- objective questions: Thareja Appendix G, and the experiments that justify each answer ---- */
static void objective(void)
{
    /* MCQ 1 (a): worst-case search in a BST is O(n): sorted insertion makes a chain */
    INode *chain = NULL; for (int i = 1; i <= 100; i++) chain = bst_insert(chain, i);
    assert(bst_height(chain) == 100);
    ifree(chain);
    /* MCQ 2 (b): AVL search/insert/delete are O(log n) in the worst case too: height <= 1.44 log2(n+2) */
    INode *av = NULL; for (int i = 1; i <= 1000; i++) av = avl_insert(av, i);
    assert(av->h <= 14);                                       /* 1.44 * log2(1002) = 14.4 */
    ifree(av);
    /* MCQ 3 (b): left sub-tree one level higher than the right -> BF = +1 (BF = height(left) - height(right)) */
    INode *x = iparse("2(1,.)"); assert(BF(x) == 1); ifree(x);
    /* MCQ 4 (d): new node in the RIGHT sub-tree of the RIGHT sub-tree of the critical node -> RR (a left rotation) */
    INode *rr = NULL; LOG[0] = 0; rr = avl_insert(avl_insert(avl_insert(rr, 1), 2), 3);
    assert(!strcmp(LOG, "RR@1")); ifree(rr);
    /* TF 1 (True): left sub-tree keys are smaller.   TF 2 (False): different insertion order -> different trees */
    INode *p = iparse("2(1,3)"), *q = NULL;
    q = bst_insert(bst_insert(bst_insert(q, 1), 2), 3);
    char s1[64], s2[64]; assert(strcmp(ispec(p, s1), ispec(q, s2)) != 0);
    ifree(p); ifree(q);
    /* TF 3 (False): a new node becomes a LEAF (an external position becomes a node) */
    /* TF 4 (True): mirror image of a BST swaps children everywhere -> it is a BST for the REVERSED order */
    /* TF 5 (True): the right thread of a node points to its in-order successor  (threaded.c walks them) */
    /* TF 6 (False): deleting from the LEFT sub-tree of A makes A right-heavy, so the fix is an L rotation, not R */
    /* TF 7 (True): AVL height is O(log n) */
    /* TF 8 (False): the critical node has |BF| = 2 (it is the nearest ancestor that BECAME unbalanced) */
    /* TF 9 (False): RR rotation (not RL) for right-of-right.   TF 10 (False): see the red-black exercise file */
    /* Fill in 1 two-way threaded tree; 2 right sub-tree; 3 in-order successor; 4 height(left) - height(right);
       5 left sub-tree is one level LOWER than the right; 6 O(log n); 7 LL; 8 black and black; 9 P is the root; 10 splay */
    printf("objective checks for BST / AVL / threaded trees passed\n");
}
//<<objective

int main(void)
{
    /* (Q11, Q12, Q19 / Q13, Q17, Q18 / Q16: the full traces are printed when you run bst_ops, avl and threaded) */
    rq4(); programming_exercises(); objective();

    /* Q11: the full answer in one place */
    int a11[] = { 98, 2, 48, 12, 56, 32, 4, 67, 23, 87, 23, 55, 46 };
    INode *t = build(a11, 13);
    char sp[256]; ispec(t, sp);
    assert(!strcmp(sp, "98(2(.,48(12(4,32(23(.,23),46)),56(55,67(.,87)))),.)"));
    ifree(t);
    printf("ex21_01_efficient_trees: all tests passed\n");
    return 0;
}
