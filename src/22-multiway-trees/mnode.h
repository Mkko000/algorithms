/* ============================================================
 *  mnode.h  —  one node type shared by the M-way search tree, the B tree, the B+ tree and the 2-3 tree
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Ch. 11 (Fig. 11.2: P0 K0 P1 K1 ... Pn)
 *  Build:   header only — included by mway.c, btree.c, bplus.c, twothree.c
 *
 *  A node holds n keys  key[0..n-1]  (ascending) and n+1 child pointers  ch[0..n].
 *  ch[i] is the subtree of keys between key[i-1] and key[i].  A leaf has every ch[] == NULL.
 *  Room is kept for ONE extra key and ONE extra child so a node can overflow *before* it is split.
 *  `next` is only used by the B+ tree (the leaf chain).
 *
 *  Text notation (used for printing, for the asserts and for building the book's trees by hand):
 *      [18 45]{[9 11] [27 36]{[23] [29 30] -} [54 63]{- - [72 81]}}
 *  `[..]` = the keys of a node,  `{..}` = its children in order,  `-` = a NULL child (only an M-way tree has those).
 * ============================================================ */
#ifndef MNODE_H
#define MNODE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

//>>mnode
#define MW_MAXM 16                         /* largest order we support */

typedef struct MNode {
    int n;                                 /* number of keys in use */
    int key[MW_MAXM + 1];                  /* one spare slot: a node may hold m keys for a moment */
    struct MNode *ch[MW_MAXM + 2];         /* ch[0..n] */
    struct MNode *next;                    /* B+ tree: next leaf */
} MNode;

static inline MNode *mn_new(void)
{
    MNode *x = calloc(1, sizeof *x);       /* zeroed: n = 0, every pointer NULL */
    if (!x) { perror("calloc"); exit(1); }
    return x;
}
static inline int mn_leaf(const MNode *x) { for (int i = 0; i <= x->n; i++) if (x->ch[i]) return 0; return 1; }
static inline void mn_free(MNode *x)
{
    if (!x) return;
    for (int i = 0; i <= x->n; i++) mn_free(x->ch[i]);
    free(x);
}
//<<mnode

//>>notation
/* ---- print: tree -> text ---- */
static inline void mn_str_(const MNode *x, char *out, size_t cap)
{
    size_t len = strlen(out);
    if (!x) { snprintf(out + len, cap - len, "-"); return; }
    snprintf(out + len, cap - len, "[");
    for (int i = 0; i < x->n; i++) { len = strlen(out); snprintf(out + len, cap - len, i ? " %d" : "%d", x->key[i]); }
    len = strlen(out); snprintf(out + len, cap - len, "]");
    if (mn_leaf(x)) return;
    len = strlen(out); snprintf(out + len, cap - len, "{");
    for (int i = 0; i <= x->n; i++) {
        if (i) { len = strlen(out); snprintf(out + len, cap - len, " "); }
        mn_str_(x->ch[i], out, cap);
    }
    len = strlen(out); snprintf(out + len, cap - len, "}");
}
static inline const char *mn_str(const MNode *root)          /* returns a static buffer */
{
    static char buf[8192];
    buf[0] = 0;
    if (!root) { snprintf(buf, sizeof buf, "(empty)"); return buf; }
    mn_str_(root, buf, sizeof buf);
    return buf;
}

/* ---- parse: text -> tree  (builds the book's pictures exactly) ---- */
static inline MNode *mn_parse_(const char **p)
{
    while (isspace((unsigned char)**p)) (*p)++;
    if (**p == '-') { (*p)++; return NULL; }
    assert(**p == '[');
    (*p)++;
    MNode *x = mn_new();
    for (;;) {
        while (isspace((unsigned char)**p)) (*p)++;
        if (**p == ']') { (*p)++; break; }
        char *end; x->key[x->n++] = (int)strtol(*p, &end, 10); assert(end != *p); *p = end;
    }
    if (**p == '{') {
        (*p)++;
        for (int i = 0; i <= x->n; i++) x->ch[i] = mn_parse_(p);
        while (isspace((unsigned char)**p)) (*p)++;
        assert(**p == '}'); (*p)++;
    }
    return x;
}
static inline MNode *mn_parse(const char *s) { const char *p = s; return mn_parse_(&p); }
//<<notation

//>>util
/* ---- small utilities used by every validator ---- */
static inline int mn_count_keys(const MNode *x)
{
    if (!x) return 0;
    int c = x->n;
    for (int i = 0; i <= x->n; i++) c += mn_count_keys(x->ch[i]);
    return c;
}
static inline int mn_count_nodes(const MNode *x)
{
    if (!x) return 0;
    int c = 1;
    for (int i = 0; i <= x->n; i++) c += mn_count_nodes(x->ch[i]);
    return c;
}
static inline int mn_height(const MNode *x)       /* nodes on the longest path; 0 for an empty tree */
{
    if (!x) return 0;
    int h = 0;
    for (int i = 0; i <= x->n; i++) { int c = mn_height(x->ch[i]); if (c > h) h = c; }
    return h + 1;
}
static inline void mn_inorder_(const MNode *x, int *a, int *k)
{
    if (!x) return;
    for (int i = 0; i < x->n; i++) { mn_inorder_(x->ch[i], a, k); a[(*k)++] = x->key[i]; }
    mn_inorder_(x->ch[x->n], a, k);
}
/* all keys in order into a[] (caller guarantees room); returns how many */
static inline int mn_inorder(const MNode *x, int *a) { int k = 0; mn_inorder_(x, a, &k); return k; }
//<<util
#endif
