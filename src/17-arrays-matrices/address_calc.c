/* ============================================================
 *  address_calc.c  —  How a compiler finds A[i][j]: row-major / column-major
 *  Chapter 17: Arrays (address calculation, n-dimensional arrays)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja, Sec. 3.3.1 & 3.13; Bandyopadhyay & Dey, Sec. 3.6;
 *                Pal & Halder, Ch. 3 (address calculation); Lipschutz Ch. 4.
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 address_calc.c -o address_calc
 * ============================================================ */
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

//>>formulas
/* 1-D:  Addr(A[i]) = Base + w * (i - LB)                                   */
long addr1(long base, long w, long lb, long i) { return base + w * (i - lb); }

/* 2-D with arbitrary lower bounds, R rows x C columns:
 *   row-major    Addr(A[i][j]) = Base + w * ((i - LBr) * C + (j - LBc))
 *   column-major Addr(A[i][j]) = Base + w * ((j - LBc) * R + (i - LBr))    */
long addr2_row(long base, long w, long lbr, long lbc, long R, long C, long i, long j)
{ (void)R; return base + w * ((i - lbr) * C + (j - lbc)); }

long addr2_col(long base, long w, long lbr, long lbc, long R, long C, long i, long j)
{ (void)C; return base + w * ((j - lbc) * R + (i - lbr)); }

/* n-D row-major: the LAST subscript varies fastest. Horner-style: */
long addrN_row(long base, long w, int n, const long lb[], const long len[], const long idx[])
{
    long off = 0;
    for (int d = 0; d < n; d++) off = off * len[d] + (idx[d] - lb[d]);
    return base + w * off;
}

/* n-D column-major: the FIRST subscript varies fastest. */
long addrN_col(long base, long w, int n, const long lb[], const long len[], const long idx[])
{
    long off = 0;
    for (int d = n - 1; d >= 0; d--) off = off * len[d] + (idx[d] - lb[d]);
    return base + w * off;
}
//<<formulas

int main(void)
{
    /* --- the textbook questions (Thareja review questions 8, 9, 13) --- */
    long q8  = addr1(1000, 2, 0, 35);                         /* int arr[50]   */
    long q9  = addr2_row(2000, 2, 0, 0, 10, 5, 8, 5);         /* Marks[10][5]  */
    long q13 = addr2_col(1000, 2, 0, 0, 10, 10, 8, 5);        /* arr[10][10]   */
    printf("Q8  arr[35] (base 1000, w 2)              = %ld\n", q8);
    printf("Q9  Marks[8][5] row-major (base 2000)     = %ld   (note: column index 5 is OUT of range 0..4!)\n", q9);
    printf("Q13 arr[8][5] column-major (base 1000)    = %ld\n", q13);
    assert(q8 == 1070 && q9 == 2090 && q13 == 1116);

    /* --- verify the formulas against the real C compiler --- */
    int M[4][6];                                   /* 4 rows x 6 cols, row-major in C */
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 6; j++)
            assert((intptr_t)&M[i][j] == addr2_row((intptr_t)&M[0][0], sizeof(int), 0, 0, 4, 6, i, j));
    puts("row-major formula matches &M[i][j] for all 24 cells");

    /* column-major: same formula as row-major of the TRANSPOSED shape */
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 6; j++)
            assert(addr2_col(0, 1, 0, 0, 4, 6, i, j) == addr2_row(0, 1, 0, 0, 6, 4, j, i));

    /* --- non-zero lower bounds: A[-2..2][1..4] (5 x 4), Pascal-style --- */
    long a = addr2_row(500, 4, -2, 1, 5, 4, 1, 3);
    printf("A[-2..2][1..4], A[1][3] row-major         = %ld\n", a);
    assert(a == 500 + 4 * ((1 + 2) * 4 + (3 - 1)));    /* = 500 + 4*14 = 556 */

    /* --- 3-D: int T[3][4][5] --- */
    int T[3][4][5];
    long lb[3] = {0, 0, 0}, len[3] = {3, 4, 5};
    for (int i = 0; i < 3; i++) for (int j = 0; j < 4; j++) for (int k = 0; k < 5; k++) {
        long idx[3] = {i, j, k};
        assert((intptr_t)&T[i][j][k] == addrN_row((intptr_t)&T[0][0][0], sizeof(int), 3, lb, len, idx));
    }
    long idx[3] = {1, 2, 3};
    printf("T[1][2][3] row-major offset = %ld elements; column-major offset = %ld\n",
           (addrN_row(0, 1, 3, lb, len, idx)), addrN_col(0, 1, 3, lb, len, idx));
    assert(addrN_row(0, 1, 3, lb, len, idx) == 33 && addrN_col(0, 1, 3, lb, len, idx) == 43);
    puts("address_calc: all tests passed");
    return 0;
}
