/* ============================================================
 *  ex16_01_isam.c  —  Exercises 16.1, 16.2, 16.3, 16.12, 16.21
 *  Chapter 16: External Searching — page counts, ISAM, and log_M N
 *
 *  16.1  Tabulate the values of log_M N, for M = 10, 100, and 1000 and
 *        N = 10^3, 10^4, 10^5, and 10^6.
 *  16.2  Draw an indexed sequential file structure for the keys 516, 177,
 *        143, 632, 572, 161, 774, 470, 411, 706, 461, 612, 761, 474, 774,
 *        635, 343, 461, 351, 430, 664, 127, 345, 171, and 357, for M = 5
 *        and M = 6.
 *  16.3  Suppose that we build an indexed sequential file structure for N
 *        items in pages of capacity M, but leave k empty spaces in each
 *        page for expansion. Give a formula for the number of probes needed
 *        for a search, as a function of N, M, and k. Use the formula to
 *        determine the number of probes needed for a search when k = M/10,
 *        for M = 10, 100, and 1000 and N = 10^3, 10^4, 10^5, and 10^6.
 *  16.12 Suppose that, in a three-level tree, we can afford to keep a links
 *        in internal memory, between b and 2b links in pages representing
 *        internal nodes, and between c and 2c items in pages representing
 *        external nodes. What is the maximum number of items that we can
 *        hold in such a tree, as a function of a, b, and c?
 *  16.21 Tabulate the difference between log_999 N and log_1000 N, for
 *        N = 10^3, 10^4, 10^5, and 10^6.
 *
 *  بالعربي: جدول log_M N، ونرسم ISAM للمفاتيح دي بـ M = 5 و6، ونحسب الـ probes
 *  لو سبنا k أماكن فاضية في كل صفحة، وأقصى عدد items في شجرة 3 مستويات،
 *  والفرق بين log_999 وlog_1000.
 *
 *  THINKING STEPS
 *   1. The whole chapter is about counting PROBES (page reads), not compares.
 *      A page with M entries splits the search space M ways, so a full index
 *      over N items needs ceil(log_M N) probes. For practical N and M that
 *      is 2, 3 or 4: "a constant".
 *   2. ISAM (16.2): sort the keys, cut them into full pages of M items, and
 *      build index pages holding the FIRST key of each page below; repeat
 *      until one page (the root) is left. Search: in each index page find
 *      the last key <= v and follow it.
 *   3. 16.3: with k free slots per page we store only M-k items per page,
 *      and every index page also uses only M-k entries, so the probes are
 *      ceil(log_{M-k} N). With k = M/10 the base drops by 10%: the real
 *      logarithm grows only a few per cent; the rounded count moves up by
 *      one only where N was exactly a power of M (a tree with zero slack).
 *   4. 16.12: the maximum is all pages full: a * 2b * 2c = 4abc items
 *      (minimum: a * b * c).
 *   5. 16.21: log_999 N - log_1000 N = ln N (1/ln 999 - 1/ln 1000), a tiny
 *      number: one item less per node changes nothing (that is why
 *      Program 16.3 can afford to keep only M-1 items per node).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Section 16.1-16.2, Exercises 16.1-16.21
 *  Build:   gcc -std=c99 -Wall -O2 ex16_01_isam.c -o ex16_01_isam -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int cmpi(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }

/* build and print an ISAM structure, return the number of levels (= probes) */
static int isam(const int *keys, int n, int M, int verbose)
{
    int *lvl = malloc(n * sizeof *lvl), cnt = n;
    for (int i = 0; i < n; i++) lvl[i] = keys[i];
    int levels = 1;
    if (verbose) {
        printf("     M = %d: data pages (sorted, full):\n       ", M);
        for (int i = 0; i < n; i++) { if (i && i % M == 0) printf(" | "); printf("%s%03d", i % M ? " " : "", keys[i]); }
        printf("\n");
    }
    while (cnt > M || levels == 1) {               /* need an index level above */
        int pages = (cnt + M - 1) / M;
        for (int p = 0; p < pages; p++) lvl[p] = lvl[p * M];   /* first key of every page */
        cnt = pages; levels++;
        if (verbose) {
            printf("       index level %d (%d entr%s = first key of each page below):\n       ", levels - 1, cnt, cnt == 1 ? "y" : "ies");
            for (int i = 0; i < cnt; i++) { if (i && i % M == 0) printf(" | "); printf("%s%03d", i % M ? " " : "", lvl[i]); }
            printf("\n");
        }
        if (cnt <= M) break;
    }
    free(lvl);
    return levels;
}
/* search in the ISAM: count page probes */
static int isamSearch(const int *keys, int n, int M, int v, int *probes)
{
    /* levels from the top: compute index arrays */
    int L = 0, sizes[16]; int *idx[16];
    idx[0] = (int *)keys; sizes[0] = n;
    while (sizes[L] > M || L == 0) {
        int pages = (sizes[L] + M - 1) / M;
        idx[L + 1] = malloc(pages * sizeof(int));
        for (int p = 0; p < pages; p++) idx[L + 1][p] = idx[L][p * M];
        sizes[L + 1] = pages; L++;
        if (pages <= M) break;
    }
    int page = 0; *probes = 0;
    for (int l = L; l >= 1; l--) {                 /* index page 'page' at level l */
        (*probes)++;
        int lo = page * M, hi = lo + M; if (hi > sizes[l]) hi = sizes[l];
        int j = lo; while (j + 1 < hi && idx[l][j + 1] <= v) j++;
        page = j;
    }
    (*probes)++;
    int found = 0, lo = page * M, hi = lo + M; if (hi > n) hi = n;
    for (int j = lo; j < hi; j++) if (keys[j] == v) found = 1;
    for (int l = 1; l <= L; l++) free(idx[l]);
    return found;
}

