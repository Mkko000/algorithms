/* ============================================================
 *  ex15_11_trie.c  —  Exercises 15.11-15.14, 15.18, 15.19, 15.24
 *  Chapter 15: Radix Search — binary tries (Programs 15.2, 15.3)
 *
 *  15.11 Draw the trie that results when you insert items with the keys
 *        E A S Y Q U T I O N in that order into an initially empty trie.
 *  15.12 What happens when you use Program 15.3 to insert a record whose
 *        key is equal to some key already in the trie?
 *  15.13 Draw the trie that results when you insert items with the keys
 *        01010011 00000111 00100001 01010001 11101100 00100001 10010101
 *        01001010 into an initially empty trie.
 *  15.14 Run empirical studies to compare the height, number of nodes, and
 *        internal path length of a trie built by insertion of N random
 *        32-bit keys into an initially empty trie with the same measures of
 *        a standard binary search tree and a red-black tree (Chapter 13)
 *        built from the same keys, for N = 10^3, 10^4, 10^5, and 10^6.
 *  15.18 Implement the sort operation for a trie-based symbol table.
 *  15.19 Write a program that prints out all keys in a trie that have the
 *        same initial t bits as a given search key.
 *  15.24 Write a program to compute the number of nodes in the trie
 *        corresponding to a given set of distinct fixed-length keys, by
 *        sorting them and comparing adjacent keys in the sorted list.
 *
 *  بالعربي: نرسم الـ trie لـ EASYQUTION وللمفاتيح الـ 8 بت، ونشوف إيه اللي بيحصل
 *  لو دخّلنا مفتاح موجود، ونقارن الـ trie بالـ BST والـ red-black على مفاتيح
 *  عشوائية، ونطبع المفاتيح بالترتيب، ونطبع كل المفاتيح اللي أول t بت فيها زي
 *  مفتاح معيّن، ونعدّ الـ nodes من غير ما نبني الـ trie أصلًا (sort + جيران).
 *
 *  THINKING STEPS
 *   1. A trie keeps keys ONLY in leaves; an internal node at depth w just
 *      says "look at bit w". The shape depends on the key SET, not on the
 *      insertion order (unlike BST and DST): the leaf for a key sits at the
 *      first depth where its prefix is unique.
 *   2. Insert = search; if we miss on a leaf holding key k, split(): make
 *      one internal node per bit where v and k agree, then one node where
 *      they differ. If v == k they never differ: Program 15.3 recurses
 *      forever (15.12). We guard it: stop when the bits run out.
 *   3. Because left = 0 < 1 = right at every level, an inorder walk of the
 *      leaves IS the sorted order (15.18), and "same first t bits" is a
 *      subtree: walk down t levels, then print the whole subtree (15.19).
 *   4. 15.24: an internal node = a prefix shared by at least two keys. In
 *      sorted order, keys sharing a prefix are contiguous, so the shared
 *      prefixes are exactly the prefixes of length 0..lcp(k[i],k[i+1]) of
 *      adjacent pairs. Pair i adds only lengths above lcp of pair i-1:
 *        internal = (lcp_0 + 1) + sum_{i>=1} max(0, lcp_i - lcp_{i-1}).
 *      Nodes = internal + N leaves. We check it against the real trie.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 15.2-15.3, Exercises 15.11-15.24
 *  Build:   gcc -std=c99 -Wall -O2 ex15_11_trie.c -o ex15_11_trie
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int W = 5;
static int digit(unsigned v, int b) { return (int)((v >> (W - 1 - b)) & 1u); }

typedef struct node *link;
struct node { unsigned key; int leaf; link l, r; };
static long nnodes;
static link NEW(unsigned v, int leaf) { link x = malloc(sizeof *x); x->key = v; x->leaf = leaf; x->l = x->r = NULL; nnodes++; return x; }
static int dupHit;

/* split p (new leaf) from q (old leaf), starting at bit w (Program 15.3) */
static link split(link p, link q, int w)
{
    if (w >= W) { dupHit = 1; free(p); nnodes--; return q; }   /* 15.12 guard: keys equal */
    link t = NEW(0, 0);
    switch (digit(p->key, w) * 2 + digit(q->key, w)) {
        case 0: t->l = split(p, q, w + 1); break;
        case 1: t->l = p; t->r = q; break;
        case 2: t->r = p; t->l = q; break;
        case 3: t->r = split(p, q, w + 1); break;
    }
    return t;
}
static link insertR(link h, unsigned v, int w)
{
    if (h == NULL) return NEW(v, 1);
    if (h->leaf) {
        if (h->key == v) { dupHit = 1; return h; }  /* 15.12: test first, or split() builds a dead chain */
        return split(NEW(v, 1), h, w);
    }
    if (digit(v, w) == 0) h->l = insertR(h->l, v, w + 1);
    else                  h->r = insertR(h->r, v, w + 1);
    return h;
}
static void freeT(link h) { if (h) { freeT(h->l); freeT(h->r); free(h); } }

