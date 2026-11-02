/* ============================================================
 *  tree_core.c  —  binary trees: representation, traversals, construction
 *
 *   * nodes with string labels (so the same code handles "A", "45" and "+")
 *   * a tiny parser:  "A(B(D,E),C(F,G))"   '.' = empty child   (to type the book's figures in)
 *   * linked memory representation LEFT / DATA / RIGHT + ROOT + AVAIL  (Thareja 9.2.3)
 *   * sequential representation  TREE[1..]  with children at 2K and 2K+1
 *   * pre / in / post / level order — recursive, with an explicit stack, and Morris (O(1) space)
 *   * height, size, leaves, depth, level, ancestors, mirror, copy, equality, complete/full tests
 *   * building a tree from  (in-order + pre-order)  or  (in-order + post-order)
 *   * counting and listing all non-similar binary trees with n nodes (Catalan numbers)
 *   * general tree  ->  binary tree (left-child / right-sibling)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja ch.9 (9.1-9.4, Examples 9.1-9.8), Lipschutz ch.7.1-7.5 (traversals with stacks),
 *                Bandyopadhyay & Dey ch.12.1-12.3, Pal & Halder ch.8
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 tree_core.c -o tree_core
 * ============================================================ */
#ifndef TREE_CORE_C
#define TREE_CORE_C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma GCC diagnostic ignored "-Wunused-function"   /* other files #include this one and use only part of it */

//>>node
typedef struct Node {
    char lab[12];
    struct Node *l, *r;
} Node;

static Node *mk(const char *lab, Node *l, Node *r)
{
    Node *n = malloc(sizeof *n);
    size_t k = strlen(lab); if (k > sizeof n->lab - 1) k = sizeof n->lab - 1;     /* labels are short */
    memcpy(n->lab, lab, k); n->lab[k] = 0;
    n->l = l; n->r = r;
    return n;
}
static void free_tree(Node *t) { if (t) { free_tree(t->l); free_tree(t->r); free(t); } }
//<<node

//>>parse
/* grammar:  tree := '.' | label | label '(' tree ',' tree ')'      e.g.  A(B(D,E),C(.,G))   */
static Node *parse_rec(const char **p)
{
    if (**p == '.') { (*p)++; return NULL; }
    char lab[12]; int k = 0;
    while (**p && **p != '(' && **p != ',' && **p != ')') lab[k++] = *(*p)++;
    lab[k] = 0;
    Node *n = mk(lab, NULL, NULL);
    if (**p == '(') {
        (*p)++; n->l = parse_rec(p);
        assert(**p == ','); (*p)++;
        n->r = parse_rec(p);
        assert(**p == ')'); (*p)++;
    }
    return n;
}
static Node *parse(const char *s) { const char *p = s; Node *t = parse_rec(&p); assert(*p == 0); return t; }
//<<parse

/* ---------- output into a buffer ---------- */
typedef struct { char s[512]; } Buf;
static void bput(Buf *b, const char *lab) { if (b->s[0]) strcat(b->s, " "); strcat(b->s, lab); }

//>>recursive
static void pre(const Node *t, Buf *b)  { if (t) { bput(b, t->lab); pre(t->l, b); pre(t->r, b); } }   /* N L R */
static void in(const Node *t, Buf *b)   { if (t) { in(t->l, b); bput(b, t->lab); in(t->r, b); } }     /* L N R */
static void post(const Node *t, Buf *b) { if (t) { post(t->l, b); post(t->r, b); bput(b, t->lab); } } /* L R N */
//<<recursive

//>>level
/* breadth-first: a queue of node pointers (array queue, enough for the sizes used here) */
static void level(const Node *t, Buf *b)
{
    const Node *q[256]; int f = 0, r = 0;
    if (t) q[r++] = t;
    while (f < r) {
        const Node *x = q[f++];
        bput(b, x->lab);
        if (x->l) q[r++] = x->l;
        if (x->r) q[r++] = x->r;
    }
}
//<<level

