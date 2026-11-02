/* ============================================================
 *  trie.c  —  Thareja's trie (11.5, Fig. 11.18): a dictionary stored by letters
 *
 *  Nodes do not store keys.  The PATH from the root spells the key; a node that ends a word carries its value.
 *  Operations on a word of length L take O(L) time, whatever the number of words.
 *    - insert / search / delete (with pruning of nodes that no word needs any more)
 *    - words_with_prefix  : walk to the prefix node, then list its subtree in alphabetical order
 *    - longest_prefix     : the longest stored word that is a prefix of a query string
 *  The radix/R-way/ternary/Patricia tries of Sedgewick's ch.15 are in ../15-tries/ — this is the smallest version.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Sec. 11.5
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 trie.c -o trie
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//>>trie
typedef struct TNode {
    struct TNode *child[26];        /* one pointer per letter 'a'..'z' */
    int value;                      /* -1 = no word ends here */
} TNode;

static long trie_nodes = 0;
static TNode *tn_new(void)
{
    TNode *x = calloc(1, sizeof *x);
    if (!x) { perror("calloc"); exit(1); }
    x->value = -1; trie_nodes++;
    return x;
}
static void tn_free(TNode *x) { if (!x) return; for (int c = 0; c < 26; c++) tn_free(x->child[c]); free(x); trie_nodes--; }

/* insert word -> value (value >= 0). Returns 1 if the word is new, 0 if it only replaced a value. */
static int trie_insert(TNode *root, const char *w, int value)
{
    TNode *x = root;
    for (; *w; w++) {
        int c = *w - 'a';
        if (!x->child[c]) x->child[c] = tn_new();
        x = x->child[c];
    }
    int fresh = x->value < 0;
    x->value = value;
    return fresh;
}
static int trie_search(const TNode *root, const char *w)      /* the value, or -1 */
{
    const TNode *x = root;
    for (; *w && x; w++) x = x->child[*w - 'a'];
    return x ? x->value : -1;
}
static int has_child(const TNode *x) { for (int c = 0; c < 26; c++) if (x->child[c]) return 1; return 0; }
/* delete: returns 1 when the node itself can be freed by the caller (no word ends here, no children) */
static int trie_del_(TNode *x, const char *w, int *removed)
{
    if (!*w) {
        if (x->value >= 0) { x->value = -1; *removed = 1; }
    } else {
        int c = *w - 'a';
        if (x->child[c] && trie_del_(x->child[c], w + 1, removed)) { free(x->child[c]); trie_nodes--; x->child[c] = NULL; }
    }
    return x->value < 0 && !has_child(x);
}
static int trie_delete(TNode *root, const char *w) { int r = 0; trie_del_(root, w, &r); return r; }

/* list every word below x (alphabetical order is automatic: children are visited a..z) */
static void trie_walk_(const TNode *x, char *buf, int len, void (*visit)(const char *, int, void *), void *ud)
{
    if (x->value >= 0) { buf[len] = 0; visit(buf, x->value, ud); }
    for (int c = 0; c < 26; c++) if (x->child[c]) { buf[len] = (char)('a' + c); trie_walk_(x->child[c], buf, len + 1, visit, ud); }
}
static void words_with_prefix(const TNode *root, const char *prefix, void (*visit)(const char *, int, void *), void *ud)
{
    const TNode *x = root;
    for (const char *p = prefix; *p && x; p++) x = x->child[*p - 'a'];
    if (!x) return;
    char buf[64]; strcpy(buf, prefix);
    trie_walk_(x, buf, (int)strlen(prefix), visit, ud);
}
/* longest stored word that is a prefix of q; writes it to out, returns its length (0 if none) */
static int longest_prefix(const TNode *root, const char *q, char *out)
{
    const TNode *x = root; int best = 0;
    for (int i = 0; q[i] && x; i++) {
        x = x->child[q[i] - 'a'];
        if (x && x->value >= 0) best = i + 1;
    }
    memcpy(out, q, (size_t)best); out[best] = 0;
    return best;
}
//<<trie

