/* ============================================================
 *  elementary_compare.c  —  Performance characteristics of the
 *                           elementary sorts, measured by counting
 *  Chapter 6: Elementary Sorting Methods (Sections 6.1-6.5)
 *
 *  Counts comparisons and exchanges/moves for selection, insertion
 *  (with sentinel, Program 6.3) and bubble sort on random, sorted,
 *  reverse-sorted and "few inversions" inputs, and prints the book's
 *  predictions next to them:
 *    Property 6.1 selection: ~N^2/2 compares, N-1 exchanges, ALWAYS
 *    Property 6.2 insertion: ~N^2/4 compares + ~N^2/4 moves on average
 *    Property 6.3 bubble:    ~N^2/2 compares and ~N^2/2 exchanges
 *    Property 6.4 insertion/bubble linear when few inversions
 *  Also shows stability (Definition 6.1) on records with equal keys.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 6.1-6.4
 *  Build:   gcc -std=c99 -Wall -O2 elementary_compare.c -o elementary_compare
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long C, X;                                   /* compares, exchanges/moves */
#define less(A, B) (C++, (A) < (B))
#define exch(A, B) { int t = A; A = B; B = t; X++; }
#define compexch(A, B) if (less(B, A)) exch(A, B)

void selection(int a[], int l, int r) {             /* Program 6.2 */
    for (int i = l; i < r; i++) {
        int min = i;
        for (int j = i + 1; j <= r; j++) if (less(a[j], a[min])) min = j;
        exch(a[i], a[min]);
    }
}
void insertion(int a[], int l, int r) {             /* Program 6.3 */
    int i;
    for (i = r; i > l; i--) compexch(a[i - 1], a[i]);   /* smallest to a[l]: sentinel */
    for (i = l + 2; i <= r; i++) {
        int j = i, v = a[i];
        while (less(v, a[j - 1])) { a[j] = a[j - 1]; j--; X++; }   /* half-exchanges */
        a[j] = v;
    }
}
void bubble(int a[], int l, int r) {                /* Program 6.4 */
    for (int i = l; i < r; i++)
        for (int j = r; j > i; j--) compexch(a[j - 1], a[j]);
}

static void fill(int *a, int n, int kind) {
    for (int i = 0; i < n; i++) a[i] = i;
    if (kind == 0) for (int i = n - 1; i > 0; i--) { int j = rand() % (i + 1), t = a[i]; a[i] = a[j]; a[j] = t; }
    if (kind == 2) for (int i = 0; i < n; i++) a[i] = n - 1 - i;
    if (kind == 3) for (int k = 0; k < 10; k++) { int i = rand() % (n - 1), t = a[i]; a[i] = a[i + 1]; a[i + 1] = t; }
}

/* stability demo: records sorted by key; does the original order of ties survive? */
typedef struct { int key; char tag; } rec;
static void ins_rec(rec a[], int n) { for (int i = 1; i < n; i++) { rec v = a[i]; int j = i; while (j > 0 && v.key < a[j-1].key) { a[j] = a[j-1]; j--; } a[j] = v; } }
static void sel_rec(rec a[], int n) { for (int i = 0; i < n - 1; i++) { int m = i; for (int j = i + 1; j < n; j++) if (a[j].key < a[m].key) m = j; rec t = a[i]; a[i] = a[m]; a[m] = t; } }

int main(void) {
    const int N = 1000;
    int *a = malloc(N * sizeof *a);
    const char *kinds[] = {"random", "sorted", "reverse", "10 swaps"};
    void (*sorts[])(int *, int, int) = {selection, insertion, bubble};
    const char *names[] = {"selection", "insertion", "bubble"};
    srand(5);
    printf("N=%d   (N^2/2=%d, N^2/4=%d)\n", N, N * N / 2, N * N / 4);
    printf("%-10s %-9s %10s %10s\n", "sort", "input", "compares", "exch/moves");
    for (int s = 0; s < 3; s++)
        for (int k = 0; k < 4; k++) {
            fill(a, N, k); C = X = 0; sorts[s](a, 0, N - 1);
            for (int i = 1; i < N; i++) if (a[i - 1] > a[i]) { printf("BUG\n"); return 1; }
            printf("%-10s %-9s %10ld %10ld\n", names[s], kinds[k], C, X);
        }
    rec r1[] = {{2,'a'},{1,'b'},{2,'c'},{1,'d'},{2,'e'}}, r2[5];
    memcpy(r2, r1, sizeof r1);
    ins_rec(r1, 5); sel_rec(r2, 5);
    printf("stability on keys 2a 1b 2c 1d 2e\n  insertion:"); for (int i = 0; i < 5; i++) printf(" %d%c", r1[i].key, r1[i].tag);
    printf("   (stable)\n  selection:"); for (int i = 0; i < 5; i++) printf(" %d%c", r2[i].key, r2[i].tag);
    printf("   (ties may be reordered: NOT stable)\n");
    free(a);
    return 0;
}
