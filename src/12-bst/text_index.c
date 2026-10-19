/* ============================================================
 *  text_index.c  —  Index implementations with symbol tables:
 *                   a string index into a text (suffix positions)
 *  Chapter 12: Symbol Tables and BSTs (Section 12.7)
 *
 *  Program 12.10: treat each position i of a text (here: each word
 *  start, to keep the demo small; the book allows every position) as a
 *  key "the string that starts at text+i", and insert the POINTER into a
 *  BST (comparing with strncmp-like logic). The items are just indices,
 *  the keys live in the text — exactly the "index items" idea of
 *  Section 4.7. Then "does pattern p occur?" is a BST search that
 *  compares p only with the first strlen(p) characters of each key.
 *  Building is ~N lg N string compares for random text (worse for
 *  repetitive text: long common prefixes, Exercise 12.67).
 *  The same index built with binary search on a SORTED array of
 *  positions is a (simple) suffix array.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 12.10
 *  Build:   gcc -std=c99 -Wall -O2 text_index.c -o text_index
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *text;
typedef struct STnode *link;
struct STnode { int pos; link l, r; };
static link head;

/* compare pattern p with the text starting at position i, only |p| chars */
static int cmp_prefix(const char *p, int i) { return strncmp(p, text + i, strlen(p)); }

static link insertR(link h, int pos) {
    if (h == NULL) { link x = malloc(sizeof *x); x->pos = pos; x->l = x->r = NULL; return x; }
    if (strcmp(text + pos, text + h->pos) < 0) h->l = insertR(h->l, pos);
    else                                       h->r = insertR(h->r, pos);
    return h;
}
static int searchR(link h, const char *p) {
    if (h == NULL) return -1;
    int c = cmp_prefix(p, h->pos);
    if (c == 0) return h->pos;
    return c < 0 ? searchR(h->l, p) : searchR(h->r, p);
}
static void freeT(link h) { if (!h) return; freeT(h->l); freeT(h->r); free(h); }

/* suffix-array version: sort positions, then binary search */
static int cmpsuf(const void *a, const void *b) { return strcmp(text + *(const int *)a, text + *(const int *)b); }
static int sa_search(int *sa, int n, const char *p) {
    int l = 0, r = n - 1;
    while (l <= r) { int m = (l + r) / 2, c = cmp_prefix(p, sa[m]); if (c == 0) return sa[m]; if (c < 0) r = m - 1; else l = m + 1; }
    return -1;
}

int main(void) {
    text = "call me ishmael some years ago never mind how long precisely having little "
           "or no money in my purse and nothing particular to interest me on shore";
    int N = (int)strlen(text);
    for (int i = 0; i < N; i++) if (i == 0 || text[i - 1] == ' ') head = insertR(head, i);  /* index word starts */
    int *sa = malloc(N * sizeof *sa); int n = 0;
    for (int i = 0; i < N; i++) if (i == 0 || text[i - 1] == ' ') sa[n++] = i;
    qsort(sa, n, sizeof *sa, cmpsuf);
    const char *q[] = {"money", "purse", "ishm", "whale", "long", "shore"};
    for (int k = 0; k < 6; k++)
        printf("%-6s -> BST: %4d   suffix array: %4d\n", q[k], searchR(head, q[k]), sa_search(sa, n, q[k]));
    freeT(head); free(sa);
    return 0;
}
