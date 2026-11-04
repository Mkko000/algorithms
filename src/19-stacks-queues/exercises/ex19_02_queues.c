/* ============================================================
 *  ex19_02_queues.c  —  Solved exercises, Chapter 19 (Queues)
 *
 *  Thareja ch.8 "Queues": Review Questions 4-6 (1-3 are written answers on the course page) and
 *  Programming Exercises 1-12.  The queue / deque / priority-queue structures are those of
 *  deque_pq.c and stack_apps.c, included below; their main()s are renamed out of the way.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Ch. 8 Exercises
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 ex19_02_queues.c -o ex19_02
 * ============================================================ */
#define main stack_apps_main
#include "../stack_apps.c"
#undef main
#undef N
#define main deque_pq_main
#include "../deque_pq.c"
#undef main

//>>RQ4
/* A "book-style" linear queue: the array is printed with front and rear so the drawings can be compared. */
#define MAXQ 10
typedef struct { char a[MAXQ]; int front, rear; } lqueue;            /* front = rear = -1 : empty */
static void lq_init(lqueue *q) { memset(q->a, '.', MAXQ); q->front = q->rear = -1; }
static int lq_add(lqueue *q, char c)
{
    if (q->rear == MAXQ - 1) return 0;                               /* OVERFLOW: rear hit the end, even if cells before front are free */
    if (q->front == -1) q->front = 0;
    q->a[++q->rear] = c; return 1;
}
static int lq_del(lqueue *q, char *c)
{
    if (q->front == -1 || q->front > q->rear) return 0;              /* UNDERFLOW */
    *c = q->a[q->front]; q->a[q->front++] = '.';
    return 1;
}
static void lq_show(const lqueue *q, const char *what)
{
    printf("  %-22s |", what);
    for (int i = 0; i < MAXQ; i++) printf("%c", q->a[i]);
    printf("|  front=%d rear=%d\n", q->front, q->rear);
}
static void rq4(void)
{
    lqueue q; lq_init(&q); char c;
    for (char x = 'A'; x <= 'F'; x++) assert(lq_add(&q, x));
    lq_show(&q, "(a) add A..F");              assert(q.front == 0 && q.rear == 5);
    lq_del(&q, &c); assert(c == 'A'); lq_del(&q, &c); assert(c == 'B');
    lq_show(&q, "(b) delete two letters");    assert(q.front == 2 && q.rear == 5);
    lq_add(&q, 'G'); lq_show(&q, "(c) add G");  assert(q.rear == 6);
    lq_add(&q, 'H'); lq_show(&q, "(d) add H");  assert(q.rear == 7);
    for (int i = 0; i < 4; i++) lq_del(&q, &c);                      /* C D E F leave */
    lq_show(&q, "(e) delete four letters");   assert(q.front == 6 && q.rear == 7 && c == 'F');
    lq_add(&q, 'I'); lq_show(&q, "(f) add I");  assert(q.front == 6 && q.rear == 8 && q.a[6] == 'G' && q.a[8] == 'I');
}
//<<RQ4

//>>RQ5
/* RQ 5: the book gives FRONT = 1, REAR = 5 holding A..E but not the array size.  We answer for two sizes:
   MAX = 10 (a linear queue just works) and MAX = 6 (indices 0..5, A..E sit in 1..5) where the LINEAR queue
   overflows at the very first operation, while the CIRCULAR queue copes. */
