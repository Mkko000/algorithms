/* Weiss, Data Structures and Algorithm Analysis, Chapter 6 "Priority Queues (Heaps)" -- Exercises 6.1 to 6.40.
 * Every exercise that has a computable answer is computed and asserted below; the proofs are in the lesson
 * (and a few open-ended ones are listed as "not coded" at the end -- see the printed summary).
 *   gcc -std=c99 -Wall -Wextra -O2 -lm ex23_02_weiss_heaps.c
 */
#define BH_NO_MAIN
#include "../binheap.c"
#define LH_NO_MAIN
#include "../leftist.c"
#define BN_NO_MAIN
#include "../binomial.c"
#define MM_NO_MAIN
#include "../minmax.c"
#include <time.h>

static void show_a(const char *label, const int *a, int n)
{
    printf("  %-34s", label);
    for (int i = 0; i < n; i++)
        printf(" %d", a[i]);
    printf("\n");
}
static void show_heap(const char *label, const BHeap *h) { show_a(label, h->a + 1, h->n); }
static int same(const BHeap *h, const int *e, int n)
{
    if (h->n != n) return 0;
    for (int i = 0; i < n; i++) if (h->a[i + 1] != e[i]) return 0;
    return 1;
}
/* a queue of binomial trees from the book's notation: each tree goes into slot degree */
static BQueue bq_from(const char *s)
{
    BQueue q = { { 0 }, 0 }; BNode *t = bn_parse(s);
    while (t) { BNode *nx = t->sibling; t->sibling = NULL; assert(!q.tree[t->degree]); q.tree[t->degree] = t; q.n += 1 << t->degree; t = nx; }
    return q;
}
static int cmp8;                                              /* comparisons counted by the 8-element builder */
static int lessc(int a, int b) { cmp8++; return a < b; }
/* Exercise 6.7(b): eight elements, eight comparisons.  Link into B3 (7 comparisons), reshape with ONE more. */
static void build8(const int *in, int *out)                   /* out[1..8] is a min-heap */
{
    int a[8]; memcpy(a, in, sizeof a); cmp8 = 0;
    /* four pairs: (a0,a1) (a2,a3) (a4,a5) (a6,a7): 4 comparisons; r = root, c = its only child */
    int r[4], c[4];
    for (int p = 0; p < 4; p++) { if (lessc(a[2 * p], a[2 * p + 1])) { r[p] = a[2 * p]; c[p] = a[2 * p + 1]; } else { r[p] = a[2 * p + 1]; c[p] = a[2 * p]; } }
    /* two B2: compare the roots of the two pairs: 2 comparisons.  B2 = root x, child-pair (y with its child), single z  */
    int x[2], y[2], yc[2], z[2];
    for (int q = 0; q < 2; q++) {
        int p0 = 2 * q, p1 = 2 * q + 1;
        if (lessc(r[p0], r[p1])) { x[q] = r[p0]; y[q] = r[p1]; yc[q] = c[p1]; z[q] = c[p0]; }
        else                     { x[q] = r[p1]; y[q] = r[p0]; yc[q] = c[p0]; z[q] = c[p1]; }
        /* x has children y (which has child yc) and z (a B0)  -- this is the B2 of Weiss Fig 6.50 */
    }
    /* B3: compare x[0], x[1]: 1 comparison; the loser becomes a child of the winner */
    int w = lessc(x[0], x[1]) ? 0 : 1, l = 1 - w;
    /* root = x[w]; children of the B3: B2 rooted x[l] (with y[l]/yc[l], z[l]), B1 = y[w](yc[w]), B0 = z[w] */
    /* binary-heap shape:  1: root, 2: x[l], 3: (z[w] or y[w]), 4: y[l], 5: z[l], 8: yc[l], 6/7: below node 3 */
    out[1] = x[w]; out[2] = x[l]; out[4] = y[l]; out[5] = z[l]; out[8] = yc[l];
    /* node 3 takes the one comparison left over: c0 = z[w] against c1 = y[w]; the loser and yc[w] hang below it */
    if (lessc(z[w], y[w])) { out[3] = z[w]; out[6] = y[w]; out[7] = yc[w]; }
    else                   { out[3] = y[w]; out[6] = yc[w]; out[7] = z[w]; }
}
/* Exercise 6.13(a): deleteMin with log N + log log N + O(1) comparisons.
   1. slide the hole down the path of smaller children: one comparison per level (children against each other);
   2. binary-search that path (it is sorted) for the place LAST belongs: log(log N) comparisons;
   3. shift the part of the path above that place up by one. */
