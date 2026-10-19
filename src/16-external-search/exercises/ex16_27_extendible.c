/* ============================================================
 *  ex16_27_extendible.c  —  Exercises 16.27-16.32, 16.34, 16.38, 16.39
 *  Chapter 16: External Searching — extendible hashing (Programs 16.5-16.8)
 *
 *  16.27 How many pages would be empty if we were to use a directory of
 *        size 32 in Figure 16.10?
 *  16.28 Draw figures corresponding to Figures 16.11 through 16.13, to
 *        illustrate the process of inserting the keys 562, 221, 240, 771,
 *        274, 233, 401, 273, and 201 in that order into an initially empty
 *        tree, with M = 5.  (keys used AS their own hash values)
 *  16.29 (same keys and M; reverse order — Property 16.4)
 *  16.30 Assume that you are given an array of items in sorted order.
 *        Describe how you would determine the directory size of the
 *        extendible hash table corresponding to that set of items.
 *  16.31 Write a program that constructs an extendible hash table from an
 *        array of items that is in sorted order, by doing two passes through
 *        the items: one to determine the size of the directory (see 16.30)
 *        and one to allocate the items to pages and fill in the directory.
 *  16.32 Give a set of keys for which the corresponding extendible hash
 *        table has directory size 16, with eight pointers to a single page.
 *  16.34 Write a program to compute the average number of external pages
 *        and the average directory size for an extendible hash table built
 *        from N random insertions into an initially empty tree, when the
 *        page capacity is M. Compute the percentage of empty space, for
 *        M = 10, 100, and 1000 and N = 10^3, 10^4, 10^5, and 10^6.
 *  16.38 Implement the sort operation for an extendible hash table.
 *  16.39 Implement the select operation for an extendible hash table.
 *
 *  بالعربي: نعدّ الصفحات الفاضية في دليل حجمه 32، ونتتبّع extendible hashing
 *  للمفاتيح دي بـ M = 5، ونبني الجدول من مصفوفة مرتّبة بتمريرتين، ونلاقي
 *  مجموعة مفاتيح دليلها 16 وفيه صفحة واحدة عليها 8 pointers، ونقيس المتوسطات
 *  (Property 16.5)، ونعمل sort وselect.
 *
 *  THINKING STEPS
 *   1. Directory of size 2^d indexes by the leading d bits of the key.
 *      Each page has a local depth k <= d: all its items agree on their
 *      first k bits, and 2^(d-k) directory entries point at it. Search =
 *      one directory lookup + one page read. Property 16.4: the structure
 *      depends only on the KEY SET, not on insertion order.
 *   2. Insert into the page; if it overflows, split it on bit k (items with
 *      bit k = 0 stay, 1 move), bump both local depths. If the new depth
 *      exceeds d, DOUBLE the directory (clone every pointer).
 *   3. 16.27: the keys in Figure 16.10 are 3-digit OCTAL numbers, i.e.
 *      9-bit strings. With 32 directory slots (5 bits) and one page per
 *      slot, a page is empty when no key has that 5-bit prefix: count them.
 *   4. 16.28: same convention — octal keys as their own 9-bit strings (no
 *      hash), M = 5. Show the directory after every split or double.
 *   5. 16.30/31: in sorted order by the leading bits, a page boundary is
 *      forced wherever M+1 consecutive keys share a common prefix. The
 *      directory depth d is the smallest depth where every group of keys
 *      sharing a d-bit prefix has size <= M. Pass 1 finds d; pass 2 fills.
 *   6. 16.32: eight pointers to one page means d-k = 3 and d = 4 so k = 1.
 *      Few keys with leading bit 0 (that page never splits) + many keys
 *      with leading bit 1 (forcing d up to 4) does it.
 *   7. Property 16.5: ~1.44 N/M pages (~69% full) and directory
 *      ~3.92 N^(1+1/M)/M. We measure both for the table in 16.34.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 16.5-16.8, Exercises 16.27-16.39
 *  Build:   gcc -std=c99 -Wall -O2 ex16_27_extendible.c -o ex16_27 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define MAXM 1000
typedef struct page { int b[MAXM + 1]; int m; int k; } page;
static page **dir; static int d, D, M; static long npages;

/* n bits of x starting at bit 'start' (MSB of a 32-bit word = bit 0) */
static int W = 32;                                   /* active key width; the book's 3-digit octal keys use 9 */
static void pk(int v) { if (W <= 12) { printf(" "); for (int b = 0; b < W; b++) putchar('0' + (int)((v >> (W - 1 - b)) & 1)); printf("(%03o)", v); } else printf(" %d", v); }
static unsigned bits(unsigned x, int start, int n)
{ return n == 0 ? 0 : (x >> (W - start - n)) & ((1u << n) - 1); }

