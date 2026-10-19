/* ============================================================
 *  ex16_05_btree.c  —  Exercises 16.5-16.8, 16.10, 16.18, 16.22, 16.23
 *  Chapter 16: External Searching — B trees (Programs 16.1-16.4)
 *
 *  16.5  Give the contents of the 3-4-5-6 tree that results when you insert
 *        the keys E A S Y Q U E S T I O N W I T H P L E N T Y O F K E Y S
 *        in that order into an initially empty tree.
 *  16.6  Draw figures corresponding to Figures 16.5 through 16.7 to
 *        illustrate the process of inserting the keys 516, 177, 143, 632,
 *        572, 161, 774, 470, 411, 706, 461, 612, 761, 474, 774, 635, 343,
 *        461, 351, 430, 664, 127, 345, 171, and 357 in that order into an
 *        initially empty tree, with M = 5.
 *  16.7  Give the height of the B trees that result when you insert the keys
 *        in Exercise 16.28 (562 221 240 771 274 233 401 273 201) in that
 *        order into an initially empty tree, for each value of M > 2.
 *  16.8  Draw the B tree that results when you insert 16 equal keys into an
 *        initially empty tree, with M = 4.
 *  16.10 Modify the B-tree-insertion implementation in Program 16.3 to do
 *        splitting on the way down the tree (top-down, like Program 13.6).
 *  16.18 Could an index with all pages full be constructed by the
 *        B-tree-insertion algorithm of Program 16.3? Explain.
 *  16.22 Implement the sort operation for a B-tree-based symbol table.
 *  16.23 Implement the select operation for a B-tree-based symbol table.
 *
 *  بالعربي: نبني شجرة 3-4-5-6 بالـ top-down splitting (16.5 و16.10)، ونتتبّع
 *  B tree بـ M = 5 للمفاتيح دي (16.6)، ونحسب الارتفاع لكل M (16.7)، ونشوف
 *  16 مفتاح متساويين (16.8)، وهل ينفع كل الصفحات تبقى مليانة (16.18)،
 *  ونعمل sort وselect (16.22، 16.23).
 *
 *  THINKING STEPS
 *   1. Two different B trees live in this chapter:
 *      (a) the ABSTRACT generalisation of 2-3-4 trees (keys inside every
 *          node, nodes with 3..6 links for a 3-4-5-6 tree) — used in 16.5;
 *      (b) the book's IMPLEMENTATION (Programs 16.1-16.4): an INDEX. Leaves
 *          (external pages) hold the items; internal pages hold
 *          (key, link) pairs where key = the smallest key below that link.
 *          A page is split when it reaches M entries, into M/2 + M/2.
 *   2. 16.5 / 16.10, top-down: on the way down, split every FULL node
 *      (5 keys = a 6-node) before entering it: the middle key goes up, two
 *      3-nodes remain. Because the parent was never full, there is always
 *      room. Then insert into the leaf. Equal keys go to the right.
 *   3. 16.6: Program 16.3 bottom-up: insert into the external page; if it
 *      now has M entries, split and pass the new page's (smallest key,
 *      link) to the parent, which may split in turn; a root split adds a
 *      level. With M = 5 (odd) we keep 3 left and move 2 right.
 *   4. 16.8: equal keys always go to the LAST child whose key <= v, so all
 *      16 copies march right; splits leave half-full pages behind. Search
 *      finds only copies in the rightmost path — to find all copies you
 *      must also scan pages to the left.
 *   5. 16.18: after a split both halves hold M/2 entries and only grow from
 *      there, and the page that was just split is never "re-packed". So
 *      under random inserts we never reach 100%; but if keys come in
 *      INCREASING order, only the rightmost page ever receives keys and
 *      the left half of every split is frozen at M/2: pages ~50% full —
 *      the opposite of full. Answer: no (we check both orders).
 *   6. 16.22/16.23: leaves left to right ARE the sorted order. For select
 *      we keep it simple and walk leaves counting (a real implementation
 *      would store subtree counts in the index entries).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 16.1-16.4, Exercises 16.5-16.23
 *  Build:   gcc -std=c99 -Wall -O2 ex16_05_btree.c -o ex16_05_btree
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================= (b) Sedgewick's B-tree index, Programs 16.1-16.4 ================= */
#define MAXM 16
typedef struct page *link;
typedef struct { int key; link next; } entry;      /* next == NULL in external pages */
struct page { entry b[MAXM + 1]; int m; };
static int M, H; static link head; static long npages; static int VERB;
static link NEW(void) { link x = calloc(1, sizeof *x); npages++; return x; }
static void STinit(int m) { M = m; head = NEW(); H = 0; npages = 1; }
static link split(link h)                          /* Program 16.4, odd M allowed */
{
    link t = NEW(); int keep = (M + 1) / 2;
    for (int j = 0; j < M - keep; j++) t->b[j] = h->b[keep + j];
    t->m = M - keep; h->m = keep;
    return t;
}
static link insertR(link h, int v, int ht)        /* Program 16.3 */
{
    int i, j; entry x; link t, u;
    x.key = v; x.next = NULL;
    if (ht == 0) { for (j = 0; j < h->m; j++) if (v < h->b[j].key) break; }
    else
        for (j = 0; j < h->m; j++)
            if (j + 1 == h->m || v < h->b[j + 1].key) {
                t = h->b[j++].next;
                u = insertR(t, v, ht - 1);
                if (u == NULL) return NULL;
                x.key = u->b[0].key; x.next = u;
                break;
            }
    for (i = (h->m)++; i > j; i--) h->b[i] = h->b[i - 1];
    h->b[j] = x;
    if (h->m < M) return NULL;
    if (VERB) printf("        the %s page starting with %03d is full (%d entries): split\n", ht ? "index" : "external", h->b[0].key, M);
    return split(h);
}
static void STinsert(int v)
{
    link t, u = insertR(head, v, H);
    if (u == NULL) return;
    t = NEW(); t->m = 2;
    t->b[0].key = head->b[0].key; t->b[0].next = head;
    t->b[1].key = u->b[0].key;    t->b[1].next = u;
    head = t; H++;
    if (VERB) printf("        root split: height is now %d\n", H);
}
static int searchR(link h, int v, int ht)          /* Program 16.2 */
{
    if (ht == 0) { for (int j = 0; j < h->m; j++) if (h->b[j].key == v) return 1; return 0; }
    for (int j = 0; j < h->m; j++) if (j + 1 == h->m || v < h->b[j + 1].key) return searchR(h->b[j].next, v, ht - 1);
    return 0;
}
static void showR(link h, int ht, int indent)
{
    printf("%*s%s[", indent, "", ht ? "index " : "page  ");
    for (int j = 0; j < h->m; j++) printf("%s%03d", j ? " " : "", h->b[j].key);
    printf("]\n");
    if (ht) for (int j = 0; j < h->m; j++) showR(h->b[j].next, ht - 1, indent + 4);
}
static void freeR(link h, int ht) { if (ht) for (int j = 0; j < h->m; j++) freeR(h->b[j].next, ht - 1); free(h); }
static void fillR(link h, int ht, long *items, long *pg) { if (!ht) { *items += h->m; (*pg)++; return; } for (int j = 0; j < h->m; j++) fillR(h->b[j].next, ht - 1, items, pg); }
/* 16.22 sort */
static void sortR(link h, int ht) { if (!ht) { for (int j = 0; j < h->m; j++) printf(" %03d", h->b[j].key); return; } for (int j = 0; j < h->m; j++) sortR(h->b[j].next, ht - 1); }
/* 16.23 select k-th smallest (0-based) */
static int selectR(link h, int ht, int *k, int *out)
{
    if (!ht) { if (*k < h->m) { *out = h->b[*k].key; return 1; } *k -= h->m; return 0; }
    for (int j = 0; j < h->m; j++) if (selectR(h->b[j].next, ht - 1, k, out)) return 1;
    return 0;
}