//>>iterative
/* Lipschutz 7.5: traversals with a stack, no recursion. */
#define STK 256
static void pre_iter(const Node *t, Buf *b)
{
    const Node *st[STK]; int sp = 0;
    if (t) st[sp++] = t;
    while (sp) {
        const Node *x = st[--sp];
        bput(b, x->lab);
        if (x->r) st[sp++] = x->r;           /* right first, so left comes out first */
        if (x->l) st[sp++] = x->l;
    }
}
static void in_iter(const Node *t, Buf *b)
{
    const Node *st[STK]; int sp = 0;
    while (t || sp) {
        while (t) { st[sp++] = t; t = t->l; }  /* go as far left as possible, remembering the way */
        t = st[--sp];
        bput(b, t->lab);
        t = t->r;
    }
}
static void post_iter(const Node *t, Buf *b)
{
    const Node *st[STK]; int sp = 0; const Node *last = NULL;
    while (t || sp) {
        while (t) { st[sp++] = t; t = t->l; }
        const Node *top = st[sp - 1];
        if (top->r && top->r != last) t = top->r;      /* right subtree not finished yet */
        else { bput(b, top->lab); last = top; sp--; }
    }
}
//<<iterative

//>>morris
/* Morris traversal: O(1) extra space. It temporarily makes the in-order predecessor point
   back to the current node (a thread!), follows it, then removes it again. */
static void in_morris(Node *t, Buf *b)
{
    Node *cur = t;
    while (cur) {
        if (!cur->l) { bput(b, cur->lab); cur = cur->r; }
        else {
            Node *p = cur->l;
            while (p->r && p->r != cur) p = p->r;     /* in-order predecessor of cur */
            if (!p->r) { p->r = cur; cur = cur->l; }  /* create the thread */
            else { p->r = NULL; bput(b, cur->lab); cur = cur->r; }   /* remove it, visit */
        }
    }
}
//<<morris

//>>measures
static int height(const Node *t)  { if (!t) return 0; int a = height(t->l), b = height(t->r); return 1 + (a > b ? a : b); }
static int size(const Node *t)    { return t ? 1 + size(t->l) + size(t->r) : 0; }
static int leaves(const Node *t)  { return !t ? 0 : (!t->l && !t->r) ? 1 : leaves(t->l) + leaves(t->r); }
static int internal(const Node *t){ return size(t) - leaves(t); }
/* depth = number of edges from the root (root has depth 0); -1 if absent */
static int depth_of(const Node *t, const char *lab, int d)
{
    if (!t) return -1;
    if (!strcmp(t->lab, lab)) return d;
    int x = depth_of(t->l, lab, d + 1);
    return x >= 0 ? x : depth_of(t->r, lab, d + 1);
}
/* ancestors of lab, written root-first into b; returns 1 if lab was found */
static int ancestors(const Node *t, const char *lab, Buf *b)
{
    if (!t) return 0;
    if (!strcmp(t->lab, lab)) return 1;
    Buf tmp = {{0}};
    if (ancestors(t->l, lab, &tmp) || ancestors(t->r, lab, &tmp)) {
        Buf mine = {{0}}; bput(&mine, t->lab);
        if (tmp.s[0]) { strcat(mine.s, " "); strcat(mine.s, tmp.s); }
        *b = mine; return 1;
    }
    return 0;
}
static const Node *find(const Node *t, const char *lab)
{
    if (!t) return NULL;
    if (!strcmp(t->lab, lab)) return t;
    const Node *x = find(t->l, lab);
    return x ? x : find(t->r, lab);
}
static const Node *parent_of(const Node *t, const char *lab)
{
    if (!t) return NULL;
    if ((t->l && !strcmp(t->l->lab, lab)) || (t->r && !strcmp(t->r->lab, lab))) return t;
    const Node *x = parent_of(t->l, lab);
    return x ? x : parent_of(t->r, lab);
}
//<<measures

//>>shape
static Node *mirror(Node *t) { if (t) { Node *x = t->l; t->l = mirror(t->r); t->r = mirror(x); } return t; }
static Node *copy(const Node *t) { return t ? mk(t->lab, copy(t->l), copy(t->r)) : NULL; }
/* "similar" = same shape;  "copies" = same shape AND same contents (Thareja 9.2.3) */
static int similar(const Node *a, const Node *b)
{
    if (!a || !b) return a == b;
    return similar(a->l, b->l) && similar(a->r, b->r);
}
static int copies(const Node *a, const Node *b)
{
    if (!a || !b) return a == b;
    return !strcmp(a->lab, b->lab) && copies(a->l, b->l) && copies(a->r, b->r);
}
/* complete: all levels full except possibly the last, which is filled left to right.
   full (strict): every node has 0 or 2 children. */
