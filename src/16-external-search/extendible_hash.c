/* ============================================================
 *  extendible_hash.c  —  Extendible hashing (Fagin et al., 1979)
 *  Chapter 16: External Searching (Section 16.4)
 *
 *  Definition 16.3: a DIRECTORY of 2^d page references, indexed by the
 *  leading d bits of the (hashed) key; each page holds up to M items
 *  and has a local depth k <= d: all its items agree on their leading
 *  k bits, and 2^(d-k) directory entries point at it.
 *   - Program 16.5/16.6: search = one directory lookup + ONE page read.
 *   - Program 16.7: insert into the page; if it overflows, SPLIT it on
 *     bit k (items with bit k = 0 stay, 1 move to a new page), k+1 for
 *     both. If k+1 > d the directory DOUBLES (Program 16.8, insertDIR):
 *     every entry is copied twice — no item moves, only references.
 *  Property 16.4: the structure depends only on the key set, not on
 *  insertion order. Property 16.5: about 1.44 N/M pages (pages ~69%
 *  full) and a directory of about 3.92 N^(1+1/M) / M entries.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 16.5-16.8
 *  Build:   gcc -std=c99 -Wall -O2 extendible_hash.c -o extendible_hash
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define M 32                                     /* items per page */
typedef uint32_t Key;
typedef struct page { Key b[M]; int m; int k; } page;
static page **dir; static int d, D; static long npages, reads;

/* keys are hashed to random-looking 32-bit values; we index by LEADING bits */
static Key h(Key v) { v ^= v >> 16; v *= 0x7feb352dU; v ^= v >> 15; v *= 0x846ca68bU; v ^= v >> 16; return v; }
static unsigned bits(Key x, int k, int j) { return j == 0 ? 0 : (x >> (32 - k - j)) & ((1u << j) - 1); }  /* j bits starting at bit k */

static page *NEW(int k) { page *p = calloc(1, sizeof *p); p->k = k; npages++; return p; }
void STinit(void) { d = 0; D = 1; dir = malloc(sizeof *dir); dir[0] = NEW(0); }

int STsearch(Key v) {                             /* Program 16.6 */
    page *p = dir[bits(h(v), 0, d)]; reads++;
    for (int j = 0; j < p->m; j++) if (p->b[j] == v) return 1;
    return 0;
}
static void doubledir(void) {                     /* Program 16.8, the doubling part */
    page **old = dir; d++; D *= 2;
    dir = malloc(D * sizeof *dir);
    for (int i = 0; i < D; i++) dir[i] = old[i / 2];
    free(old);
}
static void insert_page(page *p, Key v);
static void split(page *p) {                      /* Program 16.7 */
    if (p->k == d) doubledir();
    page *t = NEW(p->k + 1);
    Key items[M]; int n = p->m;
    for (int j = 0; j < n; j++) items[j] = p->b[j];
    p->m = 0; p->k++;
    for (int j = 0; j < n; j++) {
        if (bits(h(items[j]), p->k - 1, 1) == 0) p->b[p->m++] = items[j];
        else                                     t->b[t->m++] = items[j];
    }
    /* repoint the 2^(d-k) entries whose bit (k-1) is 1 to the new page */
    unsigned prefix = bits(h(p->m ? p->b[0] : t->b[0]), 0, p->k - 1);  /* common leading k-1 bits */
    int span = 1 << (d - p->k);
    unsigned base = ((prefix << 1) | 1u) << (d - p->k);
    for (int i = 0; i < span; i++) dir[base + i] = t;
    if (p->m == M) split(p);                       /* everything went one way: split again */
    else if (t->m == M) split(t);
}
static void insert_page(page *p, Key v) { p->b[p->m++] = v; if (p->m == M) split(p); }
void STinsert(Key v) { insert_page(dir[bits(h(v), 0, d)], v); }

int main(void) {
    STinit();
    const int N = 200000;
    for (Key i = 0; i < (Key)N; i++) STinsert(i * 7 + 1);
    printf("N=%d, M=%d: directory depth d=%d (%d entries), %ld pages, %.1f%% average fill\n",
           N, M, d, D, npages, 100.0 * N / (npages * (double)M));
    printf("Property 16.5 predicts ~1.44 N/M = %.0f pages\n", 1.44 * N / M);
    int hits = 0; reads = 0;
    for (Key i = 0; i < 10000; i++) hits += STsearch(i * 7 + 1);
    int miss = 0; for (Key i = 0; i < 10000; i++) miss += !STsearch(i * 7 + 2);
    printf("10000 hits: %d found; 10000 misses: %d not found; page reads per search: %.2f\n", hits, miss, reads / 20000.0);
    return 0;
}