/* ================= (a) abstract 3-4-5-6 tree, top-down (16.5, 16.10) ================= */
#define TK 5                                       /* max keys in a node (a 6-node) */
typedef struct tnode *tlink;
struct tnode { int n; char key[TK]; tlink kid[TK + 1]; };
static tlink tNEW(void) { return calloc(1, sizeof(struct tnode)); }
static tlink troot; static int tsplits;
static void tsplitChild(tlink p, int i)            /* child i of p is full: 2 keys + middle up + 2 keys */
{
    tlink c = p->kid[i], r = tNEW(); int mid = TK / 2;
    r->n = TK - mid - 1;
    for (int j = 0; j < r->n; j++) r->key[j] = c->key[mid + 1 + j];
    for (int j = 0; j <= r->n; j++) r->kid[j] = c->kid[mid + 1 + j];
    c->n = mid;
    for (int j = p->n; j > i; j--) { p->key[j] = p->key[j - 1]; p->kid[j + 1] = p->kid[j]; }
    p->key[i] = c->key[mid]; p->kid[i + 1] = r; p->n++;
    tsplits++;
}
static void tinsert(char v)
{
    if (!troot) { troot = tNEW(); }
    if (troot->n == TK) { tlink s = tNEW(); s->kid[0] = troot; troot = s; tsplitChild(s, 0); }
    tlink h = troot;
    for (;;) {
        int i = 0; while (i < h->n && v >= h->key[i]) i++;          /* equal keys go right */
        if (!h->kid[0]) { for (int j = h->n; j > i; j--) h->key[j] = h->key[j - 1]; h->key[i] = v; h->n++; return; }
        if (h->kid[i]->n == TK) { tsplitChild(h, i); if (v >= h->key[i]) i++; }
        h = h->kid[i];
    }
}
static int theight(tlink h) { int d = 0; while (h->kid[0]) { h = h->kid[0]; d++; } return d; }
static void tlevels(tlink h, int d, char lines[][256])
{
    char buf[16]; int k = 0; buf[k++] = '(';
    for (int j = 0; j < h->n; j++) buf[k++] = h->key[j];
    buf[k++] = ')'; buf[k++] = ' '; buf[k] = 0; strcat(lines[d], buf);
    if (h->kid[0]) for (int j = 0; j <= h->n; j++) tlevels(h->kid[j], d + 1, lines);
}

