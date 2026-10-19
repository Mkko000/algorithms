/* ============================================================
 *  radix_quicksort.c  —  Three-way radix quicksort on strings
 *  Chapter 10: Radix Sorting (Sections 10.4, 10.6)
 *
 *  Program 10.3: three-way partition on byte D (the "current
 *  character") instead of the whole key; recur on the < and > parts
 *  with the same byte, and on the = part with byte D+1. Strings are C
 *  strings, so byte value 0 means "end of key": if the partitioning
 *  byte is 0, the = part is a set of identical keys and is finished.
 *  Items are char * pointers; only pointers move.
 *
 *  Measured:
 *   - Property 10.5: about 2N ln N single-byte comparisons for random
 *     keys, compared with standard quicksort on whole keys (strcmp),
 *     whose cost per compare grows with the common prefix length.
 *   - Keys with long common prefixes (URLs) — where 3-way radix
 *     quicksort shines because each byte of the prefix is looked at a
 *     few times instead of once per comparison.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 10.3
 *  Build:   gcc -std=c99 -Wall -O2 radix_quicksort.c -o radix_quicksort -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef char *Item;
static long bytecmp, charcmp;
#define ch(A) (bytecmp++, (unsigned char)(A)[D])
#define exch(A, B) { Item t_ = A; A = B; B = t_; }
#define M 10

static void insertion(Item a[], int l, int r, int D) {
    for (int i = l + 1; i <= r; i++) {
        Item v = a[i]; int j = i;
        while (j > l) {
            const unsigned char *x = (const unsigned char *)v + D, *y = (const unsigned char *)a[j-1] + D;
            while (*x && *x == *y) { x++; y++; bytecmp++; }
            bytecmp++;
            if (*x >= *y) break;
            a[j] = a[j-1]; j--;
        }
        a[j] = v;
    }
}

/* Program 10.3 */
static void quicksortX(Item a[], int l, int r, int D) {
    int i, j, k, p, q, v;
    if (r - l <= M) { insertion(a, l, r, D); return; }
    v = ch(a[r]); i = l - 1; j = r; p = l - 1; q = r;
    while (i < j) {
        while (ch(a[++i]) < v) ;
        while (v < ch(a[--j])) if (j == l) break;
        if (i > j) break;
        exch(a[i], a[j]);
        if (ch(a[i]) == v) { p++; exch(a[p], a[i]); }
        if (v == ch(a[j])) { q--; exch(a[j], a[q]); }
    }
    if (p == q) {                        /* every key has byte v here */
        if (v != '\0') quicksortX(a, l, r, D + 1);
        return;
    }
    if (ch(a[i]) < v) i++;
    for (k = l; k <= p; k++, j--) exch(a[k], a[j]);
    for (k = r; k >= q; k--, i++) exch(a[k], a[i]);
    quicksortX(a, l, j, D);
    if ((i == r) && (ch(a[i]) == v)) i++;
    if (v != '\0') quicksortX(a, j + 1, i - 1, D + 1);
    quicksortX(a, i, r, D);
}

/* standard quicksort on whole keys, counting characters compared */
static int scmp(const char *x, const char *y) {
    while (*x && *x == *y) { x++; y++; charcmp++; }
    charcmp++;
    return (unsigned char)*x - (unsigned char)*y;
}
static void qs(Item a[], int l, int r) {
    if (r <= l) return;
    exch(a[(l + r) / 2], a[r]);
    Item v = a[r]; int i = l - 1, j = r;
    for (;;) {
        while (scmp(a[++i], v) < 0) ;
        while (scmp(v, a[--j]) < 0) if (j == l) break;
        if (i >= j) break;
        exch(a[i], a[j]);
    }
    exch(a[i], a[r]);
    qs(a, l, i - 1); qs(a, i + 1, r);
}

static void check(Item *a, int n) {
    for (int i = 1; i < n; i++) if (strcmp(a[i-1], a[i]) > 0) { printf("BUG at %d: %s > %s\n", i, a[i-1], a[i]); exit(1); }
}

static void experiment(const char *name, Item *src, int n) {
    Item *a = malloc(n * sizeof *a), *b = malloc(n * sizeof *b);
    memcpy(a, src, n * sizeof *a); memcpy(b, src, n * sizeof *b);
    bytecmp = 0; quicksortX(a, 0, n - 1, 0); check(a, n);
    charcmp = 0; qs(b, 0, n - 1); check(b, n);
    printf("  %-24s 3-way radix: %9ld byte compares | quicksort+strcmp: %9ld char compares\n",
           name, bytecmp, charcmp);
    free(a); free(b);
}

int main(void) {
    char *w[] = {"now","is","the","time","for","all","good","people","to","come",
                 "the","aid","of","their","party","now","is","the","time","for"};
    int n = sizeof w / sizeof w[0];
    quicksortX(w, 0, n - 1, 0);
    for (int i = 0; i < n; i++) printf("%s ", w[i]);
    printf("\n\n");

    int N = 100000;
    Item *src = malloc(N * sizeof *src);
    srand(2);
    for (int i = 0; i < N; i++) {                   /* random variable-length words */
        int len = 3 + rand() % 10; src[i] = malloc(len + 1);
        for (int k = 0; k < len; k++) src[i][k] = 'a' + rand() % 26;
        src[i][len] = 0;
    }
    printf("N=%d, 2N ln N = %.0f\n", N, 2 * N * log(N));
    experiment("random words", src, N);
    for (int i = 0; i < N; i++) {                   /* URLs: long shared prefix */
        free(src[i]); src[i] = malloc(48);
        sprintf(src[i], "https://example.com/users/%06d/profile", rand() % 1000000);
    }
    experiment("URLs (shared prefix)", src, N);
    for (int i = 0; i < N; i++) {                   /* few distinct keys */
        sprintf(src[i], "status-%d", rand() % 5);
    }
    experiment("5 distinct keys", src, N);
    for (int i = 0; i < N; i++) free(src[i]);
    free(src);
    return 0;
}
