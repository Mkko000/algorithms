/* ============================================================
 *  dfs_bfs.c  —  Depth-first and breadth-first search on a graph
 *  Chapter 5: Recursion and Trees
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 5.21-5.22
 *  Build:   gcc -std=c99 -Wall -O2 dfs_bfs.c -o dfs_bfs
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

#define V 6
typedef struct node *link;
struct node { int v; link next; };
static link adj[V];
static int visited[V];

void edge(int u, int w) {
    link a = malloc(sizeof *a); a->v = w; a->next = adj[u]; adj[u] = a;
    link b = malloc(sizeof *b); b->v = u; b->next = adj[w]; adj[w] = b;
}

void dfs(int k) {
    visited[k] = 1;
    printf("%d ", k);
    for (link t = adj[k]; t; t = t->next)
        if (!visited[t->v]) dfs(t->v);
}

void bfs(int k) {
    int q[V], head = 0, tail = 0;
    for (int i = 0; i < V; i++) visited[i] = 0;
    q[tail++] = k; visited[k] = 1;
    while (head < tail) {
        int v = q[head++];
        printf("%d ", v);
        for (link t = adj[v]; t; t = t->next)
            if (!visited[t->v]) { visited[t->v] = 1; q[tail++] = t->v; }
    }
}

int main(void) {
    int e[][2] = {{0,2},{0,1},{1,4},{1,3},{2,5},{3,4},{4,5}};
    for (int i = 0; i < 7; i++) edge(e[i][0], e[i][1]);
    printf("DFS from 0: "); dfs(0); putchar('\n');
    printf("BFS from 0: "); bfs(0); putchar('\n');
    return 0;
}
