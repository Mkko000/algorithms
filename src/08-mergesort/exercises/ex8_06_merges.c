/* ============================================================
 *  ex8_06_merges.c  —  Exercises 8.6, 8.7, 8.8, 8.9, 8.24
 *  Chapter 8: Merging and Mergesort — solved exercises
 *
 *  8.6  Explain why Program 8.2 (bitonic abstract in-place merge) is not
 *       stable, and develop a version that is stable.
 *  8.7  What is the result when Program 8.2 is used for E A S Y Q U E S T I O N?
 *  8.8  Does Program 8.2 produce proper output if and only if the two input
 *       subarrays are in sorted order?  Prove it or give a counterexample.
 *  8.9  Show the merges that Program 8.3 (top-down) does for EASYQUESTION.
 *  8.24 Show the merges that bottom-up mergesort (Program 8.5) does.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 8.2, 8.3, 8.5, Exercises 8.6-8.24
 *  Build:   gcc -std=c99 -Wall -O2 ex8_06_merges.c -o ex8_06
 * ============================================================ */
#include <stdio.h>
#include <string.h>

typedef struct { char key; int tag; } Item;     /* tag = original position */
#define less(A, B) ((A).key < (B).key)
static Item aux[64];

static void merge_bitonic(Item a[], int l, int m, int r)     /* Program 8.2 */
{
    int i, j, k;
    for (i = m + 1; i > l; i--) aux[i - 1] = a[i - 1];
    for (j = m; j < r; j++) aux[r + m - j] = a[j + 1];
    for (k = l; k <= r; k++)
        if (less(aux[j], aux[i])) a[k] = aux[j--]; else a[k] = aux[i++];
}
static void merge_stable(Item a[], int l, int m, int r)      /* 8.6 fix */
{
    int i = l, j = m + 1, k;
    for (k = l; k <= r; k++) aux[k] = a[k];
    for (k = l; k <= r; k++)
        if (i > m) a[k] = aux[j++];
        else if (j > r) a[k] = aux[i++];
        else if (less(aux[j], aux[i])) a[k] = aux[j++];
        else a[k] = aux[i++];
}

static void show(const char *lab, Item *a, int l, int r, int n)
{
    printf("   %-12s", lab);
    for (int k = 0; k < n; k++) printf("%c", (k >= l && k <= r) ? a[k].key : ' ');
    printf("\n");
}
static void load(Item *a, const char *s) { for (int i = 0; s[i]; i++) { a[i].key = s[i]; a[i].tag = i; } }

static int depth;
static void mergesort_td(Item a[], int l, int r, int n)                  /* Program 8.3 */
{
    if (r <= l) return;
    int m = (l + r) / 2;
    mergesort_td(a, l, m, n); mergesort_td(a, m + 1, r, n);
    merge_stable(a, l, m, r);
    char lab[48]; snprintf(lab, sizeof lab, "%d-by-%d", m - l + 1, r - m);
    show(lab, a, l, r, n);
}
static void mergesort_bu(Item a[], int l, int r, int n)                  /* Program 8.5 */
{
    for (int m = 1; m <= r - l; m = m + m) {
        for (int i = l; i <= r - m; i += m + m) {
            int hi = i + m + m - 1 < r ? i + m + m - 1 : r;
            merge_stable(a, i, i + m - 1, hi);
            char lab[48]; snprintf(lab, sizeof lab, "%d-by-%d", m, hi - (i + m - 1));
            show(lab, a, i, hi, n);
        }
    }
}

static int is_sorted(Item *a, int n) { for (int i = 1; i < n; i++) if (less(a[i], a[i - 1])) return 0; return 1; }
static int is_stable(Item *a, int n) { for (int i = 1; i < n; i++) if (a[i].key == a[i - 1].key && a[i].tag < a[i - 1].tag) return 0; return 1; }

int main(void)
{
    Item a[64];
    /* 8.6 */
    printf("8.6 Program 8.2 on left = A, right = B B (tags 0 | 1 2):\n");
    load(a, "ABB"); merge_bitonic(a, 0, 0, 2);
    printf("    bitonic: "); for (int i = 0; i < 3; i++) printf("%c%d ", a[i].key, a[i].tag); printf("  stable? %s\n", is_stable(a, 3) ? "yes" : "NO");
    load(a, "ABB"); merge_stable(a, 0, 0, 2);
    printf("    fixed  : "); for (int i = 0; i < 3; i++) printf("%c%d ", a[i].key, a[i].tag); printf("  stable? %s\n", is_stable(a, 3) ? "yes" : "NO");
    {   /* how often is the bitonic merge unstable?  all files of 6 keys from {A,B,C}, both halves sorted */
        int bad = 0, tot = 0;
        for (int code = 0; code < 729; code++) {
            char s[7]; int c = code; for (int i = 0; i < 6; i++) { s[i] = 'A' + c % 3; c /= 3; } s[6] = 0;
            if (s[0] > s[1] || s[1] > s[2] || s[3] > s[4] || s[4] > s[5]) continue;
            tot++; load(a, s); merge_bitonic(a, 0, 2, 5); if (!is_stable(a, 6)) bad++;
        }
        printf("    3+3 sorted halves over {A,B,C}: %d of %d merges are unstable with Program 8.2\n", bad, tot);
    }
    /* 8.7 */
    load(a, "EASYQUESTION"); merge_bitonic(a, 0, 5, 11);
    printf("8.7 Program 8.2 on EASYQUESTION (halves EASYQU | ESTION, NOT sorted): ");
    for (int i = 0; i < 12; i++) printf("%c", a[i].key);
    printf("\n");
    /* 8.8: search all permutations of ABCDEF for unsorted halves that still merge correctly */
    {
        int found = 0, p[6] = { 0, 1, 2, 3, 4, 5 };
        printf("8.8 permutations of ABCDEF with an UNSORTED half that Program 8.2 still sorts:\n");
        for (;;) {
            char s[7]; for (int i = 0; i < 6; i++) s[i] = 'A' + p[i]; s[6] = 0;
            int halves = s[0] < s[1] && s[1] < s[2] && s[3] < s[4] && s[4] < s[5];
            load(a, s); merge_bitonic(a, 0, 2, 5);
            if (!halves && is_sorted(a, 6)) { if (found < 5) printf("    %.3s | %.3s\n", s, s + 3); found++; }
            int i = 4; while (i >= 0 && p[i] > p[i + 1]) i--;      /* next permutation */
            if (i < 0) break;
            int j = 5; while (p[j] < p[i]) j--;
            int t = p[i]; p[i] = p[j]; p[j] = t;
            for (int x = i + 1, y = 5; x < y; x++, y--) { t = p[x]; p[x] = p[y]; p[y] = t; }
        }
        printf("    ... %d such files (vs 20 with both halves sorted): the answer is NO.\n", found);
    }
    printf("8.9 top-down mergesort (Program 8.3) on EASYQUESTION:\n");
    load(a, "EASYQUESTION"); mergesort_td(a, 0, 11, 12);
    printf("8.24 bottom-up mergesort (Program 8.5) on EASYQUESTION:\n");
    load(a, "EASYQUESTION"); mergesort_bu(a, 0, 11, 12);
    printf("    sorted %s, stable %s\n", is_sorted(a, 12) ? "yes" : "NO", is_stable(a, 12) ? "yes" : "NO");
    (void)depth;
    return 0;
}
