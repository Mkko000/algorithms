/* ============================================================
 *  ex15_33_patricia.c  —  Exercises 15.32-15.36, 15.38, 15.41, 15.44
 *  Chapter 15: Radix Search — patricia tries (Programs 15.4-15.6)
 *
 *  15.32 What happens when you use Program 15.5 to insert a record whose
 *        key is equal to some key already in the trie?
 *  15.33 Draw the patricia trie that results when you insert the keys
 *        E A S Y Q U T I O N in that order into an initially empty trie.
 *  15.34 Draw the patricia trie that results when you insert the keys
 *        01010011 00000111 00100001 01010001 11101100 00100001 10010101
 *        01001010 in that order into an initially empty trie.
 *  15.35 Draw the patricia trie that results when you insert the keys
 *        01001010 10010101 00100001 11101100 01010001 00100001 00000111
 *        01010011 in that order into an initially empty trie.
 *  15.36 Run empirical studies to compare the height and internal path
 *        length of a patricia trie built by insertion of N random 32-bit
 *        keys into an initially empty trie with the same measures of a
 *        standard binary search tree and a red-black tree built from the
 *        same keys, for N = 10^3, 10^4, 10^5, and 10^6.
 *  15.38 Implement the select operation for a patricia-based symbol table.
 *  15.41 Write a program that prints out all keys in a patricia trie that
 *        have the same initial t bits as a given search key.
 *  15.44 Show that each key in a patricia trie is on its own search path.
 *
 *  بالعربي: إيه اللي بيحصل لو دخّلنا مفتاح موجود في patricia؟ ارسم patricia لـ
 *  EASYQUTION وللمفاتيح الـ 8 بت بترتيبين مختلفين، وقارنها بالـ BST والـ
 *  red-black، واعمل select وprefix search، واثبت إن كل مفتاح على طريق البحث بتاعه.
 *
 *  THINKING STEPS
 *   1. Patricia = trie with the one-way branches squeezed out. Each node
 *      stores ONE key and the index of the bit it tests. Bit indices grow
 *      as we go down; a link to a node whose bit is not larger (an "up
 *      link") plays the role of a leaf: follow it and compare the key once.
 *   2. Insert (Program 15.5): search for v, land on key t, find the first
 *      bit i where v and t differ, and walk down again until the next node
 *      tests a bit > i (or the link is an up link). Splice a node testing
 *      bit i there; its one link points to the old subtree, the other
 *      points to itself.
 *   3. 15.32: if v == t, there is no differing bit: STinsert returns
 *      without doing anything, so duplicates are silently dropped.
 *   4. 15.34 vs 15.35: same key set (reversed). The bit indices that appear
 *      depend only on the set (they are the internal nodes of the trie),
 *      so the set of tested bits is the same; WHICH key sits in which node
 *      depends on the order.
 *   5. 15.44: the node holding key k is created on the search path for k,
 *      and its self/up link is taken exactly when k's own bit is tested,
 *      so a search for k passes through k's node (and ends there by an up
 *      link). We check it for every key.
 *   6. 15.38 select: an inorder walk over up links visits keys in order,
 *      so stop at the k-th one. 15.41: the trie under the first node with
 *      bit >= t holds exactly the keys with the same first t bits (check
 *      the one key we land on, since patricia skipped some bits).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 15.4-15.6, Exercises 15.32-15.44
 *  Build:   gcc -std=c99 -Wall -O2 ex15_33_patricia.c -o ex15_33_patricia
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int W = 5, LETTERS = 1;
static int digit(unsigned v, int b) { return b >= W ? 0 : (int)((v >> (W - 1 - b)) & 1u); }

typedef struct node *link;
struct node { unsigned key; int bit; link l, r; };
static link head;

static void STinit(void) { head = malloc(sizeof *head); head->key = 0; head->bit = -1; head->l = head->r = head; }
static link searchR(link h, unsigned v, int w, long *bits)   /* Program 15.4 */
{
    if (h->bit <= w) return h;
    if (bits) (*bits)++;
    return searchR(digit(v, h->bit) ? h->r : h->l, v, h->bit, bits);
}
static link insertR(link h, unsigned v, int w, link p)       /* Program 15.5 */
{
    if (h->bit >= w || h->bit <= p->bit) {
        link x = malloc(sizeof *x); x->key = v; x->bit = w;
        x->l = digit(v, w) ? h : x;
        x->r = digit(v, w) ? x : h;
        return x;
    }
    if (digit(v, h->bit) == 0) h->l = insertR(h->l, v, w, h);
    else                       h->r = insertR(h->r, v, w, h);
    return h;
}
static int STinsert(unsigned v)               /* returns 0 if v was already there (15.32) */
{
    link t = searchR(head->l, v, -1, NULL);
    if (t->key == v) return 0;
    int i = 0; while (digit(v, i) == digit(t->key, i)) i++;
    head->l = insertR(head->l, v, i, head);
    return 1;
}
static void freeR(link h, int w) { if (h->bit <= w) return; freeR(h->l, h->bit); freeR(h->r, h->bit); free(h); }
static void STfree(void) { if (head->l != head) freeR(head->l, -1); free(head); }

