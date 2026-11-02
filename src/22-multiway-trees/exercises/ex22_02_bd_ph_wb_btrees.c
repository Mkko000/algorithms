/* ============================================================
 *  ex22_02_bd_ph_wb_btrees.c  —  Solved exercises and worked examples, Chapter 22, from the OTHER three books
 *
 *  Bandyopadhyay & Dey  "Data Structures Using C", Sec. 12.7  : Fig. 12.18 (generation), Fig. 12.19 (deletion), Exercise 12
 *  Pal & Halder         "Data Structures and Algorithms with C", Sec. 8 : Fig. 8.50 (M-way), 8.51 (order 4), 8.52 (order 3),
 *                         8.53 (deletion), 8.54 (2-3 and 2-3-4 trees), Exercises 16, 17, 21(b), MCQ viii
 *  Weiss                "Data Structures and Algorithm Analysis in C++", Sec. 4.7 : Fig. 4.63-4.67, Exercises 4.41, 4.42
 *      (the Weiss tree is in ../wbtree.c — a B+-style tree with separate M and L; this file only calls it)
 *
 *  Author:  Mohammed El Sisi
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 ex22_02_bd_ph_wb_btrees.c -o ex22_02 -lm
 * ============================================================ */
#pragma GCC diagnostic ignored "-Wunused-function"
#define MWAY_NO_MAIN
#define BTREE_NO_MAIN
#define WB_NO_MAIN
#include "../mway.c"
#include "../btree.c"
#include "../wbtree.c"

//>>BD1218
/* ---- B&D Fig. 12.18: the order-5 B tree grown from these 20 keys; B&D Fig. 12.19: the four deletions ---- */
static void bd_fig1218(void)
{
    int ks[] = { 282, 314, 307, 289, 393, 299, 337, 407, 354, 302, 462, 347, 448, 482, 293, 399, 418, 468, 471, 436 };
    BTree t = { NULL, 5 };
    printf("B&D Fig 12.18 (order 5):\n");
    for (int i = 0; i < 20; i++) {
        assert(bt_insert(&t, ks[i]) && bt_valid(&t));
        printf("  step %2d: +%d %s\n", i + 1, ks[i], mn_str(t.root));
        if (i == 3)  assert(!strcmp(mn_str(t.root), "[282 289 307 314]"));                                       /* (a) one full node */
        if (i == 4)  assert(!strcmp(mn_str(t.root), "[307]{[282 289] [314 393]}"));                              /* (b) 393 splits it, 307 goes up */
        if (i == 7)  assert(!strcmp(mn_str(t.root), "[307]{[282 289 299] [314 337 393 407]}"));                  /* (c) three more fit */
        if (i == 8)  assert(!strcmp(mn_str(t.root), "[307 354]{[282 289 299] [314 337] [393 407]}"));            /* (d) 354 is its own median */
        if (i == 12) assert(!strcmp(mn_str(t.root), "[307 354]{[282 289 299 302] [314 337 347] [393 407 448 462]}")); /* (e) */
        if (i == 13) assert(!strcmp(mn_str(t.root), "[307 354 448]{[282 289 299 302] [314 337 347] [393 407] [462 482]}")); /* (f) */
        if (i == 18) assert(!strcmp(mn_str(t.root), "[293 307 354 448]{[282 289] [299 302] [314 337 347] [393 399 407 418] [462 468 471 482]}")); /* (g) */
    }
    assert(!strcmp(mn_str(t.root),                                                                              /* (h) the root splits: +1 level */
        "[354]{[293 307]{[282 289] [299 302] [314 337 347]} [407 448]{[393 399] [418 436] [462 468 471 482]}}"));
    assert(mn_height(t.root) == 3);

    int dl[] = { 337, 448, 436, 299 };
    const char *want[] = {
        "[354]{[293 307]{[282 289] [299 302] [314 347]} [407 448]{[393 399] [418 436] [462 468 471 482]}}",       /* 12.19(a): leaf delete */
        "[354]{[293 307]{[282 289] [299 302] [314 347]} [407 462]{[393 399] [418 436] [468 471 482]}}",           /* 448: successor 462 takes its place */
        "[354]{[293 307]{[282 289] [299 302] [314 347]} [407 468]{[393 399] [418 462] [471 482]}}",               /* 436: 462 comes down, 468 goes up */
        "[307 354 407 468]{[282 289 293 302] [314 347] [393 399] [418 462] [471 482]}" };                        /* 299: two merges, the old root vanishes */
    const char *why[] = { "", "", "borrow-right", "merge-left merge-right" };
    printf("B&D Fig 12.19 (deletions):\n");
    for (int i = 0; i < 4; i++) {
        assert(bt_delete(&t, dl[i]) && bt_valid(&t));
        printf("  -%d  %s   [%s]\n", dl[i], mn_str(t.root), bt_fixlog);
        assert(!strcmp(mn_str(t.root), want[i]));
        assert(!strcmp(bt_fixlog, why[i]));
    }
    assert(mn_height(t.root) == 2);          /* the tree lost a level: B&D's "the top three nodes are combined to a single node" */
    mn_free(t.root);
}
//<<BD1218