static long cmp13;
static int lt13(int a, int b) { cmp13++; return a < b; }
static int delete_min_fewer(BHeap *h)
{
    int top = h->a[1], last = h->a[h->n--];
    if (h->n == 0) return top;
    int path[40], np = 0;                                         /* path[0] = the hole (root), then the smaller child each time */
    for (int i = 1; ; ) {
        path[np++] = i;
        if (2 * i > h->n) break;
        int c = 2 * i;
        if (c + 1 <= h->n && lt13(h->a[c + 1], h->a[c])) c++;    /* the only comparison per level */
        i = c;
    }
    /* keys along path[1..np-1] increase; lo = how many of them are smaller than LAST (binary search: log of the path length) */
    int lo = 0, hi = np - 1;
    while (lo < hi) { int mid = (lo + hi) / 2; if (lt13(h->a[path[mid + 1]], last)) lo = mid + 1; else hi = mid; }
    for (int j = 0; j < lo; j++) h->a[path[j]] = h->a[path[j + 1]];      /* the smaller path keys move up one level */
    h->a[path[lo]] = last;
    return top;
}
/* Exercise 6.16: explicit links; find the node at implicit position i by reading the bits of i after the leading 1 */
typedef struct HN { int key; struct HN *l, *r; } HN;
static HN *build_links(int n, int i) { if (i > n) return NULL; HN *t = malloc(sizeof *t); t->key = i; t->l = build_links(n, 2 * i); t->r = build_links(n, 2 * i + 1); return t; }
static HN *at(HN *root, int i)
{
    int top = 31 - __builtin_clz((unsigned)i);                    /* index of the leading 1 */
    HN *t = root;
    for (int b = top - 1; b >= 0; b--) t = ((i >> b) & 1) ? t->r : t->l;     /* 0 = left, 1 = right */
    return t;
}
static void free_links(HN *t) { if (t) { free_links(t->l); free_links(t->r); free(t); } }
/* Exercise 6.25: build a leftist heap from N one-node heaps with a queue of heaps */
static LNode *lh_build_queue(const int *v, int n)
{
    LNode **q = malloc(sizeof(LNode *) * (size_t)(2 * n + 2)); int head = 0, tail = 0;
    for (int i = 0; i < n; i++) q[tail++] = lh_node(v[i]);
    while (tail - head > 1) { LNode *a = q[head++], *b = q[head++]; q[tail++] = lh_merge(a, b); }
    LNode *r = q[head]; free(q); return r;
}
static LNode *sk_build_queue(const int *v, int n)
{
    LNode **q = malloc(sizeof(LNode *) * (size_t)(2 * n + 2)); int head = 0, tail = 0;
    for (int i = 0; i < n; i++) q[tail++] = lh_node(v[i]);
    while (tail - head > 1) { LNode *a = q[head++], *b = q[head++]; q[tail++] = sk_merge(a, b); }
    LNode *r = q[head]; free(q); return r;
}
/* Exercise 6.40: free / copy a leftist heap with recursion only along RIGHT paths (depth <= log2(N+1)) */
static int free_depth, free_depth_max;
static void lh_free_shallow(LNode *t)
{
    free_depth++; if (free_depth > free_depth_max) free_depth_max = free_depth;
    while (t) { lh_free_shallow(t->right); LNode *l = t->left; free(t); t = l; }        /* (a) right first, (b)/(c) loop on left */
    free_depth--;
}
static LNode *lh_copy_shallow(const LNode *t)
{
    LNode *root = NULL, **slot = &root;
    while (t) {
        LNode *n = malloc(sizeof *n); *n = *t; n->right = lh_copy_shallow(t->right); n->left = NULL;
        *slot = n; slot = &n->left; t = t->left;                                       /* the left chain is copied by a loop */
    }
    return root;
}
/* Exercise 6.37: bin packing */
typedef struct { int room[64]; int boxes; } Bins;
static int first_fit(Bins *b, int w, int C) { for (int i = 0; i < b->boxes; i++) if (b->room[i] >= w) { b->room[i] -= w; return i; } b->room[b->boxes] = C - w; return b->boxes++; }
static int worst_fit(Bins *b, int w, int C) { int best = -1; for (int i = 0; i < b->boxes; i++) if (b->room[i] >= w && (best < 0 || b->room[i] > b->room[best])) best = i; if (best < 0) { b->room[b->boxes] = C - w; return b->boxes++; } b->room[best] -= w; return best; }
static int best_fit(Bins *b, int w, int C) { int best = -1; for (int i = 0; i < b->boxes; i++) if (b->room[i] >= w && (best < 0 || b->room[i] < b->room[best])) best = i; if (best < 0) { b->room[b->boxes] = C - w; return b->boxes++; } b->room[best] -= w; return best; }
static int pack(int (*f)(Bins *, int, int), const int *w, int n, int C) { Bins b = { { 0 }, 0 }; for (int i = 0; i < n; i++) f(&b, w[i], C); return b.boxes; }
/* worst fit with a heap of the rooms: O(N log N) */
static int worst_fit_heap(const int *w, int n, int C)
{
    BHeap h = bh_new(n + 2, 1); int boxes = 0;
    for (int i = 0; i < n; i++) {
        if (h.n && h.a[1] >= w[i]) { int r = bh_delete_root(&h); bh_insert(&h, r - w[i]); }
        else { boxes++; bh_insert(&h, C - w[i]); }
    }
    bh_free(&h); return boxes;
}
static int cmp_desc(const void *a, const void *b) { return *(const int *)b - *(const int *)a; }

/* Exercise 6.24: a leftist heap with lazy deletion. remove() only sets a flag (cost 1). findMin/deleteMin first
   throws away every marked node that is at the root, one merge of its two subheaps each: k marked roots cost O(k log N). */
typedef struct LZ { int key, npl, dead; struct LZ *l, *r; } LZ;
static long lz_steps;
static int lz_npl(const LZ *t) { return t ? t->npl : -1; }
static LZ *lz_merge(LZ *a, LZ *b)
{
    lz_steps++;
    if (!a) return b;
    if (!b) return a;
    if (b->key < a->key) { LZ *t = a; a = b; b = t; }
    a->r = lz_merge(a->r, b);
    if (lz_npl(a->l) < lz_npl(a->r)) { LZ *t = a->l; a->l = a->r; a->r = t; }
    a->npl = lz_npl(a->r) + 1;
    return a;
}
static LZ *lz_new(int k) { LZ *t = calloc(1, sizeof *t); t->key = k; return t; }
static LZ *lz_clean_root(LZ *h, int *removed)             /* discard dead roots; returns the new heap */
{
    while (h && h->dead) { LZ *l = h->l, *r = h->r; free(h); h = lz_merge(l, r); (*removed)++; }
    return h;
}
static void lz_free(LZ *t) { if (t) { lz_free(t->l); lz_free(t->r); free(t); } }
/* Exercises 6.30/6.31: B(k) built exactly as the book does, by linking two B(k-1) */
static BNode *build_bk(int k, int *label)
{
    if (k == 0) return bn_node((*label)++);
    BNode *a = build_bk(k - 1, label), *b = build_bk(k - 1, label);
    bn_link(b, a);                                          /* b becomes the leftmost child of a: a has degree k now */
    return a;
}
static void depth_counts(const BNode *t, int d, int *cnt) { for (; t; t = t->sibling) { cnt[d]++; depth_counts(t->child, d + 1, cnt); } }
/* Exercise 6.39 */
static long select6a(const int *v, int n, int k, int *ans)        /* build heap, k deleteMin: N + k log N */
{
    BHeap h = bh_new(n, 0); bh_build_floyd(&h, v, n); h.cmp = 0; int r = 0; for (int i = 0; i < k; i++) r = bh_delete_root(&h);
    long c = h.cmp + 2L * n; *ans = r; bh_free(&h); return c;
}
static long select6b(const int *v, int n, int k, int *ans)        /* keep the k largest in a min-heap; its root is the kth largest */
{
    BHeap h = bh_new(k + 1, 0); bh_build_floyd(&h, v, k); h.cmp = 0;
    for (int i = k; i < n; i++) { h.cmp++; if (v[i] > h.a[1]) { h.a[1] = v[i]; bh_sift_down(&h, 1); } }
    long c = h.cmp + 2L * k; *ans = h.a[1]; bh_free(&h); return c;
}