static int is_complete(const Node *t)
{
    const Node *q[256]; int f = 0, r = 0, gap = 0;
    q[r++] = t;
    while (f < r) {
        const Node *x = q[f++];
        if (!x) { gap = 1; continue; }
        if (gap) return 0;                       /* a real node after an empty slot */
        q[r++] = x->l; q[r++] = x->r;
    }
    return 1;
}
static int is_full(const Node *t)
{
    if (!t) return 1;
    if (!t->l && !t->r) return 1;
    if (!t->l || !t->r) return 0;
    return is_full(t->l) && is_full(t->r);
}
//<<shape

//>>memrep
/* Linked representation exactly as in Thareja Fig. 9.10/9.11:  arrays LEFT, DATA, RIGHT, a ROOT
   index and an AVAIL list threaded through the unused slots using the LEFT field. Index 0 unused,
   -1 plays the role of NULL. */
#define MEM 32
typedef struct {
    int left[MEM + 1], right[MEM + 1];
    char data[MEM + 1][12];
    int root, avail;
} Mem;

static void mem_init(Mem *m)
{
    memset(m, 0, sizeof *m);
    m->root = -1; m->avail = 1;
    for (int i = 1; i <= MEM; i++) { m->left[i] = i < MEM ? i + 1 : -1; m->right[i] = -1; }
}
static int mem_new(Mem *m, const char *lab, int l, int r)
{
    int i = m->avail;
    assert(i != -1);                              /* OVERFLOW */
    m->avail = m->left[i];
    strcpy(m->data[i], lab); m->left[i] = l; m->right[i] = r;
    return i;
}
static int mem_from_tree(Mem *m, const Node *t)
{
    if (!t) return -1;
    int l = mem_from_tree(m, t->l), r = mem_from_tree(m, t->r);   /* children first -> parent index larger */
    return mem_new(m, t->lab, l, r);
}
static Node *tree_from_mem(const Mem *m, int i)
{
    if (i == -1) return NULL;
    return mk(m->data[i], tree_from_mem(m, m->left[i]), tree_from_mem(m, m->right[i]));
}
//<<memrep

//>>seqrep
/* sequential representation: TREE[1] = root, children of K at 2K and 2K+1, "" = empty slot.
   returns the array size needed (2^h - 1 for height h) */
static void seq_fill(const Node *t, char out[][12], int k, int cap)
{
    if (!t || k > cap) return;
    strcpy(out[k], t->lab);
    seq_fill(t->l, out, 2 * k, cap);
    seq_fill(t->r, out, 2 * k + 1, cap);
}
//<<seqrep

//>>construct
/* Build from IN-ORDER + PRE-ORDER (arrays of labels). The first pre-order item is the root; its
   position in the in-order list splits left and right. */
static Node *from_pre_in(const char **pre, const char **in, int n, int *pi)
{
    if (n == 0) return NULL;
    const char *root = pre[(*pi)++];
    int k = 0; while (strcmp(in[k], root)) k++;
    Node *t = mk(root, NULL, NULL);
    t->l = from_pre_in(pre, in, k, pi);
    t->r = from_pre_in(pre, in + k + 1, n - k - 1, pi);
    return t;
}
/* IN-ORDER + POST-ORDER: the LAST post-order item is the root; build the RIGHT subtree first. */
static Node *from_post_in(const char **post, const char **in, int n, int *pi)
{
    if (n == 0) return NULL;
    const char *root = post[(*pi)--];
    int k = 0; while (strcmp(in[k], root)) k++;
    Node *t = mk(root, NULL, NULL);
    t->r = from_post_in(post, in + k + 1, n - k - 1, pi);
    t->l = from_post_in(post, in, k, pi);
    return t;
}
//<<construct

