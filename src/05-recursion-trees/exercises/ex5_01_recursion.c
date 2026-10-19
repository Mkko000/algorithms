/* ============================================================
 *  ex5_01_recursion.c  —  Exercises 5.1-5.7
 *  Chapter 5: Recursion and Trees — solved exercises
 *
 *  5.1 Write a recursive program to compute lg(N!).
 *  5.2 Modify Program 5.1 to compute N! mod M so overflow is no issue;
 *      try M = 997 and large N to see how deep recursion behaves.
 *  5.3 Argument sequences of Program 5.2 (the 3N+1 puzzle) for N = 1..9.
 *  5.4 Find the N < 10^6 for which Program 5.2 makes the most calls.
 *  5.5 A nonrecursive Euclid.     5.7 Recursion depth of Euclid on
 *      consecutive Fibonacci numbers F_N, F_{N+1}.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 5.1-5.3, Exercises 5.1-5.7
 *  Build:   gcc -std=c99 -Wall -O2 ex5_01_recursion.c -o ex5_01 -lm
 * ============================================================ */
#include <stdio.h>
#include <math.h>

static double lgfact(int n) { return n <= 1 ? 0 : log2(n) + lgfact(n - 1); }    /* 5.1 */

static int depth, maxdepth;
static int factmod(int n, int m)                                              /* 5.2 */
{
    int r;
    if (++depth > maxdepth) maxdepth = depth;
    r = n == 0 ? 1 % m : (int)((long)n % m * factmod(n - 1, m) % m);
    depth--;
    return r;
}

static long calls;
static int puzzle(long n, int show)                                          /* Program 5.2 */
{
    calls++;
    if (show) printf(" %ld", n);
    if (n == 1) return 1;
    if (n % 2 == 0) return puzzle(n / 2, show);
    return puzzle(3 * n + 1, show);
}

static int gcd_iter(int u, int v) { while (v) { int t = u % v; u = v; v = t; } return u; }   /* 5.5 */
static int gdepth;
static int gcd_rec(int u, int v) { gdepth++; return v == 0 ? u : gcd_rec(v, u % v); }

int main(void)
{
    printf("5.1 lg(10!) = %.4f (log2(3628800) = %.4f), lg(100!) = %.1f\n", lgfact(10), log2(3628800.0), lgfact(100));

    for (int n = 1000; n <= 100000; n *= 10) {
        depth = maxdepth = 0;
        int r = factmod(n, 997);
        printf("5.2 %6d! mod 997 = %3d  (recursion depth %d)\n", n, r, maxdepth);
    }
    printf("    (N >= 997 contains the factor 997, so the answer is 0; 996! mod 997 = %d by Wilson's theorem = p-1)\n", factmod(996, 997));

    for (int n = 1; n <= 9; n++) { printf("5.3 N=%d:", n); calls = 0; puzzle(n, 1); printf("   (%ld calls)\n", calls); }

    long best = 1, bestc = 0;
    for (long n = 1; n < 1000000; n++) { calls = 0; puzzle(n, 0); if (calls > bestc) { bestc = calls; best = n; } }
    printf("5.4 N < 10^6 with the most calls: N = %ld, %ld calls\n", best, bestc);

    printf("5.5 gcd_iter(461952, 116298) = %d\n", gcd_iter(461952, 116298));
    long f[50] = { 0, 1 };
    for (int i = 2; i < 47; i++) f[i] = f[i - 1] + f[i - 2];
    printf("5.7 depth of gcd(F_{N+1}, F_N):");
    for (int n = 5; n <= 45; n += 10) { gdepth = 0; gcd_rec((int)f[n + 1], (int)f[n]); printf("  N=%d -> %d", n, gdepth); }
    printf("\n");
    return 0;
}