static const char *nm(unsigned v)
{
    static char buf[4][40]; static int c; char *b = buf[c++ & 3];
    if (LETTERS) { if (v == 0) strcpy(b, "head"); else { b[0] = 'A' + v - 1; b[1] = 0; } }
    else { for (int i = 0; i < W; i++) b[i] = '0' + digit(v, i); b[W] = 0; }
    return b;
}
/* print every node: key, bit tested, and where its links go (down or up) */
static void show(link h, int w, int d)
{
    if (h->bit <= w) return;
    printf("       %*s%s  tests bit %d   0 -> %s%s   1 -> %s%s\n", 2 * d, "", nm(h->key), h->bit,
           nm(h->l->key), h->l->bit <= h->bit ? " (up)" : "", nm(h->r->key), h->r->bit <= h->bit ? " (up)" : "");
    show(h->l, h->bit, d + 1); show(h->r, h->bit, d + 1);
}
static void sortR(link h, int w, void (*visit)(unsigned)) /* Program 15.6 */
{
    if (h->bit <= w) { if (h != head) visit(h->key); return; }
    sortR(h->l, h->bit, visit); sortR(h->r, h->bit, visit);
}
static void pr(unsigned k) { printf(" %s", nm(k)); }

/* 15.38 select: k-th smallest (k = 0, 1, ...) */
static int selk; static unsigned selv; static int found;
static void selVisit(unsigned k) { if (selk-- == 0) { selv = k; found = 1; } }
static int STselect(int k, unsigned *v) { selk = k; found = 0; sortR(head->l, -1, selVisit); *v = selv; return found; }

/* 15.41: keys with the same first t bits as v */
static void prefix(unsigned v, int t)
{
    link h = head->l, p = head;
    while (h->bit > p->bit && h->bit < t) { p = h; h = digit(v, h->bit) ? h->r : h->l; }
    /* every key below here agrees with v on all bits < t that were TESTED; check the skipped bits once */
    link x = (h->bit > p->bit) ? searchR(h, v, p->bit, NULL) : h;
    for (int i = 0; i < t; i++) if (digit(x->key, i) != digit(v, i)) { printf(" (none)"); return; }
    if (h->bit > p->bit) sortR(h, p->bit, pr); else if (h != head) pr(h->key);
}

/* 15.44: is every key on its own search path? */
static int onPathAll, onPathOK;
static void check44(unsigned k)
{
    link h = head->l, p = head; int seen = 0;
    while (h->bit > p->bit) { if (h->key == k) seen = 1; p = h; h = digit(k, h->bit) ? h->r : h->l; }
    onPathAll++; if (seen && h->key == k) onPathOK++;
}

/* stats for 15.36 */
static void stats(link h, int w, int d, long *ipl, int *ht, long *n)
{ if (h->bit <= w) return; (*n)++; *ipl += d; if (d > *ht) *ht = d; stats(h->l, h->bit, d + 1, ipl, ht, n); stats(h->r, h->bit, d + 1, ipl, ht, n); }

typedef struct bnode *blink;
struct bnode { unsigned key; int red; blink l, r; };
static blink bNEW(unsigned v) { blink x = malloc(sizeof *x); x->key = v; x->red = 1; x->l = x->r = NULL; return x; }
static blink bstIns(blink h, unsigned v) { blink x = bNEW(v); if (!h) return x; blink p = h; for (;;) { blink *q = v < p->key ? &p->l : &p->r; if (!*q) { *q = x; return h; } p = *q; } }
static int isRed(blink h) { return h && h->red; }
static blink rotL(blink h) { blink x = h->r; h->r = x->l; x->l = h; x->red = h->red; h->red = 1; return x; }
static blink rotR(blink h) { blink x = h->l; h->l = x->r; x->r = h; x->red = h->red; h->red = 1; return x; }
static blink rbIns(blink h, unsigned v)
{
    if (!h) return bNEW(v);
    if (v < h->key) h->l = rbIns(h->l, v); else h->r = rbIns(h->r, v);
    if (isRed(h->r) && !isRed(h->l)) h = rotL(h);
    if (isRed(h->l) && isRed(h->l->l)) h = rotR(h);
    if (isRed(h->l) && isRed(h->r)) { h->l->red = 0; h->r->red = 0; h->red = 1; }
    return h;
}
static void bstat(blink h, int d, long *ipl, int *ht) { if (!h) return; *ipl += d; if (d > *ht) *ht = d; bstat(h->l, d + 1, ipl, ht); bstat(h->r, d + 1, ipl, ht); }
static void bfree(blink h) { if (h) { bfree(h->l); bfree(h->r); free(h); } }
static unsigned long long rs = 3315;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 16); }

