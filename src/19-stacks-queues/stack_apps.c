/* ============================================================
 *  stack_apps.c  —  classic stack and queue constructions
 *
 *   1. balanced brackets (reports WHERE the first error is)
 *   2. two stacks sharing one array (grow towards each other) — "multiple stacks"
 *   3. a stack with O(1) getMin  (auxiliary stack of minima)
 *   4. a queue built from TWO stacks, amortised O(1), and a stack built from TWO queues
 *   5. monotonic stack: next greater element and the stock-span problem in O(n)
 *   6. copy / compare / reverse stacks using only push and pop
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja ch.7-8 (multiple stacks, PE 5, 9, 10, 13), Lipschutz ch.6.7 (applications
 *                of stacks) and 6.11, Weiss ch.3.3, Bandyopadhyay & Dey ch.6
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 stack_apps.c -o stack_apps
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define CAP 64

//>>brackets
/* ( [ { push their closer; a closer must match the top. returns -1 if balanced,
   otherwise the 0-based index of the first offending character. */
static int check_brackets(const char *s)
{
    char st[256]; int sp = 0;
    for (int i = 0; s[i]; i++) {
        char c = s[i];
        if (c == '(') st[sp++] = ')';
        else if (c == '[') st[sp++] = ']';
        else if (c == '{') st[sp++] = '}';
        else if (c == ')' || c == ']' || c == '}') {
            if (sp == 0 || st[--sp] != c) return i;      /* underflow, or mismatched pair */
        }
    }
    return sp == 0 ? -1 : (int)strlen(s);               /* leftover openers: error "at the end" */
}
//<<brackets

//>>two
/* two stacks in ONE array: stack A grows from the left end, stack B from the right end.
   Overflow happens only when the WHOLE array is full — much better than two fixed halves. */
typedef struct { int a[CAP]; int topA, topB; } two_stacks;     /* topA: next free on the left, topB on the right */
static void ts_init(two_stacks *s) { s->topA = -1; s->topB = CAP; }
static int ts_push(two_stacks *s, int which, int v)
{
    if (s->topA + 1 == s->topB) return 0;                         /* the two tops met: full */
    if (which == 0) s->a[++s->topA] = v; else s->a[--s->topB] = v;
    return 1;
}
static int ts_pop(two_stacks *s, int which, int *v)
{
    if (which == 0) { if (s->topA < 0) return 0; *v = s->a[s->topA--]; }
    else            { if (s->topB >= CAP) return 0; *v = s->a[s->topB++]; }
    return 1;
}
//<<two

//>>minstack
/* every push also pushes min(v, current min) on a second stack; getMin = peek of that stack */
typedef struct { int v[CAP], m[CAP], n; } min_stack;
static void ms_push(min_stack *s, int x)
{
    assert(s->n < CAP);
    s->m[s->n] = (s->n == 0 || x < s->m[s->n - 1]) ? x : s->m[s->n - 1];
    s->v[s->n++] = x;
}
static int ms_pop(min_stack *s) { assert(s->n > 0); return s->v[--s->n]; }
static int ms_min(const min_stack *s) { assert(s->n > 0); return s->m[s->n - 1]; }
//<<minstack

//>>queue2s
/* queue from two stacks: enqueue pushes on IN; dequeue pops from OUT, and only when OUT is
   empty moves EVERYTHING from IN to OUT (reversing it).  Each element moves at most once
   from IN to OUT, so n operations cost O(n) in total = amortised O(1) per operation. */
typedef struct { int in[CAP], out[CAP], ni, no; long moves; } queue2s;
static void q2_enqueue(queue2s *q, int x) { assert(q->ni < CAP); q->in[q->ni++] = x; }
static int  q2_dequeue(queue2s *q)
{
    if (q->no == 0) while (q->ni) { q->out[q->no++] = q->in[--q->ni]; q->moves++; }
    assert(q->no > 0);
    return q->out[--q->no];
}

