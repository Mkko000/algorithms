/* ============================================================
 *  ex15_01_dst.c  —  Exercises 15.1-15.5
 *  Chapter 15: Radix Search — digital search trees (DSTs)
 *
 *  15.1 Draw the DST that results when you insert items with the keys
 *       E A S Y Q U T I O N in that order into an initially empty tree,
 *       using the binary encoding given in Figure 15.1 (letter i = the
 *       5-bit binary representation of i, A = 00001).
 *  15.2 Give an insertion sequence for the keys A B C D E F G that results
 *       in a perfectly balanced DST that is also a valid BST.
 *  15.3 Give an insertion sequence for the keys A B C D E F G that results
 *       in a perfectly balanced DST with the property that every node has
 *       a key smaller than those of all the nodes in its subtree.
 *  15.4 Draw the DST that results when you insert items with the keys
 *       01010011 00000111 00100001 01010001 11101100 00100001 10010101
 *       01001010 in that order into an initially empty tree.
 *  15.5 Can we keep records with duplicate keys in DSTs, in the same way
 *       that we can in BSTs? Explain your answer.
 *
 *  بالعربي: (15.1) ارسم الـ DST بعد إدخال E A S Y Q U T I O N بكود الـ 5 بت.
 *  (15.2) رتّب A..G بحيث الـ DST تطلع متوازنة تمامًا وكمان BST سليمة.
 *  (15.3) رتّب A..G بحيث تطلع متوازنة تمامًا وكل node أصغر من كل اللي تحتها.
 *  (15.4) ارسم الـ DST للمفاتيح الـ 8 بت دي. (15.5) ينفع duplicates في DST؟
 *
 *  THINKING STEPS
 *   1. A DST is a BST-shaped tree with ONE key per node, but at depth w we
 *      branch on bit w of the SEARCH key (0 = left, 1 = right), never on a
 *      comparison. So the shape depends on the insertion order (like a BST)
 *      but every key sits somewhere on the path spelled by its own bits.
 *   2. 15.2/15.3: in the 5-bit code A..G are 00001..00111, so bits 0 and 1
 *      are 0 for ALL of them: everything goes left twice and a perfectly
 *      balanced tree is impossible. The exercise only makes sense if we
 *      branch on the 3 bits that matter (w = 3): A=001 ... G=111. Then
 *      7! = 5040 orders are few enough to try them all and keep the ones
 *      that satisfy each condition. Brute force + a checker = a proof.
 *   3. 15.4 contains the key 00100001 TWICE. Program 15.1's insert never
 *      tests for equality, so the second copy goes one level below the
 *      first, on the same bit path. That is exactly the point of 15.5.
 *   4. 15.5: every copy of a key follows the same bit path, so copies pile
 *      up on ONE path; after w+1 copies we run out of bits (depth > w).
 *      So: only a limited number of duplicates, and the "height <= w"
 *      guarantee is lost. Keep duplicates in a list hanging off one node.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 15.1, Exercises 15.1-15.5
 *  Build:   gcc -std=c99 -Wall -O2 ex15_01_dst.c -o ex15_01_dst
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int W;                                  /* number of key bits in use */
static int digit(unsigned v, int b) { return (v >> (W - 1 - b)) & 1u; }

typedef struct node *link;
struct node { unsigned key; char name; link l, r; };
static link NEW(unsigned v, char nm) { link x = malloc(sizeof *x); x->key = v; x->name = nm; x->l = x->r = NULL; return x; }

/* Program 15.1 insert (no equality test, exactly like the book) */
static link insertR(link h, unsigned v, char nm, int w, int *depth)
{
    if (h == NULL) { *depth = w; return NEW(v, nm); }
    if (w >= W) { *depth = -1; return h; }      /* ran out of bits: cannot go deeper */
    if (digit(v, w) == 0) h->l = insertR(h->l, v, nm, w + 1, depth);
    else                  h->r = insertR(h->r, v, nm, w + 1, depth);
    return h;
}
static void freeT(link h) { if (h) { freeT(h->l); freeT(h->r); free(h); } }
static void bin(unsigned v, char *s) { for (int i = 0; i < W; i++) s[i] = '0' + digit(v, i); s[W] = 0; }

/* print every node with the path (bits) that leads to it */
static void show(link h, char *path, int d)
{
    if (!h) return;
    char b[40]; bin(h->key, b); path[d] = 0;
    printf("       %-8s depth %d: ", d ? path : "(root)", d);
    if (h->name) printf("%c = %s\n", h->name, b); else printf("%s\n", b);
    path[d] = '0'; show(h->l, path, d + 1);
    path[d] = '1'; show(h->r, path, d + 1);
}
static int height(link h) { if (!h) return -1; int a = height(h->l), b = height(h->r); return 1 + (a > b ? a : b); }
static int count(link h) { return h ? 1 + count(h->l) + count(h->r) : 0; }
static int isBST(link h, int lo, int hi) { return !h || ((int)h->key > lo && (int)h->key < hi && isBST(h->l, lo, h->key) && isBST(h->r, h->key, hi)); }
static int heapOK(link h)                         /* every node smaller than all below it */
{
    if (!h) return 1;
    if (h->l && h->l->key < h->key) return 0;
    if (h->r && h->r->key < h->key) return 0;   /* children smaller than parent's subtree min is enough recursively */
    return heapOK(h->l) && heapOK(h->r);
}

