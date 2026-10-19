/* ============================================================
 *  batcher_network.c  —  Batcher's odd-even merge (recursive, with
 *                        shuffle/unshuffle), sorting networks, and
 *                        the 0-1 principle
 *  Chapter 11: Special-Purpose Sorting Methods (Sections 11.1, 11.2)
 *
 *  Program 11.1: perfect shuffle and unshuffle of a[l..r].
 *  Program 11.2: recursive odd-even merge: unshuffle (odds | evens),
 *     merge each half recursively, shuffle back, then ONE layer of
 *     compare-exchanges between neighbours a[i], a[i+1].
 *  Batcher's sort is NONADAPTIVE (Definition 11.1): the sequence of
 *  compare-exchanges depends only on N, so it can be drawn as a
 *  network of comparators. Property 11.1 (0-1 principle): if a network
 *  sorts every input of 0s and 1s, it sorts every input. For N = 8
 *  that means checking 2^8 = 256 inputs instead of 8! = 40320.
 *  We record the comparators the sort performs, print the network,
 *  count comparators (sorting: 1, 5, 19, 63, 191 for N = 2..32; the
 *  merging networks alone have 1, 3, 9, 25, 65) and parallel
 *  steps, and verify the 0-1 principle exhaustively.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 11.1-11.3,
 *                Property 11.1
 *  Build:   gcc -std=c99 -Wall -O2 batcher_network.c -o batcher_network
 * ============================================================ */
#include <stdio.h>
#include <string.h>

#define maxN 64
typedef int Item;
static Item aux[maxN];
static int comps[4096][2], nc, recording;

#define exch(A, B) { Item t = A; A = B; B = t; }
static int pos[maxN];       /* pos[i] = which ORIGINAL line currently sits at index i */
static void compexch_i(Item a[], int i, int j) {        /* records (line, line) as a comparator */
    if (recording) { comps[nc][0] = pos[i]; comps[nc][1] = pos[j]; nc++; }
    if (a[j] < a[i]) exch(a[i], a[j]);
}

/* Program 11.1 */
void shuffle(Item a[], int l, int r) {
    int i, j, m = (l + r) / 2;
    for (i = l, j = 0; i <= r; i += 2, j++) { aux[i] = a[l + j]; aux[i + 1] = a[m + 1 + j]; }
    for (i = l; i <= r; i++) a[i] = aux[i];
}
void unshuffle(Item a[], int l, int r) {
    int i, j, m = (l + r) / 2;
    for (i = l, j = 0; i <= r; i += 2, j++) { aux[l + j] = a[i]; aux[m + 1 + j] = a[i + 1]; }
    for (i = l; i <= r; i++) a[i] = aux[i];
}

/* To record comparators by ORIGINAL line numbers we move the "position
   tag" array pos[] through the same shuffles as the data. Shuffles keep
   relative order inside each half, so the smaller index is always the
   smaller line and the min still goes to the lower line. */
static void shuffle_both(Item a[], int l, int r)   { shuffle(a, l, r);   shuffle(pos, l, r); }
static void unshuffle_both(Item a[], int l, int r) { unshuffle(a, l, r); unshuffle(pos, l, r); }

/* Program 11.2: recursive odd-even merge of a[l..m] and a[m+1..r] */
void merge(Item a[], int l, int m, int r) {
    if (r == l + 1) { compexch_i(a, l, r); return; }
    if (r < l + 2) return;
    unshuffle_both(a, l, r);
    merge(a, l, (l + m) / 2, m);
    merge(a, m + 1, (m + 1 + r) / 2, r);
    shuffle_both(a, l, r);
    for (int i = l + 1; i < r; i += 2) compexch_i(a, i, i + 1);
}
void batchersort(Item a[], int l, int r) {
    if (r <= l) return;
    int m = (l + r) / 2;
    batchersort(a, l, m); batchersort(a, m + 1, r);
    merge(a, l, m, r);
}

/* Build the network for N: run the sort once on dummy data, recording
   every compare-exchange as a pair of original lines. */
static int build_network(int N) {
    Item dummy[maxN];
    for (int i = 0; i < N; i++) { dummy[i] = 0; pos[i] = i; }
    nc = 0; recording = 1;
    batchersort(dummy, 0, N - 1);
    recording = 0;
    return nc;
}
static void run_network(Item a[], int n) {
    for (int k = 0; k < n; k++) { int i = comps[k][0], j = comps[k][1]; if (a[j] < a[i]) exch(a[i], a[j]); }
}
static int parallel_steps(int n, int N) {        /* Exercise 11.17 labeling */
    int stage[maxN] = {0}, steps = 0;
    (void)N;
    for (int k = 0; k < n; k++) {
        int i = comps[k][0], j = comps[k][1];
        int s = (stage[i] > stage[j] ? stage[i] : stage[j]) + 1;
        stage[i] = stage[j] = s; if (s > steps) steps = s;
    }
    return steps;
}

int main(void) {
    Item a[] = {'A','S','O','R','T','I','N','G','E','X','A','M','P','L','E','S'};
    for (int i = 0; i < 16; i++) pos[i] = i;
    batchersort(a, 0, 15);
    printf("recursive Batcher on ASORTINGEXAMPLES: ");
    for (int i = 0; i < 16; i++) putchar(a[i]);
    putchar('\n');

    for (int N = 2; N <= 32; N *= 2)
        { int n = build_network(N); printf("N=%2d: %3d comparators, %2d parallel steps\n", N, n, parallel_steps(n, N)); }

    int n = build_network(8);
    printf("\nnetwork for N=8 (comparator i:j means compare-exchange lines i and j):\n ");
    for (int k = 0; k < n; k++) printf(" %d:%d", comps[k][0], comps[k][1]);
    putchar('\n');

    int ok = 1;                                     /* 0-1 principle: try all 2^8 */
    for (int m = 0; m < 256; m++) {
        Item b[8]; for (int i = 0; i < 8; i++) b[i] = (m >> i) & 1;
        run_network(b, n);
        for (int i = 1; i < 8; i++) if (b[i-1] > b[i]) ok = 0;
    }
    printf("all 256 zero-one inputs sorted: %s -> by Property 11.1 it sorts ANY 8 keys\n", ok ? "yes" : "NO");
    Item c[8] = {42, 7, 99, 7, 3, 58, 21, 0};
    run_network(c, n);
    printf("network on 42 7 99 7 3 58 21 0 ->"); for (int i = 0; i < 8; i++) printf(" %d", c[i]); putchar('\n');
    return 0;
}