static void bin(unsigned v, char *s) { for (int i = 0; i < W; i++) s[i] = '0' + digit(v, i); s[W] = 0; }
static const char *nameOf(unsigned v, int letters) { static char b[40]; if (letters) { b[0] = 'A' + v - 1; b[1] = 0; } else bin(v, b); return b; }

/* print leaves with their path; internal nodes as "(*)" */
static void show(link h, char *path, int d, int letters)
{
    if (!h) return;
    path[d] = 0;
    if (h->leaf) { char b[40]; bin(h->key, b); printf("       leaf  %-8s depth %d: %s%s%s\n", d ? path : "(root)", d, nameOf(h->key, letters), letters ? " = " : "", letters ? b : ""); return; }
    path[d] = '0'; show(h->l, path, d + 1, letters);
    path[d] = '1'; show(h->r, path, d + 1, letters);
}
static void countT(link h, long *in, long *lv, int d, long *ipl, int *ht)
{
    if (!h) return;
    if (h->leaf) { (*lv)++; *ipl += d; if (d > *ht) *ht = d; return; }
    (*in)++; countT(h->l, in, lv, d + 1, ipl, ht); countT(h->r, in, lv, d + 1, ipl, ht);
}

/* 15.18 sort: inorder over leaves */
static void sortT(link h, int letters) { if (!h) return; if (h->leaf) { printf(" %s", nameOf(h->key, letters)); return; } sortT(h->l, letters); sortT(h->r, letters); }

/* 15.19 all keys with the same first t bits as v */
static void prefixT(link h, unsigned v, int t, int w, int letters)
{
    if (!h) return;
    if (h->leaf) {                                   /* a leaf above depth t: check the rest of its bits */
        for (int i = w; i < t; i++) if (digit(h->key, i) != digit(v, i)) return;
        printf(" %s", nameOf(h->key, letters)); return;
    }
    if (w < t) prefixT(digit(v, w) ? h->r : h->l, v, t, w + 1, letters);
    else       sortT(h, letters);                    /* whole subtree matches */
}

/* 15.24: nodes from the sorted key list only */
static int cmpu(const void *a, const void *b) { unsigned x = *(const unsigned *)a, y = *(const unsigned *)b; return x < y ? -1 : x > y; }
static int lcp(unsigned a, unsigned b) { int i = 0; while (i < W && digit(a, i) == digit(b, i)) i++; return i; }
static long nodesFromSorted(unsigned *k, int n)
{
    qsort(k, n, sizeof *k, cmpu);
    if (n < 2) return n;
    long in = lcp(k[0], k[1]) + 1; int prev = lcp(k[0], k[1]);
    for (int i = 1; i + 1 < n; i++) { int c = lcp(k[i], k[i + 1]); if (c > prev) in += c - prev; prev = c; }
    return in + n;
}