int main(void)
{
    /* ---------- 16.1 ---------- */
    printf("16.1 log_M N:\n     %8s", "N \\ M");
    for (int M = 10; M <= 1000; M *= 10) printf(" %10d", M);
    printf("\n");
    for (double N = 1e3; N <= 1e6; N *= 10) {
        printf("     %8.0f", N);
        for (int M = 10; M <= 1000; M *= 10) printf(" %10.2f", log(N) / log(M));
        printf("\n");
    }

    /* ---------- 16.2 ---------- */
    int keys[] = { 516, 177, 143, 632, 572, 161, 774, 470, 411, 706, 461, 612, 761,
                   474, 774, 635, 343, 461, 351, 430, 664, 127, 345, 171, 357 };
    int n = 25;
    qsort(keys, n, sizeof *keys, cmpi);
    printf("16.2 ISAM for the 25 keys (774 and 461 appear twice):\n");
    for (int M = 5; M <= 6; M++) {
        int L = isam(keys, n, M, 1);
        int pr, f1 = isamSearch(keys, n, M, 635, &pr);
        int pr2, f2 = isamSearch(keys, n, M, 500, &pr2);
        printf("       levels = %d: search 635 -> %s in %d probes, search 500 -> %s in %d probes\n", L, f1 ? "hit" : "miss", pr, f2 ? "hit" : "miss", pr2);
    }

    /* ---------- 16.3 ---------- */
    printf("16.3 probes = ceil(log_(M-k) N); with k = 0 vs k = M/10:\n     %8s", "N \\ M");
    for (int M = 10; M <= 1000; M *= 10) printf("   %6d (k=0) (k=M/10)", M);
    printf("\n");
    for (double N = 1e3; N <= 1e6; N *= 10) {
        printf("     %8.0f", N);
        for (int M = 10; M <= 1000; M *= 10) {
            int k = M / 10;
            double a = log(N) / log(M), b = log(N) / log(M - k);
            printf("   %6s %4.0f(%4.2f) %4.0f(%4.2f)", "", ceil(a - 1e-9), a, ceil(b - 1e-9), b);
        }
        printf("\n");
    }
    printf("     the real value (in parentheses) grows only 1-5%%; the rounded probe count goes up by one\n"
           "     only where N sat exactly on a power of M (a full tree with no room left), never by more\n");

    /* ---------- 16.12 ---------- */
    printf("16.12 three levels: a links in memory -> pages with b..2b links -> pages with c..2c items\n"
           "     maximum = a * 2b * 2c = 4abc items   (minimum a*b*c)\n");
    { long a = 1000, b = 500, c = 500;
      printf("     e.g. a = %ld, b = c = %ld: max %ld items, min %ld; still 2 probes per search\n", a, b, 4 * a * b * c, a * b * c); }

    /* ---------- 16.21 ---------- */
    printf("16.21 log_999 N - log_1000 N:\n");
    for (double N = 1e3; N <= 1e6; N *= 10)
        printf("     N = %8.0f: %.6f - %.6f = %.6f\n", N, log(N) / log(999), log(N) / log(1000), log(N) / log(999) - log(N) / log(1000));
    return 0;
}