int main(void)
{
    /* ---------- 16.5 / 16.10 ---------- */
    const char *s = "EASYQUESTIONWITHPLENTYOFKEYS";
    troot = NULL; tsplits = 0;
    for (const char *c = s; *c; c++) tinsert(*c);
    printf("16.5/16.10 3-4-5-6 tree (top-down splits of 6-nodes) for %s:\n", s);
    { char lines[8][256]; memset(lines, 0, sizeof lines); tlevels(troot, 0, lines);
      for (int d = 0; d <= theight(troot); d++) printf("     level %d: %s\n", d, lines[d]); }
    printf("     %d splits, height %d, every leaf on the same level\n", tsplits, theight(troot));

    /* ---------- 16.6 ---------- */
    int k25[] = { 516, 177, 143, 632, 572, 161, 774, 470, 411, 706, 461, 612, 761,
                  474, 774, 635, 343, 461, 351, 430, 664, 127, 345, 171, 357 };
    printf("16.6 Program 16.3 (bottom-up), M = 5: a page splits when it reaches 5 entries (3 stay, 2 move)\n");
    STinit(5); VERB = 1;
    for (int i = 0; i < 25; i++) {
        printf("     insert %03d\n", k25[i]);
        int hb = H; STinsert(k25[i]);
        if (H != hb) showR(head, H, 8);                /* a new level: show the whole tree */
    }
    VERB = 0;
    printf("     final tree (height %d, %ld pages):\n", H, npages); showR(head, H, 6);
    printf("16.22 sort:"); sortR(head, H); printf("\n");
    printf("16.23 select:"); for (int k = 0; k < 25; k += 6) { int kk = k, out = -1; selectR(head, H, &kk, &out); printf("  select(%d) = %03d", k, out); } printf("\n");
    printf("     search 635 -> %s, search 500 -> %s\n", searchR(head, 635, H) ? "hit" : "miss", searchR(head, 500, H) ? "hit" : "miss");
    freeR(head, H);

    /* ---------- 16.7 ---------- */
    int k9[] = { 562, 221, 240, 771, 274, 233, 401, 273, 201 };
    printf("16.7 heights for the 9 keys of 16.28:");
    for (int m = 3; m <= 10; m++) { STinit(m); for (int i = 0; i < 9; i++) STinsert(k9[i]); printf("  M=%d:%d", m, H); freeR(head, H); }
    printf("   (M >= 10: one page, height 0)\n");

    /* ---------- 16.8 ---------- */
    printf("16.8 sixteen equal keys (777), M = 4:\n");
    STinit(4); for (int i = 0; i < 16; i++) STinsert(777);
    showR(head, H, 6);
    { long it = 0, pg = 0; fillR(head, H, &it, &pg);
      printf("     height %d, %ld external pages for %ld items: search finds a copy only on the rightmost path\n", H, pg, it); }
    freeR(head, H);

    /* ---------- 16.18 ---------- */
    printf("16.18 page fill after inserting 10000 keys, M = 10 (100%% would mean 'all pages full'):\n");
    { unsigned long long r = 99; long it, pg;
      STinit(10); for (int i = 0; i < 10000; i++) { r ^= r << 13; r ^= r >> 7; r ^= r << 17; STinsert((int)(r % 100000000)); }
      it = pg = 0; fillR(head, H, &it, &pg);
      printf("     random order:     %ld pages, %.1f%% full (about 1/1.44 = 69%%)\n", pg, 100.0 * it / (pg * (M - 1)));
      freeR(head, H);
      STinit(10); for (int i = 0; i < 10000; i++) STinsert(i);
      it = pg = 0; fillR(head, H, &it, &pg);
      printf("     increasing order: %ld pages, %.1f%% full: every split freezes a half-full page\n", pg, 100.0 * it / (pg * (M - 1)));
      freeR(head, H); }
    return 0;
}
