/* ============================================================
 *  skip_list.c  —  Skip lists: probabilistic balancing with linked lists
 *  Chapter 13: Balanced Trees (Section 13.5)
 *
 *  Definition 13.5: an ordered linked list where each node has a random
 *  number of links; level-i links skip over nodes with fewer than i
 *  links. A node gets j links with probability 1/t^j (here t = 2), so
 *  the expected number of levels is log_t N.
 *   - Program 13.7: search: start at the top level of the head, move
 *     right while the next key is smaller, else drop down a level.
 *   - Program 13.8: initialization (head node with lgNmax links).
 *   - Program 13.9: insertion with randX() choosing the node's level.
 *   - Program 13.10: deletion: unlink the node on every level it is on.
 *  Property 13.10: search and insertion take about (t log_t N)/2
 *  comparisons on the average (= lg N for t = 2); Property 13.11: about
 *  (t/(t-1)) N links in total (= 2N for t = 2). Same order as balanced
 *  trees, much simpler code.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 13.7-13.10
 *  Build:   gcc -std=c99 -Wall -O2 skip_list.c -o skip_list -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define lgNmax 20
typedef int Key;
typedef struct STnode *link;
struct STnode { Key key; link *next; int sz; };
static link head, z; static int lgN;
static long steps;

static link NEW(Key k, int sz) {
    link x = malloc(sizeof *x);
    x->next = malloc(sz * sizeof(link));
    x->key = k; x->sz = sz;
    for (int i = 0; i < sz; i++) x->next[i] = z;
    return x;
}
/* Program 13.8 */
void STinit(void) { z = NEW(0x7fffffff, 0); lgN = 0; head = NEW(0, lgNmax + 1); }

/* Program 13.7 */
static link searchR(link t, Key v, int k) {
    if (t == z) return z;
    if (v == t->key) return t;
    link x = t->next[k];
    steps++;
    if ((x == z) || (v < x->key)) { if (k == 0) return z; return searchR(t, v, k - 1); }
    return searchR(x, v, k);
}
link STsearch(Key v) { link t = searchR(head, v, lgN); return t; }

/* Program 13.9 */
static int randX(void) {            /* j links with probability 1/2^j */
    int i, j, t = rand();
    for (i = 1, j = 2; i < lgNmax; i++, j += j) if (t > RAND_MAX / j) break;
    if (i > lgN) lgN = i;
    return i;
}
static void insertR(link t, link x, int k) {
    Key v = x->key; link tk = t->next[k];
    if ((tk == z) || (v < tk->key)) {
        if (k < x->sz) { x->next[k] = tk; t->next[k] = x; }
        if (k == 0) return;
        insertR(t, x, k - 1); return;
    }
    insertR(tk, x, k);
}
void STinsert(Key v) { insertR(head, NEW(v, randX()), lgN); }

/* Program 13.10 (with one fix: free the node only if it really has key v) */
static void deleteR(link t, Key v, int k) {
    link x = t->next[k];
    if (!(x->key < v)) {                 /* z holds the largest key, so we always stop */
        if (v == x->key) t->next[k] = x->next[k];
        if (k == 0) { if (x != z && v == x->key) { free(x->next); free(x); } return; }
        deleteR(t, v, k - 1); return;
    }
    deleteR(x, v, k);
}
void STdelete(Key v) { deleteR(head, v, lgN); }

int main(void) {
    STinit(); srand(21);
    const int N = 1 << 15;
    int *keys = malloc(N * sizeof *keys);
    for (int i = 0; i < N; i++) keys[i] = i * 3;
    for (int i = N - 1; i > 0; i--) { int j = rand() % (i + 1), t = keys[i]; keys[i] = keys[j]; keys[j] = t; }
    for (int i = 0; i < N; i++) STinsert(keys[i]);
    int cnt[lgNmax + 1] = {0};
    for (link t = head->next[0]; t != z; t = t->next[0]) cnt[t->sz]++;
    printf("N=%d, levels used lgN=%d; nodes with j links:", N, lgN);
    for (int j = 1; j <= lgN; j++) printf(" %d:%d", j, cnt[j]);
    putchar('\n');
    steps = 0; int hits = 0;
    for (int i = 0; i < N; i++) if (STsearch(keys[i]) != z) hits++;
    printf("search hits %d/%d, avg %.1f links examined incl. level drops (lg N = %.0f)\n", hits, N, (double)steps / N, log2(N));
    long links = 0; for (link t = head->next[0]; t != z; t = t->next[0]) links += t->sz;
    printf("total links %ld = %.2f N  (Property 13.11: t/(t-1) = 2)\n", links, (double)links / N);
    printf("search 7 (not a multiple of 3): %s\n", STsearch(7) == z ? "miss" : "hit");
    for (int i = 0; i < N / 2; i++) STdelete(keys[i]);
    hits = 0; for (int i = 0; i < N; i++) if (STsearch(keys[i]) != z) hits++;
    printf("after deleting half: %d keys remain\n", hits);
    int prev = -1, ok = 1; for (link t = head->next[0]; t != z; t = t->next[0]) { if (t->key <= prev) ok = 0; prev = t->key; }
    printf("bottom level still sorted: %s\n", ok ? "yes" : "NO");
    free(keys);
    return 0;
}
