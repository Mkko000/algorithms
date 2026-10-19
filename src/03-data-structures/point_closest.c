/* ============================================================
 *  point_closest.c  —  A Point data type, dynamic arrays, and the
 *                      closest-point (count pairs within d) problem
 *  Chapter 3: Elementary Data Structures (Sections 3.1, 3.2, 3.7)
 *
 *  Shows the progression the book makes:
 *   - Program 3.3/3.4: a tiny "point" data type (struct + functions)
 *   - Program 3.6:     size the array at run time with malloc
 *   - Program 3.8:     brute force: check all N^2/2 pairs
 *   - Program 3.20:    a G x G grid of linked lists, so each point is
 *                      compared only with points in nearby cells.
 *  Same answer, wildly different cost for large N.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 3.3, 3.4, 3.6, 3.8, 3.20
 *  Build:   gcc -std=c99 -Wall -O2 point_closest.c -o point_closest -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ---- Program 3.3: interface ---- */
typedef struct { float x; float y; } point;
float distance(point a, point b);

/* ---- Program 3.4: implementation ---- */
float distance(point a, point b) {
    float dx = a.x - b.x, dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

static float randFloat(void) { return 1.0f * rand() / RAND_MAX; }

/* ---- Program 3.8: brute force, ~N^2/2 distance computations ---- */
long brute(point *a, int N, float d, long *work) {
    long cnt = 0; *work = 0;
    for (int i = 0; i < N; i++)
        for (int j = i + 1; j < N; j++) { (*work)++; if (distance(a[i], a[j]) < d) cnt++; }
    return cnt;
}

/* ---- Program 3.20: 2D array of lists (grid of side d) ---- */
typedef struct node *link;
struct node { point p; link next; };
static link **grid; static int G; static long gwork, gcnt;

static link **malloc2d(int r, int c) {               /* Program 3.16 idea */
    link **t = malloc(r * sizeof *t);
    for (int i = 0; i < r; i++) { t[i] = malloc(c * sizeof **t); for (int j = 0; j < c; j++) t[i][j] = NULL; }
    return t;
}
static void gridinsert(point p, float d) {
    int X = (int)(p.x * G), Y = (int)(p.y * G);
    if (X >= G) X = G - 1;
    if (Y >= G) Y = G - 1;
    X++; Y++;                                      /* shift past the border row/column */
    for (int i = X - 1; i <= X + 1; i++)
        for (int j = Y - 1; j <= Y + 1; j++)
            for (link s = grid[i][j]; s != NULL; s = s->next) {
                gwork++; if (distance(s->p, p) < d) gcnt++;
            }
    link t = malloc(sizeof *t); t->p = p; t->next = grid[X][Y]; grid[X][Y] = t;
}
long gridcount(point *a, int N, float d, long *work) {
    G = (int)(1.0f / d);
    grid = malloc2d(G + 2, G + 2);         /* border cells avoid range checks */
    gwork = gcnt = 0;
    for (int i = 0; i < N; i++) gridinsert(a[i], d);
    *work = gwork;
    for (int i = 0; i < G + 2; i++) {
        for (int j = 0; j < G + 2; j++) { link t = grid[i][j]; while (t) { link n = t->next; free(t); t = n; } }
        free(grid[i]);
    }
    free(grid);
    return gcnt;
}

int main(int argc, char *argv[]) {
    int N = argc > 1 ? atoi(argv[1]) : 5000;
    float d = argc > 2 ? (float)atof(argv[2]) : 0.01f;
    point *a = malloc(N * sizeof *a);              /* Program 3.6: size chosen at run time */
    if (a == NULL) { printf("Insufficient memory.\n"); return 1; }
    srand(1);
    for (int i = 0; i < N; i++) { a[i].x = randFloat(); a[i].y = randFloat(); }
    long w1, w2;
    long c1 = brute(a, N, d, &w1);
    long c2 = gridcount(a, N, d, &w2);
    printf("N=%d d=%.3f\n", N, d);
    printf("  brute force: %ld pairs closer than d, %ld distance computations\n", c1, w1);
    printf("  grid lists : %ld pairs closer than d, %ld distance computations\n", c2, w2);
    free(a);
    return c1 == c2 ? 0 : 1;
}
