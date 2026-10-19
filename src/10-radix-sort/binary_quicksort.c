/* ============================================================
 *  binary_quicksort.c  —  Bits, bytes, words; and binary quicksort
 *  Chapter 10: Radix Sorting (Sections 10.1, 10.2)
 *
 *  Section 10.1 gives the vocabulary: a key is a word of bitsword bits,
 *  split into bytesword "digits" of bitsbyte bits each, radix R = 2^bitsbyte.
 *     digit(A, B) = (A >> (bitsword - (B+1)*bitsbyte)) & (R-1)
 *  Program 10.1 (binary quicksort / radix-exchange sort): partition on
 *  the leading bit — keys with bit 0 to the left, bit 1 to the right —
 *  then recurse on the next bit. Like quicksort, but the "pivot" is a
 *  bit, not a key, so no key compares at all (Property 10.2: ~N lg N
 *  bits examined for random keys).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Section 10.1, Program 10.1
 *  Build:   gcc -std=c99 -Wall -O2 binary_quicksort.c -o binary_quicksort
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef unsigned int Item;
#define bitsword 32
#define bitsbyte 8
#define bytesword 4
#define R (1 << bitsbyte)
#define digit(A, B) (((A) >> (bitsword - ((B) + 1) * bitsbyte)) & (R - 1))
#define bit(A, B)   (((A) >> (bitsword - 1 - (B))) & 1u)       /* bit B from the left */
#define exch(A, B) { Item t = A; A = B; B = t; }

static long bits_examined;

void quicksortB(Item a[], int l, int r, int w) {
    int i = l, j = r;
    if (r <= l || w > bitsword - 1) return;
    while (j != i) {
        while ((bits_examined++, bit(a[i], w) == 0) && (i < j)) i++;
        while ((bits_examined++, bit(a[j], w) == 1) && (j > i)) j--;
        exch(a[i], a[j]);
    }
    if (bit(a[r], w) == 0) j++;          /* all keys had bit 0: right side is empty */
    quicksortB(a, l, j - 1, w + 1);
    quicksortB(a, j, r, w + 1);
}

int main(void) {
    Item x = 0xCAFEBABEu;
    printf("key 0x%08X: bytes (digits, R=%d):", x, R);
    for (int b = 0; b < bytesword; b++) printf(" %02X", digit(x, b));
    printf("   first 8 bits:");
    for (int b = 0; b < 8; b++) printf("%u", bit(x, b));
    putchar('\n');

    Item s[] = {0xA0000000u, 0x30000000u, 0xF0000000u, 0x10000000u, 0x70000000u, 0x50000000u, 0xC0000000u};
    quicksortB(s, 0, 6, 0);
    printf("binary quicksort (top nibble shown):");
    for (int i = 0; i < 7; i++) printf(" %X", s[i] >> 28);
    putchar('\n');

    const int N = 1 << 16; Item *a = malloc(N * sizeof *a);
    srand(10); for (int i = 0; i < N; i++) a[i] = ((Item)rand() << 16) ^ (Item)rand();
    bits_examined = 0; quicksortB(a, 0, N - 1, 0);
    for (int i = 1; i < N; i++) if (a[i-1] > a[i]) { puts("BUG"); return 1; }
    printf("N=%d random keys: %ld bits examined, N lg N = %d\n", N, bits_examined, N * 16);
    free(a);
    return 0;
}