#ifndef TRIE_NO_MAIN
static void collect(const char *w, int v, void *ud) { char *s = ud; char tmp[80]; snprintf(tmp, sizeof tmp, "%s=%d ", w, v); strcat(s, tmp); }

int main(void)
{
    /* ---- the words of Fig. 11.18 (each word carries an arbitrary integer, as in the book) ---- */
    //>>fig1118
    TNode *root = tn_new();
    struct { const char *w; int v; } W[] = { {"am", 3}, {"arc", 11}, {"are", 7}, {"arm", 9}, {"art", 12}, {"do", 8}, {"dot", 15} };
    for (int i = 0; i < 7; i++) assert(trie_insert(root, W[i].w, W[i].v));
    printf("7 words, %ld nodes (root included); the words have %d letters in total\n", trie_nodes, 2 + 3 + 3 + 3 + 3 + 2 + 3);
    assert(trie_nodes == 1 + /*a*/1 + /*am*/1 + /*ar*/1 + /*arc arm are art*/4 + /*d do dot*/3);
    for (int i = 0; i < 7; i++) assert(trie_search(root, W[i].w) == W[i].v);
    assert(trie_search(root, "ar") == -1);                /* a prefix of words but not a word */
    assert(trie_search(root, "dog") == -1 && trie_search(root, "zoo") == -1);
    assert(!trie_insert(root, "arm", 99) && trie_search(root, "arm") == 99);   /* replace */
    trie_insert(root, "arm", 9);

    char s[256] = "";
    words_with_prefix(root, "ar", collect, s);
    printf("words starting with \"ar\": %s\n", s);
    assert(!strcmp(s, "arc=11 are=7 arm=9 art=12 "));
    s[0] = 0; words_with_prefix(root, "", collect, s);
    printf("all words, alphabetical : %s\n", s);
    assert(!strcmp(s, "am=3 arc=11 are=7 arm=9 art=12 do=8 dot=15 "));

    char out[64];
    assert(longest_prefix(root, "armchair", out) == 3 && !strcmp(out, "arm"));
    assert(longest_prefix(root, "dotted", out) == 3 && !strcmp(out, "dot"));
    assert(longest_prefix(root, "amaze", out) == 2 && !strcmp(out, "am"));
    assert(longest_prefix(root, "xyz", out) == 0);
    longest_prefix(root, "armchair", out); printf("longest stored prefix of \"armchair\": %s\n", out);

    long before = trie_nodes;
    assert(trie_delete(root, "dot") && trie_search(root, "do") == 8);
    assert(trie_nodes == before - 1);                     /* only the node 't' was needed by "dot" alone */
    assert(trie_delete(root, "do") && trie_nodes == before - 3);   /* now 'o' and 'd' go too */
    assert(!trie_delete(root, "do"));
    printf("after deleting dot and do: %ld nodes\n", trie_nodes);
    tn_free(root);
    assert(trie_nodes == 0);
    //<<fig1118

    /* ---- space and time against a sorted array of strings: many words share prefixes ---- */
    //>>compare
    {
        srand(5);
        enum { N = 20000 };
        static char words[N][9];
        TNode *t = tn_new();
        long letters = 0; int distinct = 0;
        for (int i = 0; i < N; i++) {
            int len = 3 + rand() % 6;
            for (int j = 0; j < len; j++) words[i][j] = (char)('a' + (rand() % 100 < 70 ? rand() % 6 : rand() % 26));   /* skewed letters, like real text */
            words[i][len] = 0;
            letters += len;
            distinct += trie_insert(t, words[i], i);
        }
        for (int i = 0; i < N; i++) assert(trie_search(t, words[i]) >= 0);
        printf("%d random words (%d distinct), %ld letters stored in plain strings;\n  the trie needs %ld nodes (%.0f%% of the letters) — shared prefixes cost nothing\n",
               N, distinct, letters, trie_nodes, 100.0 * (double)trie_nodes / (double)letters);
        assert(trie_nodes < letters);
        tn_free(t);
    }
    //<<compare
    puts("trie: all checks passed");
    return 0;
}
#endif
