/* ============================================================
 *  sparse_matrix.c  —  Sparse matrices in triplet (row, col, value) form
 *  Chapter 17: Arrays (sparse matrices)
 *
 *  A matrix with mostly zeros wastes memory if stored densely. We keep only
 *  the non-zero terms, sorted by (row, col):  t[0] = {rows, cols, count}
 *  and t[1..count] = the terms.  Operations: transpose (two ways), add, multiply.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja Sec. 3.15; Bandyopadhyay & Dey Ch. 3;
 *                Pal & Halder Ch. 3 (sparse matrix representation).
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 sparse_matrix.c -o sparse_matrix
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAXT 512
typedef struct { int row, col, val; } Term;
typedef struct { int rows, cols, n; Term t[MAXT]; } Sparse;   /* n = #nonzero */

//>>from_dense
void sp_from_dense(Sparse *s, int rows, int cols, const int *d)
{
    s->rows = rows; s->cols = cols; s->n = 0;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (d[i * cols + j] != 0) {
                assert(s->n < MAXT);
                s->t[s->n++] = (Term){ i, j, d[i * cols + j] };
            }
}
//<<from_dense

void sp_to_dense(const Sparse *s, int *d)
{
    memset(d, 0, sizeof(int) * s->rows * s->cols);
    for (int k = 0; k < s->n; k++) d[s->t[k].row * s->cols + s->t[k].col] = s->t[k].val;
}

//>>transpose_slow
/* Simple transpose: for each column c of A (in order), scan ALL terms.
 * Cost O(cols * n). When n ~ rows*cols this is O(rows*cols^2): worse than dense! */
void sp_transpose_slow(const Sparse *a, Sparse *b)
{
    b->rows = a->cols; b->cols = a->rows; b->n = a->n;
    int k = 0;
    for (int c = 0; c < a->cols; c++)
        for (int p = 0; p < a->n; p++)
            if (a->t[p].col == c) b->t[k++] = (Term){ c, a->t[p].row, a->t[p].val };
}
//<<transpose_slow

//>>transpose_fast
/* Fast transpose: first COUNT the terms in each column, turn the counts into
 * starting positions (prefix sums), then place every term once.
 * Cost O(cols + n)  — the same trick as key-indexed counting. */
void sp_transpose_fast(const Sparse *a, Sparse *b)
{
    int cnt[MAXT] = {0}, pos[MAXT] = {0};
    b->rows = a->cols; b->cols = a->rows; b->n = a->n;
    for (int p = 0; p < a->n; p++) cnt[a->t[p].col]++;
    for (int c = 1; c < a->cols; c++) pos[c] = pos[c - 1] + cnt[c - 1];
    for (int p = 0; p < a->n; p++) {
        int q = pos[a->t[p].col]++;
        b->t[q] = (Term){ a->t[p].col, a->t[p].row, a->t[p].val };
    }
}
//<<transpose_fast

//>>add
/* C = A + B : merge two sorted term lists (like merging two sorted arrays). */
int sp_add(const Sparse *a, const Sparse *b, Sparse *c)
{
    if (a->rows != b->rows || a->cols != b->cols) return 0;
    c->rows = a->rows; c->cols = a->cols; c->n = 0;
    int i = 0, j = 0;
    while (i < a->n || j < b->n) {
        long ka = i < a->n ? (long)a->t[i].row * a->cols + a->t[i].col : -1;
        long kb = j < b->n ? (long)b->t[j].row * b->cols + b->t[j].col : -1;
        if (j >= b->n || (i < a->n && ka < kb)) c->t[c->n++] = a->t[i++];
        else if (i >= a->n || kb < ka)          c->t[c->n++] = b->t[j++];
        else {                                  /* same cell */
            int v = a->t[i].val + b->t[j].val;
            if (v) c->t[c->n++] = (Term){ a->t[i].row, a->t[i].col, v };
            i++; j++;
        }
    }
    return 1;
}
//<<add

//>>multiply
/* C = A * B using B^T: every row of A is dotted with every row of B^T,
 * walking two sorted lists (like merge). Cost O(rowsA * n_B + colsB * n_A ...). */
