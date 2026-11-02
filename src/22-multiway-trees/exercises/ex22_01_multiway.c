/* ============================================================
 *  ex22_01_multiway.c  —  Solved exercises, Chapter 22 (multi-way trees, B / B+ / 2-3 trees, tries)
 *
 *  Thareja ch.11 Review Questions 1-13 and the objective sets (MCQ 1-5, True/False 1-9, Fill-in 1-5).
 *  The data-structure programs are ../mway.c, ../btree.c, ../bplus.c, ../twothree.c and ../trie.c
 *  (their main()s are switched off); this file only asks them the questions.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Ch. 11 Exercises
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 ex22_01_multiway.c -o ex22_01 -lm
 * ============================================================ */
/* the programs we include have more functions than any single exercise uses */
#pragma GCC diagnostic ignored "-Wunused-function"
#define MWAY_NO_MAIN
#define BTREE_NO_MAIN
#define BPLUS_NO_MAIN
#define TT_NO_MAIN
#define TRIE_NO_MAIN
#include "../mway.c"
#include "../twothree.c"
#include "../bplus.c"
#include "../trie.c"

#define LEN(a) ((int)(sizeof(a) / sizeof((a)[0])))

//>>RQ1
/* ---- RQ 1: why a large m?  One node = one disk read, so the height is the cost.
   A B tree holding N keys with order m has height at least log_m(N+1) and (at worst, every node half full)
   at most 1 + log_ceil(m/2)((N+1)/2). ---- */
static void rq1(void)
{
    double N = 1e9;
    printf("RQ1: height (= disk reads for a search) of a B tree with 10^9 keys\n      order m:   best case   worst case\n");
    int ms[] = { 2, 3, 5, 101, 1001 };
    double prev_best = 1e18;
    for (int i = 0; i < LEN(ms); i++) {
        int m = ms[i];
        double best = ceil(log(N + 1) / log((double)m));
        double worst = m == 2 ? log2((N + 1) / 2) + 1 : 1 + floor(log((N + 1) / 2) / log((m + 1) / 2));
        printf("      %6d   %9.0f   %10.0f\n", m, best, worst);
        assert(best < prev_best);
        prev_best = best;
    }
    assert(ceil(log(N + 1) / log(1001.0)) == 3);          /* three disk reads for a billion keys */
    assert(ceil(log(N + 1) / log(2.0)) == 30);            /* a binary tree needs 30 */
    /* and the page arithmetic: a 4096-byte page with 4-byte keys and 4-byte child pointers holds (m-1)*4 + m*4 <= 4096 */
    int m = (4096 + 4) / 8;
    printf("      4096-byte page, 4-byte key + 4-byte pointer  ->  m = %d\n", m);
    assert(m == 512);
}
//<<RQ1

//>>RQ2
/* ---- RQ 2 / RQ 3: B tree or B+ tree?  Same keys, same order — compare (a) height, (b) nodes read by a range query,
   (c) what the INNER nodes hold.  A B+ tree's inner nodes carry no data, which in a real page means more separators fit
   (bigger fan-out), and the leaves are chained so a range query never climbs back up. ---- */
static int bt_range_reads_(const MNode *x, int lo, int hi)  /* distinct nodes a range query must read in a B tree */
{
    if (!x) return 0;
    int r = 1;
    for (int i = 0; i <= x->n; i++) {
        long a = i ? x->key[i - 1] : -2147483647L, b = i < x->n ? x->key[i] : 2147483647L;
        if (b < lo || a > hi) continue;                      /* this child cannot contain a key of the range */
        r += bt_range_reads_(x->ch[i], lo, hi);
    }
    return r;
}
static int count_leaves(const MNode *x) { if (!x) return 0; if (mn_leaf(x)) return 1; int c = 0; for (int i = 0; i <= x->n; i++) c += count_leaves(x->ch[i]); return c; }
static void rq2(void)
{
    enum { M = 9, N = 20000 };
    BTree b = { NULL, M }; BPlus p = { NULL, M };
    srand(3);
    static int ks[N];
    for (int i = 0; i < N; i++) ks[i] = i;
    for (int i = N - 1; i > 0; i--) { int j = rand() % (i + 1), t = ks[i]; ks[i] = ks[j]; ks[j] = t; }
    for (int i = 0; i < N; i++) { bt_insert(&b, ks[i]); bp_insert(&p, ks[i]); }
    assert(bt_valid(&b) && bp_valid(&p));
    int hb = mn_height(b.root), hp = mn_height(p.root);
    int rb = bt_range_reads_(b.root, 5000, 5999);
    static int out[N];
    bp_reads = 0; int c = bp_range(&p, 5000, 5999, out); assert(c == 1000);
    printf("RQ2: %d random keys, order %d:  B tree height %d, B+ tree height %d\n", N, M, hb, hp);
    printf("      a range of 1000 keys reads %d nodes in the B tree and %ld in the B+ tree\n", rb, bp_reads);
    printf("      B+ tree: %d nodes in all, %d of them leaves;  B tree: %d nodes\n", mn_count_nodes(p.root), count_leaves(p.root), mn_count_nodes(b.root));
    assert(hp >= hb);                                        /* B+ stores every key in a leaf, so it is never shallower */
    assert(bp_reads <= rb);
    mn_free(b.root); mn_free(p.root);
}
//<<RQ2

