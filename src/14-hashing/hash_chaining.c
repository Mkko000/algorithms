/* ============================================================
 *  hash_chaining.c  —  Hash table with separate chaining
 *  Chapter 14: Hashing
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 14.1, 14.3
 *  Build:   gcc -std=c99 -Wall -O2 hash_chaining.c -o hash_chaining
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hash(const char *v, int M) {
    int h = 0, a = 127;
    for (; *v; v++) h = (a * h + (unsigned char)*v) % M;
    return h;
}

typedef struct node *link;
struct node { char *key; int val; link next; };
static link *heads;
static int N, M;

link NEW(const char *k, int v, link next) {
    link x = malloc(sizeof *x);
    x->key = malloc(strlen(k) + 1); strcpy(x->key, k);
    x->val = v; x->next = next;
    return x;
}
void STinit(int max) {
    N = 0; M = max / 5; if (M < 1) M = 1;
    heads = calloc(M, sizeof(link));
}
void STinsert(const char *k, int v) {
    int i = hash(k, M);
    heads[i] = NEW(k, v, heads[i]);   /* insert at front of list i */
    N++;
}
int STsearch(const char *k) {
    for (link t = heads[hash(k, M)]; t; t = t->next)
        if (strcmp(t->key, k) == 0) return t->val;
    return -1;
}

int main(void) {
    const char *words[] = {"search","hash","table","chain","insert","delete","key"};
    STinit(50);
    for (int i = 0; i < 7; i++) STinsert(words[i], i * 10);
    for (int i = 0; i < 7; i++)
        printf("%-8s -> bucket %d, value %d\n",
               words[i], hash(words[i], M), STsearch(words[i]));
    printf("miss \"foo\" -> %d\n", STsearch("foo"));
    printf("N=%d M=%d avg list length ~ %.1f\n", N, M, (double)N / M);
    return 0;
}
