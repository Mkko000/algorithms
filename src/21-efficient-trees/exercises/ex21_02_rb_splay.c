/* ============================================================
 *  ex21_02_rb_splay.c  —  Solved exercises, Chapter 21 (red-black and splay trees)
 *
 *  Thareja ch.10 Review Questions 9, 10, 14, 15 and the red-black / splay items of the objective sets:
 *    MCQ 5 (a) root;  TF 10;  Fill-in 8 "black and black", 9 "P is the root", 10 "splay"
 *  The programs are ../rbtree.c and ../splay.c (their main()s are switched off).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Ch. 10 Exercises
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 ex21_02_rb_splay.c -o ex21_02
 * ============================================================ */
#define RB_NO_MAIN
#define SPLAY_NO_MAIN
#include "../rbtree.c"
#include "../splay.c"

//>>RQ9
/* ---- RQ 9: how does a red-black tree beat a plain BST?  The height of a BST depends on the insertion order
   (a sorted input gives a chain, height n), the red-black rules cap it at 2*log2(n+1).  ---- */
static int bst_chain_height(int n) { return n; }
static void rq9(void)
{
    RBT t = { NIL, "" };
    for (int i = 1; i <= 1024; i++) rb_insert(&t, i);
    int h = rb_height(t.root);
    printf("RQ9: 1024 sorted keys: plain BST height %d, red-black height %d (bound %d)\n", bst_chain_height(1024), h, 2 * 11);
    assert(h <= 22 && rb_valid(&t));
    rb_free(t.root);
}
//<<RQ9

//>>RQ14
/* ---- RQ 14: properties and the insertion cases — every property is checked by rb_valid() after each step;
   here we additionally watch which cases fire for a few sequences ---- */
static void rq14(void)
{
    struct { const char *name; int keys[8]; int n; const char *cases; } T[] = {
        { "uncle is NIL (rotation, case 3)",        { 10, 5, 1 }, 3, "I3" },
        { "uncle is red (recolour, case 1)",        { 10, 5, 15, 1 }, 4, "I1" },
        { "triangle (double: case 2 then 3)",       { 10, 5, 7 }, 3, "I2 I3" },
    };
    for (int i = 0; i < 3; i++) {
        RBT t = { NIL, "" }; char sp[128];
        for (int j = 0; j < T[i].n; j++) rb_insert(&t, T[i].keys[j]);
        printf("RQ14: %-36s -> %-28s cases: %s\n", T[i].name, rb_str(&t, sp), t.log);
        assert(!strcmp(t.log, T[i].cases));
        assert(rb_valid(&t));
        rb_free(t.root);
    }
    /* TF 10 (False in the book): "some leaf nodes can be red".  In the book's drawings the leaves are the NIL
       externals, which are always black.  If you call the real nodes without children "leaves", RED ones DO exist:
       insert 10, 5, 15 -> both children are red leaves.  We print this to be precise about the wording. */
    RBT t = { NIL, "" };
    rb_insert(&t, 10); rb_insert(&t, 5); rb_insert(&t, 15);
    assert(t.root->l->color == RED && t.root->l->l == NIL && t.root->r->color == RED);
    assert(NIL->color == BLACK);                                 /* the external leaves are black */
    rb_free(t.root);
    printf("RQ14: root is black, every NIL leaf is black (Fill-in 8: black and black)\n");
}
//<<RQ14

//>>RQ15
/* ---- RQ 15, MCQ 5, Fill-in 9: splay trees ---- */
static void rq15(void)
{
    ST t = { NULL, "" };
    for (int i = 1; i <= 15; i++) s_insert(&t, i);
    t.log[0] = 0;
    s_search(&t, 1);                            /* MCQ 5: the accessed node becomes the ROOT */
    assert(t.root->key == 1);
    /* Fill-in 9: zig is used when the parent of N is the root */
    ST u = { NULL, "" };
    s_insert(&u, 5); s_insert(&u, 3);           /* 3 is now root, 5 its right child */
    u.log[0] = 0; s_search(&u, 5);
    assert(!strcmp(u.log, "zig"));
    /* zig-zig needs N and P on the same side, zig-zag on opposite sides */
    ST v = { NULL, "" };
    s_insert(&v, 1); s_insert(&v, 2); s_insert(&v, 3);          /* 3(2(1)) */
    v.log[0] = 0; s_search(&v, 1);
    assert(!strcmp(v.log, "zigzig"));
    ST w = { NULL, "" };                                        /* build 3(1(.,2)) by hand: 2 is a RIGHT child of a LEFT child */
    SN *a = s_new(3, NULL), *b = s_new(1, a), *c = s_new(2, b);
    a->l = b; b->r = c; w.root = a; w.log[0] = 0;
    splay(&w, c);
    assert(!strcmp(w.log, "zigzag") && w.root->key == 2);
    printf("RQ15: zig, zig-zig and zig-zag all observed; accessed key always ends at the root\n");
    s_free(t.root); s_free(u.root); s_free(v.root); s_free(w.root);
}
//<<RQ15

int main(void)
{
    rq9(); rq14(); rq15();
    printf("ex21_02_rb_splay: all tests passed\n");
    return 0;
}