static void rq5(void)
{
    lqueue q; lq_init(&q); char c;
    memcpy(q.a + 1, "ABCDE", 5); q.front = 1; q.rear = 5;          /* the picture of the question */
    lq_show(&q, "start");
    assert(lq_add(&q, 'F'));  lq_show(&q, "(a) add F");
    lq_del(&q, &c); lq_del(&q, &c); lq_show(&q, "(b) delete two letters");
    lq_add(&q, 'G'); lq_show(&q, "(c) add G");
    lq_add(&q, 'H'); lq_show(&q, "(d) add H");
    for (int i = 0; i < 4; i++) lq_del(&q, &c);
    lq_show(&q, "(e) delete four letters");
    assert(lq_add(&q, 'I')); lq_show(&q, "(f) add I");
    assert(q.front == 7 && q.rear == 9 && q.a[7] == 'G' && q.a[8] == 'H' && q.a[9] == 'I');

    puts("  with MAX = 6 and the SAME start (FRONT=1, REAR=5):");
    /* linear: REAR = MAX-1 already, so "add F" is an overflow although cell 0 is free */
    int rear = 5, MAXS = 6;
    assert(rear == MAXS - 1);
    puts("    linear queue : (a) add F -> OVERFLOW (REAR = MAX-1), the free cell 0 cannot be used");
    /* circular, book convention: indices wrap */
    char a[6] = {'.', 'A', 'B', 'C', 'D', 'E'}; int front = 1, rr = 5, count = 5;
    rr = (rr + 1) % 6; a[rr] = 'F'; count++;                                       /* (a) F goes to cell 0 */
    assert(rr == 0 && count == 6);                                                 /* the queue is now exactly full */
    front = (front + 2) % 6; count -= 2;                                           /* (b) A, B leave */
    rr = (rr + 1) % 6; a[rr] = 'G'; count++;                                       /* (c) G -> cell 1 */
    rr = (rr + 1) % 6; a[rr] = 'H'; count++;                                       /* (d) H -> cell 2 */
    assert(count == 6 && front == 3 && rr == 2);                                   /* C D E F G H : full again */
    front = (front + 4) % 6; count -= 4;                                           /* (e) C D E F leave */
    rr = (rr + 1) % 6; a[rr] = 'I'; count++;                                       /* (f) I -> cell 3 */
    printf("    circular queue: front=%d rear=%d, contents:", front, rr);
    for (int i = 0, k = front; i < count; i++, k = (k + 1) % 6) printf(" %c", a[k]);
    puts("");
    assert(front == 1 && rr == 3 && a[1] == 'G' && a[2] == 'H' && a[3] == 'I' && count == 3);
}
//<<RQ5

//>>RQ6
/* RQ 6: deque in a circular array of 10 cells, LEFT = 1, RIGHT = 5 holding A..E.
   LEFT moves back (wrapping) on insert-left and forward on delete-left; RIGHT is the mirror image. */
#define DN 10
typedef struct { char a[DN]; int left, right, n; } bdeque;
static void bd_show(const bdeque *d, const char *what)
{
    printf("  %-26s |", what);
    for (int i = 0; i < DN; i++) printf("%c", d->a[i]);
    printf("|  LEFT=%d RIGHT=%d\n", d->left, d->right);
}
static void bd_left(bdeque *d, char c)  { assert(d->n < DN); d->left = (d->left + DN - 1) % DN; d->a[d->left] = c; d->n++; }
static void bd_right(bdeque *d, char c) { assert(d->n < DN); d->right = (d->right + 1) % DN; d->a[d->right] = c; d->n++; }
static char bd_dell(bdeque *d) { assert(d->n); char c = d->a[d->left]; d->a[d->left] = '.'; d->left = (d->left + 1) % DN; d->n--; return c; }
static char bd_delr(bdeque *d) { assert(d->n); char c = d->a[d->right]; d->a[d->right] = '.'; d->right = (d->right + DN - 1) % DN; d->n--; return c; }
static void rq6(void)
{
    bdeque d; memset(d.a, '.', DN); memcpy(d.a + 1, "ABCDE", 5); d.left = 1; d.right = 5; d.n = 5;
    bd_show(&d, "start");
    bd_left(&d, 'F');  bd_show(&d, "(a) add F on the left");
    bd_right(&d, 'G'); bd_show(&d, "(b) add G on the right");
    bd_right(&d, 'H'); bd_show(&d, "(c) add H on the right");
    char x = bd_dell(&d), y = bd_dell(&d); bd_show(&d, "(d) delete 2 from left");   assert(x == 'F' && y == 'A');
    bd_right(&d, 'I'); bd_show(&d, "(e) add I on the right");
    bd_left(&d, 'J');  bd_show(&d, "(f) add J on the left");
    x = bd_delr(&d); y = bd_delr(&d); bd_show(&d, "(g) delete 2 from right");        assert(x == 'I' && y == 'H');
    assert(d.left == 1 && d.right == 6 && d.n == 6);
    char s[8]; int w = 0; for (int i = 0, k = d.left; i < d.n; i++, k = (k + 1) % DN) s[w++] = d.a[k]; s[w] = 0;
    printf("  final deque, left to right: %s\n", s);
    assert(strcmp(s, "JBCDEG") == 0);
}
//<<RQ6

//>>PE1
/* ---- PE 1: number of items. Linear: rear - front + 1.  Circular: (rear - front + MAX) % MAX + 1 (empty handled apart) ---- */
static int lq_count(const lqueue *q) { return q->front == -1 || q->front > q->rear ? 0 : q->rear - q->front + 1; }
static int circ_count(int front, int rear, int max) { return front == -1 ? 0 : (rear - front + max) % max + 1; }
//<<PE1

