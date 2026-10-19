/* ============================================================
 *  tst_advanced.c  —  Existence TSTs, partial-match search, and a
 *                     text-string index built on a TST
 *  Chapter 15: Radix Search (Sections 15.4, 15.5)
 *
 *  - Program 15.8: existence TST: each node has a character and THREE
 *    links (less, equal, greater). Only the "equal" link advances in the
 *    key. Space is about 3 links per character actually stored, not R
 *    per node (Property 15.7).
 *  - Program 15.9: partial-match search: a '.' in the query matches any
 *    character, so at a '.' we explore all three links. ("s.a" finds
 *    "sea"; "...." finds every 4-letter key.)
 *  - Prefix search (keys starting with p): walk to the end of p, then
 *    collect the subtree — what autocomplete does.
 *  - Section 15.5 (text string index): insert EVERY word position of a
 *    text as a key (the suffix starting there, cut at the word end), so
 *    "where does this word occur?" is one TST search.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 15.8-15.9, Section 15.5
 *  Build:   gcc -std=c99 -Wall -O2 tst_advanced.c -o tst_advanced
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node *link;
struct node { char c; int end; int pos[8]; int npos; link l, m, r; };
static link head; static long nodes;

static link NEW(char c) { link x = calloc(1, sizeof *x); x->c = c; nodes++; return x; }

/* Program 15.8 (existence TST), extended to remember positions */
static link insertR(link h, const char *s, int pos) {
    if (h == NULL) h = NEW(*s);
    if (*s < h->c) h->l = insertR(h->l, s, pos);
    else if (*s > h->c) h->r = insertR(h->r, s, pos);
    else if (s[1] != 0) h->m = insertR(h->m, s + 1, pos);
    else { h->end = 1; if (h->npos < 8 && pos >= 0) h->pos[h->npos++] = pos; }
    return h;
}
void STinsert(const char *s, int pos) { head = insertR(head, s, pos); }
static link find(link h, const char *s) {
    while (h) {
        if (*s < h->c) h = h->l;
        else if (*s > h->c) h = h->r;
        else if (s[1] == 0) return h;
        else { h = h->m; s++; }
    }
    return NULL;
}

/* Program 15.9: partial match, '.' = any character */
static char word[64];
static void matchR(link h, const char *s, int j) {
    if (h == NULL) return;
    if (*s == '.' || *s < h->c) matchR(h->l, s, j);
    if (*s == '.' || *s == h->c) {
        word[j] = h->c;
        if (s[1] == 0) { if (h->end) { word[j + 1] = 0; printf(" %s", word); } }
        else matchR(h->m, s + 1, j + 1);
    }
    if (*s == '.' || *s > h->c) matchR(h->r, s, j);
}
static void collect(link h, int j) {               /* all keys in subtree, in order */
    if (h == NULL) return;
    collect(h->l, j);
    word[j] = h->c;
    if (h->end) { word[j + 1] = 0; printf(" %s", word); }
    collect(h->m, j + 1);
    collect(h->r, j);
}
static void prefix(const char *p) {
    link h = find(head, p); int n = (int)strlen(p);
    memcpy(word, p, n);
    printf("prefix \"%s\":", p);
    if (h) { if (h->end) { word[n] = 0; printf(" %s", word); } collect(h->m, n); }
    putchar('\n');
}

int main(void) {
    const char *w[] = {"she", "sells", "sea", "shells", "by", "the", "sea", "shore",
                       "shell", "shelter", "seat", "sun", "sat"};
    for (int i = 0; i < 13; i++) STinsert(w[i], -1);
    printf("partial match \"s.a\":");  matchR(head, "s.a", 0);  putchar('\n');
    printf("partial match \"s..l.\":"); matchR(head, "s..l.", 0); putchar('\n');
    printf("partial match \"...\":");  matchR(head, "...", 0);  putchar('\n');
    prefix("shel");
    printf("TST nodes: %ld (3 links each) for %d keys\n", nodes, 12);

    /* Section 15.5: text-string index on word positions */
    head = NULL; nodes = 0;
    const char *text = "it was the best of times it was the worst of times it was the age of wisdom";
    char buf[32]; int n = (int)strlen(text);
    for (int i = 0; i < n; i++) {
        if (i > 0 && text[i - 1] != ' ') continue;
        int k = 0; while (text[i + k] && text[i + k] != ' ') { buf[k] = text[i + k]; k++; }
        buf[k] = 0; STinsert(buf, i);
    }
    const char *q[] = {"times", "worst", "was", "love"};
    for (int i = 0; i < 4; i++) {
        link h = find(head, q[i]);
        printf("index \"%s\":", q[i]);
        if (h && h->end) for (int j = 0; j < h->npos; j++) printf(" @%d", h->pos[j]); else printf(" not found");
        putchar('\n');
    }
    return 0;
}
