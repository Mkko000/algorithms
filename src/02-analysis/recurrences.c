/* ============================================================
 *  recurrences.c  —  The five basic recurrences of Section 2.5,
 *                    computed exactly and compared with their
 *                    closed-form solutions.
 *  Chapter 2: Principles of Algorithm Analysis (Sections 2.5-2.6)
 *
 *    F2.1  C(N) = C(N-1) + N      C(1)=1  ->  N(N+1)/2   (loop removes 1 item)
 *    F2.2  C(N) = C(N/2) + 1      C(1)=1  ->  lg N        (halve, one step)
 *    F2.3  C(N) = C(N/2) + N      C(1)=0  ->  2N          (halve, scan all)
 *    F2.4  C(N) = 2C(N/2) + N     C(1)=0  ->  N lg N      (divide & conquer)
 *    F2.5  C(N) = 2C(N/2) + 1     C(1)=1  ->  2N          (split, 1 step)
 *
 *  For N = 2^n the closed forms are exact; for other N they are
 *  within small constant terms. That is the whole point of the
 *  "telescoping" trick the book uses: unroll, then sum.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Formulas 2.1-2.5
 *  Build:   gcc -std=c99 -Wall -O2 recurrences.c -o recurrences -lm
 * ============================================================ */
#include <stdio.h>
#include <math.h>

long f21(long n) { return n <= 1 ? 1 : f21(n - 1) + n; }
long f22(long n) { return n <= 1 ? 1 : f22(n / 2) + 1; }
long f23(long n) { return n <= 1 ? 0 : f23(n / 2) + n; }
long f24(long n) { return n <= 1 ? 0 : f24(n / 2) + f24(n - n / 2) + n; }
long f25(long n) { return n <= 1 ? 1 : f25(n / 2) + f25(n - n / 2) + 1; }

/* Amortized cost demo (Section 2.7 "guarantees"): a doubling array.
   Each push is O(1) amortized even though some pushes copy everything. */
long doubling_copies(long pushes) {
    long cap = 1, size = 0, copies = 0;
    for (long i = 0; i < pushes; i++) {
        if (size == cap) { copies += size; cap *= 2; }
        size++;
    }
    return copies;
}

int main(void) {
    printf("%8s | %12s %12s | %5s %5s | %8s %8s | %10s %10s | %8s %8s\n",
           "N", "F2.1", "N(N+1)/2", "F2.2", "lgN+1", "F2.3", "2N",
           "F2.4", "N lgN", "F2.5", "2N-1");
    for (long n = 8; n <= 4096; n *= 4) {
        double lg = log2((double)n);
        printf("%8ld | %12ld %12ld | %5ld %5.0f | %8ld %8ld | %10ld %10.0f | %8ld %8ld\n",
               n, f21(n), n * (n + 1) / 2, f22(n), lg + 1, f23(n), 2 * n,
               f24(n), n * lg, f25(n), 2 * n - 1);
    }
    printf("\nNon powers of two (closed forms are approximations):\n");
    for (long n = 1000; n <= 9000; n += 4000)
        printf("  N=%ld  F2.4=%ld  N lg N=%.0f   ratio=%.3f\n",
               n, f24(n), n * log2((double)n), f24(n) / (n * log2((double)n)));

    printf("\nAmortized analysis: array doubling\n");
    for (long p = 1000; p <= 1000000; p *= 10)
        printf("  %8ld pushes -> %8ld element copies  (%.2f per push)\n",
               p, doubling_copies(p), (double)doubling_copies(p) / p);
    return 0;
}
