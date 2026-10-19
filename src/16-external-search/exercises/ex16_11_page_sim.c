/* ============================================================
 *  ex16_11_page_sim.c  —  Exercises 16.11, 16.13, 16.16
 *  Chapter 16: External Searching — how full do B-tree pages get?
 *
 *  16.11 Write a program to compute the average number of external pages
 *        for a B tree of order M built from N random insertions into an
 *        initially empty tree, using the probabilistic process described
 *        after Property 16.1. Run your program for M = 10, 100, and 1000
 *        and N = 10^3, 10^4, 10^5, and 10^6.
 *  16.13 Consider the sibling split (or B* tree) heuristic for B trees:
 *        when it comes time to split a node because it contains M entries,
 *        we combine the node with its sibling. If the sibling has k entries
 *        with k < M - 1, we reallocate the items giving the sibling and the
 *        full node each about (M + k)/2 entries. Otherwise, we create a new
 *        node and give each of the three nodes about 2M/3 entries. State
 *        bounds on the number of probes used for a search or an insertion
 *        in a B* tree of order M with N items, and compare with B trees,
 *        for M = 10, 100, and 1000 and N = 10^3 .. 10^6.
 *  16.16 Run a probabilistic simulation (see Exercise 16.11) to determine
 *        the average number of pages used when we use the sibling-split
 *        heuristic, building a B* tree of order M by inserting random nodes
 *        into an initially empty tree, for the same M and N.
 *
 *  بالعربي: نحاكي نمو الصفحات الخارجية في B tree من غير ما نبني الشجرة
 *  (Markov chain بتاع Yao)، ونقارنه بـ B* (sibling split) اللي بيوزّع على
 *  الجار قبل ما يقسم، ونحسب حدود الـ probes للاتنين.
 *
 *  THINKING STEPS
 *   1. We only care about EXTERNAL pages and how many items each holds; the
 *      index above them is tiny. A random key lands in a page with
 *      probability proportional to the page's size (each gap between keys
 *      is equally likely). So: pick a random ITEM uniformly; its page gets
 *      the new key. Implementation trick: pick a random page uniformly and
 *      accept it with probability size/M (rejection sampling) — no tree
 *      needed, O(1) expected per insert.
 *   2. B tree: a page that reaches M items splits into two of M/2.
 *      Property 16.3 (Yao): about 1.44 N/M pages, i.e. ~69% full.
 *   3. B*: the pages are kept in key order in a doubly linked list, so a
 *      page has real siblings. When a page fills, look at the right (then
 *      left) sibling; if it has room, even them out (no new page). If both
 *      neighbours are full, turn the two full pages into three pages of
 *      ~2M/3. Pages are now at least 2/3 full: expected ~1.23 N/M pages
 *      (the book: "reduces the excess storage from 44 to 23 per cent").
 *   4. 16.13 bounds: a B tree node has between M/2 and M links, so the
 *      height is between log_M N and log_{M/2} N. In a B* tree every non-
 *      root node has at least 2M/3 links: between log_M N and
 *      log_{2M/3} N. We print both.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Property 16.3, Exercises 16.11-16.16
 *  Build:   gcc -std=c99 -Wall -O2 ex16_11_page_sim.c -o ex16_11 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

static uint64_t rs = 1611;
static uint32_t rnd32(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (uint32_t)(rs >> 16); }
static double rnd01(void) { return rnd32() / 4294967296.0; }

/* page pool: size, and prev/next in key order (for B* siblings) */
static int *sz, *prv, *nxt; static long np, cap;
static long newPage(int s)
{
    if (np == cap) { cap = cap ? 2 * cap : 1024; sz = realloc(sz, cap * sizeof *sz); prv = realloc(prv, cap * sizeof *prv); nxt = realloc(nxt, cap * sizeof *nxt); }
    sz[np] = s; prv[np] = nxt[np] = -1; return np++;
}
static void linkAfter(long a, long b) { nxt[b] = nxt[a]; prv[b] = a; if (nxt[a] >= 0) prv[nxt[a]] = b; nxt[a] = b; }
static long pickPage(int M)                    /* page with probability ~ its size */
{
    for (;;) { long p = rnd32() % np; if (rnd01() * M < sz[p]) return p; }
}

static long simBtree(int M, long N)
{
    np = 0; newPage(0); sz[0] = 1;              /* start with one page and one item */
    for (long i = 1; i < N; i++) {
        long p = pickPage(M);
        if (++sz[p] == M) { long q = newPage(M / 2); sz[p] = M - M / 2; linkAfter(p, q); }
    }
    return np;
}
static long simBstar(int M, long N)
{
    np = 0; newPage(0); sz[0] = 1;
    for (long i = 1; i < N; i++) {
        long p = pickPage(M);
        if (++sz[p] < M) continue;
        long s = (nxt[p] >= 0 && sz[nxt[p]] < M - 1) ? nxt[p] : (prv[p] >= 0 && sz[prv[p]] < M - 1) ? prv[p] : -1;
        if (s >= 0) { int tot = sz[p] + sz[s]; sz[p] = tot / 2; sz[s] = tot - tot / 2; continue; }   /* even out */
        long t = nxt[p] >= 0 ? nxt[p] : prv[p];
        if (t < 0) { long q = newPage(M / 2); sz[p] = M - M / 2; linkAfter(p, q); continue; }       /* lone root page */
        int tot = sz[p] + sz[t];                                   /* two full pages -> three */
        long a = (t == nxt[p]) ? p : t;                            /* a is the left one */
        long q = newPage(0); linkAfter(a, q);
        sz[p] = tot / 3; sz[t] = tot / 3; sz[q] = tot - 2 * (tot / 3);
    }
    return np;
}

int main(void)
{
    printf("16.11 / 16.16 probabilistic simulation of external pages (one run each)\n");
    printf("     %5s %8s | %10s %8s %6s | %10s %8s %6s\n", "M", "N", "B tree", "N/pages", "x N/M", "B* tree", "N/pages", "x N/M");
    for (int M = 10; M <= 1000; M *= 10)
        for (long N = 1000; N <= 1000000; N *= 10) {
            if (N < 4L * M) continue;
            long b = simBtree(M, N), s = simBstar(M, N);
            printf("     %5d %8ld | %10ld %8.1f %6.2f | %10ld %8.1f %6.2f\n",
                   M, N, b, (double)N / b, (double)b * M / N, s, (double)N / s, (double)s * M / N);
        }
    printf("     Property 16.3 says B tree ~ 1.44 N/M pages; B* ~ 1.2 N/M (excess 44%% -> about 23%%)\n");

    printf("16.13 probes for a search: between log_M N and log_(M/2) N (B tree) or log_(2M/3) N (B*)\n");
    printf("     %5s %8s | %7s | %12s | %12s\n", "M", "N", "log_M N", "B log_(M/2)", "B* log_(2M/3)");
    for (int M = 10; M <= 1000; M *= 10)
        for (double N = 1e3; N <= 1e6; N *= 10)
            printf("     %5d %8.0f | %7.2f | %12.2f | %12.2f\n", M, N, log(N) / log(M), log(N) / log(M / 2.0), log(N) / log(2.0 * M / 3));
    free(sz); free(prv); free(nxt);
    return 0;
}
