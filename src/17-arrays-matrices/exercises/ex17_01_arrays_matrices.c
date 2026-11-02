/* ============================================================
 *  ex17_01_arrays_matrices.c  —  Solved exercises, Chapter 17 (Arrays)
 *
 *  Thareja ch.3 review questions 8, 9, 13, 14 and programming exercises 1-25.
 *  Each solution is a small function plus an assert-based check in main().
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Ch. 3 Exercises
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 ex17_01_arrays_matrices.c -o ex17_01
 * ============================================================ */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define N 4

//>>RQ14
/* ---- RQ 14: names array, insert "Andrew", delete "Esha" ---- */
static int moves_insert(int n, int pos) { return n - pos; }
static int moves_delete(int n, int pos) { return n - pos - 1; }

//<<RQ14
//>>PE1
/* ---- PE 1: marks[20][5] statistics ---- */
static void marks_stats(int m[][5], int rows, double subj_avg[5], double stud_avg[], int *below50)
{
    *below50 = 0;
    for (int j = 0; j < 5; j++) { double s = 0; for (int i = 0; i < rows; i++) s += m[i][j]; subj_avg[j] = s / rows; }
    for (int i = 0; i < rows; i++) {
        double s = 0; for (int j = 0; j < 5; j++) s += m[i][j];
        stud_avg[i] = s / 5; if (stud_avg[i] < 50) (*below50)++;
    }
}

//<<PE1
//>>PE2
/* ---- PE 2: pairs whose sum is 50, sort + two pointers O(n log n) ---- */
static int cmp_int(const void *a, const void *b) { return (*(const int *)a > *(const int *)b) - (*(const int *)a < *(const int *)b); }
static int pairs_sum(int a[], int n, int target, int out[][2])
{
    qsort(a, n, sizeof(int), cmp_int);
    int i = 0, j = n - 1, c = 0;
    while (i < j) {
        int s = a[i] + a[j];
        if (s == target) { out[c][0] = a[i]; out[c][1] = a[j]; c++; i++; j--; }
        else if (s < target) i++; else j--;
    }
    return c;
}

//<<PE2
//>>PE3
/* ---- PE 3: swap 2nd with 2nd-last; PE 4: sum of squares ---- */
static void swap_second(int a[], int n) { int t = a[1]; a[1] = a[n - 2]; a[n - 2] = t; }
static long sum_squares(const int a[], int n) { long s = 0; for (int i = 0; i < n; i++) s += (long)a[i] * a[i]; return s; }

//<<PE3
//>>PE5
/* ---- PE 5-6-7-8-9-10: 2-D array tasks (pointers where asked) ---- */
static void sum_mean(int a[][N], int r, int c, long *sum, double *mean)
{ *sum = 0; for (int i = 0; i < r; i++) for (int j = 0; j < c; j++) *sum += a[i][j]; *mean = (double)*sum / (r * c); }
static long main_diag_ptr(int (*a)[N], int n) { long s = 0; for (int i = 0; i < n; i++) s += *(*(a + i) + i); return s; }
static void add_ptr(int (*a)[3], int (*b)[3], int (*c)[3])
{ for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) *(*(c + i) + j) = *(*(a + i) + j) + *(*(b + i) + j); }
static long prod_above_diag(int a[][N], int n) { long p = 1; for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) p *= a[i][j]; return p; }
static int count_nonzero(int a[][N], int r, int c) { int k = 0; for (int i = 0; i < r; i++) for (int j = 0; j < c; j++) k += a[i][j] != 0; return k; }

//<<PE5
//>>PE11
/* ---- PE 11: split odd / even ---- */
static void split_odd_even(int a[][N], int r, int c, int odd[], int *no, int even[], int *ne)
{ *no = *ne = 0; for (int i = 0; i < r; i++) for (int j = 0; j < c; j++) { if (a[i][j] & 1) odd[(*no)++] = a[i][j]; else even[(*ne)++] = a[i][j]; } }

//<<PE11
//>>PE12
/* ---- PE 12: merge two float arrays, print reversed ---- */
static int merge_rev(const float a[], int n, const float b[], int m, float out[])
{ int k = 0; for (int i = 0; i < n; i++) out[k++] = a[i]; for (int j = 0; j < m; j++) out[k++] = b[j];
  for (int i = 0, j = k - 1; i < j; i++, j--) { float t = out[i]; out[i] = out[j]; out[j] = t; } return k; }

//<<PE12
//>>PE13
/* ---- PE 13: swap 2nd biggest and 2nd smallest using pointers ---- */
static void swap_second_big_small(int *a, int n)
{
    int *max1 = a, *max2 = NULL, *min1 = a, *min2 = NULL;
    for (int *p = a + 1; p < a + n; p++) {            /* distinct values assumed */
        if (*p > *max1) { max2 = max1; max1 = p; } else if (!max2 || *p > *max2) max2 = p;
        if (*p < *min1) { min2 = min1; min1 = p; } else if (!min2 || *p < *min2) min2 = p;
    }
    int t = *max2; *max2 = *min2; *min2 = t;
}

