/* ============================================================
 *  ex10_35_lsd_msd.c  —  Exercises 10.35, 10.39, 10.41, 10.43
 *  Chapter 10: Radix Sorting — LSD, MSD and three-way radix quicksort
 *
 *  10.35 Show the result of LSD radix sort on the leading two characters
 *        of: now is the time for all good people to come the aid of their party
 *  10.39 A file holds 1000 copies of each of the numbers 1..1000 (32-bit
 *        words).  How do you exploit that for a fast radix sort?
 *  10.41 Total bytes examined by three-way radix quicksort on fixed-length
 *        bytestrings, worst case.
 *  10.43 Bytes examined by MSD radix sort and three-way radix quicksort for
 *        the N keys A, AA, AAA, AAAA, ...
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 10.2-10.4, Exercises 10.35-10.43
 *  Build:   gcc -std=c99 -Wall -O2 ex10_35_lsd_msd.c -o ex10_35
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef const char *Item;
static long bytes;                                   /* bytes examined */
static int digit(Item s, int d) { bytes++; return (unsigned char)s[d]; }   /* s[d] valid up to '\0' */
#define Rx 256

/* LSD on the first W characters (stable key-indexed counting per pass) */
static void lsd(Item a[], int n, int W)
{
    Item *aux = malloc(n * sizeof *aux);
    for (int d = W - 1; d >= 0; d--) {
        int cnt[Rx + 1] = { 0 };
        for (int i = 0; i < n; i++) { int c = (int)strlen(a[i]) > d ? (unsigned char)a[i][d] : 0; cnt[c + 1]++; }
        for (int j = 1; j <= Rx; j++) cnt[j] += cnt[j - 1];
        for (int i = 0; i < n; i++) { int c = (int)strlen(a[i]) > d ? (unsigned char)a[i][d] : 0; aux[cnt[c]++] = a[i]; }
        for (int i = 0; i < n; i++) a[i] = aux[i];
        printf("     pass on char %d:", d);
        for (int i = 0; i < n; i++) printf(" %s", a[i]);
        printf("\n");
    }
    free(aux);
}

/* Program 10.2: MSD radix sort for strings (no small-file cutoff) */
static Item *aux2;
static void msd(Item a[], int l, int r, int d)
{
    if (r <= l) return;
    int cnt[Rx + 1] = { 0 };
    for (int i = l; i <= r; i++) cnt[digit(a[i], d) + 1]++;
    for (int j = 1; j <= Rx; j++) cnt[j] += cnt[j - 1];
    for (int i = l; i <= r; i++) aux2[l + cnt[digit(a[i], d)]++] = a[i];
    for (int i = l; i <= r; i++) a[i] = aux2[i];
    /* bin j holds keys with digit j-1 .. recurse on every non-end bin */
    int start = l + cnt[0];
    for (int j = 1; j < Rx; j++) { int end = l + cnt[j] - 1; msd(a, start, end, d + 1); start = end + 1; }
}

/* Program 10.3: three-way radix quicksort (with the book's small-file
   cutoff to insertion sort, which also counts every byte it looks at) */
#define ex(A, B) { Item t = A; A = B; B = t; }
#define M 10
static void insertion(Item a[], int l, int r, int d)
{
    for (int i = l + 1; i <= r; i++) {
        Item v = a[i]; int j = i;
        while (j > l) {
            const unsigned char *x = (const unsigned char *)v + d, *y = (const unsigned char *)a[j - 1] + d;
            while (*x && *x == *y) { x++; y++; bytes += 2; }
            bytes += 2;
            if (*x >= *y) break;
            a[j] = a[j - 1]; j--;
        }
        a[j] = v;
    }
}
static void quick3(Item a[], int l, int r, int d)
{
    if (r - l <= M) { insertion(a, l, r, d); return; }
    int v = digit(a[r], d), i = l - 1, j = r, p = l - 1, q = r, k;
    while (i < j) {
        while (digit(a[++i], d) < v) ;
        while (v < digit(a[--j], d)) if (j == l) break;
        if (i > j) break;
        ex(a[i], a[j]);
        if (digit(a[i], d) == v) { p++; ex(a[p], a[i]); }
        if (v == digit(a[j], d)) { q--; ex(a[j], a[q]); }
    }
    if (p == q) { if (v != 0) quick3(a, l, r, d + 1); return; }
    if (digit(a[i], d) < v) i++;
    for (k = l; k <= p; k++, j--) ex(a[k], a[j]);
    for (k = r; k >= q; k--, i++) ex(a[k], a[i]);
    quick3(a, l, j, d);
    if ((i == r) && (digit(a[i], d) == v)) i++;
    if (v != 0) quick3(a, j + 1, i - 1, d + 1);
    quick3(a, i, r, d);
}
static int sorted(Item *a, int n) { for (int i = 1; i < n; i++) if (strcmp(a[i - 1], a[i]) > 0) return 0; return 1; }

