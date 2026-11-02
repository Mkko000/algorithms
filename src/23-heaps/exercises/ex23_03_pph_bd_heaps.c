/* Pal & Halder (P&H), "Data Structures and Algorithms with C": the priority-queue algorithms Insert / Remove / Heapify
 * (Chapter 6, Algorithms on pages 6.18-6.19), Fig 8.23 and Exercise 2 of Chapter 8;
 * Bandyopadhyay & Dey (B&D), "Data Structures Using C", Fig 10.7: three heaps of the same nine integers.
 * (B&D's heap-sort Algorithm 10.7 and P&H's heap sort are with the sorting chapter.)
 *   gcc -std=c99 -Wall -Wextra -O2 ex23_03_pph_bd_heaps.c
 */
#define BH_NO_MAIN
#include "../binheap.c"

/* ---- P&H Algorithm: Insert (Q, N, ITEM)   [max-heap, Q[1..N]] ---- */
static void ph_insert(int *Q, int *N, int ITEM)
{
    int I, J;
    *N = *N + 1;                                      /* 1. Set N = N + 1 */
    I = *N; J = I / 2;                                /* 2. Set I = N and J = I/2 */
    while (I > 1 && Q[J] < ITEM) {                    /* 3. Repeat steps 4 and 5 while I > 1 and Q[J] < ITEM */
        Q[I] = Q[J];                                  /* 4. Set Q[I] = Q[J] */
        I = J; J = I / 2;                             /* 5. Set I = J and J = I/2 */
    }
    Q[I] = ITEM;                                      /* 6. Set Q[I] = ITEM */
}
/* ---- P&H Heapify(Q, I): shift-down from I over Q[1..N]; the book calls Heapify(Q, 1) in Remove ---- */
static void ph_heapify(int *Q, int N, int I)
{
    for (;;) {
        int L = 2 * I, R = 2 * I + 1, largest = I;
        if (L <= N && Q[L] > Q[largest]) largest = L;
        if (R <= N && Q[R] > Q[largest]) largest = R;
        if (largest == I) return;
        int t = Q[I]; Q[I] = Q[largest]; Q[largest] = t; I = largest;
    }
}
/* ---- P&H Algorithm: Remove (Q, N, ITEM) ---- */
static int ph_remove(int *Q, int *N, int *ITEM)
{
    if (*N < 1) { printf("Queue Underflow\n"); return 0; }      /* 1 */
    *ITEM = Q[1];                                               /* 2 */
    Q[1] = Q[*N];                                               /* 3 */
    *N = *N - 1;                                                /* 4 */
    ph_heapify(Q, *N, 1);                                       /* 5 */
    return 1;
}
static int is_max_heap(const int *Q, int n) { for (int i = 2; i <= n; i++) if (Q[i] > Q[i / 2]) return 0; return 1; }
static int is_min_heap(const int *Q, int n) { for (int i = 2; i <= n; i++) if (Q[i] < Q[i / 2]) return 0; return 1; }
static void pr(const char *label, const int *Q, int n) { printf("  %-34s", label); for (int i = 1; i <= n; i++) printf(" %d", Q[i]); printf("\n"); }