/* ---- BST and left-leaning red-black tree on the same keys, for 15.14 ---- */
typedef struct bnode *blink;
struct bnode { unsigned key; int red; blink l, r; };
static blink bNEW(unsigned v) { blink x = malloc(sizeof *x); x->key = v; x->red = 1; x->l = x->r = NULL; return x; }
static blink bstIns(blink h, unsigned v)
{   /* iterative: sorted-ish input cannot overflow the stack */
    blink x = bNEW(v); if (!h) return x;
    blink p = h; for (;;) { blink *q = v < p->key ? &p->l : &p->r; if (!*q) { *q = x; return h; } p = *q; }
}
static int isRed(blink h) { return h && h->red; }
static blink rotL(blink h) { blink x = h->r; h->r = x->l; x->l = h; x->red = h->red; h->red = 1; return x; }
static blink rotR(blink h) { blink x = h->l; h->l = x->r; x->r = h; x->red = h->red; h->red = 1; return x; }
static blink rbIns(blink h, unsigned v)
{
    if (!h) return bNEW(v);
    if (v < h->key) h->l = rbIns(h->l, v); else h->r = rbIns(h->r, v);
    if (isRed(h->r) && !isRed(h->l)) h = rotL(h);
    if (isRed(h->l) && isRed(h->l->l)) h = rotR(h);
    if (isRed(h->l) && isRed(h->r)) { h->red = !h->red; h->l->red = 0; h->r->red = 0; h->red = 1; }
    return h;
}
static void bstat(blink h, int d, long *ipl, int *ht) { if (!h) return; *ipl += d; if (d > *ht) *ht = d; bstat(h->l, d + 1, ipl, ht); bstat(h->r, d + 1, ipl, ht); }
static void bfree(blink h) { if (h) { bfree(h->l); bfree(h->r); free(h); } }

static unsigned long long rs = 1500;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 16); }

