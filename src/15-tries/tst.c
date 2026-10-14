/* ============================================================
 *  tst.c  —  Ternary Search Trie for string keys
 *  Chapter 15: Radix Search
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 15.8 / 15.10 (simplified)
 *  Build:   gcc -std=c99 -Wall -O2 tst.c -o tst
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node *link;
struct node { char c; int val; link l, m, r; };
static link head;

link NEW(char c) {
    link x = calloc(1, sizeof *x); x->c = c; x->val = -1; return x;
}

link insertR(link h, const char *s, int val, int d) {
    char ch = s[d];
    if (!h) h = NEW(ch);
    if      (ch < h->c) h->l = insertR(h->l, s, val, d);
    else if (ch > h->c) h->r = insertR(h->r, s, val, d);
    else if (d < (int)strlen(s) - 1) h->m = insertR(h->m, s, val, d + 1);
    else h->val = val;
    return h;
}
void STinsert(const char *s, int val) { head = insertR(head, s, val, 0); }

int searchR(link h, const char *s, int d) {
    if (!h) return -1;
    char ch = s[d];
    if      (ch < h->c) return searchR(h->l, s, d);
    else if (ch > h->c) return searchR(h->r, s, d);
    else if (d < (int)strlen(s) - 1) return searchR(h->m, s, d + 1);
    else return h->val;
}
int STsearch(const char *s) { return searchR(head, s, 0); }

int main(void) {
    const char *words[] = {"she","sells","sea","shells","by","the","shore"};
    for (int i = 0; i < 7; i++) STinsert(words[i], i);
    for (int i = 0; i < 7; i++)
        printf("%-8s -> %d\n", words[i], STsearch(words[i]));
    printf("shell    -> %d (miss expected -1)\n", STsearch("shell"));
    return 0;
}
