/* ============================================================
 *  multiway_trie.c  —  R-way existence tries for string keys
 *  Chapter 15: Radix Search (Section 15.4)
 *
 *  Program 15.7: each node has R links, one per possible character;
 *  the key is spelled by the PATH, and an end-of-string link marks
 *  existence (Definition 15.2/15.3). Search cost = key length, no matter
 *  how many keys (Property 15.6: about log_R N chars examined for a
 *  random miss). The catch is space: R links per node, mostly NULL —
 *  about R N / ln R links (Property 15.6). We count links for R = 27
 *  (lowercase letters + end marker) to make that concrete. TSTs (tst.c,
 *  tst_advanced.c) fix exactly this waste.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 15.7
 *  Build:   gcc -std=c99 -Wall -O2 multiway_trie.c -o multiway_trie
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

#define R 27                       /* index 0 = end of string, 1..26 = 'a'..'z' */
typedef struct STnode *link;
struct STnode { link next[R]; };
static link head; static long nodes;

static int digit(const char *s, int d) { return s[d] ? s[d] - 'a' + 1 : 0; }
static link NEW(void) { nodes++; return calloc(1, sizeof(struct STnode)); }
void STinit(void) { head = NULL; nodes = 0; }

/* Program 15.7 */
static int searchR(link h, const char *v, int w, int *chars) {
    int i = digit(v, w);
    if (h == NULL) return 0;
    (*chars)++;
    if (i == 0) return h->next[0] != NULL;       /* end-of-string link marks existence */
    return searchR(h->next[i], v, w + 1, chars);
}
int STsearch(const char *v, int *chars) { *chars = 0; return searchR(head, v, 0, chars); }
static link insertR(link h, const char *v, int w) {
    int i = digit(v, w);
    if (h == NULL) h = NEW();
    if (i == 0) { h->next[0] = h; return h; }     /* any non-NULL marker */
    h->next[i] = insertR(h->next[i], v, w + 1);
    return h;
}
void STinsert(const char *v) { head = insertR(head, v, 0); }

static void show(link h, char *buf, int d) {       /* all keys, in ORDER, for free */
    if (h == NULL) return;
    if (h->next[0]) { buf[d] = 0; printf(" %s", buf); }
    for (int i = 1; i < R; i++) if (h->next[i]) { buf[d] = (char)('a' + i - 1); show(h->next[i], buf, d + 1); }
}

int main(void) {
    const char *w[] = {"now", "is", "the", "time", "for", "all", "good", "people",
                       "to", "come", "the", "aid", "of", "their", "party", "tim"};
    STinit();
    for (int i = 0; i < 16; i++) STinsert(w[i]);
    char buf[64]; printf("keys in order:"); show(head, buf, 0); putchar('\n');
    const char *q[] = {"time", "tim", "ti", "theirs", "party"};
    for (int i = 0; i < 5; i++) { int c; int f = STsearch(q[i], &c); printf("  %-7s %s after %d nodes\n", q[i], f ? "hit " : "miss", c); }
    printf("15 distinct keys -> %ld nodes x %d links = %ld links (mostly NULL!)\n", nodes, R, nodes * R);
    return 0;
}
