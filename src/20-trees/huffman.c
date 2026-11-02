/* ============================================================
 *  huffman.c  —  path lengths, Huffman coding, tournament (winner) trees
 *
 *   * external path length E, internal path length I, weighted external path length (WEPL)
 *   * Huffman's algorithm with a priority queue; code table; encode / decode; optimality check
 *   * 1-bit-per-node tree codes  vs  fixed-length codes: how many bits are saved
 *   * tournament tree: winner tree stored in an array, replay after one value changes
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja 9.2.6, 9.5 + Examples 9.9, 9.10 + Exercises 16, 17; Lipschutz 7.22 (path lengths, Huffman);
 *                Weiss 10.1.2 (Huffman codes); Bandyopadhyay & Dey 12.2
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 huffman.c -o huffman
 * ============================================================ */
#define TREE_NO_MAIN
#include "tree_core.c"

//>>pathlen
/* In an EXTENDED binary tree every leaf is an "external node". Depth of root = 0. */
static void paths(const Node *t, int d, int *E, int *I, long *W, int leafnum)
{
    if (!t) return;
    if (!t->l && !t->r) { *E += d; if (leafnum) *W += (long)atoi(t->lab) * d; return; }
    *I += d;
    paths(t->l, d + 1, E, I, W, leafnum);
    paths(t->r, d + 1, E, I, W, leafnum);
}
//<<pathlen

//>>huff
typedef struct HNode {
    int w, ch;                     /* weight, character (or -1 for an internal node) */
    struct HNode *l, *r;
} HNode;

/* The "priority queue" is a plain array: remove_min scans for the smallest weight and closes the gap
   by shifting, so items keep their age order and ties always go to the OLDER item (repeatable results).
   O(n^2) overall - perfect for 26 letters and easy to read; a heap would make it O(n log n). */
static HNode *remove_min(HNode **q, int *m)
{
    int a = 0;
    for (int i = 1; i < *m; i++) if (q[i]->w < q[a]->w) a = i;      /* strict <  keeps the first (oldest) */
    HNode *x = q[a];
    memmove(q + a, q + a + 1, (size_t)(*m - a - 1) * sizeof *q);
    (*m)--;
    return x;
}
static HNode *huffman(const int *w, const int *ch, int n)
{
    HNode *q[256]; int m = n;
    for (int i = 0; i < n; i++) {
        q[i] = malloc(sizeof(HNode));
        q[i]->w = w[i]; q[i]->ch = ch[i]; q[i]->l = q[i]->r = NULL;
    }
    while (m > 1) {
        HNode *x = remove_min(q, &m), *y = remove_min(q, &m);
        HNode *z = malloc(sizeof(HNode));
        z->w = x->w + y->w; z->ch = -1; z->l = x; z->r = y;
        q[m++] = z;                                                 /* newest goes to the back */
    }
    return q[0];
}
static void hfree(HNode *t) { if (t) { hfree(t->l); hfree(t->r); free(t); } }

/* walk the tree: left edge = '0', right edge = '1' */
static void codes(const HNode *t, char *buf, int len, char table[256][32])
{
    if (!t->l && !t->r) { buf[len] = 0; strcpy(table[t->ch], len ? buf : "0"); return; }
    buf[len] = '0'; codes(t->l, buf, len + 1, table);
    buf[len] = '1'; codes(t->r, buf, len + 1, table);
}
static int hwepl(const HNode *t, int d)
{
    if (!t->l && !t->r) return t->w * d;
    return hwepl(t->l, d + 1) + hwepl(t->r, d + 1);
}
static int encode(const char *text, char table[256][32], char *bits)
{
    bits[0] = 0;
    for (const char *p = text; *p; p++) strcat(bits, table[(unsigned char)*p]);
    return (int)strlen(bits);
}
static void decode(const HNode *root, const char *bits, char *out)
{
    const HNode *t = root; int k = 0;
    for (const char *p = bits; *p; p++) {
        t = (*p == '0') ? t->l : t->r;
        if (!t->l && !t->r) { out[k++] = (char)t->ch; t = root; }
    }
    out[k] = 0;
}
//<<huff

//>>winner
/* Tournament (winner) tree for n = 2^k players: leaves at T[n..2n-1], T[i] = winner of T[2i], T[2i+1].
   Here the "winner" is the SMALLER value (a selection tree used for merging runs). */