//>>shapes
/* number of structurally different binary trees with n nodes = Catalan(n) */
static long catalan(int n)
{
    long c[20] = {1};
    for (int i = 1; i <= n; i++) { c[i] = 0; for (int j = 0; j < i; j++) c[i] += c[j] * c[i - 1 - j]; }
    return c[n];
}
/* list all shapes with n nodes into out[] (each written like  x(x(.,.),.)  ) */
static int shapes(int n, char out[][64])
{
    if (n == 0) { strcpy(out[0], "."); return 1; }
    int cnt = 0;
    for (int k = 0; k < n; k++) {
        char left[16][64], right[16][64];
        int a = shapes(k, left), b = shapes(n - 1 - k, right);
        for (int i = 0; i < a; i++)
            for (int j = 0; j < b; j++) {
                if (!strcmp(left[i], ".") && !strcmp(right[j], ".")) strcpy(out[cnt], "x");
                else {
                    char tmp[160];                                  /* 2 children (< 64 each) + "x(" "," ")" fit easily */
                    snprintf(tmp, sizeof tmp, "x(%.60s,%.60s)", left[i], right[j]);
                    assert(strlen(tmp) < 64);                       /* n <= 4 is all we ever list */
                    strcpy(out[cnt], tmp);
                }
                cnt++;
            }
    }
    return cnt;
}
//<<shapes

//>>general
/* A general tree (any number of children) stored left-child / right-sibling. The SAME two pointers
   then ARE a binary tree:  left = first child,  right = next sibling   (Thareja 9.3). */
static Node *gparse_list(const char **p);
static Node *gparse_one(const char **p)
{
    char lab[12]; int k = 0;
    while (**p && **p != '(' && **p != ',' && **p != ')') lab[k++] = *(*p)++;
    lab[k] = 0;
    Node *n = mk(lab, NULL, NULL);
    if (**p == '(') { (*p)++; n->l = gparse_list(p); assert(**p == ')'); (*p)++; }
    return n;
}
static Node *gparse_list(const char **p)           /* a,b,c  ->  a --right--> b --right--> c */
{
    Node *first = gparse_one(p), *cur = first;
    while (**p == ',') { (*p)++; cur->r = gparse_one(p); cur = cur->r; }
    return first;
}
/* "A(B(E,F),C,D(G))" : A has children B,C,D;  B has E,F;  D has G.  returns the binary tree */
static Node *general_to_binary(const char *s) { const char *p = s; Node *t = gparse_one(&p); assert(!*p); return t; }
//<<general

#ifndef TREE_NO_MAIN
static char *B(Buf *b, void (*f)(const Node *, Buf *), const Node *t) { b->s[0] = 0; f(t, b); return b->s; }