/* stack from two queues (circular int queues): push makes the new element the FRONT by
   rotating the old ones behind it: O(n) push, O(1) pop. */
typedef struct { int a[CAP]; int head, n; } iqueue;
static void iq_put(iqueue *q, int x) { assert(q->n < CAP); q->a[(q->head + q->n++) % CAP] = x; }
static int  iq_get(iqueue *q) { assert(q->n > 0); int x = q->a[q->head]; q->head = (q->head + 1) % CAP; q->n--; return x; }
typedef struct { iqueue q1, q2; } stack2q;
static void s2_push(stack2q *s, int x)
{
    iq_put(&s->q2, x);
    while (s->q1.n) iq_put(&s->q2, iq_get(&s->q1));
    iqueue t = s->q1; s->q1 = s->q2; s->q2 = t;          /* swap the roles */
}
static int s2_pop(stack2q *s) { return iq_get(&s->q1); }
//<<queue2s

//>>mono
/* next greater element: for each i, the first a[j] > a[i] with j > i (or -1).
   The stack holds indices whose answer is still unknown, with DECREASING values; each index
   is pushed once and popped once, so the whole scan is O(n). */
static void next_greater(const int a[], int n, int out[])
{
    int st[CAP], sp = 0;
    for (int i = 0; i < n; i++) {
        while (sp && a[st[sp - 1]] < a[i]) out[st[--sp]] = a[i];
        st[sp++] = i;
    }
    while (sp) out[st[--sp]] = -1;
}
/* stock span: span[i] = number of consecutive days up to i with price <= price[i] */
static void stock_span(const int p[], int n, int span[])
{
    int st[CAP], sp = 0;
    for (int i = 0; i < n; i++) {
        while (sp && p[st[sp - 1]] <= p[i]) sp--;
        span[i] = sp ? i - st[sp - 1] : i + 1;
        st[sp++] = i;
    }
}
//<<mono

//>>stackops
typedef struct { int a[CAP]; int n; } istack;
static void is_push(istack *s, int x) { assert(s->n < CAP); s->a[s->n++] = x; }
static int  is_pop(istack *s)          { assert(s->n > 0);   return s->a[--s->n]; }
/* copy preserving order: pop into a temporary (reverses), pop back into both (restores) */
static void is_copy(istack *src, istack *dst)
{
    istack tmp = {{0}, 0};
    while (src->n) is_push(&tmp, is_pop(src));
    while (tmp.n) { int x = is_pop(&tmp); is_push(src, x); is_push(dst, x); }
}
/* compare two stacks: they are equal iff the pops agree one by one (both are restored) */
static int is_equal(istack *a, istack *b)
{
    if (a->n != b->n) return 0;
    istack ta = {{0}, 0}, tb = {{0}, 0}; int eq = 1;
    while (a->n) { int x = is_pop(a), y = is_pop(b); is_push(&ta, x); is_push(&tb, y); if (x != y) eq = 0; }
    while (ta.n) is_push(a, is_pop(&ta));
    while (tb.n) is_push(b, is_pop(&tb));
    return eq;
}
/* reverse a stack using only push/pop and two temporary stacks (three moves = identity on order twice, reverse once) */
static void is_reverse(istack *s)
{
    istack t1 = {{0}, 0}, t2 = {{0}, 0};
    while (s->n) is_push(&t1, is_pop(s));
    while (t1.n) is_push(&t2, is_pop(&t1));
    while (t2.n) is_push(s, is_pop(&t2));
}
//<<stackops

