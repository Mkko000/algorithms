/* ============================================================
 *  ex11_35_external.c  —  Exercises 11.35-11.37, 11.45-11.48
 *  Chapter 11: Special-Purpose Sorting Methods — external sorting
 *
 *  11.35 Sort E A S Y Q U E S T I O N W I T H P L E N T Y O F K E Y S with
 *        3-way balanced merging (memory for 3 records, 3+3 devices).
 *  11.36 Effect on the number of passes of doubling the number of devices.
 *  11.37 Effect of 10 times more internal memory.
 *  11.45 Runs produced by replacement selection with a PQ of size 4 for
 *        E A S Y Q U E S T I O N.
 *  11.46 Replacement selection on a file it produced itself.
 *  11.47 Average number of runs with a PQ of size 1000, N = 10^3 .. 10^6.
 *  11.48 Worst-case number of runs with a PQ of size M < N.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Section 11.3-11.4, Exercises 11.35-11.48
 *  Build:   gcc -std=c99 -Wall -O2 ex11_35_external.c -o ex11_35 -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ---- 11.35: tapes are strings of runs separated by '|' ---- */
#define P 3
static char tape[2 * P][128];
static void show_tapes(int from)
{
    for (int t = from; t < from + P; t++) printf("       dev %d: %s\n", t, tape[t][0] ? tape[t] : "-");
}
static int cmpc(const void *a, const void *b) { return *(const char *)a - *(const char *)b; }
static int balanced(const char *in, int M)
{
    int n = (int)strlen(in), t = 0, passes = 0;
    for (int i = 0; i < 2 * P; i++) tape[i][0] = 0;
    for (int i = 0; i < n; i += M, t = (t + 1) % P) {        /* sort M at a time, deal to devices */
        char run[16]; int k = (n - i < M) ? n - i : M;
        memcpy(run, in + i, k); run[k] = 0; qsort(run, k, 1, cmpc);
        if (tape[P + t][0]) strcat(tape[P + t], "|");
        strcat(tape[P + t], run);
    }
    printf("     initial runs (memory %d):\n", M); show_tapes(P);
    int src = P, dst = 0;
    for (;;) {
        int runs = 0;
        for (int i = src; i < src + P; i++) if (tape[i][0]) { runs++; for (char *c = tape[i]; *c; c++) if (*c == '|') runs++; }
        if (runs <= 1) break;
        for (int i = dst; i < dst + P; i++) tape[i][0] = 0;
        char *pos[P]; for (int i = 0; i < P; i++) pos[i] = tape[src + i];
        int out = 0;
        for (;;) {                                           /* merge one run from each input */
            int any = 0; for (int i = 0; i < P; i++) if (*pos[i]) any = 1;
            if (!any) break;
            char merged[128]; int m = 0;
            for (;;) {
                int b = -1;
                for (int i = 0; i < P; i++) if (*pos[i] && *pos[i] != '|' && (b < 0 || *pos[i] < *pos[b])) b = i;
                if (b < 0) break;
                merged[m++] = *pos[b]++;
            }
            for (int i = 0; i < P; i++) if (*pos[i] == '|') pos[i]++;
            merged[m] = 0;
            if (tape[dst + out][0]) strcat(tape[dst + out], "|");
            strcat(tape[dst + out], merged);
            out = (out + 1) % P;
        }
        passes++;
        printf("     after merge pass %d:\n", passes); show_tapes(dst);
        int t2 = src; src = dst; dst = t2;
    }
    return passes;
}

/* ---- replacement selection: heap of (run, key) ---- */
typedef struct { int run; int key; } RS;
static RS h[100001]; static int hn;
static int lessRS(RS a, RS b) { return a.run != b.run ? a.run < b.run : a.key < b.key; }
static void up(int k) { while (k > 1 && lessRS(h[k], h[k / 2])) { RS t = h[k]; h[k] = h[k / 2]; h[k / 2] = t; k /= 2; } }
static void down(int k) { while (2 * k <= hn) { int j = 2 * k; if (j < hn && lessRS(h[j + 1], h[j])) j++; if (!lessRS(h[j], h[k])) break; RS t = h[k]; h[k] = h[j]; h[j] = t; k = j; } }
/* returns number of runs; writes output to out[] and run ids to rid[] */
static int replacement(const int *in, int n, int M, int *out, int *rid)
{
    int i = 0, o = 0, maxrun = 0; hn = 0;
    while (i < n && hn < M) { h[++hn] = (RS){ 0, in[i++] }; up(hn); }
    while (hn) {
        RS m = h[1]; out[o] = m.key; rid[o++] = m.run; if (m.run > maxrun) maxrun = m.run;
        if (i < n) { int v = in[i++]; h[1] = (RS){ v >= m.key ? m.run : m.run + 1, v }; down(1); }
        else { h[1] = h[hn--]; if (hn) down(1); }
    }
    return maxrun + 1;
}
static unsigned long long rs = 31337;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }

int main(void)
{
    printf("11.35 3-way balanced merge of EASYQUESTIONWITHPLENTYOFKEYS:\n");
    int passes = balanced("EASYQUESTIONWITHPLENTYOFKEYS", 3);
    printf("     %d merge passes (28 keys, 10 runs: ceil(log_3 10) = %d)\n", passes, (int)ceil(log(10) / log(3) - 1e-9));

    printf("11.36/11.37 passes = 1 + ceil(log_P(N/M)) for N = 10^9 records:\n");
    double Nr = 1e9; double Ms[] = { 1e6, 1e7 }; int Ps[] = { 2, 4, 8, 16 };
    for (int a = 0; a < 2; a++) {
        printf("     M=%.0e:", Ms[a]);
        for (int b = 0; b < 4; b++) printf("  P=%-2d -> %d", Ps[b], 1 + (int)ceil(log(Nr / Ms[a]) / log(Ps[b]) - 1e-9));
        printf("\n");
    }

    const char *s = "EASYQUESTION"; int in[12], out[12], rid[12];
    for (int i = 0; i < 12; i++) in[i] = s[i];
    int runs = replacement(in, 12, 4, out, rid);
    printf("11.45 replacement selection, PQ of 4, on EASYQUESTION: %d runs:  ", runs);
    for (int i = 0; i < 12; i++) { if (i && rid[i] != rid[i - 1]) printf(" | "); printf("%c", out[i]); }
    printf("\n");
    int out2[12], rid2[12];
    int runs2 = replacement(out, 12, 4, out2, rid2);
    printf("11.46 run it again on its own output: %d run(s):  ", runs2);
    for (int i = 0; i < 12; i++) { if (i && rid2[i] != rid2[i - 1]) printf(" | "); printf("%c", out2[i]); }
    printf("\n");

    printf("11.47 PQ of 1000, random files (average run length should be ~2M = 2000):\n");
    static int A[1000000], O[1000000], R[1000000];
    for (int N = 1000; N <= 1000000; N *= 10) {
        for (int i = 0; i < N; i++) A[i] = (int)rnd();
        int r = replacement(A, N, 1000, O, R);
        printf("     N=%7d: %4d runs, average length %.0f\n", N, r, (double)N / r);
    }
    printf("11.48 worst case: a file in DECREASING order:\n");
    for (int M = 10; M <= 1000; M *= 10) {
        int N = 100000;
        for (int i = 0; i < N; i++) A[i] = N - i;
        int r = replacement(A, N, M, O, R);
        printf("     N=%d M=%4d: %5d runs  (ceil(N/M) = %d)\n", N, M, r, (N + M - 1) / M);
    }
    return 0;
}