int main(void)
{
    char path[64];
    link h = NULL; long in, lv, ipl; int ht;

    /* ---------- 15.11 ---------- */
    W = 5; nnodes = 0;
    for (const char *c = "EASYQUTION"; *c; c++) h = insertR(h, *c - 'A' + 1, 0);
    printf("15.11 trie for E A S Y Q U T I O N (5-bit code):\n");
    show(h, path, 0, 1);
    in = lv = ipl = 0; ht = 0; countT(h, &in, &lv, 0, &ipl, &ht);
    printf("     %ld internal nodes + %ld leaves, height %d\n", in, lv, ht);
    printf("15.18 sort (inorder over the leaves):"); sortT(h, 1); printf("\n");
    printf("15.19 keys with the same first 2 bits as Q = 10001:"); prefixT(h, 'Q' - 'A' + 1, 2, 0, 1);
    printf("\n     keys with the same first 3 bits as O = 01111:"); prefixT(h, 'O' - 'A' + 1, 3, 0, 1); printf("\n");
    { unsigned k[10]; int n = 0; for (const char *c = "EASYQUTION"; *c; c++) k[n++] = *c - 'A' + 1;
      printf("15.24 nodes from the sorted keys: %ld (built trie: %ld)\n", nodesFromSorted(k, n), in + lv); }
    printf("     the same keys inserted backwards give:");
    { link g = NULL; for (const char *c = "NOITUQYSAE"; *c; c++) g = insertR(g, *c - 'A' + 1, 0);
      long i2 = 0, l2 = 0, p2 = 0; int h2 = 0; countT(g, &i2, &l2, 0, &p2, &h2);
      printf(" %ld internal, height %d, path length %ld (vs %ld): the shape depends only on the key set\n", i2, h2, p2, ipl); freeT(g); }

    /* ---------- 15.12 ---------- */
    { link p = NEW('S' - 'A' + 1, 1), q = NEW('S' - 'A' + 1, 1);
      dupHit = 0; link t = split(p, q, 3);           /* what Program 15.3 does with an equal key at depth 3 */
      long ci = 0, cl = 0, cp = 0; int ch = 0; countT(t, &ci, &cl, 0, &cp, &ch);
      printf("15.12 Program 15.3 on a second S: split() sees equal bits at 3, 4, ... and never stops\n"
             "     (past bit %d it reads garbage / overflows the stack). With a guard at the last bit it\n"
             "     made %ld useless internal nodes before giving up (guard fired: %s).\n"
             "     Fix: compare keys before splitting; our insertR() does, so S is simply ignored.\n",
             W - 1, ci, dupHit ? "yes" : "no");
      freeT(t); }
    freeT(h); h = NULL;

    /* ---------- 15.13 ---------- */
    W = 8;
    { const char *ks[] = { "01010011", "00000111", "00100001", "01010001", "11101100", "00100001", "10010101", "01001010" };
      for (int i = 0; i < 8; i++) { dupHit = 0; h = insertR(h, (unsigned)strtoul(ks[i], NULL, 2), 0); if (dupHit) printf("15.13 note: %s is already there (it appears twice), so it is skipped\n", ks[i]); }
      printf("15.13 trie for the 8-bit keys:\n");
      show(h, path, 0, 0);
      in = lv = ipl = 0; ht = 0; countT(h, &in, &lv, 0, &ipl, &ht);
      printf("     %ld internal nodes + %ld leaves, height %d\n", in, lv, ht);
      printf("15.19 keys with the same first 3 bits as 01011111:"); prefixT(h, 0x5F, 3, 0, 0); printf("\n");
      unsigned k[7]; int n = 0; for (int i = 0; i < 8; i++) { unsigned v = (unsigned)strtoul(ks[i], NULL, 2); int seen = 0; for (int j = 0; j < n; j++) if (k[j] == v) seen = 1; if (!seen) k[n++] = v; }
      printf("15.24 nodes from the sorted keys: %ld (built trie: %ld)\n", nodesFromSorted(k, n), in + lv);
      freeT(h); h = NULL; }

    /* ---------- 15.14 ---------- */
    W = 32;
    printf("15.14 N random 32-bit keys: height / nodes / average depth of a key\n");
    printf("     %8s | %-28s | %-22s | %-22s\n", "N", "trie (internal+leaves)", "BST", "red-black");
    for (int N = 1000; N <= 1000000; N *= 10) {
        unsigned *k = malloc(N * sizeof *k);
        for (int i = 0; i < N; i++) k[i] = rnd();
        link t = NULL; blink b = NULL, r = NULL; nnodes = 0; int dups = 0;
        for (int i = 0; i < N; i++) { dupHit = 0; t = insertR(t, k[i], 0); if (dupHit) { dups++; continue; } b = bstIns(b, k[i]); r = rbIns(r, k[i]); r->red = 0; }
        long tin = 0, tlv = 0, tipl = 0; int tht = 0; countT(t, &tin, &tlv, 0, &tipl, &tht);
        long bipl = 0, ripl = 0; int bht = 0, rht = 0; bstat(b, 0, &bipl, &bht); bstat(r, 0, &ripl, &rht);
        long n = tlv;
        char s1[64], s2[40], s3[40];
        snprintf(s1, sizeof s1, "h=%2d nodes=%7ld avg %5.2f", tht, tin + tlv, (double)tipl / n);
        snprintf(s2, sizeof s2, "h=%2d nodes=%7ld avg %5.2f", bht, n, (double)bipl / n);
        snprintf(s3, sizeof s3, "h=%2d nodes=%7ld avg %5.2f", rht, n, (double)ripl / n);
        printf("     %8d | %-28s | %-22s | %-22s%s\n", N, s1, s2, s3, dups ? " (dup keys skipped)" : "");
        printf("     %8s   internal/N = %.3f (1/ln2 = 1.443), lg N = %.2f\n", "", (double)tin / n, __builtin_log2((double)n));
        freeT(t); bfree(b); bfree(r); free(k);
    }
    return 0;
}