int main(void)
{
    char text[] = "now is the time for all good people to come the aid of their party";
    Item w[32]; int n = 0;
    for (char *t = strtok(text, " "); t; t = strtok(NULL, " ")) w[n++] = t;
    printf("10.35 LSD radix sort on the leading two characters:\n");
    lsd(w, n, 2);

    /* 10.39: values 1..1000, 1000 copies each: ONE key-indexed counting pass with R = 1001 */
    {
        int N = 1000000; int *a = malloc(N * sizeof *a), *b = malloc(N * sizeof *b), *cnt = calloc(1002, sizeof *cnt);
        unsigned long long rs = 7; for (int i = 0; i < N; i++) a[i] = 1 + i % 1000;
        for (int i = N - 1; i > 0; i--) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; int j = (int)(rs % (unsigned long long)(i + 1)); int t = a[i]; a[i] = a[j]; a[j] = t; }
        long ops = 0;
        for (int i = 0; i < N; i++) { cnt[a[i] + 1]++; ops++; }
        for (int j = 1; j <= 1001; j++) cnt[j] += cnt[j - 1];
        for (int i = 0; i < N; i++) { b[cnt[a[i]]++] = a[i]; ops++; }
        int ok = 1; for (int i = 1; i < N; i++) if (b[i - 1] > b[i]) ok = 0;
        printf("10.39 1000 copies of 1..1000: one counting pass, %ld key accesses (2N), sorted %s\n", ops, ok ? "yes" : "NO");
        free(a); free(b); free(cnt);
    }

    /* 10.41: worst case = all keys equal, every byte of every key is examined */
    {
        int N = 1000, W = 16; char *buf = malloc(N * (W + 1)); Item *a = malloc(N * sizeof *a);
        for (int i = 0; i < N; i++) { memset(buf + i * (W + 1), 'x', W); buf[i * (W + 1) + W] = 0; a[i] = buf + i * (W + 1); }
        bytes = 0; quick3(a, 0, N - 1, 0);
        printf("10.41 N=%d equal keys of %d bytes: %ld bytes examined = %.2f x N(W+1)\n", N, W, bytes, (double)bytes / (N * (W + 1.0)));
        unsigned long long rs = 99;
        for (int i = 0; i < N; i++) for (int k = 0; k < W; k++) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; buf[i * (W + 1) + k] = 'a' + rs % 26; }
        for (int i = 0; i < N; i++) a[i] = buf + i * (W + 1);
        bytes = 0; quick3(a, 0, N - 1, 0);
        printf("      same size, random keys: %ld bytes examined (%.2f per key)\n", bytes, (double)bytes / N);
        free(buf); free(a);
    }

    /* 10.43 */
    printf("10.43 keys A, AA, AAA, ... (N keys):\n");
    for (int N = 10; N <= 1000; N *= 10) {
        char *buf = malloc((size_t)N * (N + 1)); Item *a = malloc(N * sizeof *a); aux2 = malloc(N * sizeof *aux2);
        for (int i = 0; i < N; i++) { char *s = buf + (size_t)i * (N + 1); memset(s, 'A', i + 1); s[i + 1] = 0; a[N - 1 - i] = s; }   /* reverse order */
        bytes = 0; msd(a, 0, N - 1, 0); long bm = bytes; int ok1 = sorted(a, N);
        for (int i = 0; i < N; i++) a[N - 1 - i] = buf + (size_t)i * (N + 1);
        bytes = 0; quick3(a, 0, N - 1, 0); long bq = bytes; int ok2 = sorted(a, N);
        printf("     N=%5d: MSD %9ld bytes (%s)   3-way radix quicksort %9ld bytes (%s)   N^2 = %ld\n",
               N, bm, ok1 ? "ok" : "BAD", bq, ok2 ? "ok" : "BAD", (long)N * N);
        free(buf); free(a); free(aux2);
    }
    return 0;
}
