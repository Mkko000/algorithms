/* ============================================================
 *  graph.c  —  Two graph representations: adjacency matrix & lists
 *  Chapter 3: Elementary Data Structures
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 3.18-3.19
 *  Build:   gcc -std=c99 -Wall -O2 graph.c -o graph
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

#define V 6

/* ---- adjacency matrix ---- */
int adj[V][V];
void matrix_insert(int u, int w) { adj[u][w] = adj[w][u] = 1; }

/* ---- adjacency lists ---- */
typedef struct node *link;
struct node { int v; link next; };
link lists[V];
void list_insert(int u, int w) {
    link a = malloc(sizeof *a); a->v = w; a->next = lists[u]; lists[u] = a;
    link b = malloc(sizeof *b); b->v = u; b->next = lists[w]; lists[w] = b;
}

int main(void) {
    int edges[][2] = {{0,1},{0,2},{0,5},{1,2},{2,3},{3,4},{4,5}};
    int m = sizeof(edges)/sizeof(edges[0]);
    for (int i = 0; i < m; i++) {
        matrix_insert(edges[i][0], edges[i][1]);
        list_insert(edges[i][0], edges[i][1]);
    }
    puts("Adjacency matrix:");
    for (int u = 0; u < V; u++) {
        printf("  %d:", u);
        for (int w = 0; w < V; w++) printf(" %d", adj[u][w]);
        putchar('\n');
    }
    puts("Adjacency lists:");
    for (int u = 0; u < V; u++) {
        printf("  %d:", u);
        for (link t = lists[u]; t; t = t->next) printf(" %d", t->v);
        putchar('\n');
    }
    return 0;
}
