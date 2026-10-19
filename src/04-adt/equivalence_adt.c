/* ============================================================
 *  equivalence_adt.c  —  Creating a NEW ADT: union-find as an
 *                        interface + implementation + client
 *  Chapter 4: Abstract Data Types (Section 4.5)
 *
 *  Program 4.6 is the interface (UFinit, UFfind, UFunion), Program 4.7
 *  is the client (the connectivity program from Chapter 1, now unaware
 *  of how the job is done), Program 4.8 implements it with weighted
 *  quick-union. Swapping in another implementation would not change a
 *  single line of the client: that separation is the whole lesson.
 *  In a real project the three parts live in UF.h, client.c, UF.c;
 *  here they are three marked sections of one file so it builds alone.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 4.6-4.8
 *  Build:   gcc -std=c99 -Wall -O2 equivalence_adt.c -o equivalence_adt
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

/* ===================== UF.h  (Program 4.6) ===================== */
void UFinit(int);
int  UFfind(int, int);
void UFunion(int, int);

/* ===================== UF.c  (Program 4.8) ===================== */
static int *id, *sz;
void UFinit(int N) {
    id = malloc(N * sizeof(int)); sz = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) { id[i] = i; sz[i] = 1; }
}
static int find(int x) { int i = x; while (i != id[i]) i = id[i]; return i; }
int UFfind(int p, int q) { return find(p) == find(q); }
void UFunion(int p, int q) {
    int i = find(p), j = find(q);
    if (i == j) return;
    if (sz[i] < sz[j]) { id[i] = j; sz[j] += sz[i]; }
    else               { id[j] = i; sz[i] += sz[j]; }
}

/* ===================== client.c  (Program 4.7) ================= */
int main(void) {
    int p, q, N = 10;
    int pairs[][2] = {{3,4},{4,9},{8,0},{2,3},{5,6},{2,9},{5,9},{7,3},{4,8},{5,6},{0,2},{6,1}};
    UFinit(N);
    for (int k = 0; k < (int)(sizeof pairs / sizeof pairs[0]); k++) {
        p = pairs[k][0]; q = pairs[k][1];
        if (!UFfind(p, q)) { UFunion(p, q); printf(" %d %d\n", p, q); }
    }
    free(id); free(sz);
    return 0;
}
