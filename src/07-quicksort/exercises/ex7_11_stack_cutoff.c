/* ============================================================
 *  ex7_11_stack_cutoff.c  —  Exercises 7.11, 7.12, 7.15, 7.16, 7.22, 7.23
 *  Chapter 7: Quicksort — solved exercises
 *
 *  7.11 Stack contents after each push/pop pair when Program 7.3 sorts
 *       E A S Y Q U E S T I O N (larger subfile pushed first).
 *  7.12 Same, always pushing the right subfile, then the left.
 *  7.15/7.16 Maximum stack size of Program 7.3 for N = 2^n, 2^n - 1, 2^n + 1
 *       (worst case over all files, computed by a recurrence).
 *  7.22 Percentage of compares spent partitioning subfiles of size
 *       < 10, < 100, < 1000, for random files.
 *  7.23 Find the best cutoff M for insertion sort on small subfiles.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 7.1-7.3, Exercises 7.11-7.23
 *  Build:   gcc -std=c99 -Wall -O2 ex7_11_stack_cutoff.c -o ex7_11
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef int Item;
static long C, Csmall[3];
#define less(A, B) (C++, (A) < (B))
#define exch(A, B) { Item t = A; A = B; B = t; }

static int partition(Item a[], int l, int r)
{
    int i = l - 1, j = r; Item v = a[r];
    for (;;) {
        while (less(a[++i], v)) ;
        while (less(v, a[--j])) if (j == l) break;
        if (i >= j) break;
        exch(a[i], a[j]);
    }
    exch(a[i], a[r]);
    return i;
}

static int st[256], sp, maxsp;
static void push(int x) { st[sp++] = x; if (sp > maxsp) maxsp = sp; }
static int pop(void) { return st[--sp]; }
static void showstack(void) { printf("   stack:"); for (int k = sp - 2; k >= 0; k -= 2) printf(" [%d,%d]", st[k + 1], st[k]); if (!sp) printf(" (empty)"); printf("\n"); }

static void quick_stack(Item a[], int l, int r, int bigfirst, int trace)   /* Program 7.3 */
{
    sp = maxsp = 0;
    push(r); push(l);
    while (sp) {
        l = pop(); r = pop();
        if (r <= l) continue;
        int i = partition(a, l, r);
        if (trace) printf("   partition [%d,%d] at %d (%c)\n", l, r, i, a[i]);
        /* push2(A,B) = push(B); push(A): the subfile pushed LAST is on top */
        if (!bigfirst)          { push(r); push(i + 1); push(i - 1); push(l); }   /* 7.12: left on top    */
        else if (i - l > r - i) { push(i - 1); push(l); push(r); push(i + 1); }   /* left larger: right on top */
        else                    { push(r); push(i + 1); push(i - 1); push(l); }   /* right larger: left on top */
        if (trace) showstack();
    }
}

/* 7.15/7.16: T(N) = largest number of subfiles on Program 7.3's stack,
   over ALL files of size N, counting from the moment the subfile is
   popped.  A partition pushes 2 subfiles; the smaller (on top) is then
   worked on with the larger waiting underneath, so
       T(N) = max over splits of  max(2, 1 + T(small), T(large)),
   T(0) = T(1) = 0.  (Stack entries = 2 per subfile.)               */
static int S[70000];

static void quick_rec(Item a[], int l, int r, int M)
{
    if (r - l <= M) return;
    long c0 = C;
    int i = partition(a, l, r);
    long used = C - c0; int n = r - l + 1;
    if (n < 10) Csmall[0] += used;
    if (n < 100) Csmall[1] += used;
    if (n < 1000) Csmall[2] += used;
    quick_rec(a, l, i - 1, M);
    quick_rec(a, i + 1, r, M);
}
static void insertion(Item a[], int l, int r)
{
    for (int i = l + 1; i <= r; i++) { Item v = a[i]; int j = i; while (j > l && less(v, a[j - 1])) { a[j] = a[j - 1]; j--; } a[j] = v; }
}

static unsigned long long rs = 271828;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }

int main(void)
{
    char s[] = "EASYQUESTION"; Item a[1000000];
    printf("7.11 Program 7.3 (larger subfile pushed first, smaller on top):\n");
    for (int i = 0; i < 12; i++) a[i] = s[i];
    quick_stack(a, 0, 11, 1, 1);
    printf("     max stack = %d entries (%d subfiles)\n", maxsp, maxsp / 2);
    printf("7.12 always right then left (left on top):\n");
    for (int i = 0; i < 12; i++) a[i] = s[i];
    quick_stack(a, 0, 11, 0, 1);
    printf("     max stack = %d entries (%d subfiles)\n", maxsp, maxsp / 2);

    S[0] = S[1] = 0;
    for (int n = 2; n < 70000; n++) {
        int best = 2, lim = n > 4096 ? 1 : n;          /* for big n the balanced split is the max */
        for (int small = (n - 1) / 2; small >= 0 && lim-- > 0; small--) {
            int large = n - 1 - small;
            if (S[small] + 1 > best) best = S[small] + 1;
            if (S[large] > best) best = S[large];
        }
        S[n] = best;
    }
    printf("7.15/7.16 worst case T(N) over all files vs Program 7.3 measured (max subfiles on the stack):\n");
    for (int n = 4; n <= 16; n += 4)
    {
        int Ns[3] = { (1 << n) - 1, 1 << n, (1 << n) + 1 };
        printf("     n=%2d:", n);
        for (int t = 0; t < 3; t++) {
            int N = Ns[t], mr = 0;
            for (int rep = 0; rep < 5; rep++) { for (int i = 0; i < N; i++) a[i] = (int)rnd(); quick_stack(a, 0, N - 1, 1, 0); if (maxsp / 2 > mr) mr = maxsp / 2; }
            for (int i = 0; i < N; i++) a[i] = i;
            quick_stack(a, 0, N - 1, 1, 0);
            printf("  N=%6d: T=%2d random %2d sorted %d |", N, S[N], mr, maxsp / 2);
        }
        printf("\n");
    }

    printf("7.22 share of compares spent on subfiles of size <10 / <100 / <1000:\n");
    for (int N = 1000; N <= 1000000; N *= 10) {
        for (int i = 0; i < N; i++) a[i] = (int)rnd();
        C = 0; Csmall[0] = Csmall[1] = Csmall[2] = 0;
        quick_rec(a, 0, N - 1, 0);
        printf("     N=%7d: %5.1f%%  %5.1f%%  %5.1f%%\n", N, 100.0 * Csmall[0] / C, 100.0 * Csmall[1] / C, 100.0 * Csmall[2] / C);
    }

    printf("7.23 cutoff M (N=1000000, time in ms, insertion sort pass at the end):\n    ");
    static Item src[1000000];
    for (int i = 0; i < 1000000; i++) src[i] = (int)rnd();
    int bestM = 0; double bt = 1e9;
    for (int M = 0; M <= 40; M += (M < 20 ? 2 : 5)) {
        double tot = 0;
        for (int rep = 0; rep < 3; rep++) {
            for (int i = 0; i < 1000000; i++) a[i] = src[i];
            clock_t t0 = clock();
            quick_rec(a, 0, 999999, M); insertion(a, 0, 999999);
            tot += (double)(clock() - t0) * 1000 / CLOCKS_PER_SEC;
        }
        printf(" M=%d:%.0f", M, tot / 3);
        if (tot / 3 < bt) { bt = tot / 3; bestM = M; }
    }
    printf("\n     fastest here: M = %d (times vary by machine; the curve is flat from ~6 to ~25)\n", bestM);
    return 0;
}