//<<PE13
//>>PE14
/* ---- PE 14: p x q x r arrays: sum, transpose (swap the last two subscripts), "product" ----
 * Product of 3-D arrays is not standard; we multiply each p-slice as a q x r by r x q? Here we
 * take the batch product of slice k of A (q x r) with slice k of B^T (r x q).           */
#define P3 2
#define Q3 2
#define R3 3
static void add3(int a[P3][Q3][R3], int b[P3][Q3][R3], int c[P3][Q3][R3])
{ for (int i = 0; i < P3; i++) for (int j = 0; j < Q3; j++) for (int k = 0; k < R3; k++) c[i][j][k] = a[i][j][k] + b[i][j][k]; }
static void transpose3(int a[P3][Q3][R3], int t[P3][R3][Q3])
{ for (int i = 0; i < P3; i++) for (int j = 0; j < Q3; j++) for (int k = 0; k < R3; k++) t[i][k][j] = a[i][j][k]; }
static void batch_product(int a[P3][Q3][R3], int b[P3][Q3][R3], int c[P3][Q3][Q3])
{
    int bt[P3][R3][Q3]; transpose3(b, bt);
    for (int s = 0; s < P3; s++) for (int i = 0; i < Q3; i++) for (int j = 0; j < Q3; j++) {
        int v = 0; for (int k = 0; k < R3; k++) v += a[s][i][k] * bt[s][k][j]; c[s][i][j] = v; }
}

//<<PE14
//>>PE15
/* ---- PE 15-17: diagonal, above, below sums ---- */
static long diag_sum(int a[][N], int n)  { long s = 0; for (int i = 0; i < n; i++) s += a[i][i]; return s; }
static long above_sum(int a[][N], int n) { long s = 0; for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) s += a[i][j]; return s; }
static long below_sum(int a[][N], int n) { long s = 0; for (int i = 0; i < n; i++) for (int j = 0; j < i; j++) s += a[i][j]; return s; }

//<<PE15
//>>PE18
/* ---- PE 18-20: triangular and symmetric tests ---- */
static int isUpperTriangular(int a[][N], int n) { for (int i = 0; i < n; i++) for (int j = 0; j < i; j++) if (a[i][j]) return 0; return 1; }
static int isLowerTriangular(int a[][N], int n) { for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) if (a[i][j]) return 0; return 1; }
static int isSymmetric(int a[][N], int n)       { for (int i = 0; i < n; i++) for (int j = 0; j < i; j++) if (a[i][j] != a[j][i]) return 0; return 1; }

//<<PE18
//>>PE21
/* ---- PE 21: XA + YB ---- */
static void lincomb(int x, int a[][N], int y, int b[][N], int c[][N])
{ for (int i = 0; i < N; i++) for (int j = 0; j < N; j++) c[i][j] = x * a[i][j] + y * b[i][j]; }

//<<PE21
//>>PE22
/* ---- PE 22: pointer to a 2D array ---- */
static void ptr_to_2d(void)
{
    int a[2][3] = {{1, 2, 3}, {4, 5, 6}};
    int (*p)[3] = a;                                  /* pointer to an array of 3 ints */
    assert(p[1][2] == 6 && *(*(p + 1) + 2) == 6 && (*p)[1] == 2);
    assert((char *)(p + 1) - (char *)p == 3 * (long)sizeof(int));   /* p+1 skips a whole ROW */
}

//<<PE22
//>>PE23
/* ---- PE 23: digits of a number ---- */
static int digits(long x, int d[])
{
    if (x < 0) x = -x;
    int tmp[20], k = 0;
    do { tmp[k++] = (int)(x % 10); x /= 10; } while (x);
    for (int i = 0; i < k; i++) d[i] = tmp[k - 1 - i];
    return k;
}

//<<PE23
//>>PE24
/* ---- PE 24: delete ALL duplicates (keep first occurrence), unsorted input, O(n^2) ---- */
static int dedup(int a[], int n)
{
    int w = 0;
    for (int r = 0; r < n; r++) {
        int dup = 0;
        for (int k = 0; k < w; k++) if (a[k] == a[r]) { dup = 1; break; }
        if (!dup) a[w++] = a[r];
    }
    return w;
}

//<<PE24
//>>PE25
/* ---- PE 25: insert in a float array at a position ---- */
static int finsert(float a[], int n, int cap, int pos, float v)
{ if (n >= cap || pos < 0 || pos > n) return -1; memmove(a + pos + 1, a + pos, (n - pos) * sizeof a[0]); a[pos] = v; return n + 1; }

