/* ============================================================
 *  ex14_40_dynamic.c  —  Exercises 14.40-14.43
 *  Chapter 14: Hashing — dynamic hash tables
 *
 *  14.40 Contents of the table after inserting E A S Y Q U T I O N into a
 *        table of initial size M = 4 that doubles whenever it is half full
 *        (linear probing, hash 11k mod M).
 *  14.41 Is it more economical to triple (instead of double) when half full?
 *  14.42 ... to triple when one-third full?
 *  14.43 ... to double when three-quarters full?
 *  (measured: total probes including rehashing, final memory, search costs)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 14.7, Exercises 14.40-14.43
 *  Build:   gcc -std=c99 -Wall -O2 ex14_40_dynamic.c -o ex14_40
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

static unsigned *T; static int M, n; static long probes;
static unsigned (*H)(unsigned, int);
static unsigned hletter(unsigned c, int m) { return (11 * (c - 'A' + 1)) % m; }
static unsigned hmix(unsigned x, int m) { x ^= x >> 16; x *= 0x45d9f3bu; x ^= x >> 16; return x % (unsigned)m; }
static void put(unsigned *t, int m, unsigned v) { unsigned i = H(v, m); while (t[i]) { probes++; i = (i + 1) % m; } probes++; t[i] = v; }
static int grow_num, grow_den, factor;
static void insert(unsigned v)
{
    put(T, M, v); n++;
    if ((long)n * grow_den >= (long)M * grow_num) {               /* load reached the threshold */
        int m2 = M * factor; unsigned *t2 = calloc(m2, sizeof *t2);
        for (int i = 0; i < M; i++) if (T[i]) put(t2, m2, T[i]);
        free(T); T = t2; M = m2;
    }
}
static void show(void) { printf("M=%2d:", M); for (int i = 0; i < M; i++) printf(" %c", T[i] ? (char)T[i] : '.'); printf("\n"); }
static unsigned long long rs = 1414;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11) | 1u; }

int main(void)
{
    const char *s = "EASYQUTION";
    H = hletter; M = 4; n = 0; T = calloc(M, sizeof *T); grow_num = 1; grow_den = 2; factor = 2;
    printf("14.40 start M=4, double when half full:\n");
    for (int i = 0; s[i]; i++) { int before = M; insert(s[i]); printf("     +%c  ", s[i]); show(); if (M != before) printf("          (grew %d -> %d, everything rehashed with 11k mod %d)\n", before, M, M); }
    free(T);

    printf("14.41-14.43 N = 10^6 random keys; total probes (inserts + rehashing), final M, final load, avg search-miss probes:\n");
    struct { const char *name; int num, den, f; } st[] = {
        { "double at 1/2 (Program 14.7)", 1, 2, 2 }, { "triple at 1/2  (14.41)", 1, 2, 3 },
        { "triple at 1/3  (14.42)", 1, 3, 3 }, { "double at 3/4  (14.43)", 3, 4, 2 } };
    for (int k = 0; k < 4; k++) {
        H = hmix; M = 4; n = 0; T = calloc(M, sizeof *T); probes = 0;
        grow_num = st[k].num; grow_den = st[k].den; factor = st[k].f;
        rs = 1414;
        for (int i = 0; i < 1000000; i++) insert(rnd());
        long ins = probes;
        /* search-miss cost: probes until an empty slot, averaged over all start positions */
        long tot = 0;
        for (int i = 0; i < M; i++) { int j = i, c = 1; while (T[j]) { j = (j + 1) % M; c++; } tot += c; }
        printf("     %-30s probes %9ld (%.2f per key)  M=%8d  load %.2f  miss %.2f\n",
               st[k].name, ins, ins / 1e6, M, (double)n / M, (double)tot / M);
        free(T);
    }
    return 0;
}
