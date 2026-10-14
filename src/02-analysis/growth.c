/* ============================================================
 *  growth.c  —  Feeling the growth functions (1, logN, N, NlogN, N^2, 2^N)
 *  Chapter 2: Principles of Algorithm Analysis
 *
 *  Prints a table so you can SEE why an O(N^2) algorithm dies long
 *  before an O(N log N) one does. Numbers beat hand-waving.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Chapter 2.
 *  Build:   gcc -std=c99 -Wall -O2 growth.c -o growth -lm
 * ============================================================ */
#include <stdio.h>
#include <math.h>

int main(void) {
    long Ns[] = {10, 100, 1000, 100000, 1000000};
    printf("%10s %8s %12s %14s %16s\n", "N", "lgN", "N", "N lgN", "N^2");
    for (int i = 0; i < 5; i++) {
        double N = (double)Ns[i];
        printf("%10ld %8.1f %12.0f %14.0f %16.0f\n",
               Ns[i], log2(N), N, N * log2(N), N * N);
    }
    puts("\nSame machine at ~1e9 ops/sec, how long an N^2 pass takes:");
    for (int i = 0; i < 5; i++) {
        double N = (double)Ns[i];
        printf("  N=%-8ld  N^2 ops ~ %.0f  -> %.4g seconds\n",
               Ns[i], N * N, (N * N) / 1e9);
    }
    return 0;
}