static page *NEW(int k) { page *p = calloc(1, sizeof *p); p->k = k; npages++; return p; }
static void STinit(int m) { M = m; d = 0; D = 1; npages = 0; dir = malloc(sizeof *dir); dir[0] = NEW(0); }
static void STinitW(int m, int w) { W = w; STinit(m); }
static void doubledir(void)
{
    page **old = dir; d++; D *= 2;
    dir = malloc(D * sizeof *dir);
    for (int i = 0; i < D; i++) dir[i] = old[i / 2];
    free(old);
}
static void split(page *p)
{
    if (p->k == d) doubledir();
    page *t = NEW(p->k + 1);
    int items[MAXM + 1], n = p->m;
    for (int j = 0; j < n; j++) items[j] = p->b[j];
    p->m = 0; p->k++;
    for (int j = 0; j < n; j++) {
        if (bits((unsigned)items[j], p->k - 1, 1) == 0) p->b[p->m++] = items[j];
        else                                             t->b[t->m++] = items[j];
    }
    unsigned prefix = bits((unsigned)(p->m ? p->b[0] : t->b[0]), 0, p->k - 1);
    int span = 1 << (d - p->k);
    unsigned base = ((prefix << 1) | 1u) << (d - p->k);
    for (int i = 0; i < span; i++) dir[base + i] = t;
    if (p->m > M) split(p);
    else if (t->m > M) split(t);
}
static void insert_page(page *p, int v)
{
    for (int j = 0; j < p->m; j++) if (p->b[j] == v) return;
    int j = p->m++;
    while (j > 0 && p->b[j - 1] > v) { p->b[j] = p->b[j - 1]; j--; }
    p->b[j] = v;
    if (p->m > M) split(p);
}
static void STinsert(int v) { insert_page(dir[bits((unsigned)v, 0, d)], v); }
static int STsearch(int v)
{
    page *p = dir[bits((unsigned)v, 0, d)];
    for (int j = 0; j < p->m; j++) if (p->b[j] == v) return 1;
    return 0;
}
static void show(const char *title)
{
    printf("%s  d = %d, directory size %d, %ld pages\n", title, d, D, npages);
    for (int i = 0; i < D; i++) {
        if (i && dir[i] == dir[i - 1]) continue;
        int count = 0; for (int j = i; j < D && dir[j] == dir[i]; j++) count++;
        if (dir[i]->m == 0) continue;                   /* skip empty pages left by one-way splits */
        printf("       page k=%d  (%d dir entr%s):", dir[i]->k, count, count == 1 ? "y" : "ies");
        for (int j = 0; j < dir[i]->m; j++) pk(dir[i]->b[j]);
        printf("\n");
    }
}
static void freeAll(void)
{
    for (int i = 0; i < D; i++) if (i == 0 || dir[i] != dir[i - 1]) free(dir[i]);
    free(dir);
}
static void STsort(void)
{
    for (int i = 0; i < D; i++) if (i == 0 || dir[i] != dir[i - 1])
        for (int j = 0; j < dir[i]->m; j++) printf(" %03o", dir[i]->b[j]);
}
static int STselect(int k)
{
    for (int i = 0; i < D; i++) if (i == 0 || dir[i] != dir[i - 1]) {
        if (k < dir[i]->m) return dir[i]->b[k];
        k -= dir[i]->m;
    }
    return -1;
}

