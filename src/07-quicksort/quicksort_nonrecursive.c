/* ============================================================
 *  quicksort_nonrecursive.c  —  Quicksort with an explicit stack,
 *                               and why "smaller subfile first" matters
 *  Chapter 7: Quicksort (Section 7.3)
 *
 *  Program 7.3 replaces recursion with a stack of (l, r) pairs. The
 *  trick: after partitioning, push the LARGER subfile first so the
 *  SMALLER one is processed next. Then the stack never holds more than
 *  about lg N entries (Property 7.3). With the naive order, a sorted
 *  file makes the stack grow to ~N — which, for the recursive version,
 *  means a stack overflow on big inputs. The input 1 2 3 ... N-1 0
 *  keeps that shape after every partition (the pivot is always the
 *  minimum), so it is a worst case for the naive push order.
 *  We measure the maximum stack depth both ways.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 7.3
 *  Build:   gcc -std=c99 -Wall -O2 quicksort_nonrecursive.c -o quicksort_nonrecursive
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include "../common_item.h"

int partition(Item a[], int l, int r) {
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

static int *stk, sp, maxsp;
#define push2(A, B) { stk[sp++] = (B); stk[sp++] = (A); if (sp > maxsp) maxsp = sp; }
#define pop() (stk[--sp])

void quicksort(Item a[], int l, int r, int smaller_first) {
    sp = 0; maxsp = 0;
    push2(l, r);
    while (sp > 0) {
        l = pop(); r = pop();
        if (r <= l) continue;
        int i = partition(a, l, r);
        if (!smaller_first || i - l > r - i) { push2(l, i - 1); push2(i + 1, r); }
        else                                 { push2(i + 1, r); push2(l, i - 1); }
    }
}

int main(void) {
    const int N = 20000;
    Item *a = malloc(N * sizeof *a);
    stk = malloc(4 * (N + 2) * sizeof *stk);
    const char *in[] = {"random", "1 2 .. N-1 0"};
    for (int kind = 0; kind < 2; kind++)
        for (int sf = 0; sf < 2; sf++) {
            srand(1);
            for (int i = 0; i < N; i++) a[i] = kind ? i + 1 : rand();
            if (kind) a[N - 1] = 0;   /* pivot is always the minimum: left side always empty */
            quicksort(a, 0, N - 1, sf);
            for (int i = 1; i < N; i++) if (a[i-1] > a[i]) { puts("BUG"); return 1; }
            printf("%-13s %-22s max stack entries = %d\n", in[kind],
                   sf ? "smaller subfile first" : "naive push order", maxsp / 2);
        }
    free(a); free(stk);
    return 0;
}
