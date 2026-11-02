/* ============================================================
 *  ex20_01_trees.c  —  Solved exercises, Chapter 20 (Binary trees)
 *
 *  Thareja ch.9 "Trees", Review Questions 3, 4, 6-17 (1, 2, 5 are written notes: see the course page)
 *  and the Multiple-choice / True-False / Fill-in-the-blank sets, checked against Appendix G.
 *  The heavy lifting lives in ../tree_core.c, ../expr_tree.c and ../huffman.c — we include them here
 *  (their main() functions are switched off) so there is NO duplicated code.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Ch. 9 Exercises
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 ex20_01_trees.c -o ex20_01 -lm
 * ============================================================ */
#define TREE_NO_MAIN
#define HUFF_NO_MAIN
#include "../tree_core.c"
#include "../huffman.c"

static char *B(Buf *b, void (*f)(const Node *, Buf *), const Node *t) { b->s[0] = 0; f(t, b); return b->s; }
static const char *Z(int ok) { return ok ? "ok" : "WRONG"; }

//>>RQ3
/* ---- RQ 3 and 13: all non-similar binary trees with 4 (and 3) nodes.  Catalan numbers: 1 1 2 5 14 42 ... ---- */
static void rq3_rq13(void)
{
    char sh[16][64];
    int n3 = shapes(3, sh);
    printf("RQ13: %d binary trees with 3 nodes:\n", n3);
    for (int i = 0; i < n3; i++) printf("   %s\n", sh[i]);
    assert(n3 == 5);
    int n4 = shapes(4, sh);
    printf("RQ3: %d binary trees with 4 nodes:\n", n4);
    for (int i = 0; i < n4; i++) printf("   %s\n", sh[i]);
    assert(n4 == 14);
    /* all of them are pairwise different */
    for (int i = 0; i < n4; i++) for (int j = i + 1; j < n4; j++) assert(strcmp(sh[i], sh[j]) != 0);
}
//<<RQ3

//>>RQ6
/* ---- RQ 6: the tree  A(B(D,E), C(F,G(H,I)))  ---- */
static void rq6(void)
{
    Node *t = parse("A(B(D,E),C(F,G(H,I)))");
    Buf b;
    printf("RQ6 (c) ancestors of E : "); Buf an = {{0}}; ancestors(t, "E", &an); printf("%s\n", an.s);
    printf("RQ6 (d) descendants of A: every other node\n");
    printf("RQ6 (e) siblings of C   : B\n");
    printf("RQ6 (f) height of tree  : %d\n", height(t));
    printf("RQ6 (g) height of E     : %d  (a leaf: 1)\n", height(find(t, "E")));
    printf("RQ6 (h) level of E      : %d  (root = level 0)\n", depth_of(t, "E", 0));
    printf("RQ6 (i) in   : %s\n", B(&b, in, t));
    printf("        pre  : %s\n", B(&b, pre, t));
    printf("        post : %s\n", B(&b, post, t));
    printf("        level: %s\n", B(&b, level, t));
    assert(leaves(t) == 5 && internal(t) == 4 && height(t) == 4);
    assert(!strcmp(B(&b, pre, t), "A B D E C F G H I"));
    assert(!strcmp(B(&b, in, t), "D B E A F C H G I"));
    assert(!strcmp(B(&b, post, t), "D E B F H I G C A"));
    assert(!strcmp(B(&b, level, t), "A B C D E F G H I"));
    assert(!strcmp(parent_of(t, "C")->lab, "A"));          /* siblings of C: the other child of A */
    assert(!strcmp(parent_of(t, "B")->lab, "A"));
    free_tree(t);
}
//<<RQ6

//>>RQ14
/* ---- RQ 14: the memory table.  ROOT = 3, AVAIL = 15; LEFT / DATA / RIGHT of each used row. ---- */
static void rq14(void)
{
    static const int L[21]   = { 0, -1, -1, 5, 0, 9, 0, 0, 20, 1, 0, -1, -1, 0, -1, 0, -1, 0, -1, 0, 2 };
    static const int D[21]   = { 0,  8, 10, 1, 0, 2, 0, 0,  3, 4, 0,  7,  9, 0,  5, 0, 11, 0, 12, 0, 6 };
    static const int R[21]   = { 0, -1, -1, 8, 0, 14, 0, 0, 11, 12, 0, 18, -1, 0, -1, 0, -1, 0, -1, 0, 16 };
    /* (rows 4, 6, 7, 10, 13, 15, 17, 19 are unused = the AVAIL list) */
    Buf pre_b = {{0}}, in_b = {{0}};
    /* iterative walk straight on the arrays: that IS how a program would read this table */
    int stack[32], sp = 0, x = 3;
    char tmp[16];
    while (x != -1 || sp) {                                    /* in-order */
        while (x != -1) { stack[sp++] = x; x = L[x]; }
        x = stack[--sp]; snprintf(tmp, sizeof tmp, "%d", D[x]); bput(&in_b, tmp); x = R[x];
    }
    sp = 0; x = 3;
    while (x != -1 || sp) {                                    /* pre-order */
        while (x != -1) { snprintf(tmp, sizeof tmp, "%d", D[x]); bput(&pre_b, tmp); stack[sp++] = x; x = L[x]; }
        x = stack[--sp]; x = R[x];
    }
    printf("RQ14 pre = %s | in = %s\n", pre_b.s, in_b.s);
    assert(!strcmp(pre_b.s, "1 2 4 8 9 5 3 6 10 11 7 12"));
    assert(!strcmp(in_b.s, "8 4 9 2 5 1 10 6 11 3 7 12"));
    /* it is exactly the tree of RQ15 */
    Node *q15 = parse("1(2(4(8,9),5),3(6(10,11),7(.,12)))");
    Buf b; assert(!strcmp(B(&b, pre, q15), pre_b.s) && !strcmp(B(&b, in, q15), in_b.s));
    free_tree(q15);
}
//<<RQ14

