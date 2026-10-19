/* ============================================================
 *  msd_radix.c  —  MSD (most-significant-digit) radix sort on strings
 *  Chapter 10: Radix Sorting (Sections 10.3, 10.6)
 *
 *  Program 10.2: key-indexed counting on byte d, then recur on each
 *  bin with byte d+1. Keys are C strings, so byte value 0 marks the end
 *  of a key: bin 0 holds keys that are completely equal so far, and is
 *  NOT sorted further (that is why the recursion starts at bin 1).
 *  Items are pointers (char *), so only pointers move (Section 6.8).
 *
 *  Experiments:
 *   - the EMPTY-BINS problem (end of Section 10.3, Property 10.3: at
 *     least 2N + 2R steps per call): with R = 256 and no cutoff, almost
 *     all the work is clearing and scanning counters of tiny subfiles;
 *     a cutoff to insertion sort for small subfiles fixes it.
 *   - sublinear behaviour (Property 10.4): bytes examined vs total bytes.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 10.2
 *  Build:   gcc -std=c99 -Wall -O2 msd_radix.c -o msd_radix
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef char *Item;
#define R 256
#define digit(A, B) ((unsigned char)(A)[B])
static Item *aux;
static long counter_ops, bytes_seen;
static int M = 0;                       /* cutoff to insertion sort */

/* insertion sort comparing from byte d on (all keys agree before d) */
static void insertion(Item a[], int l, int r, int d) {
    for (int i = l + 1; i <= r; i++) {
        Item v = a[i]; int j = i;
        while (j > l && strcmp(v + d, a[j-1] + d) < 0) { a[j] = a[j-1]; j--; }
        a[j] = v;
    }
}

/* Program 10.2 */
#define bin(A) l + count[A]
static void radixMSD(Item a[], int l, int r, int d) {
    int i, j, count[R + 1];
    if (r <= l) return;
    if (r - l <= M) { insertion(a, l, r, d); return; }
    for (j = 0; j < R; j++) count[j] = 0;
    for (i = l; i <= r; i++) { count[digit(a[i], d) + 1]++; bytes_seen++; }
    for (j = 1; j < R; j++) count[j] += count[j - 1];
    for (i = l; i <= r; i++) aux[l + count[digit(a[i], d)]++] = a[i];
    for (i = l; i <= r; i++) a[i] = aux[i];
    counter_ops += 3 * R;               /* clear, prefix-sum, recursion scan */
    /* now count[j] = start of bin j+1; bin 0 (end of key) is done */
    for (j = 0; j < R - 1; j++)
        radixMSD(a, bin(j), bin(j + 1) - 1, d + 1);
}

static char *randword(int len) {
    char *s = malloc(len + 1);
    for (int i = 0; i < len; i++) s[i] = 'a' + rand() % 26;
    s[len] = 0;
    return s;
}

static void run(const char *name, Item *src, int n, int cutoff, long total) {
    Item *a = malloc(n * sizeof *a);
    memcpy(a, src, n * sizeof *a);
    M = cutoff; counter_ops = bytes_seen = 0;
    clock_t t0 = clock();
    radixMSD(a, 0, n - 1, 0);
    double t = (double)(clock() - t0) / CLOCKS_PER_SEC;
    for (int i = 1; i < n; i++) if (strcmp(a[i-1], a[i]) > 0) { puts("BUG"); exit(1); }
    printf("  %-34s %.3fs  counter ops=%10ld  bytes read by counting=%9ld (%.0f%% of %ld)\n",
           name, t, counter_ops, bytes_seen, 100.0 * bytes_seen / total, total);
    free(a);
}

int main(void) {
    char *words[] = {"she","sells","seashells","by","the","sea","shore","the",
                     "shells","she","sells","are","surely","seashells"};
    int n = sizeof words / sizeof words[0];
    aux = malloc(200000 * sizeof *aux);
    radixMSD(words, 0, n - 1, 0);
    for (int i = 0; i < n; i++) printf("%s ", words[i]);
    printf("\n\n");

    int N = 100000; long total = 0;
    Item *src = malloc(N * sizeof *src);
    srand(10);
    for (int i = 0; i < N; i++) { src[i] = randword(20); total += 20; }
    printf("N=%d random 20-letter keys, R=%d:\n", N, R);
    run("no cutoff (empty-bins problem)", src, N, 0, total);
    run("cutoff M=16 to insertion sort", src, N, 16, total);
    free(src);
    return 0;
}
