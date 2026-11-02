/* ============================================================
 *  twothree.c  —  2-3 trees (Thareja 11.4, Fig. 11.14-11.17, Examples 11.7-11.9)
 *
 *  A 2-3 tree is a B tree of order 3: every inner node has 2 children (a "2-node", 1 key) or 3 children
 *  (a "3-node", 2 keys), and all the leaves are on one level.  Thareja's 2-3 trees keep keys in the inner nodes
 *  as well as in the leaves, which is exactly btree.c with m = 3 — so insert/delete are btree.c's, and this file adds
 *    - the search with the book's k1 / k2 rules and a trace of the path (Example 11.7),
 *    - the three book examples checked step by step,
 *    - the height bounds  log3(n+1) <= h <= log2(n+1).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Sec. 11.4
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 twothree.c -o twothree -lm
 * ============================================================ */
#define BTREE_NO_MAIN
#include "btree.c"

//>>tt
/* search with the rules of Sec. 11.4.1; `path` receives the keys of the nodes visited, e.g. "[54 90] [72] [63]" */
static int tt_search(const MNode *root, int x, char *path, size_t cap)
{
    path[0] = 0;
    for (const MNode *n = root; n; ) {
        size_t len = strlen(path);
        snprintf(path + len, cap - len, "%s[", len ? " " : "");
        for (int i = 0; i < n->n; i++) { len = strlen(path); snprintf(path + len, cap - len, i ? " %d" : "%d", n->key[i]); }
        len = strlen(path); snprintf(path + len, cap - len, "]");
        if (n->key[0] == x || (n->n == 2 && n->key[1] == x)) return 1;
        if (x < n->key[0]) n = n->ch[0];                                 /* x < k1: left */
        else if (n->n == 1) n = n->ch[1];                                /* x >= k1 and only two children: right */
        else if (x < n->key[1]) n = n->ch[1];                            /* three children, x < k2: middle */
        else n = n->ch[2];                                               /* x >= k2: right */
    }
    return 0;
}
/* every node is a 2-node or a 3-node and the leaves are level: btree.c's validator with m = 3 says exactly that */
//<<tt

#ifndef TT_NO_MAIN
int main(void)
{
    /* ---- Example 11.7: search 63 in Fig. 11.14 ---- */
    //>>ex117
    {
        BTree t = { mn_parse("[54 90]{[27]{[18] [36 45]} [72]{[63] [81]} [117 153]{[108 111] [135 144] [162]}}"), 3 };
        assert(bt_valid(&t));
        char path[128];
        assert(tt_search(t.root, 63, path, sizeof path));
        printf("Fig 11.14, search 63 : %s   (54 < 63 < 90 middle, 63 < 72 left)\n", path);
        assert(!strcmp(path, "[54 90] [72] [63]"));
        assert(tt_search(t.root, 90, path, sizeof path) && !strcmp(path, "[54 90]"));
        assert(!tt_search(t.root, 100, path, sizeof path));
        printf("Fig 11.14, search 100: %s   (absent: ends at a leaf)\n", path);
        assert(!strcmp(path, "[54 90] [117 153] [108 111]"));
        assert(bt_search(&t, 63) && !bt_search(&t, 100));        /* btree.c's search agrees */
        mn_free(t.root);
    }
    //<<ex117

    /* ---- Example 11.8: insert 39, 37, 42, 47 ---- */
    //>>ex118
    {
        BTree t = { mn_parse("[54]{[36]{[18 27] [45]} [72 90]{[63] [81] [99]}}"), 3 };
        assert(bt_valid(&t));
        bt_insert(&t, 39); assert(!strcmp(mn_str(t.root), "[54]{[36]{[18 27] [39 45]} [72 90]{[63] [81] [99]}}"));
        bt_insert(&t, 37); assert(!strcmp(mn_str(t.root), "[54]{[36 39]{[18 27] [37] [45]} [72 90]{[63] [81] [99]}}"));
        bt_insert(&t, 42); assert(!strcmp(mn_str(t.root), "[54]{[36 39]{[18 27] [37] [42 45]} [72 90]{[63] [81] [99]}}"));
        bt_insert(&t, 47);
        assert(!strcmp(mn_str(t.root), "[39 54]{[36]{[18 27] [37]} [45]{[42] [47]} [72 90]{[63] [81] [99]}}"));
        printf("Example 11.8 final: %s\n", mn_str(t.root));
        bt_levels(&t);
        assert(bt_valid(&t));
        mn_free(t.root);
    }
    //<<ex118

    /* ---- Example 11.9: delete 69, 72, 99, 81 ---- */
    //>>ex119
    {
        BTree t = { mn_parse("[54]{[36]{[18 27] [45]} [69 90]{[63] [72 81] [99]}}"), 3 };
        assert(bt_valid(&t));
        struct { int k; const char *tree; const char *how; } S[] = {
            { 69, "[54]{[36]{[18 27] [45]} [72 90]{[63] [81] [99]}}",        "" },
            { 72, "[54]{[36]{[18 27] [45]} [90]{[63 81] [99]}}",             "merge-left" },
            { 99, "[54]{[36]{[18 27] [45]} [81]{[63] [90]}}",                "borrow-left" },
            { 81, "[36 54]{[18 27] [45] [63 90]}",                           "merge-left merge-left" },
        };
        for (int i = 0; i < 4; i++) {
            assert(bt_delete(&t, S[i].k)); assert(bt_valid(&t));
            printf("  delete %2d -> %-52s %s\n", S[i].k, mn_str(t.root), bt_fixlog);
            assert(!strcmp(mn_str(t.root), S[i].tree));
            assert(!strcmp(bt_fixlog, S[i].how));
        }
        mn_free(t.root);
    }
    //<<ex119

    /* ---- height of a 2-3 tree with n keys:  log3(n+1) <= h <= log2(n+1) ---- */
    //>>tth
    {
        printf("sorted insertion into a 2-3 tree:   n    height   log3(n+1)  log2(n+1)\n");
        for (int n = 10; n <= 100000; n *= 10) {
            BTree t = { NULL, 3 };
            for (int k = 0; k < n; k++) bt_insert(&t, k);
            int h = mn_height(t.root);
            double lo = log(n + 1.0) / log(3.0), hi = log(n + 1.0) / log(2.0);
            printf("                              %7d  %5d   %8.2f  %8.2f\n", n, h, lo, hi);
            assert(h >= lo - 1e-9 && h <= hi + 1e-9);
            assert(bt_valid(&t));
            mn_free(t.root);
        }
    }
    //<<tth
    puts("twothree: all checks passed");
    return 0;
}
#endif