//>>PH851
/* ---- P&H Fig. 8.51 (order 4) — the book inserts 19 TWICE, and a B tree has distinct keys, so we store the keys x10
        and call the second 19 "191".  Printed back /10 the trees are the book's. ---- */
static void ph_fig851(void)
{
    int ks[] = { 74, 72, 19, 87, 51, 10, 35, 18, 39, 60, 76, 58, 19, 45 };
    BTree t = { NULL, 4 };
    int seen19 = 0;
    printf("P&H Fig 8.51 (order 4; keys x10, the second 19 is 191):\n");
    for (int i = 0; i < 14; i++) {
        int k = ks[i] * 10; if (ks[i] == 19 && seen19++) k = 191;
        assert(bt_insert(&t, k) && bt_valid(&t));
        printf("  ins %2d: %s\n", ks[i], mn_str(t.root));
        if (i == 3)  assert(!strcmp(mn_str(t.root), "[720]{[190] [740 870]}"));                                   /* (d) */
        if (i == 6)  assert(!strcmp(mn_str(t.root), "[190 720]{[100] [350 510] [740 870]}"));                     /* (g) */
        if (i == 9)  assert(!strcmp(mn_str(t.root), "[190 390 720]{[100 180] [350] [510 600] [740 870]}"));       /* (j) */
        if (i == 12) assert(!strcmp(mn_str(t.root), "[190 390 720]{[100 180] [191 350] [510 580 600] [740 760 870]}")); /* (m) */
    }
    assert(!strcmp(mn_str(t.root), "[390]{[190]{[100 180] [191 350]} [510 720]{[450] [580 600] [740 760 870]}}"));  /* (n) = the book's final tree */
    mn_free(t.root);
}
//<<PH851

//>>PH852
/* ---- P&H Fig. 8.52 (order 3, letters) and Exercise 17 (the same with a 'g' where the figure has a 'q') ---- */
static void ph_fig852(void)
{
    const char *seq = "pqrdhmlskn";
    BTree t = { NULL, 3 };
    printf("P&H Fig 8.52 (order 3): ");
    for (int i = 0; seq[i]; i++) {
        assert(bt_insert(&t, seq[i]) && bt_valid(&t));
        if (seq[i] == 'r') { assert(!strcmp(mn_str(t.root), "[113]{[112] [114]}")); }                              /* (c) q goes up */
        if (seq[i] == 'l') { assert(!strcmp(mn_str(t.root), "[109]{[104]{[100] [108]} [113]{[112] [114]}}")); }   /* (g) a new level: m on top */
    }
    /* [m]{[h]{[d] [k l]} [q]{[n p] [r s]}} with the letters as codes */
    assert(!strcmp(mn_str(t.root), "[109]{[104]{[100] [107 108]} [113]{[110 112] [114 115]}}"));
    printf("[m]{[h]{[d] [k l]} [q]{[n p] [r s]}}\n");
    mn_free(t.root);

    const char *seq2 = "pgrdhmlskn";                                         /* Exercise 17, exactly as printed */
    BTree u = { NULL, 3 };
    for (int i = 0; seq2[i]; i++) { assert(bt_insert(&u, seq2[i]) && bt_valid(&u)); }
    assert(!strcmp(mn_str(u.root), "[108]{[103]{[100] [104 107]} [112]{[109 110] [114 115]}}"));
    printf("P&H Ex 17 (p g r d h m l s k n): [l]{[g]{[d] [h k]} [p]{[m n] [r s]}}\n");
    mn_free(u.root);
}
//<<PH852

//>>PH853
/* ---- P&H Fig. 8.53: deleting 10, 87, 39, 60, 72, 35 — an order-5 tree (a leaf with 4 keys appears in (a)).
        P&H combine with the RIGHT neighbour and replace an inner key by its PREDECESSOR: two switches in btree.c reproduce it ---- */
