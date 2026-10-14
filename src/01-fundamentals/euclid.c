/* ============================================================
 *  euclid.c  —  Euclid's algorithm for the greatest common divisor
 *  Chapter 1 (also used as a recursion example in Chapter 5)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 5.3
 *  Build:   gcc -std=c99 -Wall -O2 euclid.c -o euclid
 * ============================================================ */
#include <stdio.h>

/* gcd(u,v): if v is 0 the answer is u, otherwise gcd(v, u mod v). */
int gcd(int u, int v) {
    if (v == 0) return u;
    return gcd(v, u % v);
}

int main(void) {
    int a = 461952, b = 116298;
    printf("gcd(%d, %d) = %d\n", a, b, gcd(a, b));   /* 18 */
    printf("gcd(%d, %d) = %d\n", 314159, 271828, gcd(314159, 271828));
    return 0;
}
