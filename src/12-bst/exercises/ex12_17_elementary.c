/* ============================================================
 *  ex12_17_elementary.c  —  Exercises 12.17, 12.34, 12.35
 *  Chapter 12: Symbol Tables and BSTs — elementary symbol tables
 *
 *  12.17 Number of comparisons to put E A S Y Q U E S T I O N into an
 *        initially empty table implemented with an ordered or unordered
 *        array or list: a search for each key, then an insert on a miss
 *        (search-insert, Exercise 12.15: the insert reuses the position the
 *        search found, so it costs no extra comparisons).
 *  12.34 For which N does binary search become 10, 100 and 1000 times
 *        faster than sequential search?  Predict, then verify.
 *  12.35 Inserts done as in insertion sort, searches by binary search,
 *        searches 1000 times more frequent than inserts: what percentage of
 *        the time goes to insertions, for N = 10^3 .. 10^6?
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 12.4-12.6, Exercises 12.17-12.35
 *  Build:   gcc -std=c99 -Wall -O2 ex12_17_elementary.c -o ex12_17 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

static long C;
static int cmp3(int a, int b) { C++; return (a > b) - (a < b); }

/* the four elementary tables; lists are simulated by arrays with the same
   access order (front-to-back), which is all that matters for counting  */
static long run(const char *s, int ordered, int list)
{
    int t[64], n = 0; C = 0;
    for (int k = 0; s[k]; k++) {
        int v = s[k], i, found = 0;
        if (!ordered) {
            for (i = 0; i < n; i++) if (cmp3(t[i], v) == 0) { found = 1; break; }
            if (!found) {
                if (list) { memmove(t + 1, t, n * sizeof *t); t[0] = v; }   /* list: insert at front */
                else t[n] = v;                                              /* array: append */
                n++;
            }
        } else {
            for (i = 0; i < n; i++) { int c = cmp3(t[i], v); if (c >= 0) { found = (c == 0); break; } }
            if (!found) { memmove(t + i + 1, t + i, (n - i) * sizeof *t); t[i] = v; n++; }
        }
    }
    return C;
}
static int bsearch_count(const int *a, int n, int v)          /* Program 12.6, counting 3-way compares */
{
    int l = 0, r = n - 1;
    while (r >= l) { int m = (l + r) / 2, c = cmp3(v, a[m]); if (c == 0) return m; if (c < 0) r = m - 1; else l = m + 1; }
    return -1;
}

int main(void)
{
    const char *s = "EASYQUESTION";
    printf("12.17 compares for search-insert of EASYQUESTION:\n");
    printf("     unordered array: %ld\n", run(s, 0, 0));
    printf("     unordered list : %ld   (new items go to the front)\n", run(s, 0, 1));
    printf("     ordered array  : %ld\n", run(s, 1, 0));
    printf("     ordered list   : %ld\n", run(s, 1, 1));

    printf("12.34 search hits: sequential ~N/2 compares vs binary ~lg N - 1; speedup = (N/2)/(lg N - 1):\n");
    static int a[1 << 20];
    for (int target = 10; target <= 1000; target *= 10) {
        int N = 4; while ((N / 2.0) / (log2(N) - 1) < target) N++;
        for (int i = 0; i < N; i++) a[i] = 2 * i;
        long seq = 0, bin = 0;
        for (int i = 0; i < N; i++) { seq += i + 1; C = 0; bsearch_count(a, N, 2 * i); bin += C; }
        printf("     %4dx predicted at N = %6d; measured: sequential %.1f, binary %.2f, ratio %.1f\n",
               target, N, (double)seq / N, (double)bin / N, (double)seq / bin);
    }

    printf("12.35 1000 binary searches per insertion-sort-style insert (time shares):\n");
    for (int N = 1000; N <= 1000000; N *= 10) {
        for (int i = 0; i < N; i++) a[i] = 2 * i;
        unsigned long long rs = 42; volatile long sink = 0;
        int ins = N >= 100000 ? 20 : 200;                    /* fewer reps for big N */
        clock_t c0 = clock();
        for (int r = 0; r < ins; r++) {                       /* insert in the middle, then remove: average move N/2 */
            int pos = N / 2; memmove(a + pos + 1, a + pos, (N - pos) * sizeof *a); a[pos] = 2 * pos - 1;
            memmove(a + pos, a + pos + 1, (N - pos) * sizeof *a);
        }
        double ti = (double)(clock() - c0) / ins / 2;
        c0 = clock();
        for (int r = 0; r < ins * 1000; r++) { rs = rs * 6364136223846793005ULL + 1; int v = (int)(rs >> 33) % N * 2; sink += bsearch_count(a, N, v); }
        double ts = (double)(clock() - c0) / ins;
        double model = (N / 2.0) / (N / 2.0 + 1000 * log2(N));      /* moves vs compares, unit cost each */
        printf("     N=%7d: model (N/2 moves vs 1000 lg N compares) %5.1f%%   measured time %5.1f%%\n", N, 100 * model, 100 * ti / (ti + ts));
    }
    return 0;
}
