/* Thareja, Data Structures Using C, Chapter 12 "Heaps" -- Review Questions 1-30, Multiple-choice, True/False and
 * Fill-in-the-blanks (Appendix G is the key).  Everything that can be computed is computed and asserted here;
 * the pure "explain / define / discuss" questions (1-6, 8-20, 29) are answered in the lesson and listed in the map.
 *   gcc -std=c99 -Wall -Wextra -O2 -lm ex23_01_thareja_heaps.c
 */
#define BH_NO_MAIN
#include "../binheap.c"
#define BN_NO_MAIN
#include "../binomial.c"
#define FH_NO_MAIN
#include "../fibheap.c"

static void show(const char *label, const BHeap *h)
{
    printf("  %-28s", label); for (int i = 1; i <= h->n; i++) printf(" %d", h->a[i]); printf("\n");
}
static int is_heap_array(const int *v, int n, int mx)         /* v[0..n-1] as positions 1..n */
{
    for (int i = 2; i <= n; i++) if (mx ? v[i - 1] > v[i / 2 - 1] : v[i - 1] < v[i / 2 - 1]) return 0;
    return 1;
}
/* Fig 12.10 read literally, Step 5 included: HEAP[RIGHT] is looked at even when RIGHT > N (a stale cell) */
static void delete_root_literal(int *a, int *n)               /* max-heap, a has room at n+1 and keeps old values */
{
    int last = a[*n]; (*n)--;
    if (*n == 0) return;
    int ptr = 1, left = 2, right = 3; a[ptr] = last;
    while (left <= *n) {
        if (a[ptr] >= a[left] && a[ptr] >= a[right]) break;       /* Step 5, with a[right] possibly stale */
        int c = (a[right] <= a[left]) ? left : right;             /* Step 6 */
        int t = a[ptr]; a[ptr] = a[c]; a[c] = t; ptr = c; left = 2 * ptr; right = left + 1;
    }
}

