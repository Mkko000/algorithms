/* ============================================================
 *  recursive_examples.c  —  The small recursive programs of 5.1-5.2
 *  Chapter 5: Recursion and Trees (Sections 5.1, 5.2)
 *
 *   - Program 5.2: a "questionable" recursive function (3n+1 / Collatz):
 *     nobody has proved it terminates for every N — a reminder that a
 *     recursive program needs an argument that it reaches the base case.
 *   - Program 5.4: evaluate a PREFIX expression with one recursive
 *     function (the recursion IS the stack).
 *   - Program 5.5: recursive functions on linked lists: count, traverse,
 *     traverse in reverse, delete all nodes with a given item.
 *   - Program 5.6: divide and conquer to find the maximum; the
 *     recursion tree has N leaves and N-1 internal nodes (Property 5.1).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 5.2, 5.4-5.6
 *  Build:   gcc -std=c99 -Wall -O2 recursive_examples.c -o recursive_examples
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

/* ---- Program 5.2: does it always stop? (believed yes, unproved) ---- */
int puzzle_calls;
int puzzle(int N) {
    puzzle_calls++;
    if (N == 1) return 1;
    if (N % 2 == 0) return puzzle(N / 2);
    else            return puzzle(3 * N + 1);
}

/* ---- Program 5.4: prefix expression evaluation ---- */
static const char *a; static int i;
int eval(void) {
    int x = 0;
    while (a[i] == ' ') i++;
    if (a[i] == '+') { i++; return eval() + eval(); }
    if (a[i] == '*') { i++; return eval() * eval(); }
    while (a[i] >= '0' && a[i] <= '9') x = 10 * x + (a[i++] - '0');
    return x;
}

/* ---- Program 5.5: recursion on linked lists ---- */
typedef struct node *link;
struct node { int item; link next; };
link cons(int v, link n) { link x = malloc(sizeof *x); x->item = v; x->next = n; return x; }
int count(link x) { if (x == NULL) return 0; return 1 + count(x->next); }
void traverse(link h, void (*visit)(link)) { if (h == NULL) return; (*visit)(h); traverse(h->next, visit); }
void traverseR(link h, void (*visit)(link)) { if (h == NULL) return; traverseR(h->next, visit); (*visit)(h); }
link delete_all(link x, int v) {
    if (x == NULL) return NULL;
    if (x->item == v) { link t = x->next; free(x); return delete_all(t, v); }
    x->next = delete_all(x->next, v);
    return x;
}
void pr(link x) { printf(" %d", x->item); }

/* ---- Program 5.6: divide-and-conquer maximum ---- */
int calls;
int max(int b[], int l, int r) {
    calls++;
    if (l == r) return b[l];
    int m = (l + r) / 2;
    int u = max(b, l, m), v = max(b, m + 1, r);
    return u > v ? u : v;
}

int main(void) {
    for (int n = 3; n <= 27; n += 12) {
        puzzle_calls = 0; puzzle(n);
        printf("puzzle(%d) reached 1 after %d calls\n", n, puzzle_calls);
    }
    a = "* + 7 * * 4 6 + 8 9 5"; i = 0;
    printf("prefix \"%s\" = %d\n", a, eval());          /* (7 + 4*6*(8+9)) * 5 = 2075 */

    link L = NULL; int v[] = {5, 1, 5, 2, 5, 3};
    for (int k = 5; k >= 0; k--) L = cons(v[k], L);
    printf("count=%d  forward:", count(L)); traverse(L, pr);
    printf("  reverse:"); traverseR(L, pr); putchar('\n');
    L = delete_all(L, 5);
    printf("after delete 5:"); traverse(L, pr); printf("  (count=%d)\n", count(L));
    while (L) { link t = L->next; free(L); L = t; }

    int b[11] = {3, 9, 1, 4, 7, 12, 2, 8, 6, 5, 0}; calls = 0;
    int mx = max(b, 0, 10);                 /* call first: argument order is unspecified */
    printf("max = %d, recursive calls = %d (= 2N-1 for N=11)\n", mx, calls);
    return 0;
}
