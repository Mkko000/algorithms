/* ============================================================
 *  array_ops.c  —  Core operations on one-dimensional arrays
 *  Chapter 17: Arrays (insert, delete, merge, reverse, rotate, ...)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Sec. 3.5;
 *                Bandyopadhyay & Dey, "Data Structures Using C", Sec. 3.4;
 *                S. Lipschutz, "Data Structures" (Schaum), Ch. 4.
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 array_ops.c -o array_ops
 * ============================================================ */
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* every routine returns the NEW logical length (or -1 on a bad request) */

//>>insert
/* insert value v at position pos (0-based); shifts a[pos..n-1] one step right.
 * cost: n - pos moves -> O(n) worst case (pos = 0), O(1) best (pos = n). */
int array_insert(int a[], int n, int cap, int pos, int v)
{
    if (n >= cap || pos < 0 || pos > n) return -1;   /* overflow / bad index */
    for (int i = n - 1; i >= pos; i--)               /* go from the END */
        a[i + 1] = a[i];
    a[pos] = v;
    return n + 1;
}
//<<insert

//>>delete
/* delete the element at position pos; shifts a[pos+1..n-1] one step left. */
int array_delete(int a[], int n, int pos)
{
    if (pos < 0 || pos >= n) return -1;              /* underflow / bad index */
    for (int i = pos; i < n - 1; i++)                /* go from the FRONT */
        a[i] = a[i + 1];
    return n - 1;
}
//<<delete

/* delete the first occurrence of value v (linear search + delete) */
int array_delete_value(int a[], int n, int v)
{
    for (int i = 0; i < n; i++)
        if (a[i] == v) return array_delete(a, n, i);
    return n;                                        /* not found: unchanged */
}

//>>merge
/* merge two SORTED arrays into c; returns n1 + n2. Stable, O(n1 + n2). */
int array_merge_sorted(const int a[], int n1, const int b[], int n2, int c[])
{
    int i = 0, j = 0, k = 0;
    while (i < n1 && j < n2) c[k++] = (b[j] < a[i]) ? b[j++] : a[i++];
    while (i < n1) c[k++] = a[i++];
    while (j < n2) c[k++] = b[j++];
    return k;
}
//<<merge

/* plain concatenation (Thareja 3.5.4 "merging two arrays") */
int array_concat(const int a[], int n1, const int b[], int n2, int c[])
{
    memcpy(c, a, n1 * sizeof a[0]);
    memcpy(c + n1, b, n2 * sizeof b[0]);
    return n1 + n2;
}

//>>reverse
static void rev(int a[], int i, int j) { while (i < j) { int t = a[i]; a[i++] = a[j]; a[j--] = t; } }

void array_reverse(int a[], int n) { rev(a, 0, n - 1); }

/* rotate LEFT by k using three reversals: O(n) time, O(1) extra space.
 *   [A | B]  ->  reverse(A), reverse(B), reverse(all)  ->  [B | A]       */
void array_rotate_left(int a[], int n, int k)
{
    if (n == 0) return;
    k %= n;
    rev(a, 0, k - 1);
    rev(a, k, n - 1);
    rev(a, 0, n - 1);
}
//<<reverse

/* largest and second largest in ONE pass (distinct positions, k=2) */
int array_two_largest(const int a[], int n, int *first, int *second)
{
    if (n < 2) return 0;
    int m1 = a[0] > a[1] ? a[0] : a[1], m2 = a[0] > a[1] ? a[1] : a[0];
    for (int i = 2; i < n; i++) {
        if (a[i] > m1) { m2 = m1; m1 = a[i]; }
        else if (a[i] > m2) m2 = a[i];
    }
    *first = m1; *second = m2;
    return 1;
}

/* remove duplicates from a SORTED array in place (two-pointer technique) */
int array_unique_sorted(int a[], int n)
{
    if (n == 0) return 0;
    int w = 1;
    for (int r = 1; r < n; r++)
        if (a[r] != a[w - 1]) a[w++] = a[r];
    return w;
}

/* all pairs (i<j) with a[i] + a[j] == target : hash-free O(n log n) after sort
 * would need sorting; here the simple O(n^2) version (Thareja prog. ex. 2) */
int array_pairs_with_sum(const int a[], int n, int target, int out[][2], int maxout)
{
    int c = 0;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (a[i] + a[j] == target && c < maxout) { out[c][0] = a[i]; out[c][1] = a[j]; c++; }
    return c;
}

static void show(const char *tag, const int a[], int n)
{
    printf("%-22s [", tag);
    for (int i = 0; i < n; i++) printf(i ? " %d" : "%d", a[i]);
    printf("]  (n=%d)\n", n);
}

int main(void)
{
    int a[16] = {10, 20, 30, 40, 50}, n = 5;
    show("start", a, n);
    n = array_insert(a, n, 16, 2, 25);          show("insert 25 at 2", a, n);
    assert(n == 6 && a[2] == 25 && a[3] == 30 && a[5] == 50);
    n = array_insert(a, n, 16, 0, 5);           show("insert 5 at front", a, n);
    n = array_insert(a, n, 16, n, 99);          show("insert 99 at end", a, n);
    assert(array_insert(a, n, 16, 20, 1) == -1);        /* bad position */
    n = array_delete(a, n, 3);                  show("delete index 3", a, n);
    n = array_delete_value(a, n, 99);           show("delete value 99", a, n);
    assert(array_delete(a, n, n) == -1);                 /* underflow side */

    int b[] = {1, 3, 5, 7, 40}, c[] = {2, 3, 4, 8}, m[16];
    int k = array_merge_sorted(b, 5, c, 4, m);  show("merge sorted", m, k);
    for (int i = 1; i < k; i++) assert(m[i - 1] <= m[i]);
    k = array_concat(b, 5, c, 4, m);            show("concat", m, k);

    int r[] = {1, 2, 3, 4, 5, 6, 7};
    array_rotate_left(r, 7, 3);                 show("rotate left 3", r, 7);
    assert(r[0] == 4 && r[6] == 3);
    array_reverse(r, 7);                        show("reverse", r, 7);
    assert(r[0] == 3 && r[6] == 4);

    int f, s, t[] = {7, 3, 9, 4, 9, 1};
    assert(array_two_largest(t, 6, &f, &s) && f == 9 && s == 9);
    printf("two largest of {7,3,9,4,9,1}: %d, %d  (duplicates count separately)\n", f, s);

    int u[] = {1, 1, 2, 2, 2, 3, 5, 5};
    int un = array_unique_sorted(u, 8);         show("unique", u, un);
    assert(un == 4 && u[3] == 5);

    int p[] = {10, 40, 25, 30, 20, 15, 35}, pr[10][2];
    int np = array_pairs_with_sum(p, 7, 50, pr, 10);
    printf("pairs summing to 50:");
    for (int i = 0; i < np; i++) printf(" (%d,%d)", pr[i][0], pr[i][1]);
    printf("\n");
    assert(np == 3);
    puts("array_ops: all tests passed");
    return 0;
}