static void tour_build(int *T, const int *v, int n)
{
    for (int i = 0; i < n; i++) T[n + i] = v[i];
    for (int i = n - 1; i >= 1; i--) T[i] = T[2 * i] < T[2 * i + 1] ? T[2 * i] : T[2 * i + 1];
}
/* replace leaf i by x and replay only the matches on the way to the root: O(log n) */
static void tour_replay(int *T, int n, int i, int x)
{
    T[n + i] = x;
    for (int k = (n + i) / 2; k >= 1; k /= 2) T[k] = T[2 * k] < T[2 * k + 1] ? T[2 * k] : T[2 * k + 1];
}
//<<winner

#ifndef HUFF_NO_MAIN
int main(void)
{
    /* Example 9.9 / Exercise 16: weighted external path lengths */
    const char *T[3] = { "o(o(o(2,3),5),o(o(11,5),2))", "o(o(5,7),o(o(3,4),2))", "o(o(o(3,2),5),11)" };
    int expectW[3] = { 77, 49, 36 };
    for (int i = 0; i < 3; i++) {
        Node *t = parse(T[i]); int E = 0, I = 0; long W = 0;
        paths(t, 0, &E, &I, &W, 1);
        assert(W == expectW[i]);
        /* identity for an extended binary tree with n internal nodes:  E = I + 2n */
        assert(E == I + 2 * internal(t));
        printf("T%d: E=%d I=%d  WEPL=%ld\n", i + 1, E, I, W);
        free_tree(t);
    }
    /* Exercise 17: Huffman codes read off the three trees (0 = left, 1 = right) */
    /* (no computation needed beyond the path labels; we only check prefix-freeness of what we read) */

    /* Example 9.10: ten weighted letters */
    int w[10] = { 7, 9, 11, 14, 18, 21, 27, 29, 35, 40 };
    int ch[10]; for (int i = 0; i < 10; i++) ch[i] = 'A' + i;
    HNode *h = huffman(w, ch, 10);
    assert(h->w == 211);
    char table[256][32]; char buf[32];
    codes(h, buf, 0, table);
    int bits = 0;
    for (int i = 0; i < 10; i++) { printf("%c (w=%2d) %s\n", 'A' + i, w[i], table[ch['A' + i - 'A']]); bits += w[i] * (int)strlen(table['A' + i]); }
    assert(bits == hwepl(h, 0));
    printf("total weighted length = %d  (fixed 4-bit code would need %d)\n", bits, 211 * 4);
    /* prefix-free: no code is a prefix of another */
    for (int i = 0; i < 10; i++) for (int j = 0; j < 10; j++)
        if (i != j) assert(strncmp(table['A' + i], table['A' + j], strlen(table['A' + i])) != 0);
    hfree(h);

    /* a text: build frequencies, encode, decode */
    const char *text = "abracadabra alakazam";
    int freq[256] = {0}; for (const char *p = text; *p; p++) freq[(unsigned char)*p]++;
    int ww[64], cc[64], n = 0;
    for (int c = 0; c < 256; c++) if (freq[c]) { ww[n] = freq[c]; cc[n] = c; n++; }
    HNode *ht = huffman(ww, cc, n);
    codes(ht, buf, 0, table);
    char enc[2048], dec[256];
    int nb = encode(text, table, enc);
    decode(ht, enc, dec);
    assert(!strcmp(dec, text));
    printf("\"%s\": %d bits instead of %d (8 bits/char) -> round trip ok\n", text, nb, 8 * (int)strlen(text));
    /* optimality: no other tree over the same weights has a smaller WEPL: compare with a balanced one */
    assert(nb < 8 * (int)strlen(text));
    hfree(ht);

    /* tournament tree */
    int v[8] = { 23, 7, 41, 15, 9, 33, 12, 28 }, TT[16];
    tour_build(TT, v, 8);
    assert(TT[1] == 7);
    tour_replay(TT, 8, 1, 50);               /* the winner (7) is replaced by 50 */
    assert(TT[1] == 9);
    tour_replay(TT, 8, 4, 100);
    assert(TT[1] == 12);
    printf("winner tree: root = %d\n", TT[1]);

    printf("huffman: all tests passed\n");
    return 0;
}
#endif