/* 16.30 / 16.31: two-pass build from a sorted array */
static int dirDepthFor(const int *a, int n, int m)
{
    for (int dd = 0; dd <= 20; dd++) {
        int ok = 1, i = 0;
        while (i < n) {
            unsigned pref = bits((unsigned)a[i], 0, dd);
            int j = i; while (j < n && bits((unsigned)a[j], 0, dd) == pref) j++;
            if (j - i > m) { ok = 0; break; }
            i = j;
        }
        if (ok) return dd;
    }
    return 20;
}
/* pass 2: keys a[lo..hi) all share their first k bits (= pref). If they fit,
   they get ONE page of local depth k, pointed at by the 2^(d-k) slots under
   that prefix; otherwise cut the sorted run at the first key whose bit k is 1. */
static void fillR(const int *a, int lo, int hi, int k, unsigned pref)
{
    if (hi - lo <= M) {
        page *p = NEW(k);
        for (int t = lo; t < hi; t++) p->b[p->m++] = a[t];
        int span = 1 << (d - k);
        for (int s = 0; s < span; s++) dir[(pref << (d - k)) + s] = p;
        return;
    }
    int mid = lo; while (mid < hi && bits((unsigned)a[mid], k, 1) == 0) mid++;
    fillR(a, lo, mid, k + 1, pref << 1);
    fillR(a, mid, hi, k + 1, (pref << 1) | 1u);
}
static void buildSorted(const int *a, int n, int m)
{
    int dd = dirDepthFor(a, n, m);                   /* pass 1 */
    M = m; d = dd; D = 1 << dd; npages = 0;
    dir = malloc(D * sizeof *dir);
    fillR(a, 0, n, 0, 0);                            /* pass 2 */
}