int main(void)
{
    /* brackets */
    assert(check_brackets("{[()]}(a)[b]") == -1);
    assert(check_brackets("") == -1);
    assert(check_brackets("(]") == 1);
    assert(check_brackets("())") == 2);               /* extra closer: underflow */
    assert(check_brackets("((") == 2);                /* never closed: reported at the end */
    assert(check_brackets("{[}]") == 2);
    printf("brackets: \"{[()]}\" ok, \"(]\" bad at index %d, \"())\" bad at index %d\n", check_brackets("(]"), check_brackets("())"));

    /* two stacks in one array */
    two_stacks ts; ts_init(&ts);
    int pushed = 0;
    for (int i = 0; i < 50; i++) pushed += ts_push(&ts, 0, i);        /* 50 on A ... */
    for (int i = 0; i < 50; i++) pushed += ts_push(&ts, 1, 100 + i);  /* ... and only 14 fit on B */
    assert(pushed == CAP && !ts_push(&ts, 0, 7) && !ts_push(&ts, 1, 7));
    int v; assert(ts_pop(&ts, 0, &v) && v == 49 && ts_pop(&ts, 1, &v) && v == 113);
    assert(ts_push(&ts, 1, 5) && ts_pop(&ts, 1, &v) && v == 5);
    while (ts_pop(&ts, 0, &v)) ;
    assert(!ts_pop(&ts, 0, &v));                                      /* underflow reported, not undefined */
    printf("two stacks: %d cells shared, A took 50, B took %d before overflow\n", CAP, CAP - 50);

    /* min stack */
    min_stack ms = {{0}, {0}, 0};
    int in[] = {5, 3, 7, 3, 2, 8};
    int want[] = {5, 3, 3, 3, 2, 2};
    for (int i = 0; i < 6; i++) { ms_push(&ms, in[i]); assert(ms_min(&ms) == want[i]); }
    for (int i = 5; i >= 0; i--) { assert(ms_min(&ms) == want[i]); ms_pop(&ms); }

    /* queue from two stacks (random model check) and stack from two queues */
    queue2s q = {{0}, {0}, 0, 0, 0};
    int model[4096], mh = 0, mt = 0; srand(9); long ops = 0;
    for (int i = 0; i < 4000; i++) {
        if ((rand() & 1) && q.ni + q.no < CAP) { q2_enqueue(&q, i); model[mt++] = i; ops++; }
        else if (mh < mt) { assert(q2_dequeue(&q) == model[mh++]); ops++; }
    }
    while (mh < mt) assert(q2_dequeue(&q) == model[mh++]);
    printf("queue from 2 stacks: %ld ops, %ld element moves (<= one per element, amortised O(1))\n", ops, q.moves);
    assert(q.moves <= ops);
    stack2q s2; memset(&s2, 0, sizeof s2);
    for (int i = 1; i <= 5; i++) s2_push(&s2, i);
    for (int i = 5; i >= 1; i--) assert(s2_pop(&s2) == i);

    /* monotonic stack */
    int a[] = {4, 5, 2, 25, 7, 8}, ng[6];
    next_greater(a, 6, ng);
    int ng_want[] = {5, 25, 25, -1, 8, -1};
    for (int i = 0; i < 6; i++) assert(ng[i] == ng_want[i]);
    int price[] = {100, 80, 60, 70, 60, 75, 85}, sp[7], sp_want[] = {1, 1, 1, 2, 1, 4, 6};
    stock_span(price, 7, sp);
    for (int i = 0; i < 7; i++) assert(sp[i] == sp_want[i]);
    puts("next greater of {4,5,2,25,7,8} = {5,25,25,-1,8,-1};  span of {100,80,60,70,60,75,85} = {1,1,1,2,1,4,6}");

    /* stack ops */
    istack s = {{0}, 0}, d = {{0}, 0}; for (int i = 1; i <= 5; i++) is_push(&s, i);
    is_copy(&s, &d); assert(s.n == 5 && d.n == 5 && is_equal(&s, &d));
    is_push(&d, 99); assert(!is_equal(&s, &d));
    is_pop(&d); is_reverse(&d);
    assert(is_pop(&d) == 1 && is_pop(&d) == 2);
    puts("stack_apps ok");
    return 0;
}