//<<PE25
int main(void)
{
    /* RQ 8, 9, 13 */
    assert(1000 + 2 * 35 == 1070);                            /* arr[35]                */
    assert(2000 + 2 * (8 * 5 + 5) == 2090);                   /* Marks[8][5], row-major (index 5 is out of bounds for 5 cols) */
    assert(1000 + 2 * (5 * 10 + 8) == 1116);                  /* arr[8][5] column-major */
    printf("RQ8=1070  RQ9=2090  RQ13=1116\n");
    /* RQ 14 */
    int m_in = moves_insert(7, 1), m_del = moves_delete(7, 3);
    printf("RQ14: insert Andrew moves %d names, delete Esha moves %d names\n", m_in, m_del);
    assert(m_in == 6 && m_del == 3);

    /* PE 1 */
    int marks[4][5] = {{60,70,80,90,100},{10,20,30,40,50},{50,50,50,50,50},{90,40,45,30,20}};
    double sa[5], st[4]; int b50;
    marks_stats(marks, 4, sa, st, &b50);
    printf("PE1: subject avgs %.1f %.1f %.1f %.1f %.1f; below-50 students = %d\n", sa[0], sa[1], sa[2], sa[3], sa[4], b50);
    assert(sa[0] == 52.5 && st[0] == 80 && st[1] == 30 && b50 == 2);

    /* PE 2 */
    int a2[] = {10, 40, 25, 30, 20, 15, 35, 0, 50}, pr[10][2];
    int np = pairs_sum(a2, 9, 50, pr);
    printf("PE2: %d pairs sum to 50:", np); for (int i = 0; i < np; i++) printf(" (%d,%d)", pr[i][0], pr[i][1]); puts("");
    assert(np == 4);               /* (0,50) (10,40) (15,35) (20,30); 25 has no partner */

    /* PE 3, 4 */
    int a3[] = {1, 2, 3, 4, 5}; swap_second(a3, 5); assert(a3[1] == 4 && a3[3] == 2);
    assert(sum_squares(a3, 5) == 1 + 16 + 9 + 4 + 25);

    /* PE 5-10 */
    int g[N][N] = {{1,2,3,4},{0,5,6,7},{0,0,8,9},{0,0,0,10}};
    long s; double mean; sum_mean(g, N, N, &s, &mean);
    assert(s == 55 && mean == 55.0 / 16);
    assert(main_diag_ptr(g, N) == 24);
    int A3[3][3] = {{1,2,3},{4,5,6},{7,8,9}}, B3[3][3] = {{9,8,7},{6,5,4},{3,2,1}}, C3[3][3];
    add_ptr(A3, B3, C3); for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) assert(C3[i][j] == 10);
    assert(prod_above_diag(g, N) == 2L * 3 * 4 * 6 * 7 * 9);
    assert(count_nonzero(g, N, N) == 10);

    /* PE 11 */
    int od[16], ev[16], no, ne; split_odd_even(g, N, N, od, &no, ev, &ne);
    assert(no == 5 && ne == 11);               /* odd: 1,3,5,7,9 ; even: ten cells that are 0 or even */
    /* PE 12 */
    float fa[] = {1.5f, 2.5f}, fb[] = {3.5f, 4.5f, 5.5f}, fo[8];
    int k = merge_rev(fa, 2, fb, 3, fo); assert(k == 5 && fo[0] == 5.5f && fo[4] == 1.5f);
    /* PE 13 */
    int a13[] = {50, 10, 40, 20, 30};          /* 2nd biggest 40, 2nd smallest 20 */
    swap_second_big_small(a13, 5);
    assert(a13[2] == 20 && a13[3] == 40);
    /* PE 14 */
    int X[P3][Q3][R3] = {{{1,2,3},{4,5,6}},{{1,0,1},{0,1,0}}}, Y[P3][Q3][R3] = {{{1,1,1},{2,2,2}},{{3,3,3},{4,4,4}}};
    int Z[P3][Q3][R3], T[P3][R3][Q3], Pm[P3][Q3][Q3];
    add3(X, Y, Z); assert(Z[0][1][2] == 8);
    transpose3(X, T); assert(T[0][2][1] == 6);
    batch_product(X, Y, Pm); assert(Pm[0][0][0] == 6 && Pm[0][1][1] == 30 && Pm[1][0][1] == 8);
    /* PE 15-17 */
    assert(diag_sum(g, N) == 24 && above_sum(g, N) == 2+3+4+6+7+9 && below_sum(g, N) == 0);
    /* PE 18-20 */
    assert(isUpperTriangular(g, N) && !isLowerTriangular(g, N) && !isSymmetric(g, N));
    int sy[N][N] = {{1,2,3,4},{2,5,6,7},{3,6,8,9},{4,7,9,10}};
    assert(isSymmetric(sy, N));
    /* PE 21 */
    int c21[N][N]; lincomb(2, g, 3, sy, c21); assert(c21[0][0] == 2 + 3 && c21[1][0] == 0 + 6);
    /* PE 22-25 */
    ptr_to_2d();
    int d[20], nd = digits(90817, d); assert(nd == 5 && d[0] == 9 && d[4] == 7);
    int a24[] = {4, 2, 4, 7, 2, 9, 7, 7}; int u = dedup(a24, 8);
    assert(u == 4 && a24[0] == 4 && a24[1] == 2 && a24[2] == 7 && a24[3] == 9);
    float f25[8] = {1.1f, 2.2f, 4.4f}; int nf = finsert(f25, 3, 8, 2, 3.3f);
    assert(nf == 4 && f25[2] == 3.3f && f25[3] == 4.4f);
    puts("ex17_01: all 25 programming exercises + review questions verified");
    return 0;
}
