/* ============================================================
 *  hash_functions.c  —  Hash functions: what makes one good or bad
 *  Chapter 14: Hashing (Section 14.1)
 *
 *  - floating-point keys in [0,1): h = (int)(k * M)   (multiplicative)
 *  - w-bit integer keys: modular hashing h = k mod M, with M PRIME.
 *    If M is a power of 2, k mod M just keeps the low bits: keys that
 *    share low bits (all even, all multiples of 64, ...) collide.
 *  - string keys, Program 14.1: Horner's method, h = (a*h + c) mod M,
 *    reducing mod M at every step so nothing overflows.
 *  - Program 14.2: universal (randomized) string hash: the multiplier
 *    changes pseudo-randomly at every character, so no fixed set of
 *    keys is bad for every table.
 *  We hash the same keys into M buckets and report the longest bucket
 *  and the chi-square-like spread, for good and bad choices.
 *  Honest note: on this very regular key set plain Horner with a prime
 *  M already spreads well; hashU's point is not to beat it on one input
 *  but to make it very unlikely that ANY fixed input is bad.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Section 14.1,
 *                Programs 14.1-14.2
 *  Build:   gcc -std=c99 -Wall -O2 hash_functions.c -o hash_functions
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hashF(double k, int M) { return (int)(k * M); }
int hashI(unsigned k, int M) { return (int)(k % (unsigned)M); }
int hash(const char *v, int M) {                    /* Program 14.1 */
    int h = 0, a = 127;
    for (; *v != '\0'; v++) h = (a * h + (unsigned char)*v) % M;
    return h;
}
int hashU(const char *v, int M) {                   /* Program 14.2 */
    int h, a = 31415, b = 27183;
    for (h = 0; *v != '\0'; v++, a = a * b % (M - 1)) h = (a * h + (unsigned char)*v) % M;
    return h;
}

static void report(const char *name, int *cnt, int M, int N) {
    int mx = 0, empty = 0; double chi = 0, e = (double)N / M;
    for (int i = 0; i < M; i++) { if (cnt[i] > mx) mx = cnt[i]; if (!cnt[i]) empty++; chi += (cnt[i] - e) * (cnt[i] - e) / e; }
    printf("  %-38s longest bucket %4d, empty %4d, chi2/M %.2f (~1 is ideal)\n", name, mx, empty, chi / M);
}

int main(void) {
    const int N = 10000;
    int M; int *cnt = malloc(4096 * sizeof *cnt);

    printf("integer keys that are multiples of 64 (N=%d):\n", N);
    M = 1024; memset(cnt, 0, M * sizeof *cnt); for (int i = 0; i < N; i++) cnt[hashI(64u * i, M)]++;
    report("k mod 1024 (power of 2)", cnt, M, N);
    M = 1021; memset(cnt, 0, M * sizeof *cnt); for (int i = 0; i < N; i++) cnt[hashI(64u * i, M)]++;
    report("k mod 1021 (prime)", cnt, M, N);

    printf("floating keys uniform in [0,1):\n");
    M = 1021; memset(cnt, 0, M * sizeof *cnt); srand(1); for (int i = 0; i < N; i++) cnt[hashF((double)rand() / ((double)RAND_MAX + 1), M)]++;
    report("(int)(k*M)", cnt, M, N);

    printf("string keys \"key0\"..\"key9999\":\n");
    char buf[32];
    M = 1024; memset(cnt, 0, M * sizeof *cnt); for (int i = 0; i < N; i++) { sprintf(buf, "key%d", i); cnt[hash(buf, M)]++; }
    report("Horner a=127, M=1024 (power of 2!)", cnt, M, N);
    M = 1021; memset(cnt, 0, M * sizeof *cnt); for (int i = 0; i < N; i++) { sprintf(buf, "key%d", i); cnt[hash(buf, M)]++; }
    report("Horner a=127, M=1021 (prime)", cnt, M, N);
    M = 1021; memset(cnt, 0, M * sizeof *cnt); for (int i = 0; i < N; i++) { sprintf(buf, "key%d", i); cnt[hashU(buf, M)]++; }
    report("universal hashU, M=1021", cnt, M, N);
    M = 1021; memset(cnt, 0, M * sizeof *cnt); for (int i = 0; i < N; i++) { sprintf(buf, "key%d", i); int h = 0; for (char *p = buf; *p; p++) h += *p; cnt[h % M]++; }
    report("BAD: sum of characters mod 1021", cnt, M, N);
    free(cnt);
    return 0;
}