int main(void)
{
    /* 16.27: the 25 keys of Figure 16.10 (3-digit octal = 9 bits) */
    { int f[] = { 0001, 0017, 0061, 0107, 0147, 0153, 0176, 0207, 0275, 0277, 0373, 0434, 0513,
                  0526, 0527, 0562, 0574, 0601, 0641, 0706, 0736, 0737, 0742, 0766 };
      int nf = (int)(sizeof f / sizeof f[0]), used[32] = {0}, u8[8] = {0}, ne = 0, n8 = 0;
      W = 9;
      for (int i = 0; i < nf; i++) { used[bits((unsigned)f[i], 0, 5)] = 1; u8[bits((unsigned)f[i], 0, 3)] = 1; }
      for (int i = 0; i < 32; i++) ne += !used[i];
      for (int i = 0; i < 8; i++) n8 += !u8[i];
      printf("16.27 Figure 16.10: %d keys (octal, 9 bits). Directory of 8 (3 bits): %d of the 8 prefixes\n"
             "      are empty, so 7 pages suffice (slots 010 and 011 share the '01' page).\n"
             "      Directory of 32 (5 bits), one page per slot: %d of the 32 pages would be EMPTY:\n      ",
             nf, n8, ne);
      for (int i = 0; i < 32; i++) if (!used[i]) printf(" %d%d%d%d%d", i >> 4 & 1, i >> 3 & 1, i >> 2 & 1, i >> 1 & 1, i & 1);
      printf("\n      (with page sharing, extendible hashing still uses only the 7 real pages)\n"); }

    int k9[] = { 0562, 0221, 0240, 0771, 0274, 0233, 0401, 0273, 0201 };   /* octal, like the book */
    W = 9;
    printf("16.28/16.29 the keys are octal, so each is a 9-bit string (3 bits per digit); M = 5:\n");
    for (int i = 0; i < 9; i++) { printf("      %03o =", k9[i]); pk(k9[i]); printf("\n"); }
    STinitW(5, 9);
    for (int i = 0; i < 9; i++) {
        int db = d; long pb = npages;
        STinsert(k9[i]);
        if (d != db || npages != pb || i == 8) {
            char title[96];
            snprintf(title, sizeof title, "      after inserting %03o%s", k9[i],
                     d != db ? " (directory doubled)" : npages != pb ? " (page split)" : "");
            show(title);
        }
    }
    printf("16.38 sort:"); STsort(); printf("\n");
    printf("16.39 select:"); for (int k = 0; k < 9; k += 2) printf("  select(%d) = %03o", k, STselect(k)); printf("\n");
    printf("      search 273 -> %s, search 777 -> %s\n", STsearch(0273) ? "hit" : "miss", STsearch(0777) ? "hit" : "miss");

    { int n1 = 0; for (int i = 0; i < D; i++) if (i == 0 || dir[i] != dir[i - 1]) n1++;
      int d1 = d; freeAll();
      STinitW(5, 9); for (int i = 8; i >= 0; i--) STinsert(k9[i]);
      printf("16.29 reverse order: d = %d (was %d), %ld pages (was %d): Property 16.4, the set decides\n",
             d, d1, npages, n1);
      freeAll(); }

    { int a[] = { 0201, 0221, 0233, 0240, 0273, 0274, 0401, 0562, 0771 };
      W = 9; buildSorted(a, 9, 5);
      printf("16.30/16.31 two-pass build from the sorted array: directory depth = %d\n", d);
      show("      ");
      freeAll(); }

    printf("16.32 want d = 4 (directory size 16) and one page with 8 directory pointers (k = 1).\n"
           "      M = 3, W = 6. Keys 0,1,2 (leading bit 0: page never splits).\n"
           "      Keys 32,33,36,37 = 100000,100001,100100,100101 (LCP 100: force d = 4):\n");
    { /* W = 6, M = 3.
         leading-bit 0: keys 0,1,2 — page never splits, so once d = 4 it has
         local depth 1 and 2^(4-1) = 8 directory pointers.
         leading-bit 1: four keys 32..35 = 100000..100011 share the 3-bit
         prefix 100 and force one split up to local depth 4 (and d = 4). */
      STinitW(3, 6);
      STinsert(0); STinsert(1); STinsert(2);
      STinsert(32); STinsert(33); STinsert(36); STinsert(37);  /* 100000,100001,100100,100101 */
      show("      ");
      int maxAlias = 0, maxK = 0;
      for (int i = 0; i < D; i++) if (i == 0 || dir[i] != dir[i - 1]) {
          int c = 0; for (int j = i; j < D && dir[j] == dir[i]; j++) c++;
          if (c > maxAlias) { maxAlias = c; maxK = dir[i]->k; }
      }
      printf("      directory size %d, one page has %d pointers (k = %d). Done.\n", D, maxAlias, maxK);
      freeAll(); }

    printf("16.34 random keys; Property 16.5: pages ~ 1.44 N/M, directory ~ 3.92 N^(1+1/M)/M\n");
    printf("     %7s %5s | %12s %12s | %12s %12s | %%empty\n", "N", "M", "pages", "1.44 N/M", "dir", "theory");
    uint64_t rs = 1634;
    for (int Msz = 10; Msz <= 1000; Msz *= 10)
        for (int N = 1000; N <= 1000000; N *= 10) {
            if (N / Msz < 2) continue;
            STinitW(Msz, 32);
            for (int i = 0; i < N; i++) {
                rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17;
                STinsert((int)(rs & 0x7fffffff));
            }
            long items = 0, uniq = 0;
            for (int i = 0; i < D; i++) if (i == 0 || dir[i] != dir[i - 1]) { uniq++; items += dir[i]->m; }
            printf("     %7d %5d | %12ld %12.0f | %12d %12.0f | %5.1f%%\n",
                   N, Msz, uniq, 1.44 * N / Msz, D,
                   3.92 * pow((double)N, 1.0 + 1.0 / Msz) / Msz,
                   100.0 * (1.0 - (double)items / (uniq * Msz)));
            freeAll();
        }
    return 0;
}
