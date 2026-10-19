/* ============================================================
 *  st_elementary.c  —  The symbol-table ADT and its four elementary
 *                      implementations
 *  Chapter 12: Symbol Tables and BSTs (Sections 12.1-12.4)
 *
 *  Program 12.1: the ST interface: init, count, insert, search,
 *  delete, select (k-th smallest), sort (visit in order).
 *  Implementations (Table 12.1 costs, N items):
 *                        insert   search hit/miss   select  sort
 *    key-indexed array     1          1    1           M      M
 *    ordered array         N        lg N lg N          1      N
 *    ordered linked list   N        N/2  N/2           N      N
 *    unordered list        1        N/2  N             -    N lg N
 *  - Program 12.3: key-indexed (keys are small ints < M), O(1) ops
 *  - Program 12.4/12.6: ordered array + binary search
 *  - Program 12.5: unordered linked list (sequential search)
 *  - interpolation search (Section 12.4): guess the position from the
 *    key VALUE: ~lg lg N probes for uniformly distributed keys.
 *  Counts of comparisons are printed to match Properties 12.2-12.5.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 12.1-12.6
 *  Build:   gcc -std=c99 -Wall -O2 st_elementary.c -o st_elementary -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef int Key;
typedef struct { Key key; int val; } Item;
static Item NULLitem = {-1, -1};
static long cmps;
#define eq(A, B)   (cmps++, (A) == (B))
#define less(A, B) (cmps++, (A) < (B))

/* ---------- Program 12.3: key-indexed array ---------- */
#define M 1000
static Item kst[M];
void KSTinit(void) { for (int i = 0; i < M; i++) kst[i] = NULLitem; }
void KSTinsert(Item x) { kst[x.key] = x; }
Item KSTsearch(Key v) { return kst[v]; }
void KSTdelete(Key v) { kst[v] = NULLitem; }
Item KSTselect(int k) { for (int i = 0; i < M; i++) if (kst[i].key != -1) if (k-- == 0) return kst[i]; return NULLitem; }

/* ---------- Programs 12.4 + 12.6: ordered array + binary search ---------- */
static Item *ast; static int an;
void ASTinit(int maxN) { ast = malloc(maxN * sizeof *ast); an = 0; }
void ASTinsert(Item x) {                        /* shift larger keys right: O(N) */
    int j = an++;
    while (j > 0 && less(x.key, ast[j - 1].key)) { ast[j] = ast[j - 1]; j--; }
    ast[j] = x;
}
Item ASTsearch(Key v) {
    int l = 0, r = an - 1;
    while (r >= l) {
        int m = (l + r) / 2;
        if (eq(v, ast[m].key)) return ast[m];
        if (less(v, ast[m].key)) r = m - 1; else l = m + 1;
    }
    return NULLitem;
}
Item ASTselect(int k) { return ast[k]; }       /* O(1): the array is the order */
Item ASTinterp(Key v) {                         /* interpolation search */
    int l = 0, r = an - 1;
    while (r >= l && v >= ast[l].key && v <= ast[r].key) {
        int m = ast[r].key == ast[l].key ? l
              : l + (int)((double)(v - ast[l].key) * (r - l) / (ast[r].key - ast[l].key));
        if (eq(v, ast[m].key)) return ast[m];
        if (less(v, ast[m].key)) r = m - 1; else l = m + 1;
    }
    return NULLitem;
}

/* ---------- Program 12.5: unordered linked list ---------- */
typedef struct STnode *link;
struct STnode { Item item; link next; };
static link lhead; static int ln;
void LSTinit(void) { lhead = NULL; ln = 0; }
void LSTinsert(Item x) { link t = malloc(sizeof *t); t->item = x; t->next = lhead; lhead = t; ln++; }  /* O(1) */
Item LSTsearch(Key v) { for (link t = lhead; t; t = t->next) if (eq(t->item.key, v)) return t->item; return NULLitem; }

int main(void) {
    const int N = 1000;
    /* distinct random keys 0..999 in random order */
    int *keys = malloc(N * sizeof *keys);
    for (int i = 0; i < N; i++) keys[i] = i;
    srand(12); for (int i = N - 1; i > 0; i--) { int j = rand() % (i + 1), t = keys[i]; keys[i] = keys[j]; keys[j] = t; }

    KSTinit(); ASTinit(N); LSTinit();
    for (int i = 0; i < N / 2; i++) { Item x = {keys[i], i}; KSTinsert(x); ASTinsert(x); LSTinsert(x); }
    int n = N / 2;
    cmps = 0; for (int i = 0; i < n; i++) LSTsearch(keys[i]);
    printf("unordered list : hit  avg %.1f compares (N/2 = %d)\n", (double)cmps / n, n / 2);
    cmps = 0; for (int i = n; i < N; i++) LSTsearch(keys[i]);
    printf("unordered list : miss avg %.1f compares (N = %d)\n", (double)cmps / n, n);
    cmps = 0; for (int i = 0; i < n; i++) ASTsearch(keys[i]);
    printf("binary search  : hit  avg %.1f compares (lg N = %.1f; counts 2 per probe here)\n", (double)cmps / n, log2(n));
    cmps = 0; for (int i = 0; i < n; i++) ASTinterp(keys[i]);
    printf("interpolation  : hit  avg %.1f compares (lg lg N = %.1f)\n", (double)cmps / n, log2(log2(n)));
    printf("key-indexed    : search %d -> val %d in ONE array access\n", keys[7], KSTsearch(keys[7]).val);
    printf("select(10): ordered array -> key %d, key-indexed -> key %d\n", ASTselect(10).key, KSTselect(10).key);
    KSTdelete(keys[7]);
    printf("after delete, key-indexed search %d -> %s\n", keys[7], KSTsearch(keys[7]).key == -1 ? "miss" : "hit");
    while (lhead) { link t = lhead->next; free(lhead); lhead = t; }
    free(ast); free(keys);
    return 0;
}
