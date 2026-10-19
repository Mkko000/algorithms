/* ============================================================
 *  dfs_bfs.c  —  Graph traversal: depth-first and breadth-first search
 *  Chapter 5: Recursion and Trees (Section 5.8)
 *
 *  Uses the 8-vertex example graph of Section 5.8 (edges 0-2 0-5 0-7
 *  1-7 2-6 3-4 3-5 4-5 4-6 4-7) in adjacency-lists form (Program 3.19).
 *   - Program 5.21: recursive DFS. Counts list nodes touched to check
 *     Property 5.10 (time proportional to V + E).
 *   - Nonrecursive DFS with an explicit stack (push all neighbours),
 *     showing that duplicates pile up unless we forbid them.
 *   - Program 5.22: BFS with a FIFO queue, with and without an
 *     ignore-the-new-item policy (max queue size), plus the distance
 *     (number of edges) from the start, i.e. shortest paths.
 *   - Connectivity (Chapter 1's problem) answered by DFS: count the
 *     connected components after adding an isolated pair 8-9.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 5.21-5.22
 *  Build:   gcc -std=c99 -Wall -O2 dfs_bfs.c -o dfs_bfs
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

#define V 10
typedef struct node *link;
struct node { int v; link next; };
static link adj[V];
static int visited[V];
static long touched;

/* append at the end so each list keeps input order (as in the book's figures) */
static void addlist(int u, int w) {
    link a = malloc(sizeof *a), *p = &adj[u];
    a->v = w; a->next = NULL;
    while (*p) p = &(*p)->next;
    *p = a;
}
static void edge(int u, int w) { addlist(u, w); addlist(w, u); }

/* Program 5.21: recursive depth-first search */
static void dfs(int k) {
    visited[k] = 1;
    printf("%d ", k);
    for (link t = adj[k]; t; t = t->next) {
        touched++;
        if (!visited[t->v]) dfs(t->v);
    }
}

/* explicit stack: visit the top node (if unvisited), push all its neighbours */
static void dfs_stack(int k) {
    int st[4 * V * V], n = 0, maxn = 0;
    for (int i = 0; i < V; i++) visited[i] = 0;
    st[n++] = k;
    while (n > 0) {
        int v = st[--n];
        if (visited[v]) continue;
        visited[v] = 1; printf("%d ", v);
        /* push in reverse so the first neighbour is on top */
        int tmp[V], m = 0;
        for (link t = adj[v]; t; t = t->next) if (!visited[t->v]) tmp[m++] = t->v;
        while (m > 0) st[n++] = tmp[--m];
        if (n > maxn) maxn = n;
    }
    printf("  (max stack %d)", maxn);
}

/* Program 5.22: breadth-first search; ignore_new forbids duplicates */
static void bfs(int k, int ignore_new, int dist[]) {
    int q[4 * V * V], head = 0, tail = 0, maxq = 0, inq[V] = {0};
    for (int i = 0; i < V; i++) { visited[i] = 0; dist[i] = -1; }
    q[tail++] = k; inq[k] = 1; dist[k] = 0;
    while (head < tail) {
        int v = q[head++];
        if (visited[v]) continue;
        visited[v] = 1; printf("%d ", v);
        for (link t = adj[v]; t; t = t->next) {
            if (visited[t->v]) continue;
            if (ignore_new && inq[t->v]) continue;
            q[tail++] = t->v; inq[t->v] = 1;
            if (dist[t->v] < 0) dist[t->v] = dist[v] + 1;
        }
        if (tail - head > maxq) maxq = tail - head;
    }
    printf("  (max queue %d)", maxq);
}

int main(void) {
    int e[][2] = {{0,2},{0,5},{0,7},{1,7},{2,6},{3,4},{3,5},{4,5},{4,6},{4,7}};
    int E = sizeof e / sizeof e[0], dist[V];
    for (int i = 0; i < E; i++) edge(e[i][0], e[i][1]);

    printf("adjacency lists:\n");
    for (int v = 0; v < 8; v++) {
        printf("  %d:", v);
        for (link t = adj[v]; t; t = t->next) printf(" %d", t->v);
        putchar('\n');
    }

    printf("recursive DFS from 0     : "); dfs(0);
    printf("\n  list nodes touched = %ld = 2E (E=%d); plus V=8 heads -> V+E work\n", touched, E);
    printf("stack DFS (push all)     : "); dfs_stack(0); putchar('\n');
    printf("BFS, duplicates allowed  : "); bfs(0, 0, dist); putchar('\n');
    printf("BFS, ignore-the-new-item : "); bfs(0, 1, dist); putchar('\n');
    printf("edges from 0 (shortest)  :");
    for (int v = 0; v < 8; v++) printf(" %d:%d", v, dist[v]);
    putchar('\n');

    /* connectivity: add an isolated component 8-9 and count components */
    edge(8, 9);
    for (int i = 0; i < V; i++) visited[i] = 0;
    int comps = 0;
    printf("components:");
    for (int v = 0; v < V; v++)
        if (!visited[v]) { comps++; printf(" {"); dfs(v); printf("}"); }
    printf("  -> %d components\n", comps);
    return 0;
}