int main(void)
{
    /* ---------- P&H Table 6.6 / 6.7: a priority queue three ways ---------- */
    printf("== P&H priority queue: Insert / Peek / Remove on a max-heap ==\n");
    { int Q[64] = { 0 }, N = 0, item; int in[] = { 5, 17, 3, 22, 9, 30, 1 };
      for (int i = 0; i < 7; i++) { ph_insert(Q, &N, in[i]); assert(is_max_heap(Q, N)); }
      pr("after inserting 5 17 3 22 9 30 1", Q, N);
      assert(Q[1] == 30);                                       /* Peek: O(1), the root */
      int out[7], k = 0; while (N) { assert(ph_remove(Q, &N, &item)); out[k++] = item; assert(is_max_heap(Q, N)); }
      printf("  removals come out in priority order: "); for (int i = 0; i < 7; i++) printf("%d ", out[i]); printf("\n");
      for (int i = 1; i < 7; i++) assert(out[i - 1] >= out[i]);
      assert(!ph_remove(Q, &N, &item));                         /* Queue Underflow */
    }
    /* the same sequence against the book's other two representations, with the costs of Table 6.7 counted */
    printf("== P&H Table 6.7: insert / peek / remove costs, counted ==\n");
    { srand(67); int n = 4096; long c_unsorted_rem = 0, c_sorted_ins = 0, c_heap_ins = 0, c_heap_rem = 0;
      int *U = malloc(sizeof(int) * (size_t)n), *S = malloc(sizeof(int) * (size_t)n), *H = malloc(sizeof(int) * (size_t)(n + 2)); int un = 0, sn = 0, hn = 0;
      for (int i = 0; i < n; i++) {
          int x = rand();
          U[un++] = x;                                                         /* unsorted array: O(1) insert */
          int p = sn; while (p > 0 && S[p - 1] > x) { S[p] = S[p - 1]; p--; c_sorted_ins++; } S[p] = x; sn++;   /* sorted: shifts */
          int before = hn; ph_insert(H, &hn, x); (void)before;
          { int I = hn; while (I > 1) { c_heap_ins++; I /= 2; } }              /* upper bound: depth */
      }
      for (int i = 0; i < n; i++) {
          int best = 0; for (int j = 1; j < un; j++) { c_unsorted_rem++; if (U[j] > U[best]) best = j; } U[best] = U[--un];
          int s = S[--sn];                                                      /* sorted: O(1) */
          int item; assert(ph_remove(H, &hn, &item)); assert(item == s); c_heap_rem++;
      }
      printf("  n=%d: unsorted remove scanned %ld cells; sorted insert shifted %ld; heap insert <= %ld levels; heap remove %ld ops of log n\n", n, c_unsorted_rem, c_sorted_ins, c_heap_ins, c_heap_rem);
      assert(c_unsorted_rem > 10 * c_heap_ins && c_sorted_ins > c_heap_ins);
      free(U); free(S); free(H); }

    /* ---------- P&H Fig 8.23 ---------- */
    printf("== P&H Fig 8.23: (a) a max-heap, (b) a min-heap of the same keys ==\n");
    { int a[] = { 0, 14, 12, 7, 10, 9, 6 }, b[] = { 0, 2, 7, 5, 10, 9, 6 };
      assert(is_max_heap(a, 6) && is_min_heap(b, 6) && !is_min_heap(a, 6) && !is_max_heap(b, 6));
      pr("(a) max-heap array", a, 6); pr("(b) min-heap array", b, 6); }

    /* ---------- P&H Chapter 8, Exercise 2 ---------- */
    printf("== P&H Ch 8 Ex 2: a minimum heap from 12, 11, 7, 3, 10, -5, 0, 9, 2 ==\n");
    { int v[] = { 12, 11, 7, 3, 10, -5, 0, 9, 2 };
      BHeap ins = bh_new(32, 0), fl = bh_new(32, 0); bh_build_insert(&ins, v, 9); bh_build_floyd(&fl, v, 9);
      pr("by successive insertion", ins.a, 9); pr("by shift-down from n/2 (Floyd)", fl.a, 9);
      assert(bh_valid(&ins) && bh_valid(&fl) && ins.a[1] == -5 && fl.a[1] == -5);
      int e1[] = { -5, 2, 0, 7, 10, 11, 3, 12, 9 }, e2[] = { -5, 2, 0, 3, 10, 7, 12, 9, 11 };   /* cross-checked with an independent script */
      for (int i = 0; i < 9; i++) assert(ins.a[i + 1] == e1[i] && fl.a[i + 1] == e2[i]);
      bh_free(&ins); bh_free(&fl); }

    /* ---------- B&D Fig 10.7 ---------- */
    printf("== B&D Fig 10.7: three heaps of 10 20 25 30 40 42 50 52 55 ==\n");
    { int a[] = { 0, 10, 20, 25, 30, 40, 42, 50, 52, 55 }, b[] = { 0, 10, 30, 20, 40, 50, 25, 55, 52, 42 }, c[] = { 0, 10, 40, 20, 50, 42, 30, 25, 52, 55 };
      assert(is_min_heap(a, 9) && is_min_heap(b, 9) && is_min_heap(c, 9));
      int sa = 0, sb = 0, sc = 0; for (int i = 1; i <= 9; i++) { sa += a[i]; sb += b[i]; sc += c[i]; } assert(sa == sb && sb == sc);
      printf("  all three satisfy A[i] <= A[2i], A[i] <= A[2i+1]; a sorted array is always a heap, but so are other arrangements\n");
      /* how many of the 9! arrangements are heaps? exhaustively */
      int p[9] = { 10, 20, 25, 30, 40, 42, 50, 52, 55 }, c9[9] = { 0 }, i = 1; long heaps = 0, total = 0;
      for (;;) {
          int Q[10]; for (int j = 0; j < 9; j++) Q[j + 1] = p[j];
          total++; if (is_min_heap(Q, 9)) heaps++;
          while (i < 9) { if (c9[i] < i) { int t; if (i % 2 == 0) { t = p[0]; p[0] = p[i]; p[i] = t; } else { t = p[c9[i]]; p[c9[i]] = p[i]; p[i] = t; } c9[i]++; i = 1; break; } else { c9[i] = 0; i++; } }
          if (i >= 9) break; }
      printf("  of the %ld orderings of these nine keys, exactly %ld are heaps\n", total, heaps);
      assert(total == 362880);
      /* the hook-length formula for heap orderings: n! / product of subtree sizes */
      int sz[10]; for (int j = 9; j >= 1; j--) { sz[j] = 1 + (2 * j <= 9 ? sz[2 * j] : 0) + (2 * j + 1 <= 9 ? sz[2 * j + 1] : 0); }
      long num = 362880, den = 1; for (int j = 1; j <= 9; j++) den *= sz[j];
      assert(num % den == 0 && heaps == num / den);
      printf("  = 9! / (product of the subtree sizes) = %ld / %ld\n", num, den); }

    printf("ex23_03: all checks passed\n");
    return 0;
}