//>>RQ5
/* ---- RQ 5 / RQ 6: trie against a BST and a hash table ----
   BST: a search compares the key with ~log2(n) nodes, and every comparison may look at many characters.
   Trie: a search looks at each character of the key exactly once.  Count both for the same words. */
typedef struct SB { char w[16]; struct SB *l, *r; } SB;
static long cmp_chars = 0;
static SB *sb_insert(SB *x, const char *w)
{
    if (!x) { SB *n = calloc(1, sizeof *n); strcpy(n->w, w); return n; }
    int i = 0; while (x->w[i] && x->w[i] == w[i]) i++;       /* the characters examined by this comparison */
    int c = (unsigned char)w[i] - (unsigned char)x->w[i];
    if (c < 0) x->l = sb_insert(x->l, w); else if (c > 0) x->r = sb_insert(x->r, w);
    return x;
}
static int sb_search(const SB *x, const char *w)
{
    while (x) {
        int i = 0; while (x->w[i] && x->w[i] == w[i]) i++;
        cmp_chars += i + 1;
        int c = (unsigned char)w[i] - (unsigned char)x->w[i];
        if (!c) return 1;
        x = c < 0 ? x->l : x->r;
    }
    return 0;
}
static void sb_free(SB *x) { if (x) { sb_free(x->l); sb_free(x->r); free(x); } }
static void rq5(void)
{
    enum { N = 5000 };
    static char words[N][9];
    srand(8);
    SB *bst = NULL; TNode *t = tn_new();
    for (int i = 0; i < N; i++) {
        int len = 6 + rand() % 3;
        for (int j = 0; j < len; j++) words[i][j] = (char)('a' + rand() % 4);       /* small alphabet -> long shared prefixes */
        words[i][len] = 0;
        bst = sb_insert(bst, words[i]); trie_insert(t, words[i], i);
    }
    long trie_chars = 0, found = 0;
    cmp_chars = 0;
    for (int i = 0; i < N; i++) { found += sb_search(bst, words[i]); trie_chars += (long)strlen(words[i]); assert(trie_search(t, words[i]) >= 0); }
    assert(found == N);
    printf("RQ5: %d words: a BST looks at %.1f characters per search, a trie at %.1f\n", N, (double)cmp_chars / N, (double)trie_chars / N);
    assert(cmp_chars > trie_chars);
    sb_free(bst); tn_free(t);
}
//<<RQ5

//>>RQ6
/* ---- RQ 6: hash table against trie.  A hash table answers "is this exact word here?" in O(1) average, but
   (a) it needs a hash function and a collision policy, (b) it keeps NO order, so "all words starting with ar" means scanning
   every entry, while the trie walks straight to the prefix node.  Count the entries each one must touch. ---- */
static void collect(const char *w, int v, void *ud) { char *s = ud; char tmp[80]; snprintf(tmp, sizeof tmp, "%s=%d ", w, v); strcat(s, tmp); }
static void rq6(void)
{
    TNode *t = tn_new();
    const char *dict[] = { "am", "arc", "are", "arm", "art", "do", "dot", "dog", "cat", "car", "card", "care", "carp", "zoo", "zone" };
    for (int i = 0; i < LEN(dict); i++) trie_insert(t, dict[i], i);
    char s[256] = "";
    words_with_prefix(t, "car", collect, s);
    int scanned = 0, hits = 0;                               /* what a hash table must do: look at every entry */
    for (int i = 0; i < LEN(dict); i++) { scanned++; hits += strncmp(dict[i], "car", 3) == 0; }
    printf("RQ6: prefix \"car\": trie lists %s with 3 steps down + the subtree;  hash table must scan all %d entries (%d match)\n", s, scanned, hits);
    assert(hits == 4 && !strcmp(s, "car=9 card=10 care=11 carp=12 "));
    tn_free(t);
}
//<<RQ6

