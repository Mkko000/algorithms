/* ============================================================
 *  open_addressing.c  —  Linear probing (with deletion), double
 *                        hashing, and dynamic (doubling) hash tables
 *  Chapter 14: Hashing (Sections 14.3, 14.4, 14.5)
 *
 *  Integer keys, table of size M, load factor alpha = N/M.
 *  - Program 14.4  linear probing: on collision try i+1, i+2, ...
 *  - Program 14.5  deletion in linear probing: you can NOT just empty
 *                  the slot (it would cut a cluster and lose keys);
 *                  remove it, then REINSERT every key to its right in
 *                  the same cluster.
 *  - Program 14.6  double hashing: step size k = hash2(v), different
 *                  for different keys, so clusters do not grow together.
 *                  hash2 must never be 0 and must be relatively prime
 *                  to M (M prime does it).
 *  - Program 14.7  dynamic table: double M when the table gets half full
 *                  (rehash everything) — amortized O(1) per insert.
 *  Property 14.3: linear probing, hits ~ (1 + 1/(1-a))/2,
 *                                 misses ~ (1 + 1/(1-a)^2)/2
 *  Property 14.4: double hashing, hits ~ (1/a) ln(1/(1-a)), misses ~ 1/(1-a)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 14.4-14.7
 *  Build:   gcc -std=c99 -Wall -O2 open_addressing.c -o open_addressing -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EMPTY (-1)
static int *st, M, N, dbl;
static long probes;

static int hash(int v)  { return v % M; }
static int hash2(int v) { return 97 - v % 97; }          /* never 0; 97 prime < M */
static int step(int v)  { return dbl ? hash2(v) : 1; }

void STinit(int m, int use_double) { M = m; N = 0; dbl = use_double; st = malloc(M * sizeof *st); for (int i = 0; i < M; i++) st[i] = EMPTY; }
void STinsert(int v) {
    int i = hash(v), k = step(v);
    while (st[i] != EMPTY) i = (i + k) % M;
    st[i] = v; N++;
}
int STsearch(int v) {
    int i = hash(v), k = step(v);
    probes++;
    while (st[i] != EMPTY) { if (st[i] == v) return 1; i = (i + k) % M; probes++; }
    return 0;
}
void STdelete(int v) {                                    /* Program 14.5 (linear probing) */
    int i = hash(v);
    while (st[i] != EMPTY) { if (st[i] == v) break; i = (i + 1) % M; }
    if (st[i] == EMPTY) return;
    st[i] = EMPTY; N--;
    for (int j = (i + 1) % M; st[j] != EMPTY; j = (j + 1) % M) {
        int t = st[j]; st[j] = EMPTY; N--; STinsert(t);  /* reinsert the rest of the cluster */
    }
}

/* Program 14.7: dynamic hash table (linear probing), double when half full */
static long rehash_moves;
void expand(void) {
    int *old = st, oldM = M;
    STinit(2 * M + 1, 0);                                  /* keep M odd */
    for (int i = 0; i < oldM; i++) if (old[i] != EMPTY) { STinsert(old[i]); rehash_moves++; }
    free(old);
}
void STinsert_dyn(int v) { STinsert(v); if (N >= M / 2) expand(); }

int main(void) {
    const int Mtab = 100003;                              /* prime */
    double alphas[] = {0.25, 0.5, 0.75, 0.9};
    int *keys = malloc(Mtab * sizeof *keys);
    srand(14); for (int i = 0; i < Mtab; i++) keys[i] = rand();
    printf("%5s | %-27s | %-27s\n", "alpha", "linear: hit (pred) miss (pred)", "double: hit (pred) miss (pred)");
    for (int t = 0; t < 4; t++) {
        double a = alphas[t]; int n = (int)(a * Mtab);
        double r[2][2];
        for (int d = 0; d < 2; d++) {
            STinit(Mtab, d); for (int i = 0; i < n; i++) STinsert(keys[i]);
            probes = 0; for (int i = 0; i < 20000; i++) STsearch(keys[rand() % n]); r[d][0] = probes / 20000.0;
            probes = 0; for (int i = 0; i < 20000; i++) STsearch(rand()); r[d][1] = probes / 20000.0;  /* random key: almost surely a miss */
            free(st);
        }
        printf("%5.2f | %5.2f (%5.2f) %6.2f (%6.2f) | %5.2f (%5.2f) %5.2f (%5.2f)\n", a,
               r[0][0], (1 + 1 / (1 - a)) / 2, r[0][1], (1 + 1 / ((1 - a) * (1 - a))) / 2,
               r[1][0], log(1 / (1 - a)) / a, r[1][1], 1 / (1 - a));
    }

    STinit(13, 0);
    int demo[] = {5, 18, 31, 6, 44};                       /* 5,18,31,44 all hash to 5 (mod 13) */
    for (int i = 0; i < 5; i++) STinsert(demo[i]);
    printf("\nlinear probing M=13 after 5 18 31 6 44:"); for (int i = 0; i < M; i++) printf(" %d", st[i]);
    STdelete(18);
    printf("\nafter delete 18 (cluster re-inserted):    "); for (int i = 0; i < M; i++) printf(" %d", st[i]);
    printf("\nsearch 31 -> %s, 44 -> %s (naive emptying would lose them)\n", STsearch(31) ? "hit" : "miss", STsearch(44) ? "hit" : "miss");
    free(st);

    STinit(5, 0); rehash_moves = 0;
    for (int i = 0; i < 100000; i++) STinsert_dyn(keys[i]);
    printf("dynamic table: 100000 inserts -> M=%d, total rehash moves %ld (%.2f per insert)\n", M, rehash_moves, rehash_moves / 100000.0);
    free(st); free(keys);
    return 0;
}