//>>RQ17
/* ---- RQ 17: Huffman codes read from the three trees (left edge = 0, right edge = 1).
   Fig. T1 shows the letter C twice. Both are printed; the second one is most probably a typo of E. ---- */
static void leaf_codes(const Node *t, char *pre_code, int len, const char **names, char codes[][16], int *k)
{
    if (!t) return;
    if (!t->l && !t->r) { pre_code[len] = 0; names[*k] = t->lab; strcpy(codes[*k], pre_code); (*k)++; return; }
    pre_code[len] = '0'; leaf_codes(t->l, pre_code, len + 1, names, codes, k);
    pre_code[len] = '1'; leaf_codes(t->r, pre_code, len + 1, names, codes, k);
}
static void show_codes(const char *title, const char *spec, const char *want)
{
    Node *t = parse(spec);
    char buf[16], codes[8][16], line[128] = ""; const char *names[8]; int k = 0;
    leaf_codes(t, buf, 0, names, codes, &k);
    for (int i = 0; i < k; i++) { char s[32]; snprintf(s, sizeof s, "%s=%s ", names[i], codes[i]); strcat(line, s); }
    printf("RQ17 %s: %s\n", title, line);
    assert(!strcmp(line, want));
    /* prefix-free */
    for (int i = 0; i < k; i++) for (int j = 0; j < k; j++) if (i != j) assert(strncmp(codes[i], codes[j], strlen(codes[i])) != 0);
    free_tree(t);
}
static void rq17(void)
{
    show_codes("T1", "o(o(o(A,B),C),o(o(C,D),F))", "A=000 B=001 C=01 C=100 D=101 F=11 ");
    show_codes("T2", "o(o(A,B),o(o(C,D),E))",       "A=00 B=01 C=100 D=101 E=11 ");
    show_codes("T3", "o(o(o(A,B),C),D)",            "A=000 B=001 C=01 D=1 ");
}
//<<RQ17

//>>numbers
/* ---- RQ 10, 11, 12 and the formulas of 9.x ---- */
static void numbers(void)
{
    /* RQ10: a BST with 100 nodes is deepest when it is a chain (sorted insertion): 100 levels (0..99).
       RQ11: the same for any tree with 32 nodes -> height 32 (counted in nodes, Thareja's convention). */
    Node *chain = NULL;
    for (int i = 99; i >= 0; i--) { char s[8]; snprintf(s, sizeof s, "%d", i); chain = mk(s, NULL, chain); }
    assert(height(chain) == 100 && size(chain) == 100);
    free_tree(chain);
    /* RQ12: max nodes at level L is 2^L (root at level 0): levels 3, 4, 12 */
    long m3 = 1L << 3, m4 = 1L << 4, m12 = 1L << 12;
    printf("RQ12: level 3 -> %ld, level 4 -> %ld, level 12 -> %ld\n", m3, m4, m12);
    assert(m3 == 8 && m4 == 16 && m12 == 4096);
    /* height bounds: ceil(log2(n+1)) <= h <= n, and a binary tree of height h has at most 2^h - 1 nodes */
    for (int h = 1; h <= 20; h++) assert(((1L << h) - 1) >= h);
    int n = 1000000, h = 0; while ((1L << h) - 1 < n) h++;
    printf("a tree of 1,000,000 nodes is at least %d levels high (the book says 21, a loose estimate) %s\n", h, Z(h == 20));
}
//<<numbers

/* ---- objective questions of ch.9, with Thareja's Appendix G answers ---- */
static void objective(void)
{
    /* MCQ 1..6: (a) (a) (d) (a) (c) (d) */
    const char *mcq = "aadacd";
    /* MCQ1 leaf degree = 0 (a);  MCQ2 depth of root = 0 (a);  MCQ3 at most 2^h - 1 (d);
       MCQ4 pre-order = depth first (a);  MCQ5 Huffman uses a priority queue (c);
       MCQ6: the key says (d) = 2^(n-1) which is right if levels are counted from 1;
             with the root at level 0 (as everywhere else in the chapter) the count is 2^n. */
    assert(strlen(mcq) == 6);
    /* True/False 1..8: T T T F T F T F */
    const char *tf = "TTTFTFTF";
    /* 4: a node with no successors is a LEAF, not the root;  6: the ROOT has no parent;
       8: internal path length sums paths to INTERNAL nodes (external is E) */
    assert(strlen(tf) == 8);
    /* Fill in the blanks: 1 ascendant 2 nodes 3 2^k - 1 (see MCQ6 note: 2^k at level k from 0) 4 two 5 siblings
       6 structure and contents 7 log2(n+1) and n 8 each node has 0 or 2 children 9 pre-order
       10 the estimated probability (frequency) of each character */
    /* the formula checks we can verify with the code */
    Node *t = parse("1(2(4,5),3(6,7))");
    assert(size(t) == 7 && height(t) == 3 && size(t) == (1 << height(t)) - 1);   /* a full tree of height 3 has 2^3 - 1 nodes */
    free_tree(t);
    printf("objective answers recorded (MCQ %s, T/F %s)\n", mcq, tf);
}

int main(void)
{
    rq3_rq13(); rq6(); rq14(); rq17(); numbers(); objective();
    printf("(RQ4, RQ7, RQ8 are checked in expr_tree.c; RQ9, RQ15 in tree_core.c; RQ16 in huffman.c)\n");
    printf("ex20_01_trees: all tests passed\n");
    return 0;
}
