/* ============================================================
 *  ex10_01_digits.c  —  Exercises 10.1, 10.2, 10.3, 10.4
 *  Chapter 10: Radix Sorting — digits and bytes
 *
 *  10.1 How many digits does a 32-bit quantity have in radix 256, and in
 *       radix 2^16?  Describe how to extract each digit.
 *  10.2 For N = 10^3, 10^6, 10^9, give the smallest byte size that allows
 *       any number between 0 and N to be represented in a 4-byte word.
 *  10.3 Implement less() using the digit abstraction.
 *  10.4 Compare the cost of extracting digits by bit-shifting and by
 *       arithmetic (/ and %).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Section 10.1, Exercises 10.1-10.4
 *  Build:   gcc -std=c99 -Wall -O2 ex10_01_digits.c -o ex10_01
 * ============================================================ */
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define bitsword 32
static int bitsbyte = 8;
#define bytesword (bitsword / bitsbyte)
#define R (1u << bitsbyte)
static unsigned digit(uint32_t a, int b)              /* b = 0 is the most significant byte */
{ return (a >> (bitsbyte * (bytesword - 1 - b))) & (R - 1); }

static int less_digits(uint32_t a, uint32_t b)          /* 10.3 */
{
    for (int i = 0; i < bytesword; i++) {
        unsigned x = digit(a, i), y = digit(b, i);
        if (x != y) return x < y;
    }
    return 0;
}

int main(void)
{
    uint32_t v = 0xDEADBEEF;
    printf("10.1 0x%08X in radix 256 (4 digits):", (unsigned)v);
    bitsbyte = 8; for (int i = 0; i < bytesword; i++) printf(" %u", digit(v, i));
    printf("\n     in radix 65536 (2 digits):");
    bitsbyte = 16; for (int i = 0; i < bytesword; i++) printf(" %u", digit(v, i));
    printf("\n");

    long Ns[] = { 1000, 1000000, 1000000000 };
    for (int t = 0; t < 3; t++) {
        int need = 0; while ((1L << need) <= Ns[t]) need++;          /* bits for 0..N */
        int b = (need + 3) / 4;                                       /* 4 bytes per word */
        printf("10.2 N=%10ld needs %2d bits -> bytes of %d bits (4 x %d = %d >= %d)\n", Ns[t], need, b, b, 4 * b, need);
    }

    bitsbyte = 8;
    uint32_t xs[] = { 5, 300, 70000, 0xFFFFFFFF, 0 }; int ok = 1;
    for (int i = 0; i < 5; i++) for (int j = 0; j < 5; j++) if (less_digits(xs[i], xs[j]) != (xs[i] < xs[j])) ok = 0;
    printf("10.3 less() via digits agrees with < on all test pairs: %s\n", ok ? "yes" : "NO");

    /* 10.4: volatile divisor stops the compiler turning / and % into shifts */
    volatile uint32_t base = 256; uint32_t B = base; volatile uint32_t sink = 0;
    long n = 50000000; uint32_t x = 12345;
    clock_t c0 = clock();
    for (long i = 0; i < n; i++) { x = x * 1664525u + 1013904223u; sink += (x >> 16) & 255; }
    clock_t c1 = clock();
    for (long i = 0; i < n; i++) { x = x * 1664525u + 1013904223u; sink += (x / (B * B)) % B; }
    clock_t c2 = clock();
    (void)sink;
    double ts = (double)(c1 - c0) / CLOCKS_PER_SEC, ta = (double)(c2 - c1) / CLOCKS_PER_SEC;
    printf("10.4 %ld digit extractions: shift/mask %.3f s (%.0f M/s), divide/mod %.3f s (%.0f M/s), ratio %.1fx\n",
           n, ts, n / ts / 1e6, ta, n / ta / 1e6, ta / ts);
    return 0;
}
