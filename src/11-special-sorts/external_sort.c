/* ============================================================
 *  external_sort.c  —  External sorting: replacement selection +
 *                      balanced multiway merging (simulated)
 *  Chapter 11: Special-Purpose Sorting Methods (Sections 11.3, 11.4)
 *
 *  The file is much bigger than memory. We pretend memory holds only
 *  M records and the "tapes"/disks are files on disk (tmpfile()).
 *  Pass 0 (run formation):
 *    - naive: read M records, sort, write a run of length M;
 *    - replacement selection (Section 11.3): keep a PQ of M records;
 *      output the smallest; replace it by the next input; if the new
 *      record is smaller than the one just written it is "frozen" for
 *      the NEXT run. On random input runs come out about 2M long, so we
 *      need about half as many runs (and maybe one merge pass less).
 *  Merge passes (balanced P-way merge, Section 11.4): repeatedly merge
 *  P runs at a time using a PQ of the P current heads, until one run
 *  remains. #passes ~ 1 + ceil(log_P(N / run length)).
 *  (Polyphase merging, also in 11.3, distributes runs unevenly over
 *  fewer tapes to avoid copying; we discuss it in the lesson.)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Sections 11.3-11.4
 *  Build:   gcc -std=c99 -Wall -O2 external_sort.c -o external_sort
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

#define MEM 1000           /* records that fit in "memory" */
#define P   4              /* merge order                    */
#define MAXRUNS 4096

typedef struct { FILE *f; long start, len; } run;
static run runs[MAXRUNS], next[MAXRUNS]; static int nruns;

/* ---- tiny heap ordered by (run, key); src remembers where a record came from ---- */
typedef struct { int key; int run; int src; } hitem;
static hitem h[MEM + 1]; static int hn;
static int hless(hitem a, hitem b) { return a.run != b.run ? a.run < b.run : a.key < b.key; }
static void up(int k) { while (k > 1 && hless(h[k], h[k/2])) { hitem t = h[k]; h[k] = h[k/2]; h[k/2] = t; k /= 2; } }
static void down(int k) { while (2*k <= hn) { int j = 2*k; if (j < hn && hless(h[j+1], h[j])) j++; if (!hless(h[j], h[k])) break; hitem t = h[k]; h[k] = h[j]; h[j] = t; k = j; } }
static void hpush(hitem x) { h[++hn] = x; up(hn); }
static hitem hpop(void) { hitem t = h[1]; h[1] = h[hn--]; if (hn) down(1); return t; }

static int cmpint(const void *a, const void *b) { int x = *(const int *)a, y = *(const int *)b; return (x > y) - (x < y); }
static long io;    /* records read + written, the real cost of external sorting */

/* naive run formation: chunks of MEM, sorted in memory */
static void runs_naive(FILE *in, FILE *out) {
    static int buf[MEM]; int n;
    nruns = 0; rewind(in);
    while ((n = (int)fread(buf, sizeof(int), MEM, in)) > 0) {
        qsort(buf, n, sizeof(int), cmpint);
        runs[nruns].f = out; runs[nruns].start = ftell(out); runs[nruns].len = n; nruns++;
        fwrite(buf, sizeof(int), n, out); io += 2L * n;
    }
}

/* replacement selection: the PQ is ordered by (run number, key) */
static void runs_replacement(FILE *in, FILE *out) {
    int x, cur = 0; hn = 0; rewind(in);
    while (hn < MEM && fread(&x, sizeof x, 1, in) == 1) { hpush((hitem){x, 0, 0}); io++; }
    runs[0].f = out; runs[0].start = ftell(out); runs[0].len = 0; nruns = 1;
    while (hn > 0) {
        hitem m = hpop();
        if (m.run != cur) {                                 /* current run exhausted */
            cur = m.run; runs[nruns].f = out; runs[nruns].start = ftell(out); runs[nruns].len = 0; nruns++;
        }
        fwrite(&m.key, sizeof m.key, 1, out); io++; runs[nruns - 1].len++;
        if (fread(&x, sizeof x, 1, in) == 1) {
            io++;
            hpush((hitem){x, x < m.key ? cur + 1 : cur, 0}); /* smaller: frozen for next run */
        }
    }
}

/* one balanced P-way merge pass: runs[] -> next[] appended to file out */
static void merge_pass(FILE *out) {
    int nn = 0;
    for (int g = 0; g < nruns; g += P) {
        long pos[P], left[P]; int k, x;
        hn = 0;
        next[nn].f = out; next[nn].start = ftell(out); next[nn].len = 0;
        for (k = 0; k < P && g + k < nruns; k++) {
            pos[k] = runs[g + k].start; left[k] = runs[g + k].len;
            fseek(runs[g + k].f, pos[k], SEEK_SET);
            if (left[k] > 0 && fread(&x, sizeof x, 1, runs[g + k].f) == 1) {
                io++; pos[k] += sizeof x; left[k]--; hpush((hitem){x, 0, k});
            }
        }
        while (hn > 0) {
            hitem m = hpop();
            fseek(out, 0, SEEK_END);
            fwrite(&m.key, sizeof m.key, 1, out); io++; next[nn].len++;
            k = m.src;
            if (left[k] > 0) {
                fseek(runs[g + k].f, pos[k], SEEK_SET);
                if (fread(&x, sizeof x, 1, runs[g + k].f) == 1) { io++; pos[k] += sizeof x; left[k]--; hpush((hitem){x, 0, k}); }
            }
        }
        nn++;
    }
    for (int i = 0; i < nn; i++) runs[i] = next[i];
    nruns = nn;
}

static int sort_file(FILE *in, int replacement, int N) {
    FILE *cur = tmpfile(); io = 0;
    if (replacement) runs_replacement(in, cur); else runs_naive(in, cur);
    int initial = nruns, passes = 0;
    long total = 0; for (int i = 0; i < nruns; i++) total += runs[i].len;
    printf("  %-22s %4d initial runs (avg length %.0f)", replacement ? "replacement selection:" : "sort M at a time:",
           initial, (double)total / initial);
    while (nruns > 1) { FILE *nx = tmpfile(); merge_pass(nx); fclose(cur); cur = nx; passes++; }
    /* verify */
    fseek(cur, runs[0].start, SEEK_SET);
    int prev = -1, x, ok = 1; long cnt = 0;
    while (fread(&x, sizeof x, 1, cur) == 1) { if (x < prev) ok = 0; prev = x; cnt++; }
    printf(", %d merge passes, %ld record I/Os, %s\n", passes, io, ok && cnt == N ? "sorted OK" : "FAILED");
    fclose(cur);
    return ok;
}

int main(void) {
    const int N = 70000;      /* 70 naive runs need 4 passes; ~35 runs need only 3 */
    FILE *in = tmpfile();
    srand(2024);
    for (int i = 0; i < N; i++) { int x = rand(); fwrite(&x, sizeof x, 1, in); }
    printf("N=%d records, memory M=%d, %d-way balanced merge\n", N, MEM, P);
    int ok = sort_file(in, 0, N) & sort_file(in, 1, N);
    fclose(in);
    return ok ? 0 : 1;
}
