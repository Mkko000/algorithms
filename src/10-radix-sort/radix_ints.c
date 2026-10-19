/* ============================================================
 *  radix_ints.c  —  LSD radix sort on 32-bit integers, and a
 *                   "sublinear" (in key bits) MSD+insertion hybrid
 *  Chapter 10: Radix Sorting (Sections 10.5, 10.6, 10.7)
 *
 *  LSD (Program 10.4 on words): 4 passes of key-indexed counting, one
 *  per byte, right to left. Each pass must be STABLE, which is why it
 *  works (Property 10.6 style argument).
 *  Section 10.7: we do not need to look at all the bits! For N random
 *  keys, one key-indexed pass on the leading lg N + c bits leaves the
 *  file almost sorted (few inversions), and insertion sort finishes in
 *  linear time. That examines far fewer than N * 32 bits ("sublinear"
 *  in the total number of key bits). We count passes and moves.
 *  Section 10.6 trade-off: bigger radix = fewer passes but bigger
 *  count[] arrays; with R > N the count-array overhead dominates.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 10.4, Sections 10.6-10.7
 *  Build:   gcc -std=c99 -Wall -O2 radix_ints.c -o radix_ints
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned int Item;

/* LSD with radix 2^bits (bits divides 32) */
void lsd(Item a[], Item aux[], int N, int bits) {
    int R = 1 << bits;
    int *count = malloc((R + 1) * sizeof *count);
    for (int shift = 0; shift < 32; shift += bits) {
        memset(count, 0, (R + 1) * sizeof *count);
        for (int i = 0; i < N; i++) count[((a[i] >> shift) & (R - 1)) + 1]++;
        for (int j = 1; j < R; j++) count[j] += count[j - 1];
        for (int i = 0; i < N; i++) aux[count[(a[i] >> shift) & (R - 1)]++] = a[i];
        memcpy(a, aux, N * sizeof *a);
    }
    free(count);
}

/* Section 10.7: one MSD pass on the top b bits, then insertion sort */
long hybrid(Item a[], Item aux[], int N, int b) {
    int R = 1 << b; int *count = calloc(R + 1, sizeof *count);
    for (int i = 0; i < N; i++) count[(a[i] >> (32 - b)) + 1]++;
    for (int j = 1; j < R; j++) count[j] += count[j - 1];
    for (int i = 0; i < N; i++) aux[count[a[i] >> (32 - b)]++] = a[i];
    memcpy(a, aux, N * sizeof *a);
    free(count);
    long moves = 0;
    for (int i = 1; i < N; i++) { Item v = a[i]; int j = i; while (j > 0 && v < a[j-1]) { a[j] = a[j-1]; j--; moves++; } a[j] = v; }
    return moves;
}

static int cmpu(const void *x, const void *y) { Item a = *(const Item *)x, b = *(const Item *)y; return (a > b) - (a < b); }
static void check(Item *a, int N, const char *m) { for (int i = 1; i < N; i++) if (a[i-1] > a[i]) { printf("BUG %s\n", m); exit(1); } }

int main(void) {
    const int N = 1 << 20;
    Item *a = malloc(N * sizeof *a), *b = malloc(N * sizeof *b), *aux = malloc(N * sizeof *aux);
    srand(99); for (int i = 0; i < N; i++) b[i] = ((Item)rand() << 16) ^ (Item)rand();
    int radixbits[] = {4, 8, 16};
    for (int k = 0; k < 3; k++) {
        memcpy(a, b, N * sizeof *a); clock_t t = clock();
        lsd(a, aux, N, radixbits[k]); check(a, N, "lsd");
        printf("LSD radix 2^%-2d: %2d passes, %.3fs\n", radixbits[k], 32 / radixbits[k], (double)(clock() - t) / CLOCKS_PER_SEC);
    }
    memcpy(a, b, N * sizeof *a); clock_t t = clock();
    qsort(a, N, sizeof *a, cmpu); check(a, N, "qsort");
    printf("library qsort  :            %.3fs\n", (double)(clock() - t) / CLOCKS_PER_SEC);
    memcpy(a, b, N * sizeof *a); t = clock();
    long mv = hybrid(a, aux, N, 20); check(a, N, "hybrid");
    printf("1 pass on top 20 bits + insertion: %.3fs, insertion moves = %ld (%.2f per key)\n",
           (double)(clock() - t) / CLOCKS_PER_SEC, mv, (double)mv / N);
    free(a); free(b); free(aux);
    return 0;
}