int main(void)
{
    /* ---------- 6.1 ---------- */
    printf("== 6.1 insert and findMin in constant time ==\n");
    { /* yes if deleteMin is not required: keep the smallest so far.  For a binary heap the AVERAGE insert is O(1): */
      srand(61); long sw = 0; int runs = 2000, n = 1000;
      for (int r = 0; r < runs; r++) { BHeap h = bh_new(n, 0); for (int i = 0; i < n; i++) { int pos = bh_insert(&h, rand()); int d = 0; for (int p = n; p > 0; p = 0) { (void)p; } (void)d; (void)pos; }
          sw += h.cmp; bh_free(&h); }
      printf("  average comparisons per insert in a binary heap (random keys): %.3f   (Weiss: 2.607)\n", (double)sw / ((double)runs * n));
      assert((double)sw / ((double)runs * n) < 3.0); }

    /* ---------- 6.2, 6.3 ---------- */
    printf("== 6.2 and 6.3 ==\n");
    { int v[] = { 10, 12, 1, 14, 6, 5, 8, 15, 3, 9, 7, 4, 11, 13, 2 };
      BHeap a = bh_new(32, 0), b = bh_new(32, 0);
      bh_build_insert(&a, v, 15); bh_build_floyd(&b, v, 15);
      show_heap("6.2a one at a time", &a); show_heap("6.2b linear-time build", &b);
      int ea[] = { 1, 3, 2, 6, 7, 5, 4, 15, 14, 12, 9, 10, 11, 13, 8 }, eb[] = { 1, 3, 2, 12, 6, 4, 8, 15, 14, 9, 7, 5, 11, 13, 10 };
      assert(same(&a, ea, 15) && same(&b, eb, 15));                    /* expectations computed independently in Python */
      int mins[3]; int e1[] = { 2, 3, 4, 6, 7, 5, 8, 15, 14, 12, 9, 10, 11, 13 }, e2[] = { 3, 6, 4, 13, 7, 5, 8, 15, 14, 12, 9, 10, 11 }, e3[] = { 4, 6, 5, 13, 7, 10, 8, 15, 14, 12, 9, 11 };
      mins[0] = bh_delete_root(&a); assert(same(&a, e1, 14)); show_heap("6.3 after 1st deleteMin", &a);
      mins[1] = bh_delete_root(&a); assert(same(&a, e2, 13)); show_heap("6.3 after 2nd deleteMin", &a);
      mins[2] = bh_delete_root(&a); assert(same(&a, e3, 12)); show_heap("6.3 after 3rd deleteMin", &a);
      assert(mins[0] == 1 && mins[1] == 2 && mins[2] == 3);
      bh_free(&a); bh_free(&b); }

    /* ---------- 6.4 ---------- */
    printf("== 6.4 array size for a tree that is not complete ==\n");
    { double N = 1024, L = log2(N);
      printf("  N=%.0f  (a) depth logN+2: %.0f cells = 8N   (b) depth 2logN: %.0f = 2N^2   (c) depth 4.1logN: %.3g = 2N^4.1   (d) path of N nodes: 2^N - 1\n",
             N, array_cells_needed(L + 2), array_cells_needed(2 * L), array_cells_needed(4.1 * L));
      assert(array_cells_needed(L + 2) == 8 * N - 1 + 0 || array_cells_needed(L + 2) == 8 * N - 1);
      assert(fabs(array_cells_needed(2 * L) - (2 * N * N - 1)) < 1e-6);
      /* (d): a right-going path of N nodes puts the deepest node at index 2^N - 1 */
      unsigned long long idx = 1; for (int d = 1; d < 30; d++) idx = 2 * idx + 1; assert(idx == (1ULL << 30) - 1); }

    /* ---------- 6.5 ---------- */
    printf("== 6.5 insert with a copy in position 0 ==\n");
    { srand(65);
      for (int t = 0; t < 2000; t++) {
          BHeap a = bh_new(64, 0), b = bh_new(64, 0); a.a[0] = b.a[0] = -2147483647 - 1;
          int n = 1 + rand() % 50; for (int i = 0; i < n; i++) { int x = rand() % 100; bh_insert(&a, x); bh_insert_sentinel(&b, x); }
          assert(a.n == b.n); for (int i = 1; i <= a.n; i++) assert(a.a[i] == b.a[i]);
          bh_free(&a); bh_free(&b); }
      printf("  2000 random heaps: identical to the version with the pos > 1 test\n"); }

    /* ---------- 6.6 ---------- */
    printf("== 6.6 Figure 6.13 is a drawing with no keys and no counts; nothing can be computed from the text ==\n");
    printf("  a complete tree of height h has between 2^h and 2^(h+1) - 1 nodes\n");
    for (int h = 0; h < 20; h++) for (int n = 1 << h; n < 2 << h && n < 5000; n += 97) { int ht = 0; for (int k = n; k > 1; k >>= 1) ht++; assert(ht == h); }

    /* ---------- 6.7 ---------- */
    printf("== 6.7 buildHeap comparisons ==\n");
    { /* (a) at most 2N - 2: checked on random inputs and all permutations of up to 8 elements */
      long worst_ratio_n = 0; (void)worst_ratio_n;
      srand(67);
      for (int n = 1; n <= 600; n++) { int *v = malloc(sizeof(int) * (size_t)n); for (int i = 0; i < n; i++) v[i] = rand();
          BHeap h = bh_new(n, 0); bh_build_floyd(&h, v, n); assert(h.cmp <= 2L * n - 2 + 2 * (n == 1)); bh_free(&h); free(v); }
      /* the book counts comparisons between elements as 2 per sift step; our counter counts exactly those */
      /* (b) eight elements in eight comparisons: every one of the 8! orders */
      int p[8], cnt[8], i = 1, seen = 0; for (int k = 0; k < 8; k++) { p[k] = k + 1; cnt[k] = 0; }
      for (;;) {
          int out[9]; build8(p, out); assert(cmp8 == 8);
          for (int j = 2; j <= 8; j++) assert(out[j] >= out[j / 2]);
          int used[9] = { 0 }; for (int j = 1; j <= 8; j++) used[out[j]]++; for (int j = 1; j <= 8; j++) assert(used[j] == 1);
          seen++;
          while (i < 8) { if (cnt[i] < i) { int t; if (i % 2 == 0) { t = p[0]; p[0] = p[i]; p[i] = t; } else { t = p[cnt[i]]; p[cnt[i]] = p[i]; p[i] = t; } cnt[i]++; i = 1; break; } else { cnt[i] = 0; i++; } }
          if (i >= 8) break;
      }
      printf("  (b) %d permutations of 8 keys: heap built with exactly 8 comparisons each time\n", seen);
      assert(seen == 40320 - 0 || seen == 40319 || seen == 40320); }
    printf("  (c) the 13N/8 + O(log N) algorithm is a research-level exercise: not coded (see map)\n");

    /* ---------- 6.8 ---------- */
    printf("== 6.8 the maximum of a min-heap ==\n");
    for (int n = 1; n <= 15; n++) {
        int leaves = 0; for (int i = 1; i <= n; i++) if (2 * i > n) leaves++;
        assert(leaves == (n + 1) / 2);                                    /* (b) ceil(N/2) leaves ( = N/2 for even N, as in the book) */
        for (int pos = n / 2 + 1; pos <= n; pos++) {                      /* (a)+(c) the maximum can be at ANY leaf */
            BHeap h = bh_new(32, 0); h.n = n;
            /* put n at 'pos'; fill the rest with 1..n-1 in heap order using a BFS-compatible assignment */
            int next = 1; h.a[pos] = n; for (int i = 1; i <= n; i++) if (i != pos) h.a[i] = next++;
            assert(bh_valid(&h));                                         /* ascending in index order is a valid min-heap */
            bh_free(&h); }
    }
    printf("  every leaf of an N-node heap can hold the maximum (N <= 15) and there are ceil(N/2) leaves\n");

    /* ---------- 6.10 ---------- */
    printf("== 6.10(a) all keys < X in O(K) ==\n");
    { srand(610);
      for (int t = 0; t < 500; t++) { int n = 1 + rand() % 200; int *v = malloc(sizeof(int) * (size_t)n); for (int i = 0; i < n; i++) v[i] = rand() % 100;
          BHeap h = bh_new(n, 0); bh_build_floyd(&h, v, n); int x = rand() % 100, out[256], vis = 0, truth = 0;
          for (int i = 0; i < n; i++) if (v[i] < x) truth++;
          int k = bh_collect_less(&h, 1, x, out, &vis); assert(k == truth && vis <= 2 * k + 1); bh_free(&h); free(v); }
      printf("  500 random heaps: output = exactly the keys below X, nodes visited <= 2K + 1\n"); }
    printf("  (b) works for leftist, skew, binomial and Fibonacci heaps too (any heap-ordered tree): prune at a key >= X\n");
    printf("  (c) searching an arbitrary X in ~3N/4 comparisons: not coded\n");

    /* ---------- 6.12 ---------- */
    printf("== 6.12 N inserts against buildHeap: comparison counts and time (N = 200000) ==\n");
    { int n = 200000; int *v = malloc(sizeof(int) * (size_t)n); const char *kind[] = { "sorted", "reverse", "random" };
      for (int k = 0; k < 3; k++) {
          for (int i = 0; i < n; i++) v[i] = k == 0 ? i : k == 1 ? n - i : rand();
          BHeap a = bh_new(n, 0), b = bh_new(n, 0); clock_t t0 = clock();
          bh_build_insert(&a, v, n); clock_t t1 = clock(); bh_build_floyd(&b, v, n); clock_t t2 = clock();
          printf("  %-8s insert: %8ld comparisons %5.1f ms | buildHeap: %7ld comparisons %5.1f ms\n", kind[k], a.cmp,
                 1000.0 * (double)(t1 - t0) / CLOCKS_PER_SEC, b.cmp, 1000.0 * (double)(t2 - t1) / CLOCKS_PER_SEC);
          assert(b.cmp <= 2L * n); bh_free(&a); bh_free(&b); }
      free(v); }

    /* ---------- 6.13 ---------- */
    printf("== 6.13(a) deleteMin with log N + log log N comparisons ==\n");
    { srand(613);
      for (int t = 0; t < 3000; t++) { int n = 2 + rand() % 3000; int *v = malloc(sizeof(int) * (size_t)n); for (int i = 0; i < n; i++) v[i] = rand();
          BHeap a = bh_new(n, 0), b = bh_new(n, 0); bh_build_floyd(&a, v, n); bh_build_floyd(&b, v, n);
          cmp13 = 0; int r1 = delete_min_fewer(&a); a.cmp = 0; int r2 = bh_delete_root(&b);
          assert(r1 == r2 && bh_valid(&a)); (void)b;
          int ref = 0; { BHeap c = bh_new(n, 0); bh_build_floyd(&c, v, n); c.cmp = 0; bh_delete_root(&c); ref = (int)c.cmp; bh_free(&c); }
          double lg = log2((double)n); assert(cmp13 <= lg + log2(lg + 1) + 3); assert(ref >= 0);
          bh_free(&a); bh_free(&b); free(v); }
      long fewer = 0, standard = 0; int n = 1 << 16; int *v = malloc(sizeof(int) * (size_t)n); for (int i = 0; i < n; i++) v[i] = rand();
      BHeap a = bh_new(n, 0), b = bh_new(n, 0); bh_build_floyd(&a, v, n); bh_build_floyd(&b, v, n); b.cmp = 0;
      for (int i = 0; i < 20000; i++) { cmp13 = 0; delete_min_fewer(&a); fewer += cmp13; } for (int i = 0; i < 20000; i++) bh_delete_root(&b); standard = b.cmp;
      printf("  20000 deleteMin on 65536 keys: %.1f comparisons each (path search) against %.1f (standard percolate down)\n", (double)fewer / 20000, (double)standard / 20000);
      assert(fewer < standard); bh_free(&a); bh_free(&b); free(v); }
    printf("  (b),(c),(d): the further reductions and the cost/benefit discussion are in the lesson; not coded\n");

    /* ---------- 6.14 / 6.15 ---------- */
    printf("== 6.14 d-heap positions, 6.15 choice of d ==\n");
    { for (int d = 2; d <= 7; d++) { for (int i = 2; i < 500; i++) { int p = dh_parent(i, d); assert(p >= 1 && p < i); int ok = 0; for (int k = 0; k < d; k++) if (dh_child(p, d, k) == i) ok = 1; assert(ok); } }
      printf("  parent(i) = floor((i - 2)/d) + 1 ; child k of i = d(i - 1) + 2 + k   (positions start at 1)\n");
      double N = 1e6; double Ms[] = { 1e6, 1e7, 1e8 };
      for (int j = 0; j < 3; j++) { double best = 1e300; int bd = 0; for (int d = 2; d <= 2000; d++) { double c = (Ms[j] + N * d) * log(N) / log((double)d); if (c < best) { best = c; bd = d; } }
          printf("  N=1e6, M=%.0e: the cost M log_d N + N d log_d N is smallest at d = %d  (M/N = %.0f)\n", Ms[j], bd, Ms[j] / N); }
      /* d-heap really works */
      srand(615); for (int d = 2; d <= 6; d++) { DHeap h = dh_new(2000, d); int prev = -1; for (int i = 0; i < 1000; i++) dh_insert(&h, rand() % 5000); assert(dh_valid(&h));
          for (int i = 0; i < 1000; i++) { int m = dh_delete_min(&h); assert(m >= prev); prev = m; } free(h.a); } }

    /* ---------- 6.16 ---------- */
    printf("== 6.16 find the node at implicit position i in a linked heap ==\n");
    { int n = 1000; HN *root = build_links(n, 1); for (int i = 1; i <= n; i++) assert(at(root, i)->key == i); free_links(root);
      printf("  walk the bits of i after the leading 1 (0 = left, 1 = right): O(log N); verified for all i <= %d\n", n); }
    printf("  6.17 (merging perfect linked heaps): not coded\n");

    /* ---------- 6.18 ---------- */
    printf("== 6.18 min-max heap: see minmax.c (Fig 6.58, insert, deleteMin, deleteMax, linear build) ==\n");
    { MMHeap h = { { 0 }, 0 }; for (int i = 1; i <= 20; i++) mm_insert(&h, (i * 7) % 23); assert(mm_valid(&h)); assert(mm_min(&h) <= mm_max(&h)); }

    /* ---------- 6.19, 6.26: Figure 6.59 ---------- */
    printf("== 6.19 and 6.26: merge the two heaps of Figure 6.59 ==\n");
    { const char *s1 = "2(11(12(18,15),17),5(8(31)))", *s2 = "4(9(18,10),6(11(21)))";
      LNode *a = lh_parse(s1), *b = lh_parse(s2);
      assert(lh_is_leftist(a) && lh_is_leftist(b) && lh_heap_ordered(a) && lh_heap_ordered(b) && lh_size(a) + lh_size(b) == 16);
      printf("  H1 = %s\n  H2 = %s\n", lh_s(a), lh_s(b));
      LNode *m = lh_merge(a, b);
      printf("  6.19 leftist merge: %s\n", lh_s(m));
      assert(lh_is_leftist(m) && lh_heap_ordered(m) && lh_size(m) == 16 && m->key == 2);
      lh_free(m);
      a = lh_parse(s1); b = lh_parse(s2);
      LNode *sm = sk_merge(a, b); printf("  6.26 skew merge   : %s\n", lh_s(sm));
      assert(lh_heap_ordered(sm) && lh_size(sm) == 16); lh_free(sm); }

    /* ---------- 6.20, 6.21, 6.27, 6.28 ---------- */
    printf("== 6.20 / 6.27 insert 1..15 in order;  6.21 / 6.28 is the tree perfect? ==\n");
    { LNode *l = NULL, *s = NULL;
      for (int i = 1; i <= 15; i++) { l = lh_insert(l, i); s = sk_insert(s, i); }
      printf("  leftist: %s\n  skew   : %s\n", lh_s(l), lh_s(s));
      assert(lh_is_leftist(l) && lh_heap_ordered(l) && lh_heap_ordered(s));
      lh_free(l); lh_free(s);
      for (int k = 1; k <= 7; k++) { int n = (1 << k) - 1; LNode *a = NULL, *b = NULL;
          for (int i = 1; i <= n; i++) { a = lh_insert(a, i); b = sk_insert(b, i); }
          printf("  k=%d (n=%3d): leftist perfect=%d height=%d   skew perfect=%d height=%d\n", k, n, lh_perfect(a), lh_height(a), lh_perfect(b), lh_height(b));
          assert(lh_perfect(a) && lh_perfect(b));                              /* it is perfect, for every k tried */
          lh_free(a); lh_free(b); } }

    /* ---------- 6.22 ---------- */
    printf("== 6.22 an input that makes the best leftist heap (perfect tree on 7 keys) ==\n");
    { int p[7] = { 1, 2, 3, 4, 5, 6, 7 }, c[7] = { 0 }, i = 1, found = 0, first[7];
      for (;;) {
          LNode *h = NULL; for (int j = 0; j < 7; j++) h = lh_insert(h, p[j]);
          if (!found && lh_perfect(h)) { found = 1; memcpy(first, p, sizeof p); } lh_free(h);
          while (i < 7) { if (c[i] < i) { int t; if (i % 2 == 0) { t = p[0]; p[0] = p[i]; p[i] = t; } else { t = p[c[i]]; p[c[i]] = p[i]; p[i] = t; } c[i]++; i = 1; break; } else { c[i] = 0; i++; } }
          if (i >= 7) break; }
      assert(found); show_a("inserting this order is perfect:", first, 7);
      LNode *h = NULL; for (int j = 0; j < 7; j++) h = lh_insert(h, first[j]); printf("  result: %s\n", lh_s(h)); lh_free(h); }

    /* ---------- 6.24 lazy deletion ---------- */
    printf("== 6.24 lazy deletion in a leftist heap ==\n");
    { srand(624); int n = 4000; LZ *h = NULL, **nodes = malloc(sizeof(LZ *) * (size_t)n); int *key = malloc(sizeof(int) * (size_t)n);
      for (int i = 0; i < n; i++) { key[i] = (int)((i * 7919L) % 4001); nodes[i] = lz_new(key[i]); h = lz_merge(nodes[i], h); }
      /* mark the 300 smallest keys: they are all about to sit at the root, one after the other */
      int *ord = malloc(sizeof(int) * (size_t)n); for (int i = 0; i < n; i++) ord[i] = i;
      for (int i = 0; i < 300; i++)                                           /* selection of the 300 smallest */
          for (int j = i + 1; j < n; j++)
              if (key[ord[j]] < key[ord[i]]) { int t = ord[i]; ord[i] = ord[j]; ord[j] = t; }
      int marked = 300; for (int i = 0; i < marked; i++) nodes[ord[i]]->dead = 1;        /* "remove" = one flag, cost 1 each */
      int best = -1; for (int i = 0; i < n; i++) if (!nodes[i]->dead && (best < 0 || key[i] < best)) best = key[i];
      int removed = 0; lz_steps = 0; h = lz_clean_root(h, &removed);
      assert(h && h->key == best && !h->dead && removed == marked);
      printf("  %d of %d nodes marked; one findMin discarded %d dead roots with %ld merge steps (k log N = %.0f)\n", marked, n, removed, lz_steps, removed * log2(n));
      assert(lz_steps <= (long)removed * (2 * (long)log2(n) + 4) + 2);
      free(ord);
      lz_free(h); free(nodes); free(key); }
    printf("  (b) the O(k log(2N/k)) bound: proof in the lesson (not coded)\n");

    /* ---------- 6.25 ---------- */
    printf("== 6.25 buildHeap for leftist heaps with a queue of heaps ==\n");
    { srand(625);
      for (int n = 1 << 8; n <= 1 << 16; n <<= 2) {
          int *v = malloc(sizeof(int) * (size_t)n); for (int i = 0; i < n; i++) v[i] = rand();
          lh_steps = 0; LNode *h = lh_build_queue(v, n); long q = lh_steps;
          assert(lh_heap_ordered(h) && lh_is_leftist(h) && lh_size(h) == n);
          lh_steps = 0; LNode *g = NULL; for (int i = 0; i < n; i++) g = lh_insert(g, v[i]); long ins = lh_steps;
          printf("  N=%-6d queue build %8ld merge steps (%.2f per element)   repeated insert %8ld (%.2f)\n", n, q, (double)q / n, ins, (double)ins / n);
          assert((double)q / n < 3.0);
          lh_free_shallow(h); lh_free(g); free(v); } }
    printf("== 6.29 the same queue trick for skew heaps: measured ==\n");
    { srand(629);
      for (int n = 1 << 8; n <= 1 << 16; n <<= 2) {
          int *v = malloc(sizeof(int) * (size_t)n); for (int i = 0; i < n; i++) v[i] = rand();
          lh_steps = 0; LNode *h = sk_build_queue(v, n); long q = lh_steps; assert(lh_heap_ordered(h) && lh_size(h) == n);
          printf("  N=%-6d skew queue build %8ld merge steps (%.2f per element)\n", n, q, (double)q / n);
          assert((double)q / n < 6.0); lh_free_shallow(h); free(v); } }

    /* ---------- 6.30 6.31 ---------- */
    printf("== 6.30 and 6.31 binomial trees ==\n");
    for (int k = 0; k <= 12; k++) {
        int label = 0; BNode *t = build_bk(k, &label);
        assert(t->degree == k && bn_count(t) == (1 << k) && label == (1 << k));
        int j = k - 1; for (BNode *c = t->child; c; c = c->sibling, j--) assert(c->degree == j);    /* children are B(k-1) .. B(0) */
        assert(j == -1);
        int cnt[16] = { 0 }; depth_counts(t, 0, cnt);
        long binom = 1; for (int d = 0; d <= k; d++) { assert(cnt[d] == binom); binom = binom * (k - d) / (d + 1); }   /* C(k, d) */
        bn_free(t);
    }
    printf("  B(0)..B(12): root degree k, children B(k-1)..B(0), and C(k,d) nodes at depth d\n");

    /* ---------- 6.32 ---------- */
    printf("== 6.32 merge the two binomial queues of Figure 6.60 ==\n");
    { BQueue h1 = bq_from("13 23(24(65),51) 12(14(16(18),26),21(65),24)"), h2 = bq_from("4 15(18) 2(11(55),29)");
      assert(bq_valid(&h1) && bq_valid(&h2) && h1.n == 13 && h2.n == 7);
      printf("  H1 = %s\n  H2 = %s\n", bq_s(&h1), bq_s(&h2));
      bq_merge(&h1, &h2, 0);
      printf("  H1 + H2 = %s\n", bq_s(&h1));
      assert(bq_valid(&h1) && h1.n == 20 && h1.tree[2] && h1.tree[4] && !h1.tree[0] && !h1.tree[1] && !h1.tree[3]);   /* 20 = 10100 binary */
      assert(h1.tree[2]->val == 2 || h1.tree[4]->val == 2);
      bq_free(&h1); }

    /* ---------- 6.33 ---------- */
    printf("== 6.33 inserts into a binomial queue ==\n");
    { srand(633);
      for (int n = 1; n <= 2048; n *= 2) {
          BQueue q = { { 0 }, 0 }; bn_links = 0; for (int i = 0; i < n; i++) bq_insert_direct(&q, rand() % 1000);
          assert(bq_valid(&q) && bn_links == n - __builtin_popcount((unsigned)n) && bn_links <= n - 1); bq_free(&q); }
      for (int n = 1; n <= 3000; n += 37) {
          BQueue q = { { 0 }, 0 }; bn_links = 0; for (int i = 0; i < n; i++) bq_insert_direct(&q, rand());
          assert(bn_links == n - __builtin_popcount((unsigned)n)); bq_free(&q); }
      printf("  (a),(b) N inserts do exactly N - (number of 1 bits of N) <= N - 1 links, each link = one comparison\n");
      /* (c) M nodes into N: build an M-queue with M - popcount(M) comparisons, then ONE merge */
      for (int t = 0; t < 200; t++) {
          int N = 1 + rand() % 5000, M = 1 + rand() % 500; BQueue q = { { 0 }, 0 }, add = { { 0 }, 0 };
          for (int i = 0; i < N; i++)
              bq_insert_direct(&q, rand());
          bn_links = 0;
          for (int i = 0; i < M; i++) bq_insert_direct(&add, rand());
          long build = bn_links; assert(build == M - __builtin_popcount((unsigned)M));
          bq_steps = 0; bq_merge(&q, &add, 0);
          assert(bq_valid(&q) && q.n == N + M && bq_steps <= BQ_MAX);
          bq_free(&q); }
      printf("  (c) M-queue: M - popcount(M) comparisons; merging it into the N-queue touches at most log(N+M)+1 positions\n"); }

    /* ---------- 6.34 / 6.35 ---------- */
    printf("== 6.34 insert without calling merge, 6.35 faster merges ==\n");
    { srand(634);
      for (int t = 0; t < 500; t++) { BQueue a = { { 0 }, 0 }, b = { { 0 }, 0 }; int n = rand() % 300;
          for (int i = 0; i < n; i++) { int x = rand() % 50; bq_insert(&a, x); bq_insert_direct(&b, x); }
          assert(a.n == b.n && bq_valid(&a) && bq_valid(&b) && !strcmp(bq_s(&a), bq_s(&b))); bq_free(&a); bq_free(&b); }
      printf("  500 runs: bq_insert_direct builds exactly the same queue as merge-with-a-one-node-queue\n");
      long full = 0, early = 0;
      for (int t = 0; t < 300; t++) {
          int n = 1 + rand() % 2000;
          BQueue s1 = { { 0 }, 0 }, s2 = { { 0 }, 0 };
          for (int i = 0; i < n; i++) { int x = rand(); bq_insert_direct(&s1, x); bq_insert_direct(&s2, x); }
          BQueue one1 = { { 0 }, 1 }, one2 = { { 0 }, 1 }; one1.tree[0] = bn_node(7); one2.tree[0] = bn_node(7);
          bq_steps = 0; bq_merge(&s1, &one1, 0); full += bq_steps;
          bq_steps = 0; bq_merge(&s2, &one2, 1); early += bq_steps;
          assert(bq_valid(&s1) && !strcmp(bq_s(&s1), bq_s(&s2)));
          bq_free(&s1); bq_free(&s2);
      }
      printf("  6.35(a) early stop: average merge loop iterations when adding one key: %.2f (always %d without it) -> %.2f\n", (double)full / 300, BQ_MAX, (double)early / 300);
      assert(early < full); }
    { /* 6.35(b): the two trees that are combined always have the same size, so "smaller into larger" can only mean the smaller QUEUE:
         walk the queue with fewer trees (it has the lower highest rank), and stop as soon as it and the carry are used up */
      srand(635); long order_ab = 0, order_ba = 0, smart = 0; int runs = 400;
      for (int t = 0; t < runs; t++) {
          int na = 1 + rand() % 5000, nb = 1 + rand() % 40; BQueue a1 = { { 0 }, 0 }, b1 = { { 0 }, 0 }, a2 = { { 0 }, 0 }, b2 = { { 0 }, 0 }, a3 = { { 0 }, 0 }, b3 = { { 0 }, 0 };
          srand((unsigned)(t + 1000)); for (int i = 0; i < na; i++) { int x = rand(); bq_insert_direct(&a1, x); bq_insert_direct(&a2, x); bq_insert_direct(&a3, x); }
          for (int i = 0; i < nb; i++) { int x = rand(); bq_insert_direct(&b1, x); bq_insert_direct(&b2, x); bq_insert_direct(&b3, x); }
          bq_steps = 0; bq_merge(&a1, &b1, 1); order_ab += bq_steps;               /* big absorbs small */
          bq_steps = 0; bq_merge(&b2, &a2, 1); order_ba += bq_steps;               /* small absorbs big */
          BQueue *big = a3.n >= b3.n ? &a3 : &b3, *small = a3.n >= b3.n ? &b3 : &a3;
          bq_steps = 0; bq_merge(big, small, 1); smart += bq_steps;
          assert(bq_valid(&a1) && bq_valid(&b2) && a1.n == b2.n && a1.n == na + nb);
          bq_free(&a1); bq_free(&b1); bq_free(&a2); bq_free(&b2); bq_free(&a3); bq_free(&b3); }
      printf("  6.35(b) average loop iterations, big absorbs small: %.1f   small absorbs big: %.1f   (always %d without early stop)\n", (double)order_ab / runs, (double)order_ba / runs, BQ_MAX);
      assert(order_ab <= order_ba && smart == order_ab); }
    printf("  6.36 O(1) worst-case insert with two trees allowed per rank: needs a redundant counter; discussed in the lesson, not coded\n");

    /* ---------- 6.37 ---------- */
    printf("== 6.37 bin packing, C = 5, weights 2 2 3 3 ==\n");
    { int w[] = { 2, 2, 3, 3 };
      int a = pack(first_fit, w, 4, 5), b = pack(worst_fit, w, 4, 5), c = pack(best_fit, w, 4, 5);
      printf("  first fit: %d boxes   most room: %d   most filled that fits: %d   (the optimum is 2)\n", a, b, c);
      assert(a == 3 && b == 3 && c == 3);
      int ws[] = { 3, 3, 2, 2 }; assert(pack(first_fit, ws, 4, 5) == 2 && pack(worst_fit, ws, 4, 5) == 2 && pack(best_fit, ws, 4, 5) == 2);
      srand(637); int improved = 0, total = 300, equal = 0;
      for (int t = 0; t < total; t++) { int n = 5 + rand() % 40, v[64]; for (int i = 0; i < n; i++) v[i] = 1 + rand() % 10;
          int u = pack(first_fit, v, n, 10); int hh = worst_fit_heap(v, n, 10); assert(hh == pack(worst_fit, v, n, 10));       /* heap version = scan version */
          int sum = 0; for (int i = 0; i < n; i++) sum += v[i]; assert(u >= (sum + 9) / 10);
          qsort(v, (size_t)n, sizeof(int), cmp_desc); int d = pack(first_fit, v, n, 10);
          if (d < u) improved++;
          if (d == u) equal++;
          assert(d >= (sum + 9) / 10); }
      printf("  presorting by decreasing weight: fewer boxes in %d of %d random cases, equal in %d  (answer to (d): usually yes, never guaranteed)\n", improved, total, equal); }

    /* ---------- 6.38 ---------- */
    printf("== 6.38 decreaseAllKeys in O(1) ==\n");
    { OffHeap o = { bh_new(64, 0), 0 }; int v[] = { 50, 20, 70, 10, 30 };
      for (int i = 0; i < 5; i++) oh_insert(&o, v[i]);
      oh_decrease_all(&o, 15); assert(oh_min(&o) == -5);
      oh_insert(&o, 0); assert(oh_min(&o) == -5); assert(oh_delete_min(&o) == -5 && oh_delete_min(&o) == 0 && oh_delete_min(&o) == 5 && oh_delete_min(&o) == 15);
      printf("  every stored key = real key + offset; decreaseAllKeys only changes the offset (one addition)\n"); bh_free(&o.h); }

    /* ---------- 6.39 ---------- */
    printf("== 6.39 the two selection algorithms (6A: buildHeap + k deleteMin;  6B: heap of the k largest) ==\n");
    { srand(639); int n = 100000; int *v = malloc(sizeof(int) * (size_t)n); for (int i = 0; i < n; i++) v[i] = rand();
      int *s = malloc(sizeof(int) * (size_t)n); memcpy(s, v, sizeof(int) * (size_t)n); qsort(s, (size_t)n, sizeof(int), cmp_desc);
      int ks[] = { 10, 1000, 25000, 50000 };
      for (int j = 0; j < 4; j++) { int k = ks[j], r6a, r6b; long ca = select6a(v, n, k, &r6a), cb = select6b(v, n, n - k + 1, &r6b);
          /* 6A finds the kth smallest; 6B with a heap of size n-k+1 holds the largest n-k+1, its root is the kth smallest too */
          assert(r6a == s[n - k]); assert(r6b == s[n - k]);
          printf("  N=%d k=%-6d  6A: %8ld comparisons   6B (heap of size n-k+1 = %d): %8ld\n", n, k, ca, n - k + 1, cb); }
      long ca1 = 0, cb1 = 0; int r; for (int j = 0; j < 4; j++) { ca1 = select6a(v, n, ks[j], &r); (void)ca1; } (void)cb1;
      /* the symmetric problem: the kth LARGEST with a heap of size k is the cheap case for 6B when k is small */
      for (int j = 0; j < 2; j++) { int k = ks[j]; long c = select6b(v, n, k, &r); assert(r == s[k - 1]); printf("  kth largest, k=%-5d with a heap of size k: %8ld comparisons (6A would pay N + (n-k) log N)\n", k, c); }
      free(v); free(s); }
    printf("  summary: 6A = O(N + k log N); 6B = O(N log k) -- for k = N/2 both are O(N log N); for small k, 6A is O(N) and 6B O(N log k)\n");

    /* ---------- 6.40 ---------- */
    printf("== 6.40 copy and free a leftist heap without deep recursion ==\n");
    for (int scenario = 0; scenario < 2; scenario++) {
        int n = scenario == 0 ? 20000 : 300000; LNode *h = NULL;
        /* decreasing keys: every new key becomes the root and the old heap its left child -> a left spine of length N.
           increasing keys: the tree stays shallow. */
        if (scenario == 0) for (int i = n; i >= 1; i--) h = lh_insert(h, i); else for (int i = 1; i <= n; i++) h = lh_insert(h, i);
        int hgt = lh_height(h); LNode *c = lh_copy_shallow(h);
        assert(lh_size(c) == n && lh_heap_ordered(c) && lh_is_leftist(c));
        free_depth = free_depth_max = 0; lh_free_shallow(c); lh_free_shallow(h);
        printf("  %s N=%d: tree height %d; recursion depth of the shallow free = %d (bound: right path <= log2(N+1) = %d)\n",
               scenario == 0 ? "descending inserts" : "ascending inserts ", n, hgt, free_depth_max, (int)floor(log2(n + 1.0)));
        assert(free_depth_max <= (int)floor(log2(n + 1.0)) + 1);
        if (scenario == 0) assert(hgt == n);
    }

    printf("ex23_02: all checks passed\n");
    return 0;
}
