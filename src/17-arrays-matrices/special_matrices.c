/* ============================================================
 *  special_matrices.c  —  Packing structured matrices into 1-D arrays
 *  Chapter 17: Arrays (lower/upper triangular, symmetric, tri-diagonal)
 *
 *  A lower-triangular n x n matrix has only n(n+1)/2 possibly non-zero
 *  cells; a tri-diagonal matrix has 3n-2. We store just those, with an
 *  index function that maps (i, j) to a slot in a 1-D array.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja Sec. 3.15 (lower/upper triangular, tri-diagonal);
 *                Lipschutz Ch. 4 (triangular and tri-diagonal arrays).
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 special_matrices.c -o special_matrices
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//>>index
/* 0-based (i, j).  Row i of a lower triangle holds i+1 cells, so the rows
 * before it hold 0+1+...+i = i(i+1)/2 cells.                                 */
static long lower_idx(int i, int j)  { return (long)i * (i + 1) / 2 + j; }    /* needs j <= i */

/* upper triangle, row by row: row r holds n-r cells; rows before i hold
 * n + (n-1) + ... + (n-i+1) = i*n - i(i-1)/2                                  */
static long upper_idx(int n, int i, int j) { return (long)i * n - (long)i * (i - 1) / 2 + (j - i); } /* j >= i */

/* tri-diagonal: only |i-j| <= 1. Row 0 has 2 cells (slots 0,1); every later row
 * has 3, so row i starts at slot 2 + 3(i-1) = 3i-1  =>  slot(i,j) = 3i-1 + (j-i+1) = 2i+j. */
static long tri_idx(int i, int j) { return 2L * i + j; }                      /* j in {i-1,i,i+1} */
//<<index

typedef struct { int n; int *a; } LowerM;

static LowerM lower_new(int n) { LowerM m = { n, calloc((size_t)n * (n + 1) / 2, sizeof(int)) }; return m; }
static int  lower_get(const LowerM *m, int i, int j) { return j > i ? 0 : m->a[lower_idx(i, j)]; }
static void lower_set(LowerM *m, int i, int j, int v) { assert(j <= i); m->a[lower_idx(i, j)] = v; }

//>>tri
typedef struct { int n; int *a; } TriM;                 /* 3n-2 cells */
static TriM tri_new(int n) { TriM m = { n, calloc((size_t)(3 * n - 2), sizeof(int)) }; return m; }
static int  tri_get(const TriM *m, int i, int j) { return (j < i - 1 || j > i + 1) ? 0 : m->a[tri_idx(i, j)]; }
static void tri_set(TriM *m, int i, int j, int v) { assert(j >= i - 1 && j <= i + 1); m->a[tri_idx(i, j)] = v; }
//<<tri

int main(void)
{
    int n = 5;

    /* ---- lower triangular: the indices must be a bijection onto 0..n(n+1)/2-1 ---- */
    int seen[15] = {0};
    for (int i = 0; i < n; i++) for (int j = 0; j <= i; j++) {
        long k = lower_idx(i, j); assert(k >= 0 && k < 15 && !seen[k]); seen[k] = 1; }
    puts("lower-triangular index map is a bijection onto 0..14");

    /* ---- upper triangular ---- */
    int seen2[15] = {0};
    for (int i = 0; i < n; i++) for (int j = i; j < n; j++) {
        long k = upper_idx(n, i, j); assert(k >= 0 && k < 15 && !seen2[k]); seen2[k] = 1; }
    puts("upper-triangular index map is a bijection onto 0..14");

    /* ---- multiply lower-triangular matrix by a vector using only packed cells ---- */
    LowerM L = lower_new(n);
    int dense[5][5] = {{0}};
    for (int i = 0; i < n; i++) for (int j = 0; j <= i; j++) { int v = (i + 1) * 10 + j; lower_set(&L, i, j, v); dense[i][j] = v; }
    int x[5] = {1, 2, 3, 4, 5};
    for (int i = 0; i < n; i++) {
        long s = 0, d = 0;
        for (int j = 0; j <= i; j++) s += (long)lower_get(&L, i, j) * x[j];     /* skip known zeros */
        for (int j = 0; j < n; j++) d += (long)dense[i][j] * x[j];
        assert(s == d);
        printf("row %d: %ld\n", i, s);
    }
    printf("memory: dense %d ints, packed lower %d ints\n", n * n, n * (n + 1) / 2);

    /* ---- symmetric matrix: store the lower triangle, mirror on read ---- */
    #define SYM(i, j) ((i) >= (j) ? lower_idx(i, j) : lower_idx(j, i))
    assert(SYM(1, 3) == SYM(3, 1));

    /* ---- tri-diagonal: bijection + comparison with the dense matrix ---- */
    int seen3[13] = {0};
    for (int i = 0; i < n; i++) for (int j = (i ? i - 1 : 0); j <= (i < n - 1 ? i + 1 : i); j++) {
        long k = tri_idx(i, j); assert(k >= 0 && k < 13 && !seen3[k]); seen3[k] = 1; }
    TriM T = tri_new(n);
    int Td[5][5] = {{0}};
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++)
        if (abs(i - j) <= 1) { Td[i][j] = 100 + 10 * i + j; tri_set(&T, i, j, Td[i][j]); }
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) assert(tri_get(&T, i, j) == Td[i][j]);
    printf("tri-diagonal: %d cells instead of %d (n = %d); slot of (3,2)=%ld, (3,3)=%ld, (3,4)=%ld\n",
           3 * n - 2, n * n, n, tri_idx(3, 2), tri_idx(3, 3), tri_idx(3, 4));
    free(L.a); free(T.a);
    puts("special_matrices: all tests passed");
    return 0;
}
