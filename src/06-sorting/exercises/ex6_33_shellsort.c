/* ============================================================
 *  ex6_33_shellsort.c  —  Exercises 6.33, 6.35, 6.36, 6.39, 6.43
 *  Chapter 6: Elementary Sorting Methods — solved exercises
 *
 *  6.33 Is shellsort stable?
 *  6.35 Show shellsort (increments 13 4 1) on  E A S Y Q U E S T I O N.
 *  6.36 Running time of shellsort with increments 1 2 4 8 16 ... on the
 *       file with 1..N in the odd positions and N+1..2N in the even ones.
 *  6.39 Find the x that gives the fewest compares on random files of
 *       N = 10000 when 13 is replaced by x in 1 4 13 40 121 364 ...
 *  6.43 If h and k are relatively prime, every number >= (h-1)(k-1) is
 *       a nonnegative combination of h and k (checked exhaustively).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 6.6, Exercises 6.33-6.43
 *  Build:   gcc -std=c99 -Wall -O2 ex6_33_shellsort.c -o ex6_33
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int k, tag; } rec;
static long C;

/* h-sort for each increment in inc[] (largest first), Program 6.6 style */
static void shell(rec *a, int n, const int *inc, int ni, int show)
{
    for (int t = 0; t < ni; t++) {
        int h = inc[t];
        if (h >= n) continue;
        for (int i = h; i < n; i++) {
            rec v = a[i]; int j = i;
            while (j >= h && (C++, v.k < a[j - h].k)) { a[j] = a[j - h]; j -= h; }
            a[j] = v;
        }
        if (show) { printf("   %2d-sorted: ", h); for (int i = 0; i < n; i++) printf("%c%s ", a[i].k, a[i].k == 'E' || a[i].k == 'S' ? (a[i].tag < 6 ? "1" : "2") : ""); printf("\n"); }
    }
}

static unsigned long long rs = 4242;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }
static int gcd(int a, int b) { return b ? gcd(b, a % b) : a; }

int main(void)
{
    const char *E = "EASYQUESTION"; int n = 12; rec a[20000];
    for (int i = 0; i < n; i++) { a[i].k = E[i]; a[i].tag = i; }
    int inc1[] = { 13, 4, 1 };
    printf("6.35 shellsort 13 4 1 on EASYQUESTION (E1/E2, S1/S2 = first/second copy):\n");
    shell(a, n, inc1, 3, 1);
    int st = 1; for (int i = 1; i < n; i++) if (a[i].k == a[i - 1].k && a[i].tag < a[i - 1].tag) st = 0;
    printf("6.33 stable? %s (S2 ends before S1: the 4-sort jumped it over its twin)\n", st ? "yes" : "no");

    /* 6.36 */
    for (int N = 256; N <= 2048; N *= 2) {
        int M = 2 * N, pw[16], np = 0, kn[16], nk = 0;
        for (int h = 1; h < M; h *= 2) pw[np++] = h;
        for (int i = 0; i < np / 2; i++) { int t = pw[i]; pw[i] = pw[np - 1 - i]; pw[np - 1 - i] = t; }
        for (int h = 1; h < M / 3; h = 3 * h + 1) kn[nk++] = h;
        for (int i = 0; i < nk / 2; i++) { int t = kn[i]; kn[i] = kn[nk - 1 - i]; kn[nk - 1 - i] = t; }
        for (int i = 0; i < M; i++) a[i].k = (i % 2 == 0) ? i / 2 + 1 : N + i / 2 + 1;   /* positions 1,3,5.. = 1..N */
        C = 0; shell(a, M, pw, np, 0); long cp = C;
        for (int i = 0; i < M; i++) a[i].k = (i % 2 == 0) ? i / 2 + 1 : N + i / 2 + 1;
        C = 0; shell(a, M, kn, nk, 0);
        printf("6.36 2N=%5d: powers of 2 -> %8ld compares (N(N-1)/2 = %8ld),  1 4 13 40... -> %6ld\n", M, cp, (long)N * (N - 1) / 2, C);
    }

    /* 6.39 */
    {
        int N = 10000, base[] = { 9841, 3280, 1093, 364, 121, 40, 13, 4, 1 };
        static rec src[5][10000];
        for (int f = 0; f < 5; f++) for (int i = 0; i < N; i++) src[f][i].k = (int)rnd();
        long best = -1; int bx = 0;
        printf("6.39 x :");
        for (int x = 5; x <= 39; x++) {
            int inc[9]; memcpy(inc, base, sizeof inc); inc[6] = x;
            C = 0;
            for (int f = 0; f < 5; f++) { memcpy(a, src[f], N * sizeof(rec)); shell(a, N, inc, 9, 0); }
            if (x % 4 == 1 || x == 13) printf(" %d:%ld", x, C / 5);
            if (best < 0 || C < best) { best = C; bx = x; }
        }
        printf("\n     best x = %d with %ld compares per file (x = 13 is the standard)\n", bx, best / 5);
    }

    /* 6.43 */
    {
        int ok = 1, sharp = 1;
        for (int h = 2; h <= 40; h++) for (int k = h + 1; k <= 41; k++) {
            if (gcd(h, k) != 1) continue;
            int F = (h - 1) * (k - 1);
            for (int v = F - 1; v <= F + h * k; v++) {
                int rep = 0;
                for (int i = 0; i * h <= v; i++) if ((v - i * h) % k == 0) { rep = 1; break; }
                if (v >= F && !rep) ok = 0;
                if (v == F - 1 && rep) sharp = 0;
            }
        }
        printf("6.43 every v >= (h-1)(k-1) representable: %s; (h-1)(k-1)-1 never is: %s  (h<k<=41)\n", ok ? "yes" : "NO", sharp ? "yes" : "NO");
    }
    return 0;
}