//>>RQ8
/* ---- RQ 8: the B tree of Q.8 (order 4): (a) insert 1, 5, 7, 11, 13, 15, 17, 19   (b) delete 30, 59, 67 ----
   Convention: an overflowing node of 4 keys is split with the LOWER middle key moved up (left half 1 key, right half 2). */
static void rq8(void)
{
    BTree t = { mn_parse("[45]{[29 32]{[18 27] [30 31] [36 39]} [49 63]{[46 47] [54 59 61] [67 72]}}"), 4 };
    int add[] = { 1, 5, 7, 11, 13, 15, 17, 19 };
    for (int i = 0; i < LEN(add); i++) assert(bt_insert(&t, add[i]));
    printf("RQ8(a): %s\n", mn_str(t.root));
    assert(!strcmp(mn_str(t.root), "[11 18 45]{[5]{[1] [7]} [15]{[13] [17]} [29 32]{[19 27] [30 31] [36 39]} [49 63]{[46 47] [54 59 61] [67 72]}}"));
    assert(mn_height(t.root) == 3 && bt_valid(&t));
    int del[] = { 30, 59, 67 };
    for (int i = 0; i < LEN(del); i++) assert(bt_delete(&t, del[i]) && bt_valid(&t));
    printf("RQ8(b): %s\n", mn_str(t.root));
    assert(!strcmp(mn_str(t.root), "[11 18 45]{[5]{[1] [7]} [15]{[13] [17]} [29 32]{[19 27] [31] [36 39]} [49 63]{[46 47] [54 61] [72]}}"));
    mn_free(t.root);
}
//<<RQ8

//>>RQ10
/* ---- RQ 10: B+ tree of order 5 from 90, 27, 7, 9, 18, 21, 3, 4, 16, 11, 21, 72
   (21 appears twice in the question: the second one is a duplicate key and is ignored) ---- */
static void rq10(void)
{
    BPlus t = { NULL, 5 };
    int seq[] = { 90, 27, 7, 9, 18, 21, 3, 4, 16, 11, 21, 72 };
    int dups = 0;
    for (int i = 0; i < LEN(seq); i++) dups += !bp_insert(&t, seq[i]);
    assert(dups == 1 && bp_valid(&t));
    printf("RQ10: %s\n", mn_str(t.root));
    assert(!strcmp(mn_str(t.root), "[7 18 27]{[3 4] [7 9 11 16] [18 21] [27 72 90]}"));
    mn_free(t.root);
}
//<<RQ10

//>>RQ12
/* ---- RQ 12: "B trees of order 2 are full binary trees".  Order 2: max 2 children and (non-root, non-leaf) at least
   ceil(2/2) = 1 child — but a node with ONE child would hold ZERO keys, so every inner node has exactly 2 children and 1 key.
   With all leaves on one level that is a perfect binary tree, so n = 2^h - 1.  Check it for h = 1..6. ---- */
static MNode *perfect(int lo, int hi)                         /* perfect tree over the keys lo..hi (hi-lo+1 = 2^h - 1) */
{
    if (lo > hi) return NULL;
    int mid = (lo + hi) / 2;
    MNode *x = mn_new(); x->n = 1; x->key[0] = mid;
    if (lo < hi) { x->ch[0] = perfect(lo, mid - 1); x->ch[1] = perfect(mid + 1, hi); }
    return x;
}
static void rq12(void)
{
    for (int h = 1; h <= 6; h++) {
        int n = (1 << h) - 1;
        BTree t = { perfect(1, n), 2 };
        assert(bt_valid(&t) && mn_height(t.root) == h && mn_count_keys(t.root) == n);
        mn_free(t.root);
    }
    /* and a non-perfect size cannot be a valid order-2 B tree: 4 keys */
    BTree bad = { mn_parse("[2]{[1] [3 4]}"), 2 };
    assert(!bt_valid(&bad));                                  /* a leaf with 2 keys breaks "at most m-1 = 1 key" */
    mn_free(bad.root);
    printf("RQ12: order-2 B trees checked for 1..6 levels: always 2^h - 1 keys, perfect binary tree\n");
}
//<<RQ12

