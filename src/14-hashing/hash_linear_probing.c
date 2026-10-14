/* ============================================================
 *  hash_linear_probing.c  —  Open-addressing hash with linear probing
 *  Chapter 14: Hashing
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 14.4
 *  Build:   gcc -std=c99 -Wall -O2 hash_linear_probing.c -o hash_linear_probing
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hash(const char *v, int M) {
    int h = 0, a = 127;
    for (; *v; v++) h = (a * h + (unsigned char)*v) % M;
    return h;
}

static char **st;
static int N, M;

void STinit(int max) {
    N = 0; M = 2 * max;
    st = calloc(M, sizeof(char *));
}
void STinsert(const char *k) {
    int i = hash(k, M);
    while (st[i] != NULL) i = (i + 1) % M;
    st[i] = malloc(strlen(k) + 1); strcpy(st[i], k);
    N++;
}
int STsearch(const char *k) {
    int i = hash(k, M);
    while (st[i] != NULL) {
        if (strcmp(st[i], k) == 0) return i;
        i = (i + 1) % M;
    }
    return -1;
}

int main(void) {
    const char *words[] = {"A","S","E","R","C","H","I","N","G","X","M","P","L"};
    STinit(16);
    for (int i = 0; i < 13; i++) STinsert(words[i]);
    printf("table (M=%d):\n", M);
    for (int i = 0; i < M; i++)
        printf("  [%2d] %s\n", i, st[i] ? st[i] : ".");
    printf("search H -> slot %d\n", STsearch("H"));
    printf("search Z -> %d (miss)\n", STsearch("Z"));
    return 0;
}