int sp_mul(const Sparse *a, const Sparse *b, Sparse *c)
{
    if (a->cols != b->rows) return 0;
    Sparse bt; sp_transpose_fast(b, &bt);
    c->rows = a->rows; c->cols = b->cols; c->n = 0;
    int ra = 0;
    while (ra < a->n) {
        int row = a->t[ra].row, rend = ra;
        while (rend < a->n && a->t[rend].row == row) rend++;      /* terms [ra,rend) */
        int cb = 0;
        while (cb < bt.n) {
            int col = bt.t[cb].row, cend = cb;                    /* row of B^T = col of B */
            while (cend < bt.n && bt.t[cend].row == col) cend++;
            int i = ra, j = cb, sum = 0;
            while (i < rend && j < cend) {
                if (a->t[i].col < bt.t[j].col) i++;
                else if (a->t[i].col > bt.t[j].col) j++;
                else sum += a->t[i++].val * bt.t[j++].val;
            }
            if (sum) { assert(c->n < MAXT); c->t[c->n++] = (Term){ row, col, sum }; }
            cb = cend;
        }
        ra = rend;
    }
    return 1;
}
//<<multiply

static void sp_print(const char *tag, const Sparse *s)
{
    printf("%s  (%d x %d, %d non-zero)\n  row:", tag, s->rows, s->cols, s->n);
    for (int k = 0; k < s->n; k++) printf(" %2d", s->t[k].row);
    printf("\n  col:");
    for (int k = 0; k < s->n; k++) printf(" %2d", s->t[k].col);
    printf("\n  val:");
    for (int k = 0; k < s->n; k++) printf(" %2d", s->t[k].val);
    printf("\n");
}

static void dense_mul(int n, int m, int p, const int *a, const int *b, int *c)
{
    for (int i = 0; i < n; i++) for (int j = 0; j < p; j++) {
        int s = 0; for (int k = 0; k < m; k++) s += a[i*m+k] * b[k*p+j]; c[i*p+j] = s; }
}

int main(void)
{
    int D[5 * 6] = {
        15,  0,  0, 22,  0, -15,
         0, 11,  3,  0,  0,   0,
         0,  0,  0, -6,  0,   0,
         0,  0,  0,  0,  0,   0,
        91,  0,  0,  0,  0,   0 };
    static Sparse A, T1, T2, B, C;
    sp_from_dense(&A, 5, 6, D);
    sp_print("A", &A);
    printf("dense cells: %d,  triplet cells: %d (3 per term + header)\n", 5 * 6, 3 * (A.n + 1));

    sp_transpose_slow(&A, &T1);
    sp_transpose_fast(&A, &T2);
    sp_print("A^T (fast)", &T2);
    assert(T1.n == T2.n && memcmp(T1.t, T2.t, sizeof(Term) * T1.n) == 0);

    int dt[6 * 5], d1[6 * 5];
    sp_to_dense(&T2, dt);
    for (int i = 0; i < 5; i++) for (int j = 0; j < 6; j++) d1[j * 5 + i] = D[i * 6 + j];
    assert(memcmp(dt, d1, sizeof dt) == 0);

    /* random tests: add and multiply versus the dense computation */
    srand(7);
    for (int round = 0; round < 200; round++) {
        int n = 1 + rand() % 7, m = 1 + rand() % 7, p = 1 + rand() % 7;
        int x[49] = {0}, y[49] = {0}, z[49] = {0}, w[49], u[49];
        for (int i = 0; i < n*m; i++) if (rand() % 3 == 0) x[i] = rand() % 9 - 4;
        for (int i = 0; i < n*m; i++) if (rand() % 3 == 0) z[i] = rand() % 9 - 4;
        for (int i = 0; i < m*p; i++) if (rand() % 3 == 0) y[i] = rand() % 9 - 4;
        static Sparse sx, sy, sz, sr;
        sp_from_dense(&sx, n, m, x); sp_from_dense(&sy, m, p, y); sp_from_dense(&sz, n, m, z);
        assert(sp_add(&sx, &sz, &sr));
        sp_to_dense(&sr, w);
        for (int i = 0; i < n*m; i++) assert(w[i] == x[i] + z[i]);
        assert(sp_mul(&sx, &sy, &sr));
        sp_to_dense(&sr, w); dense_mul(n, m, p, x, y, u);
        assert(memcmp(w, u, sizeof(int) * n * p) == 0);
    }
    puts("200 random add / multiply cases agree with the dense computation");

    int E[6 * 2] = { 1,0, 0,2, 0,0, 3,0, 0,0, 0,-1 };
    sp_from_dense(&B, 6, 2, E);
    assert(sp_mul(&A, &B, &C));
    sp_print("A * B", &C);
    int dd[5 * 2]; dense_mul(5, 6, 2, D, E, dd);
    int cd[5 * 2]; sp_to_dense(&C, cd);
    assert(memcmp(dd, cd, sizeof dd) == 0);
    puts("sparse_matrix: all tests passed");
    return 0;
}