static void tests(void)
{
    Buf b;
    /* Example 9.6-9.8: the two trees of Fig 9.19(a)/(b) */
    Node *ta = parse("A(B(D(G,H(.,L)),E),C(.,F(I,J(K,.))))");
    Node *tb = parse("A(B(.,D),C(E(.,F(G(H,I),.)),.))");
    assert(!strcmp(B(&b, pre, ta),  "A B D G H L E C F I J K"));
    assert(!strcmp(B(&b, in, ta),   "G D H L B E A C I F K J"));
    assert(!strcmp(B(&b, post, ta), "G L H D E B I K J F C A"));
    assert(!strcmp(B(&b, level, ta),"A B C D E F G H I J L K"));
    assert(!strcmp(B(&b, pre, tb),  "A B D C E F G H I"));
    assert(!strcmp(B(&b, in, tb),   "B D A E H G I F C"));
    assert(!strcmp(B(&b, post, tb), "D B H I G F E C A"));
    assert(!strcmp(B(&b, level, tb),"A B C D E F G H I"));
    /* the three implementations must agree on both trees */
    Node *tr[2] = { ta, tb };
    for (int i = 0; i < 2; i++) {
        Buf x, y;
        assert(!strcmp(B(&x, pre, tr[i]),  B(&y, pre_iter, tr[i])));
        assert(!strcmp(B(&x, in, tr[i]),   B(&y, in_iter, tr[i])));
        assert(!strcmp(B(&x, post, tr[i]), B(&y, post_iter, tr[i])));
        Buf m = {{0}}; Buf r = {{0}}; in(tr[i], &r); in_morris(tr[i], &m);
        assert(!strcmp(m.s, r.s));
        assert(!strcmp(B(&x, pre, tr[i]), B(&y, pre, tr[i])));   /* Morris restored the tree */
    }
    printf("traversals ok (recursive == explicit stack == Morris)\n");

    /* 9.4.5: in D B E A F C G + pre A B D E C F G */
    const char *PI[] = {"D","B","E","A","F","C","G"}, *PP[] = {"A","B","D","E","C","F","G"};
    int pi = 0; Node *x = from_pre_in(PP, PI, 7, &pi);
    assert(!strcmp(B(&b, post, x), "D E B F G C A"));
    const char *PO[] = {"D","E","B","F","G","C","A"};
    pi = 6; Node *y = from_post_in(PO, PI, 7, &pi);
    assert(copies(x, y));
    printf("pre/in and post/in give the same tree: %s\n", B(&b, level, x));

    /* measures on Q6 of Thareja */
    Node *q6 = parse("A(B(D,E),C(F,G(H,I)))");
    assert(leaves(q6) == 5 && internal(q6) == 4 && height(q6) == 4);   /* leaves D E F H I */
    assert(depth_of(q6, "E", 0) == 2 && depth_of(q6, "A", 0) == 0);
    Buf an = {{0}}; assert(ancestors(q6, "E", &an) && !strcmp(an.s, "A B"));
    assert(height(find(q6, "E")) == 1);
    assert(!strcmp(parent_of(q6, "H")->lab, "G"));
    printf("Q6: leaves=%d internal=%d height=%d\n", leaves(q6), internal(q6), height(q6));

    /* memory representation */
    Mem m; mem_init(&m);
    Node *q15 = parse("1(2(4(8,9),5),3(6(10,11),7(.,12)))");
    m.root = mem_from_tree(&m, q15);
    Node *back = tree_from_mem(&m, m.root);
    assert(copies(q15, back));
    char seq[32][12]; memset(seq, 0, sizeof seq);
    seq_fill(q15, seq, 1, 31);
    assert(!strcmp(seq[1], "1") && !strcmp(seq[3], "3") && !strcmp(seq[6], "6") && !strcmp(seq[7], "7"));
    assert(!strcmp(seq[12], "10") && !strcmp(seq[13], "11") && !strcmp(seq[15], "12"));
    assert(seq[10][0] == 0 && seq[11][0] == 0 && seq[14][0] == 0);   /* 5 has no children; 7 has no left child */
    printf("memory + sequential representation round-trip ok\n");

    /* shapes */
    assert(catalan(3) == 5 && catalan(4) == 14 && catalan(10) == 16796);
    char sh[16][64]; assert(shapes(3, sh) == 5); assert(shapes(4, sh) == 14);

    /* mirror / similar / complete / full */
    Node *c = copy(ta); mirror(c); mirror(c); assert(copies(c, ta));
    mirror(c); assert(!copies(c, ta) && !similar(c, ta));
    Node *rt = parse("63(29(18(9,11),23),54(45,49))");          /* Q9: right tree */
    Node *lt = parse("63(29(18(9,11),23),54(45,.))");          /* Q9: left tree  */
    assert(is_complete(rt) && is_full(rt));
    assert(!is_complete(lt) && !is_full(lt));
    free_tree(rt); free_tree(lt);
    printf("shape tests ok\n");

    /* general tree to binary tree: preorder(forest) = preorder(binary), postorder(forest) = inorder(binary) */
    Node *g = general_to_binary("A(B(E,F),C,D(G,H(I)))");
    assert(!strcmp(B(&b, pre, g), "A B E F C D G H I"));
    assert(!strcmp(B(&b, in, g),  "E F B C G I H D A"));
    { Buf p1, i1; printf("general -> binary: pre = %s | in = %s\n", B(&p1, pre, g), B(&i1, in, g)); }

    free_tree(ta); free_tree(tb); free_tree(x); free_tree(y); free_tree(q6); free_tree(q15);
    free_tree(back); free_tree(c); free_tree(g);
}
int main(void) { tests(); printf("tree_core: all tests passed\n"); return 0; }
#endif
#endif /* TREE_CORE_C */