//>>PE4
/* ---- PE 4: deque as a doubly linked list with head and tail: all four operations O(1) ---- */
typedef struct dnode { int val; struct dnode *prev, *next; } dnode;
typedef struct { dnode *head, *tail; int n; } ldeque;
static void ld_push_left(ldeque *d, int v)
{
    dnode *x = malloc(sizeof *x); if (!x) exit(1);
    x->val = v; x->prev = NULL; x->next = d->head;
    if (d->head) d->head->prev = x; else d->tail = x;
    d->head = x; d->n++;
}
static void ld_push_right(ldeque *d, int v)
{
    dnode *x = malloc(sizeof *x); if (!x) exit(1);
    x->val = v; x->next = NULL; x->prev = d->tail;
    if (d->tail) d->tail->next = x; else d->head = x;
    d->tail = x; d->n++;
}
static int ld_pop_left(ldeque *d, int *v)
{
    if (!d->head) return 0;
    dnode *x = d->head; *v = x->val; d->head = x->next;
    if (d->head) d->head->prev = NULL; else d->tail = NULL;
    free(x); d->n--; return 1;
}
static int ld_pop_right(ldeque *d, int *v)
{
    if (!d->tail) return 0;
    dnode *x = d->tail; *v = x->val; d->tail = x->prev;
    if (d->tail) d->tail->next = NULL; else d->head = NULL;
    free(x); d->n--; return 1;
}
//<<PE4

//>>PE11
/* ---- PE 11: reverse a queue with a stack: dequeue everything onto the stack, pop everything back.
        PE 12: compare two queues: dequeue/enqueue each exactly n times so both are rotated back to where they started. ---- */
static void queue_reverse(cqueue *q)
{
    istack s = {{0}, 0}; int x;
    while (cq_get(q, &x)) is_push(&s, x);
    while (s.n) cq_put(q, is_pop(&s));
}
static int queue_equal(cqueue *a, cqueue *b)
{
    if (a->n != b->n) return 0;
    int n = a->n, eq = 1, x = 0, y = 0;
    for (int i = 0; i < n; i++) {
        cq_get(a, &x); cq_get(b, &y);
        if (x != y) eq = 0;
        cq_put(a, x); cq_put(b, y);                    /* rotate: after n steps both are back in the original order */
    }
    return eq;
}
//<<PE11