/* next lexicographic permutation */
static int nextperm(char *a, int n)
{
    int i = n - 2; while (i >= 0 && a[i] >= a[i + 1]) i--;
    if (i < 0) return 0;
    int j = n - 1; while (a[j] <= a[i]) j--;
    char t = a[i]; a[i] = a[j]; a[j] = t;
    for (int x = i + 1, y = n - 1; x < y; x++, y--) { t = a[x]; a[x] = a[y]; a[y] = t; }
    return 1;
}
static link build(const char *s, int *bad)
{
    link h = NULL; int dep; *bad = 0;
    for (int i = 0; s[i]; i++) { h = insertR(h, s[i] - 'A' + 1, s[i], 0, &dep); if (dep < 0) *bad = 1; }
    return h;
}

int main(void)
{
    char path[40], b[40];

    /* ---------- 15.1 ---------- */
    W = 5;
    printf("15.1 DST for E A S Y Q U T I O N (5-bit code, A = 00001):\n");
    { int bad; link h = build("EASYQUTION", &bad);
      show(h, path, 0);
      printf("     %d nodes, height %d (never more than w = 5)\n", count(h), height(h)); freeT(h); }

    /* ---------- 15.2 and 15.3 ---------- */
    { int bad; link h;
      W = 5; h = build("DBFACEG", &bad);
      printf("15.2/15.3 with the 5-bit code every key A..G starts with 00, so the root's right link\n"
             "     stays empty: 'DBFACEG' gives height %d, not 2. We branch on the last 3 bits (w = 3):\n", height(h));
      freeT(h); }
    W = 3;
    { char p[] = "ABCDEFG"; int nb = 0, nh = 0; char firstB[8] = "", firstH[8] = "";
      do {
          int bad; link h = build(p, &bad);
          if (!bad && height(h) == 2) {
              if (isBST(h, 0, 8)) { if (!nb++) strcpy(firstB, p); }
              if (heapOK(h))      { if (!nh++) strcpy(firstH, p); }
          }
          freeT(h);
      } while (nextperm(p, 7));
      printf("     15.2: %d of 5040 orders give a perfectly balanced DST that is also a BST; first: %s\n", nb, firstB);
      { int bad; link h = build(firstB, &bad); show(h, path, 0); freeT(h); }
      printf("     15.3: %d of 5040 orders give a perfectly balanced DST with every node smaller than its subtree.\n", nh);
      if (nh) { int bad; link h = build(firstH, &bad); show(h, path, 0); freeT(h); }
      else {
          /* why: the root must be A (the smallest), and the other six keys split by bit 0:
             B=010, C=011 go left, D..G = 1xx go right: 2 against 4, never 3 and 3. */
          char q[] = "ABCDEFG"; int best = 99; char bestp[8] = "";
          do { int bad; link h = build(q, &bad);
               if (!bad && heapOK(h) && height(h) < best) { best = height(h); strcpy(bestp, q); }
               freeT(h); } while (nextperm(q, 7));
          printf("     impossible: the root must be A, then bit 0 sends only B, C left and D E F G right (2 vs 4).\n"
                 "     best we can do with the heap condition is height %d, e.g. %s:\n", best, bestp);
          { int bad; link h = build(bestp, &bad); show(h, path, 0); freeT(h); }
      }
    }

    /* ---------- 15.4 and 15.5 ---------- */
    W = 8;
    { const char *ks[] = { "01010011", "00000111", "00100001", "01010001", "11101100", "00100001", "10010101", "01001010" };
      link h = NULL; int dep;
      printf("15.4 DST for the eight 8-bit keys (in that order):\n");
      for (int i = 0; i < 8; i++) {
          unsigned v = (unsigned)strtoul(ks[i], NULL, 2);
          h = insertR(h, v, 0, 0, &dep);
          printf("     insert %s -> depth %d%s\n", ks[i], dep, i == 5 ? "   (duplicate of key 3: lands right below it)" : "");
      }
      show(h, path, 0);
      printf("     %d nodes, height %d\n", count(h), height(h));
      freeT(h);
    }
    printf("15.5 duplicates: insert the same 3-bit key D = 100 again and again (w = 3):\n");
    W = 3;
    { link h = NULL; int dep;
      for (int c = 1; c <= 5; c++) {
          h = insertR(h, 4, 'D', 0, &dep);
          bin(4, b);
          if (dep >= 0) printf("     copy %d of D goes to depth %d (path %.*s)\n", c, dep, dep, b);
          else          printf("     copy %d of D: NO room, all %d bits used up (a DST holds at most w+1 = %d copies)\n", c, W, W + 1);
      }
      freeT(h);
    }
    return 0;
}