static void build8(const char **ks, const char *title)
{
    STinit();
    printf("%s\n", title);
    for (int i = 0; i < 8; i++) if (!STinsert((unsigned)strtoul(ks[i], NULL, 2))) printf("       (%s is already there: Program 15.5 returns, nothing changes)\n", ks[i]);
    show(head->l, -1, 0);
    printf("       (00000000 is the dummy head)\n       tested bits:");
}
static void bitsR(link h, int w, int *cnt) { if (h->bit <= w) return; cnt[h->bit]++; bitsR(h->l, h->bit, cnt); bitsR(h->r, h->bit, cnt); }

int main(void)
{
    /* ---------- 15.33 / 15.32 / 15.38 / 15.41 / 15.44 ---------- */
    W = 5; LETTERS = 1; STinit();
    for (const char *c = "EASYQUTION"; *c; c++) STinsert(*c - 'A' + 1);
    printf("15.33 patricia for E A S Y Q U T I O N (5-bit code; head is the dummy key 00000):\n");
    show(head->l, -1, 0);
    printf("     sort (Program 15.6):"); sortR(head->l, -1, pr); printf("\n");
    printf("15.32 insert S again -> %s\n", STinsert('S' - 'A' + 1) ? "inserted" : "Program 15.5 finds S, sees no differing bit and returns: duplicates are ignored");
    printf("15.38 select:");
    for (int k = 0; k < 10; k += 3) { unsigned v; STselect(k, &v); printf("  select(%d) = %s", k, nm(v)); }
    printf("\n15.41 keys with the same first 2 bits as Q (10001):"); prefix('Q' - 'A' + 1, 2);
    printf("\n      keys with the same first 3 bits as N (01110):"); prefix('N' - 'A' + 1, 3);
    printf("\n      keys with the same first 3 bits as G (00111):"); prefix('G' - 'A' + 1, 3);
    printf("\n      keys with the same first 4 bits as M (01101):"); prefix('M' - 'A' + 1, 4);
    onPathAll = onPathOK = 0; sortR(head->l, -1, check44);
    printf("\n15.44 keys met on their own search path: %d of %d\n", onPathOK, onPathAll);
    STfree();

    /* ---------- 15.34 / 15.35 ---------- */
    W = 8; LETTERS = 0;
    { const char *a[] = { "01010011", "00000111", "00100001", "01010001", "11101100", "00100001", "10010101", "01001010" };
      const char *b[] = { "01001010", "10010101", "00100001", "11101100", "01010001", "00100001", "00000111", "01010011" };
      int c1[8] = {0}, c2[8] = {0};
      build8(a, "15.34 patricia, keys in the given order:");
      bitsR(head->l, -1, c1); for (int i = 0; i < 8; i++) if (c1[i]) printf(" bit%d x%d", i, c1[i]); printf("\n"); STfree();
      build8(b, "15.35 patricia, the same keys in reverse order:");
      bitsR(head->l, -1, c2); for (int i = 0; i < 8; i++) if (c2[i]) printf(" bit%d x%d", i, c2[i]); printf("\n");
      printf("     same multiset of tested bits: %s (it is fixed by the key set; only the key placement differs)\n", memcmp(c1, c2, sizeof c1) ? "no" : "yes");
      STfree(); }

    /* ---------- 15.36 ---------- */
    W = 32;
    printf("15.36 N random 32-bit keys: height and average depth (internal path length / N)\n");
    printf("     %8s | %-27s | %-17s | %-17s\n", "N", "patricia (avg bits/search)", "BST", "red-black");
    for (int N = 1000; N <= 1000000; N *= 10) {
        STinit(); blink b = NULL, r = NULL; unsigned *k = malloc(N * sizeof *k); int n = 0;
        for (int i = 0; i < N; i++) { unsigned v = rnd(); if (v == 0) v = 1; if (STinsert(v)) { k[n++] = v; b = bstIns(b, v); r = rbIns(r, v); r->red = 0; } }
        long ipl = 0, cnt = 0, bits = 0, bipl = 0, ripl = 0; int ht = 0, bht = 0, rht = 0;
        stats(head->l, -1, 0, &ipl, &ht, &cnt); bstat(b, 0, &bipl, &bht); bstat(r, 0, &ripl, &rht);
        for (int i = 0; i < n; i++) searchR(head->l, k[i], -1, &bits);
        char s1[48], s2[32], s3[32];
        snprintf(s1, sizeof s1, "h=%2d avg %5.2f (%5.2f)", ht, (double)ipl / cnt, (double)bits / n);
        snprintf(s2, sizeof s2, "h=%2d avg %5.2f", bht, (double)bipl / n);
        snprintf(s3, sizeof s3, "h=%2d avg %5.2f", rht, (double)ripl / n);
        printf("     %8d | %-27s | %-17s | %-17s  nodes=%ld (one per distinct key)\n", N, s1, s2, s3, cnt);
        STfree(); bfree(b); bfree(r); free(k);
    }
    return 0;
}