static void ph_fig853(void)
{
    bt_merge_right_first = 1; bt_use_pred = 1;
    BTree t = { mn_parse("[19 72]{[10 18] [35 39 51 60] [74 87]}"), 5 };
    assert(bt_valid(&t));
    int dl[] = { 10, 87, 39, 60, 72, 35 };
    const char *want[] = {
        "[35 72]{[18 19] [39 51 60] [74 87]}",            /* (b) borrow from the right: 19 down, 35 up */
        "[35 60]{[18 19] [39 51] [72 74]}",               /* (c) the book says "borrow from right"; it is the LEFT neighbour that lends 60 */
        "[35]{[18 19] [51 60 72 74]}",                    /* (d) combine 51, 60, 72, 74 */
        "[35]{[18 19] [51 72 74]}",                       /* (e) */
        "[35]{[18 19] [51 74]}",                          /* (f) */
        "[18 19 51 74]" };                                /* (g) 35 -> 19 (its predecessor), then 18, 19, 51, 74 combine */
    printf("P&H Fig 8.53 (order 5):\n  start %s\n", mn_str(t.root));
    for (int i = 0; i < 6; i++) {
        assert(bt_delete(&t, dl[i]) && bt_valid(&t));
        printf("  -%2d: %s  [%s]\n", dl[i], mn_str(t.root), bt_fixlog);
        assert(!strcmp(mn_str(t.root), want[i]));
    }
    mn_free(t.root);
    /* with MY default rules (successor, combine left) the same six deletions give another valid tree */
    bt_merge_right_first = 0; bt_use_pred = 0;
    BTree u = { mn_parse("[19 72]{[10 18] [35 39 51 60] [74 87]}"), 5 };
    for (int i = 0; i < 6; i++) { assert(bt_delete(&u, dl[i])); assert(bt_valid(&u)); }
    assert(!strcmp(mn_str(u.root), "[18 19 51 74]"));            /* same final tree, different road */
    mn_free(u.root);
}
//<<PH853

//>>PH854
/* ---- P&H Fig. 8.54: a complete 2-3 tree and a 2-3-4 tree are B trees of order 3 and 4;  Fig. 8.50: an order-4 M-way tree.
        (The M-way figure is only partly legible; this is OUR reading of it and mw_valid accepts it.) ---- */
static void ph_fig854(void)
{
    BTree a = { mn_parse("[35 60]{[10 25] [40 55] [75 85]}"), 3 };
    assert(bt_valid(&a)); assert(mn_height(a.root) == 2 && mn_count_keys(a.root) == 8);
    BTree b = { mn_parse("[42]{[16 21]{[11] [17 19] [22 23 34]} [57 78 91]{[45] [63 65] [85 90] [95]}}"), 4 };
    assert(bt_valid(&b)); assert(mn_height(b.root) == 3);
    MWay w = { mn_parse("[50 60 80]{[30 35] [58 59]{[52 54] - -} [65 70 75]{[61 63]{- - [64]} - - -} [90]}"), 4 };
    assert(mw_valid(&w));
    BTree wb = { w.root, 4 };
    assert(bt_valid(&wb) == 0);          /* ... and it is NOT a B tree: leaves at different levels, nodes half-empty */
    printf("P&H Fig 8.54: 2-3 tree valid (order 3), 2-3-4 tree valid (order 4); Fig 8.50 is a valid M-way tree but not a B tree\n");
    mn_free(a.root); mn_free(b.root); mn_free(w.root);
}
//<<PH854

//>>objective
/* ---- P&H Exercise 23 (viii): "A B-tree is  a) always balanced  b) an ordered tree  c) a directed tree  d) all of these"  ->  d.
        Check what a program can check: balanced = all leaves on one level, ordered = the search-tree order holds. ---- */
static void ph_mcq(void)
{
    srand(5);
    for (int m = 3; m <= 9; m++) {
        BTree t = { NULL, m };
        for (int i = 0; i < 500; i++) bt_insert(&t, rand() % 4000);
        assert(bt_valid(&t) > 0);     /* bt_valid checks BOTH: same leaf level (balanced) and key order (ordered) */
        mn_free(t.root);
    }
    printf("P&H MCQ 23(viii): d (balanced, ordered, and every edge goes parent -> child)\n");
}
//<<objective

int main(void)
{
    bd_fig1218();
    ph_fig851();
    ph_fig852();
    ph_fig853();
    ph_fig854();
    ph_mcq();
    puts("ex22_02: all checks passed");
    return 0;
}
