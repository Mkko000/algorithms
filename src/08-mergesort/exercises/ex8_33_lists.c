/* ============================================================
 *  ex8_33_lists.c  —  Exercises 8.33, 8.35, 8.36, 8.38
 *  Chapter 8: Merging and Mergesort — linked-list mergesort
 *
 *  8.33 Top-down list mergesort that carries the list length as a
 *       parameter and uses it to split the list.
 *  8.35 Add a cutoff for small subfiles to Program 8.7 (insertion sort on
 *       short lists); measure how much the right cutoff helps.
 *  8.36 Bottom-up mergesort on a circular list (one pass = walk the circle
 *       merging pairs of runs of length m).
 *  8.38 Add a cutoff to the bottom-up queue version (Program 8.8): start
 *       the queue with sorted runs of length M instead of 1.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 8.6-8.8, Exercises 8.33-8.38
 *  Build:   gcc -std=c99 -Wall -O2 ex8_33_lists.c -o ex8_33
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct node *link;
struct node { int item; link next; };

static link merge(link a, link b)                        /* Program 8.6 (stable) */
{
    struct node head; link c = &head;
    while (a != NULL && b != NULL)
        if (a->item <= b->item) { c->next = a; c = a; a = a->next; }
        else { c->next = b; c = b; b = b->next; }
    c->next = (a == NULL) ? b : a;
    return head.next;
}
static link insertion(link t)                            /* sort a short list */
{
    struct node head; head.next = NULL;
    while (t) {
        link u = t->next, p = &head;
        while (p->next && p->next->item <= t->item) p = p->next;   /* <= keeps it stable */
        t->next = p->next; p->next = t; t = u;
    }
    return head.next;
}
/* 8.33 (+ 8.35 cutoff M): sort the first n nodes of t; *rest gets node n+1 */
static link sortn(link t, int n, link *rest, int M)
{
    if (n <= M || n == 1) {
        link p = t; for (int i = 1; i < n; i++) p = p->next;
        *rest = p->next; p->next = NULL;
        return M > 1 ? insertion(t) : t;
    }
    link mid, end;
    link a = sortn(t, n / 2, &mid, M);
    link b = sortn(mid, n - n / 2, &end, M);
    *rest = end;
    return merge(a, b);
}
/* 8.36: bottom-up on a circular list; x points to the LAST node */
static link circular_sort(link x, int n)
{
    for (int m = 1; m < n; m += m) {
        link last = x, first = x->next; last->next = NULL;          /* open the circle */
        struct node head; link tail = &head, t = first;
        while (t) {
            link a = t, b, p = a;
            for (int i = 1; i < m && p->next; i++) p = p->next;
            b = p->next; p->next = NULL;
            p = b; for (int i = 1; i < m && p && p->next; i++) p = p->next;
            if (p) { t = p->next; p->next = NULL; } else t = NULL;
            tail->next = merge(a, b);
            while (tail->next) tail = tail->next;
        }
        tail->next = head.next; x = tail;                            /* close it again */
    }
    return x;
}
/* 8.38: Program 8.8 with runs of length M; a simple array-based queue of lists */
static link Q[1 << 21]; static int qh, qt;
static void put(link t) { Q[qt++ & ((1 << 21) - 1)] = t; }
static link get(void) { return Q[qh++ & ((1 << 21) - 1)]; }
static link queue_sort(link t, int M)
{
    qh = qt = 0;
    while (t) {
        link a = t, p = t;
        for (int i = 1; i < M && p->next; i++) p = p->next;
        t = p->next; p->next = NULL;
        put(M > 1 ? insertion(a) : a);
    }
    t = get();
    while (qh != qt) { put(t); t = merge(get(), get()); }
    return t;
}

static unsigned long long rs = 4242;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }
static struct node pool[1000000];
static link build(int n, const int *v) { for (int i = 0; i < n; i++) { pool[i].item = v[i]; pool[i].next = i + 1 < n ? &pool[i + 1] : NULL; } return &pool[0]; }
static int check(link t, int n) { int c = 0; for (link p = t; p; p = p->next, c++) if (p->next && p->next->item < p->item) return 0; return c == n; }

int main(void)
{
    static int v[1000000]; int N = 1000000;
    const char *s = "EASYQUESTION"; int sv[12];
    for (int i = 0; i < 12; i++) sv[i] = s[i];
    link t = sortn(build(12, sv), 12, &(link){ NULL }, 1);
    printf("8.33 length-driven top-down list sort of EASYQUESTION: ");
    for (link p = t; p; p = p->next) printf("%c", p->item);
    printf("\n");
    t = build(12, sv); pool[11].next = &pool[0];
    link x = circular_sort(&pool[11], 12);
    printf("8.36 circular bottom-up sort: ");
    link p = x->next; for (int i = 0; i < 12; i++, p = p->next) printf("%c", p->item);
    printf("   (x points at the last node: %c, circle closed: %s)\n", x->item, x->next->item == 'A' ? "yes" : "NO");

    for (int i = 0; i < N; i++) v[i] = (int)rnd();
    printf("8.35 top-down list mergesort, N=%d, time (ms) by cutoff M:\n    ", N);
    int Ms[] = { 1, 4, 6, 8, 10, 12, 16, 24, 32 };
    for (int k = 0; k < 9; k++) {
        double best = 1e9;
        for (int rep = 0; rep < 3; rep++) {
            link h = build(N, v), rest; clock_t c0 = clock();
            h = sortn(h, N, &rest, Ms[k]);
            double ms = (double)(clock() - c0) * 1000 / CLOCKS_PER_SEC;
            if (!check(h, N)) { printf("BAD\n"); return 1; }
            if (ms < best) best = ms;
        }
        printf(" M=%d:%.0f", Ms[k], best);
    }
    printf("\n8.38 bottom-up queue mergesort (Program 8.8), time (ms) by run length M:\n    ");
    for (int k = 0; k < 9; k++) {
        double best = 1e9;
        for (int rep = 0; rep < 3; rep++) {
            link h = build(N, v); clock_t c0 = clock();
            h = queue_sort(h, Ms[k]);
            double ms = (double)(clock() - c0) * 1000 / CLOCKS_PER_SEC;
            if (!check(h, N)) { printf("BAD\n"); return 1; }
            if (ms < best) best = ms;
        }
        printf(" M=%d:%.0f", Ms[k], best);
    }
    printf("\n");
    return 0;
}
