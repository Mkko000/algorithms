/* ============================================================
 *  ex6_57_index_kic.c  —  Exercises 6.57-6.61, 6.72-6.74
 *  Chapter 6: Elementary Sorting Methods — solved exercises
 *
 *  6.58 Give the index array that results when the keys
 *       E A S Y Q U E S T I O N are index sorted.
 *  6.59 Give the data moves that permute them in place afterwards
 *       (Program 6.14, in-situ permutation).
 *  6.60 A permutation maximizing the number of times a[i] != i there.
 *  6.72 Key-indexed counting specialized to three values a, b, c.
 *  6.73 Insertion sort on a random file with only three distinct
 *       values: linear, quadratic, or in between?  (measured)
 *  6.74 Show how key-indexed counting sorts  A B R A C A D A B R A.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 6.14, 6.17, Exercises 6.57-6.74
 *  Build:   gcc -std=c99 -Wall -O2 ex6_57_index_kic.c -o ex6_57
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void insitu(char *data, int *a, int N, int show)   /* Program 6.14 */
{
    int i, j, k, moves = 0;
    for (i = 0; i < N; i++) {
        char v = data[i];
        for (k = i; a[k] != i; k = j) {          /* follow the cycle through i */
            j = a[k]; data[k] = data[j]; a[k] = k;
            if (show) printf("   data[%d] = data[%d]  (%c)\n", k, j, data[k]);
            moves++;
        }
        data[k] = v; a[k] = k;
        if (show && k != i) { printf("   data[%d] = %c (the saved key)\n", k, v); moves++; }
    }
    if (show) printf("   %d moves, result %.*s\n", moves, N, data);
}

static unsigned long long rs = 31337;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }

int main(void)
{
    char d[] = "EASYQUESTION"; int N = 12, a[12];
    for (int i = 0; i < N; i++) a[i] = i;
    for (int i = 1; i < N; i++) {                      /* insertion sort on the index (stable) */
        int v = a[i], j = i;
        while (j > 0 && d[v] < d[a[j - 1]]) { a[j] = a[j - 1]; j--; }
        a[j] = v;
    }
    printf("6.58 keys : "); for (int i = 0; i < N; i++) printf("%2c ", d[i]); printf("\n     index: ");
    for (int i = 0; i < N; i++) printf("%2d ", a[i]);
    printf("\n     sorted: ");
    for (int i = 0; i < N; i++) printf("%c", d[a[i]]);
    printf("\n");
    printf("6.59 in-place permutation (Program 6.14):\n");
    insitu(d, a, N, 1);

    /* 6.60: one long cycle (a[i] = i+1 mod N) keeps a[i] != i for the longest */
    {
        int M = 10, b[10]; char x[11] = "BCDEFGHIJA"; long tests = 0;
        for (int i = 0; i < M; i++) b[i] = (i + M - 1) % M;  /* sorted order needs x[(i-1) mod M] at i */
        for (int i = 0; i < M; i++) { int k = i; char v = x[i]; while ((tests++, b[k] != i)) { int j = b[k]; x[k] = x[j]; b[k] = k; k = j; } x[k] = v; b[k] = k; }
        printf("6.60 one cycle of length N=10: %ld tests of a[k] != i, %d of them true; result %s\n", tests, M - 1, x);
    }

    /* 6.72: three-value counting, one pass to count, one to write */
    {
        char s[] = "cabbacabcacbbacab"; int cnt[3] = { 0 };
        for (char *p = s; *p; p++) cnt[*p - 'a']++;
        int k = 0;
        for (int v = 0; v < 3; v++) for (int c = 0; c < cnt[v]; c++) s[k++] = (char)('a' + v);
        printf("6.72 counts a=%d b=%d c=%d -> %s\n", cnt[0], cnt[1], cnt[2], s);
    }

    /* 6.73: insertion sort on 3-valued random files */
    for (int n = 1000; n <= 16000; n *= 2) {
        int *x = malloc(n * sizeof *x); long c = 0;
        for (int i = 0; i < n; i++) x[i] = rnd() % 3;
        for (int i = 1; i < n; i++) { int v = x[i], j = i; while (j > 0 && (c++, v < x[j - 1])) { x[j] = x[j - 1]; j--; } x[j] = v; }
        printf("6.73 N=%5d: %9ld compares  (N^2/6 = %9.0f)\n", n, c, (double)n * n / 6);
        free(x);
    }

    /* 6.74: key-indexed counting (Program 6.17) on ABRACADABRA, M = 5 keys */
    {
        const char *s = "ABRACADABRA"; int n = 11, M = 5; int cnt[6] = { 0 }; char b[12];
        const char keys[] = "ABCDR";
        int key[11];
        for (int i = 0; i < n; i++) key[i] = (int)(strchr(keys, s[i]) - keys);
        for (int i = 0; i < n; i++) cnt[key[i] + 1]++;
        printf("6.74 counts   :"); for (int j = 0; j <= M; j++) printf(" %d", cnt[j]); printf("   (cnt[j+1] = how many of key j)\n");
        for (int j = 1; j <= M; j++) cnt[j] += cnt[j - 1];
        printf("     cumulative:"); for (int j = 0; j <= M; j++) printf(" %d", cnt[j]); printf("   (cnt[j] = first slot for key j)\n");
        for (int i = 0; i < n; i++) b[cnt[key[i]]++] = s[i];
        b[n] = 0; printf("     result    : %s\n", b);
    }
    return 0;
}