int main(void)
{
    printf("== Q7: 50, 40, 35, 25, 20, 27, 33 ==\n");
    { int v[] = { 50, 40, 35, 25, 20, 27, 33 };
      BHeap mx = bh_new(16, 1), mn = bh_new(16, 0);
      bh_build_insert(&mx, v, 7); bh_build_insert(&mn, v, 7);
      show("max-heap (insert one by one)", &mx); show("min-heap (insert one by one)", &mn);
      assert(bh_valid(&mx) && bh_valid(&mn));
      int xmx[] = { 50, 40, 35, 25, 20, 27, 33 };                     /* already a max-heap: no swap happens */
      for (int i = 0; i < 7; i++) assert(mx.a[i + 1] == xmx[i]);
      int xmn[] = { 20, 25, 27, 50, 35, 40, 33 };
      for (int i = 0; i < 7; i++) assert(mn.a[i + 1] == xmn[i]);
      BHeap f = bh_new(16, 0); bh_build_floyd(&f, v, 7); show("min-heap (Floyd, linear)", &f);
      assert(bh_valid(&f)); bh_free(&f); bh_free(&mx); bh_free(&mn); }

    printf("== Q21 and Q23: the tree 4 | 6 12 | 7 9 21 39 | 13 19 10 ==\n");
    { int v[] = { 4, 6, 12, 7, 9, 21, 39, 13, 19, 10 };
      assert(is_heap_array(v, 10, 0) && !is_heap_array(v, 10, 1));      /* Q21: yes, it is a (min-)heap */
      printf("  Q21: it is a min-heap, not a max-heap.  Q23: array = ");
      for (int i = 0; i < 10; i++) printf("%d ", v[i]);
      printf("\n  indexes:                  "); for (int i = 1; i <= 10; i++) printf("%d ", i); printf("\n"); }

    printf("== Q22: reheap 45 | 27 36 | 16 18 29 21 | 54  into a max-heap ==\n");
    { BHeap h = bh_new(16, 1); int v[] = { 45, 27, 36, 16, 18, 29, 21, 54 };
      memcpy(h.a + 1, v, sizeof v); h.n = 8; assert(!bh_valid(&h));        /* 54 under 16 breaks the order */
      bh_percolate_up(&h, 8); show("after percolating 54 up", &h); assert(bh_valid(&h));
      int x[] = { 54, 45, 36, 27, 18, 29, 21, 16 };
      for (int i = 0; i < 8; i++) assert(h.a[i + 1] == x[i]);
      bh_free(&h); }

    printf("== Q24: array 45 27 36 18 16 21 23 10 ==\n");
    { int v[] = { 45, 27, 36, 18, 16, 21, 23, 10 };
      assert(is_heap_array(v, 8, 1));                                    /* it is a max-heap as it stands */
      /* positions: 10 is at 8, 21 at 6, 23 at 7;  parent(i) = i/2 */
      assert(v[8 / 2 - 1] == 18 && v[6 / 2 - 1] == 36 && v[7 / 2 - 1] == 36);
      printf("  parent of 10 is %d, of 21 is %d, of 23 is %d.  23 is at 7: children would be at %d and %d (> n = 8: none)\n",
             v[3], v[2], v[2], 2 * 7, 2 * 7 + 1);
      assert(2 * 7 > 8); }

    printf("== Q25: which sequence is a binary heap? ==\n");
    { int a[] = { 40, 33, 35, 22, 12, 16, 5, 7 }, b[] = { 44, 37, 20, 22, 16, 32, 12 }, c[] = { 15, 15, 15, 15, 15, 15 };
      printf("  (a) max-heap=%d  (b) max-heap=%d  (c) max-heap=%d min-heap=%d\n",
             is_heap_array(a, 8, 1), is_heap_array(b, 7, 1), is_heap_array(c, 6, 1), is_heap_array(c, 6, 0));
      assert(is_heap_array(a, 8, 1) && !is_heap_array(b, 7, 1) && !is_heap_array(b, 7, 0) && is_heap_array(c, 6, 1) && is_heap_array(c, 6, 0)); }

    printf("== Q26 and Q27: heap 52 32 42 22 12 27 37 12 7 ==\n");
    { int v[] = { 52, 32, 42, 22, 12, 27, 37, 12, 7 };
      BHeap h = bh_new(32, 1); memcpy(h.a + 1, v, sizeof v); h.n = 9; assert(bh_valid(&h));
      int r1 = bh_delete_root(&h), r2 = bh_delete_root(&h), r3 = bh_delete_root(&h);
      printf("  deleting three times removes %d, %d, %d; heap is now:", r1, r2, r3);
      for (int i = 1; i <= h.n; i++)
          printf(" %d", h.a[i]);
      printf("\n");
      assert(r1 == 52 && r2 == 42 && r3 == 37 && bh_valid(&h) && h.n == 6);
      BHeap g = bh_new(32, 1); memcpy(g.a + 1, v, sizeof v); g.n = 9;
      bh_insert(&g, 35); bh_insert(&g, 24); bh_insert(&g, 10);
      show("Q27: 35, 24, 10 added", &g); assert(bh_valid(&g) && g.n == 12);
      int x[] = { 52, 35, 42, 22, 32, 27, 37, 12, 7, 12, 24, 10 };
      for (int i = 0; i < 12; i++) assert(g.a[i + 1] == x[i]);
      bh_free(&g); bh_free(&h); }

    printf("== Q28: a heap that is also a binary search tree ==\n");
    { /* complete trees of n distinct keys, shape fixed by array positions; try every permutation */
      for (int n = 1; n <= 7; n++) {
          int p[7], found = 0; for (int i = 0; i < n; i++) p[i] = i + 1;
          int idx[7]; (void)idx;
          /* iterate permutations by Heap's algorithm via simple recursion using an explicit stack of counters */
          int c[7] = { 0 };
          for (int mxm = 0; mxm < 2; mxm++) {
              for (int i = 0; i < n; i++) { p[i] = i + 1; c[i] = 0; }
              int i = 0, ok;
              do {
                  ok = is_heap_array(p, n, mxm);
                  if (ok) {                                           /* BST check: in-order of positions is increasing */
                      int ord[8], k = 0;
                      /* in-order traversal of the array-tree */
                      int st[16], sp = 0, cur = 1;
                      while (cur <= n || sp) { while (cur <= n) { st[sp++] = cur; cur = 2 * cur; } cur = st[--sp]; ord[k++] = p[cur - 1]; cur = 2 * cur + 1; }
                      int inc = 1; for (int j = 1; j < k; j++) if (ord[j] < ord[j - 1]) inc = 0;
                      if (inc) found++;
                  }
                  while (i < n) { if (c[i] < i) { int t; if (i % 2 == 0) { t = p[0]; p[0] = p[i]; p[i] = t; } else { t = p[c[i]]; p[c[i]] = p[i]; p[i] = t; } c[i]++; i = 0; break; } else { c[i] = 0; i++; } }
              } while (i < n);
          }
          printf("  n=%d: %d arrangement(s) of 1..%d are both a max-heap or min-heap and a BST\n", n, found, n);
          assert(n <= 2 ? found > 0 : found == 0);
      } }

    printf("== Q29: heapify (Floyd) costs less than 2n comparisons ==\n");
    { srand(12);
      for (int n = 1; n <= 1000; n += 7) {
          int *v = malloc(sizeof(int) * (size_t)n); for (int i = 0; i < n; i++) v[i] = rand();
          BHeap h = bh_new(n, 0); bh_build_floyd(&h, v, n); assert(bh_valid(&h) && h.cmp <= 2L * n); bh_free(&h); free(v); }
      printf("  verified for n = 1, 8, 15, ... , 995\n"); }

    printf("== Q30: the Fibonacci heap with 17, 16, 36 ==\n");
    { FHeap h = fh_parse("17(24(27(39),44),18) 16(36,19(33),18) 36(41)");
      assert(h.n == 13 && fh_valid(&h) && h.min->key == 16);
      fh_decrease(&h, fh_find(h.min, 33), 9);  assert(fh_valid(&h) && h.min->key == 9);
      fh_insert(&h, fn_new(5));                assert(fh_valid(&h) && h.min->key == 5);
      fh_delete(&h, fh_find(h.min, 19));       assert(fh_valid(&h) && !fh_find(h.min, 19) && h.n == 13);
      printf("  after decrease 33->9, insert 5, delete 19: %s\n", fh_s(&h)); fh_free(&h); }

    printf("== Multiple-choice (Appendix G): 1 b, 2 a, 3 c, 4 d, 5 b, 6 d, 7 d ==\n");
    { /* MCQ 2: left child of i is 2i ; MCQ 5: B0 has 1 node ; MCQ 4: height of Bi is i ; MCQ 6: Link is O(1) */
      BNode *t = bn_parse("1"); assert(bn_count(t) == 1); bn_free(t);
      /* MCQ 3: key (c) says O(n log n); the true worst case of the linear-time (Floyd) build is O(n): measured above */
      long ins = 0, flo = 0;
      for (int n = 100; n <= 3200; n *= 2) {
          int *v = malloc(sizeof(int) * (size_t)n); for (int i = 0; i < n; i++) v[i] = n - i;         /* worst order for min-heap insertion */
          BHeap a = bh_new(n, 0), b = bh_new(n, 0); bh_build_insert(&a, v, n); bh_build_floyd(&b, v, n);
          ins = a.cmp; flo = b.cmp; printf("  n=%-5d insert-one-by-one %6ld comparisons   Floyd %5ld\n", n, ins, flo);
          assert(flo <= 2L * n); bh_free(&a); bh_free(&b); free(v); }
      assert(ins > flo); }

    printf("== True/False (Appendix G): 1 T, 2 F, 3 T, 4 T, 5 F, 6 F, 7 T, 8 F, 9 T, 10 F, 11 F ==\n");
    { /* 6: B_i has 2^i nodes (the printed "2i" is a typesetting loss of the exponent) -> the key says False for "2i"  */
      printf("  T/F 6 'A binomial tree Bi has 2i nodes' is False (it has 2^i); T/F 9 Min_Binomial-Heap is O(log n): True\n"); }

    printf("== Fill in the blanks: 2i+1 | priority queues | partially ordered trees | min heap | root | i | set of binomial trees ... | 2^i | O(1) | collection of heap-ordered trees | whether x lost a child ==\n");
    { /* blank 5: elements are always deleted from the root -> Fig 12.10 (bh_delete_root) removes a[1] */
      BHeap h = bh_new(8, 0); int v[] = { 3, 1, 2 }; bh_build_insert(&h, v, 3); assert(bh_delete_root(&h) == 1); bh_free(&h); }

    /* Fig 12.10 Step 5 looks at HEAP[RIGHT] even when RIGHT = N+1 (a node with one child). Is that a bug?
       - as the book runs it, cell N+1 still holds LAST (Step 1 only lowers N), and PTR holds LAST until it is placed,
         so "HEAP[PTR] >= HEAP[RIGHT]" is "LAST >= LAST": true, harmless;
       - it is fragile: if that cell has been cleared (to 0) and the keys can be negative, the test is wrong. */
    printf("== Fig 12.10 Step 5 read literally ==\n");
    { srand(5); int mism_stale = 0, mism_zero = 0, tried = 0, shown = 0;
      for (int t = 0; t < 20000; t++) {
          int n = 2 + rand() % 12; BHeap h = bh_new(n + 4, 1); int v[16]; for (int i = 0; i < n; i++) v[i] = rand() % 30 - 15;
          bh_build_floyd(&h, v, n);
          int a[32] = { 0 }, z[32] = { 0 }; memcpy(a, h.a, sizeof(int) * (size_t)(n + 2)); memcpy(z, h.a, sizeof(int) * (size_t)(n + 2));
          int na = n, nz = n;
          delete_root_literal(a, &na);
          /* second run with a cleared N+1 cell: save LAST first, clear, then run the same steps */
          { int last = z[n] = h.a[n]; int ptr = 1, left = 2, right = 3; nz = n - 1; z[n] = 0; z[1] = last;
            while (left <= nz) { if (z[ptr] >= z[left] && z[ptr] >= z[right]) break;
                int c = (z[right] <= z[left]) ? left : right; int tmp = z[ptr]; z[ptr] = z[c]; z[c] = tmp; ptr = c; left = 2 * ptr; right = left + 1; } }
          bh_delete_root(&h); tried++;
          int s1 = (na == h.n), s2 = (nz == h.n);
          for (int i = 1; s1 && i <= na; i++) if (a[i] != h.a[i]) s1 = 0;
          for (int i = 1; s2 && i <= nz; i++) if (z[i] != h.a[i]) s2 = 0;
          if (!s1) mism_stale++;
          if (!s2) { mism_zero++; if (!shown) { shown = 1; printf("  example with a cleared cell (n=%d): literal gives", n); for (int i = 1; i <= nz; i++) printf(" %d", z[i]); printf("; correct is"); for (int i = 1; i <= h.n; i++) printf(" %d", h.a[i]); printf("\n"); } }
          bh_free(&h); }
      printf("  stale cell kept (as in the book): %d mismatches in %d heaps; cell cleared and keys may be negative: %d mismatches\n", mism_stale, tried, mism_zero);
      assert(mism_stale == 0 && mism_zero > 0); }

    printf("ex23_01: all checks passed\n");
    return 0;
}