//>>RQ13
/* ---- RQ 13: the 3-way search tree of Fig. 11.3 / Q.13: insert 23, 45, 67, 87, 54, 32, 11 then delete 9, 36, 54 ---- */
static void rq13(void)
{
    MWay t = { mn_parse("[18 45]{[9 11] [27 36]{- [29 30] -} [54 63]{- - [72 81]}}"), 3 };
    int ins[] = { 23, 45, 67, 87, 54, 32, 11 };
    int dup = 0;
    for (int i = 0; i < LEN(ins); i++) dup += !mw_insert(&t, ins[i]);
    assert(dup == 3);                                         /* 45, 54, 11 were already in the tree */
    int del[] = { 9, 36, 54 };
    for (int i = 0; i < LEN(del); i++) assert(mw_delete(&t, del[i]));
    printf("RQ13: %s\n", mn_str(t.root));
    assert(mw_valid(&t));
    assert(!strcmp(mn_str(t.root), "[18 45]{[11] [27 32]{[23] [29 30] -} [63]{- [72 81]{[67] - [87]}}}"));
    mn_free(t.root);
}
//<<RQ13

//>>objective
/* ---- the objective sets (key: Appendix G) ----
   MCQ 1 (b) M-1      2 (a) M      3 (c) trie      4 (c) degree of node      5 (c) O(log n)
   TF  1 T 2 T 3 F 4 F 5 F 6 T 7 T 8 F 9 F       FIB  M / M-1 ; m / m-1 ; m/2 ; B tree ; O(log N)
   Everything that can be tested is tested below. */
static void objective(void)
{
    /* MCQ 1, 2: an M-way node has M-1 keys and M children; a B tree of order m has at most m children */
    MWay w = { NULL, 4 };
    for (int k = 1; k <= 10; k++) mw_insert(&w, k);
    assert(mw_valid(&w) && w.root->n == 3);                  /* M = 4 -> at most 3 keys, 4 children */
    mn_free(w.root);

    /* TF 1: all leaves are at the same level (bt_valid checks it) — random B trees of every order */
    srand(14);
    for (int m = 3; m <= 12; m++) { BTree t = { NULL, m }; for (int i = 0; i < 300; i++) bt_insert(&t, rand() % 1000); assert(bt_valid(&t)); mn_free(t.root); }

    /* TF 3 "B tree stores unsorted data" is FALSE: the in-order sequence is sorted */
    { BTree t = { NULL, 5 }; for (int i = 0; i < 100; i++) bt_insert(&t, (i * 37) % 101); int a[200], c = mn_inorder(t.root, a); for (int i = 1; i < c; i++) assert(a[i - 1] < a[i]); mn_free(t.root); }

    /* TF 4: "every node has at most m-1 CHILDREN" is FALSE: it is m children and m-1 keys */
    { BTree t = { NULL, 5 }; for (int i = 0; i < 400; i++) bt_insert(&t, rand() % 5000); int maxch = 0; const MNode *s[1024]; int sp = 0; s[sp++] = t.root;
      while (sp) { const MNode *x = s[--sp]; if (!mn_leaf(x)) { if (x->n + 1 > maxch) maxch = x->n + 1; for (int i = 0; i <= x->n; i++) s[sp++] = x->ch[i]; } }
      assert(maxch == 5);                       /* m children, never more — and never m+1 as "at most m-1" would wrongly suggest less */ mn_free(t.root); }

    /* TF 2, 5, 6, 9: B+ — data only in leaves, leaves chained, separators are copies (redundant), and "index nodes" are the INNER nodes */
    { BPlus p = { NULL, 4 }; for (int i = 0; i < 30; i++) bp_insert(&p, i * 2); int a[64], c = bp_leaf_keys(&p, a); assert(c == 30);   /* every key is reachable along the chain */
      int rep = 0; for (int i = 0; i < p.root->n; i++) rep += bp_search(&p, p.root->key[i]); assert(rep == p.root->n);  /* root separators ARE keys that also live in a leaf, here */
      assert(!mn_leaf(p.root)); mn_free(p.root); }

    /* FIB 5 / MCQ 5: a balanced tree guarantees O(log N): height <= log2(n+1) for a 2-3 tree */
    { BTree t = { NULL, 3 }; for (int i = 0; i < 5000; i++) bt_insert(&t, i); assert(mn_height(t.root) <= (int)ceil(log2(5001.0))); mn_free(t.root); }
    printf("objective: every testable statement checked\n");
}
//<<objective

int main(void)
{
    rq1(); rq2(); rq5(); rq6(); rq8(); rq10(); rq12(); rq13(); objective();
    puts("ex22_01: all checks passed");
    return 0;
}