int main(void)
{
    puts("RQ 4:"); rq4();
    puts("RQ 5:"); rq5();
    puts("RQ 6:"); rq6();

    /* PE 1 */
    {
        lqueue q; lq_init(&q); char c;
        assert(lq_count(&q) == 0);
        for (char x = 'A'; x <= 'G'; x++) lq_add(&q, x);
        lq_del(&q, &c); lq_del(&q, &c);
        assert(lq_count(&q) == 5);
        assert(circ_count(-1, -1, 8) == 0 && circ_count(2, 6, 8) == 5 && circ_count(6, 1, 8) == 4 && circ_count(0, 7, 8) == 8);
        printf("PE 1: counts: linear %d, circular wrap-around (front=6, rear=1, MAX=8) %d\n", lq_count(&q), circ_count(6, 1, 8));
    }
    /* PE 2: a linear queue of exactly 10 values */
    {
        lqueue q; lq_init(&q); int ok = 0;
        for (int i = 0; i < 12; i++) ok += lq_add(&q, (char)('0' + i % 10));
        assert(ok == 10 && !lq_add(&q, 'x'));                          /* the 11th and 12th are rejected */
        printf("PE 2: linear queue of 10 values: %.10s (further adds rejected)\n", q.a);
    }
    /* PE 3, 6, 7: insert at both ends / delete at both ends / both = deque on a circular array */
    {
        deque d = {{0}, 0, 0}; int v;
        or_insert_left(&d, 2); or_insert_right(&d, 3); or_insert_left(&d, 1); or_insert_right(&d, 4);
        assert(or_delete(&d, &v) && v == 1 && or_delete(&d, &v) && v == 2);      /* output-restricted: deletion only at left (PE 3) */
        ir_insert(&d, 5); ir_insert(&d, 6);
        assert(ir_delete_right(&d, &v) && v == 6 && ir_delete_left(&d, &v) && v == 3);   /* input-restricted: insertion only at right (PE 6) */
        assert(dq_push_left(&d, 9) && dq_push_right(&d, 10) && dq_pop_right(&d, &v) && v == 10 && dq_pop_left(&d, &v) && v == 9);  /* PE 7 */
        int cnt = 0; while (dq_pop_left(&d, &v)) cnt++;
        assert(cnt == 2);                                                        /* 4 and 5 remain */
        int ok = 0; for (int i = 0; i < 20; i++) ok += dq_push_left(&d, i);
        assert(ok == 8);                                                         /* capacity N = 8 respected */
        puts("PE 3/6/7: output-restricted, input-restricted and full deque on one circular array");
    }
    /* PE 4 */
    {
        ldeque d = {NULL, NULL, 0}; int v;
        for (int i = 0; i < 1000; i++) { if (i & 1) ld_push_left(&d, i); else ld_push_right(&d, i); }
        assert(d.n == 1000);
        for (int i = 999; i >= 0; i -= 2) { assert(ld_pop_left(&d, &v) && v == i); }       /* odd numbers, newest first */
        for (int i = 998; i >= 0; i -= 2) { assert(ld_pop_right(&d, &v) && v == i); }      /* evens, newest first from the right */
        assert(!ld_pop_left(&d, &v) && !ld_pop_right(&d, &v) && d.head == NULL && d.tail == NULL);
        puts("PE 4: linked deque, 1000 pushes at alternating ends, popped in the expected order, empty at the end");
    }
    /* PE 5: insertion at any vacant location at the rear = reuse cells vacated at the front */
    {
        cqueue_c q = {{0}, -1, -1}; int x, ok = 0;
        for (int i = 0; i < N; i++) ok += cc_put(&q, i);
        assert(ok == N && !cc_put(&q, 99));                                     /* full */
        for (int i = 0; i < 3; i++) { assert(cc_get(&q, &x) && x == i); }
        assert(q.rear == N - 1);                                                /* REAR is at the last cell, a linear queue would say OVERFLOW */
        assert(cc_put(&q, 100) && q.rear == 0 && cc_put(&q, 101) && q.rear == 1 && cc_put(&q, 102) && q.rear == 2);
        assert(!cc_put(&q, 103));                                               /* vacant cells used up */
        puts("PE 5: after 3 deletions, the next 3 inserts land in cells 0,1,2 (wrap-around) and the 4th is refused");
    }
    /* PE 8 */
    {
        pnode *h = NULL; int v;
        h = pq_insert(h, 5, 2); h = pq_insert(h, 10, 1);                          /* the book's sample session */
        printf("PE 8: priority queue after (5,pri 2) (10,pri 1):");
        for (pnode *p = h; p; p = p->next) printf("  %d[priority=%d]", p->data, p->pri);
        puts("");
        assert(h->data == 10 && h->next->data == 5);
        h = pq_delete(h, &v); assert(v == 10); h = pq_delete(h, &v); assert(v == 5); assert(!h);
    }
    /* PE 9 (queue from stack) and PE 10 (stack from queue) */
    {
        queue2s q = {{0}, {0}, 0, 0, 0}; stack2q s; memset(&s, 0, sizeof s);
        for (int i = 1; i <= 5; i++) q2_enqueue(&q, i);
        assert(q2_dequeue(&q) == 1 && q2_dequeue(&q) == 2);
        q2_enqueue(&q, 6);
        for (int i = 3; i <= 6; i++) assert(q2_dequeue(&q) == i);
        for (int i = 1; i <= 5; i++) s2_push(&s, i);
        for (int i = 5; i >= 1; i--) assert(s2_pop(&s) == i);
        puts("PE 9/10: queue from two stacks keeps FIFO even when enqueues and dequeues interleave; stack from two queues keeps LIFO");
    }
    /* PE 11 and 12 */
    {
        cqueue a = {{0}, 0, 0}, b = {{0}, 0, 0}, c = {{0}, 0, 0}; int x = 0;
        for (int i = 1; i <= 5; i++) { cq_put(&a, i); cq_put(&b, i); }
        cq_get(&a, &x); cq_put(&a, x);                                          /* a: 1 2 3 4 5 -> 2 3 4 5 1 (wrapped around the array) */
        for (int i = 2; i <= 5; i++) cq_put(&c, i);
        cq_put(&c, 1);                                                          /* c: 2 3 4 5 1, same sequence, different storage layout */
        assert(queue_equal(&a, &c) == 1);
        assert(queue_equal(&a, &b) == 0 && a.n == 5 && b.n == 5);               /* b = 1 2 3 4 5 differs; nobody was disturbed */
        queue_reverse(&b);
        for (int i = 5; i >= 1; i--) { assert(cq_get(&b, &x) && x == i); }
        puts("PE 11/12: queue reversed through a stack; two queues compared by rotating each exactly n times");
    }
    puts("ex19_02_queues ok");
    return 0;
}
