/* ============================================================
 *  ex14_25_probing.c  —  Exercises 14.24-14.26, 14.28, 14.31, 14.32
 *  Chapter 14: Hashing — linear probing and double hashing
 *
 *  14.24 Worst-case time to insert N keys with linear probing.
 *  14.25 Table after inserting E A S Y Q U T I O N into an empty table of
 *        size M = 16 with linear probing, hash 11k mod M.
 *  14.26 The same for M = 10.
 *  14.28 Insert M/2 random keys into a table of size M; compute the average
 *        cost of a search miss from the cluster lengths.
 *  14.31 The same keys with double hashing, M = 16: first hash 11k mod M,
 *        increment (k mod 3) + 1.
 *  14.32 The same for M = 10.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 14.4, 14.6, Exercises 14.24-14.32
 *  Build:   gcc -std=c99 -Wall -O2 ex14_25_probing.c -o ex14_25
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

static int T[2000000];
static long probes;
static int kk(int c) { return c - 'A' + 1; }

/* returns final position, or -1 if the probe sequence never finds an empty slot */
static int insert(int M, int c, int dbl)
{
    int i = (11 * kk(c)) % M, step = dbl ? (kk(c) % 3) + 1 : 1, tries = 0;
    while (T[i]) { probes++; i = (i + step) % M; if (++tries > M) return -1; }
    probes++;
    T[i] = c; return i;
}
static void run(int M, int dbl)
{
    const char *s = "EASYQUTION";
    for (int i = 0; i < M; i++) T[i] = 0;
    probes = 0;
    printf("     ");
    for (int i = 0; s[i]; i++) {
        int pos = insert(M, s[i], dbl);
        if (pos < 0) { printf("\n     %c (k=%d, h=%d, step %d) CANNOT be inserted: the probe sequence cycles through occupied slots only\n     ", s[i], kk(s[i]), (11 * kk(s[i])) % M, (kk(s[i]) % 3) + 1); continue; }
    }
    printf("table:");
    for (int i = 0; i < M; i++) printf(" %c", T[i] ? T[i] : '.');
    printf("   (%ld probes)\n", probes);
}
static unsigned long long rs = 16180;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }

int main(void)
{
    printf("14.25 linear probing, M = 16:  hashes");
    for (const char *c = "EASYQUTION"; *c; c++) printf(" %c=%d", *c, (11 * kk(*c)) % 16);
    printf("\n"); run(16, 0);
    printf("14.26 linear probing, M = 10:  hashes");
    for (const char *c = "EASYQUTION"; *c; c++) printf(" %c=%d", *c, (11 * kk(*c)) % 10);
    printf("\n"); run(10, 0);
    printf("14.31 double hashing, M = 16 (steps");
    for (const char *c = "EASYQUTION"; *c; c++) printf(" %c=%d", *c, kk(*c) % 3 + 1);
    printf("):\n"); run(16, 1);
    printf("14.32 double hashing, M = 10:\n"); run(10, 1);

    printf("14.24 worst case: every key hashes to the same slot:\n");
    for (int N = 1000; N <= 8000; N *= 2) {
        for (int i = 0; i < 2 * N; i++) T[i] = 0;
        long p = 0;
        for (int k = 0; k < N; k++) { int i = 0; while (T[i]) { i++; p++; } T[i] = 1; p++; }
        printf("     N=%5d: %9ld probes (N(N+1)/2 = %ld)\n", N, p, (long)N * (N + 1) / 2);
    }

    printf("14.28 M/2 random keys (alpha = 1/2); search miss cost from clusters vs the formula (1 + 1/(1-a)^2)/2 = 2.5:\n");
    for (int M = 1000; M <= 1000000; M *= 10) {
        for (int i = 0; i < M; i++) T[i] = 0;
        long ins = 0;
        for (int k = 0; k < M / 2; k++) { int i = rnd() % M; while (T[i]) { i = (i + 1) % M; ins++; } T[i] = 1; ins++; }
        /* a miss that hashes into a cluster of length t (or the empty slot after it)
           costs on average (t+1)(t+2)/2 / ... : sum over positions of (distance to next empty + 1) */
        long total = 0; int start = 0; while (T[start]) start++;          /* start at an empty slot */
        for (int d = 0; d < M; d++) {
            int i = (start + d) % M;
            if (!T[i]) { total += 1; continue; }
            int t = 0; while (T[(i + t) % M]) t++;
            total += t + 1; /* probes for a miss starting at i */
        }
        printf("     M=%7d: average miss %.3f probes (insert cost so far %.3f per key)\n", M, (double)total / M, (double)ins / (M / 2));
    }
    return 0;
}
