/* ============================================================
 *  patricia.c  —  Patricia tries: tries without one-way branching
 *  Chapter 15: Radix Search (Section 15.3)
 *
 *  A plain trie wastes nodes on long one-way paths when keys share
 *  prefixes. Patricia (Morrison, 1968) keeps exactly N nodes for N keys:
 *  each node stores a key AND the index of the bit to test; links that
 *  point UP the tree (to a node with a smaller-or-equal bit index) are
 *  the "external nodes" — when we follow one we compare the full key
 *  once. Program 15.4 search, Program 15.5 insert (find the first bit
 *  where the new key differs from the key we ended at, then splice a
 *  node testing that bit into the right place), Program 15.6 sort
 *  (inorder traversal following only DOWN links: keys come out sorted).
 *  Property 15.5: ~lg N bit inspections and exactly ONE full-key compare
 *  per search.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 15.4-15.6
 *  Build:   gcc -std=c99 -Wall -O2 patricia.c -o patricia
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

#define bitsword 32
typedef unsigned Key;
#define digit(A, B) (((A) >> (bitsword - 1 - (B))) & 1u)

typedef struct STnode *link;
struct STnode { Key key; link l, r; int bit; };
static link head;
static long bitchecks;

void STinit(void) {
    head = malloc(sizeof *head);
    head->key = 0xFFFFFFFFu; head->bit = -1;      /* dummy: never a real key here */
    head->l = head->r = head;
}

/* Program 15.4 */
static link searchR(link h, Key v, int w) {
    if (h->bit <= w) return h;                   /* an UP link: stop */
    bitchecks++;
    if (digit(v, h->bit) == 0) return searchR(h->l, v, h->bit);
    else                       return searchR(h->r, v, h->bit);
}
int STsearch(Key v) { link t = searchR(head->l, v, -1); return t->key == v; }

/* Program 15.5 */
static link insertR(link h, Key v, int w, link p) {
    if ((h->bit >= w) || (h->bit <= p->bit)) {
        link x = malloc(sizeof *x);
        x->key = v; x->bit = w;
        x->l = digit(v, x->bit) ? h : x;
        x->r = digit(v, x->bit) ? x : h;
        return x;
    }
    if (digit(v, h->bit) == 0) h->l = insertR(h->l, v, w, h);
    else                       h->r = insertR(h->r, v, w, h);
    return h;
}
void STinsert(Key v) {
    link t = searchR(head->l, v, -1);
    if (v == t->key) return;                     /* already present */
    int i = 0;
    while (digit(v, i) == digit(t->key, i)) i++; /* first differing bit */
    head->l = insertR(head->l, v, i, head);
}

/* Program 15.6: sort = inorder, treating up-links as external */
static void sortR(link h, void (*visit)(Key), int w) {
    if (h->bit <= w) { if (h != head) visit(h->key); return; }   /* skip the dummy head */
    sortR(h->l, visit, h->bit);
    sortR(h->r, visit, h->bit);
}
static void pr(Key k) { printf(" %u", k); }

int main(void) {
    STinit();
    Key demo[] = {19, 1, 18, 5, 25, 3, 9, 14, 7, 22};
    for (int i = 0; i < 10; i++) STinsert(demo[i]);
    printf("patricia sort:"); sortR(head->l, pr, -1); putchar('\n');
    printf("search 25 -> %s, 26 -> %s\n", STsearch(25) ? "hit" : "miss", STsearch(26) ? "hit" : "miss");

    STinit(); srand(15);
    const int N = 1 << 16; Key *k = malloc(N * sizeof *k);
    for (int i = 0; i < N; i++) { k[i] = ((Key)rand() << 1) ^ (Key)rand(); if (k[i] == 0xFFFFFFFFu) k[i]--; STinsert(k[i]); }
    bitchecks = 0; int hits = 0;
    for (int i = 0; i < N; i++) hits += STsearch(k[i]);
    printf("N=%d random keys: %d hits, avg %.1f bit inspections per search (lg N = 16), 1 key compare each\n",
           N, hits, (double)bitchecks / N);
    free(k);
    return 0;
}
