/* ============================================================
 *  parallel_block_sort.c  —  Parallel sort/merge, simulated:
 *                            Batcher's network on BLOCKS
 *  Chapter 11: Special-Purpose Sorting Methods (Section 11.5)
 *
 *  Model: P processors, each holding a block of N/P records. Step 1:
 *  each processor sorts its own block (in parallel, ~ (N/P) lg(N/P)).
 *  Step 2: run Batcher's odd-even sorting network on the P BLOCKS,
 *  where each comparator "compare-exchange(i, j)" becomes a
 *  MERGE-SPLIT: merge the two blocks, give the smaller half to i and
 *  the larger half to j. By the 0-1 principle argument the result is
 *  sorted. Comparators in one parallel step touch disjoint blocks, so
 *  the whole thing takes (lg P)(lg P + 1)/2 merge-split rounds.
 *  This is the sequential simulation suggested by Exercise 11.62.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Section 11.5,
 *                Program 11.3 (bottom-up Batcher network)
 *  Build:   gcc -std=c99 -Wall -O2 parallel_block_sort.c -o parallel_block_sort
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cmpint(const void *a, const void *b) { int x = *(const int *)a, y = *(const int *)b; return (x > y) - (x < y); }

static int B;               /* block size */
static int *tmp;
static long mergesplits;

/* merge-split blocks a and b: a gets the B smallest, b the B largest */
static void merge_split(int *a, int *b) {
    int i = 0, j = 0;
    for (int k = 0; k < 2 * B; k++)
        tmp[k] = (j >= B || (i < B && a[i] <= b[j])) ? a[i++] : b[j++];
    memcpy(a, tmp, B * sizeof *a); memcpy(b, tmp + B, B * sizeof *b);
    mergesplits++;
}

/* Program 11.3's loop structure, with compexch -> merge_split on blocks */
static int rounds;
static void batcher_blocks(int *a, int P) {
    rounds = 0;
    for (int p = 1; p < P; p += p)
        for (int k = p; k > 0; k /= 2) {
            rounds++;                                   /* one parallel step */
            for (int j = k % p; j + k < P; j += k + k)
                for (int i = 0; i < k; i++)
                    if ((i + j) / (p + p) == (i + j + k) / (p + p))
                        merge_split(a + (i + j) * B, a + (i + j + k) * B);
        }
}

int main(void) {
    const int P = 16, N = 1 << 16;
    B = N / P;
    int *a = malloc(N * sizeof *a); tmp = malloc(2 * B * sizeof *tmp);
    srand(5); for (int i = 0; i < N; i++) a[i] = rand();
    for (int p = 0; p < P; p++) qsort(a + p * B, B, sizeof *a, cmpint);   /* "in parallel" */
    batcher_blocks(a, P);
    for (int i = 1; i < N; i++) if (a[i-1] > a[i]) { puts("NOT SORTED"); return 1; }
    printf("N=%d records on P=%d processors (blocks of %d): sorted OK\n", N, P, B);
    printf("  merge-split operations: %ld, parallel rounds: %d  (lgP(lgP+1)/2 = %d)\n",
           mergesplits, rounds, 4 * 5 / 2);
    printf("  each round moves at most 2N/P = %d records per processor pair\n", 2 * B);
    free(a); free(tmp);
    return 0;
}
