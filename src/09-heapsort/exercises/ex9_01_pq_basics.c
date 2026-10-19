/* ============================================================
 *  ex9_01_pq_basics.c  —  Exercises 9.1-9.5
 *  Chapter 9: Priority Queues and Heapsort — solved exercises
 *
 *  9.1 A letter means insert and an asterisk means delete the maximum in
 *      P R I O * R * * I * T * Y * * * Q U E * * * U * E
 *      Give the sequence of values returned by delete the maximum.
 *  9.2 Add '+' for join and parentheses to delimit the PQ built inside;
 *      give the contents of the PQ after
 *      ( ( ( P R I O * ) + ( R * I T * Y * ) ) * * * ) + ( Q U E * * * U * E )
 *  9.3 Use a priority-queue ADT to implement a stack.
 *  9.4 Use a priority-queue ADT to implement a queue.
 *  9.5 Criticize: "keep track of the maximum inserted so far and return it
 *      for find the maximum".
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Program 9.1-9.2, Exercises 9.1-9.5
 *  Build:   gcc -std=c99 -Wall -O2 ex9_01_pq_basics.c -o ex9_01
 * ============================================================ */
#include <stdio.h>
#include <string.h>

/* a tiny unordered-array PQ (Program 9.2) of (key, stamp) pairs */
typedef struct { long key; int val; } Item;
typedef struct { Item a[64]; int n; } PQ;
static void pqinit(PQ *q) { q->n = 0; }
static void pqinsert(PQ *q, Item x) { q->a[q->n++] = x; }
static Item pqdelmax(PQ *q)
{
    int m = 0;
    for (int j = 1; j < q->n; j++) if (q->a[m].key < q->a[j].key) m = j;
    Item t = q->a[m]; q->a[m] = q->a[q->n - 1]; q->a[q->n - 1] = t;
    return q->a[--q->n];
}
static void pqjoin(PQ *q, PQ *r) { for (int i = 0; i < r->n; i++) q->a[q->n++] = r->a[i]; r->n = 0; }
static void pqshow(PQ *q)                       /* contents, largest first */
{
    char s[64]; int n = q->n;
    for (int i = 0; i < n; i++) s[i] = (char)q->a[i].key;
    for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) if (s[j] > s[i]) { char t = s[i]; s[i] = s[j]; s[j] = t; }
    s[n] = 0; printf("%s", n ? s : "(empty)");
}

/* 9.2: recursive evaluation of the parenthesized expression */
static const char *p;
static void expr(PQ *q, char *out);
static void term(PQ *q, char *out)              /* a run of letters, stars and (...) */
{
    while (*p && *p != ')' && *p != '+') {
        if (*p == ' ') { p++; continue; }
        if (*p == '(') { PQ sub; pqinit(&sub); p++; expr(&sub, out); p++; pqjoin(q, &sub); continue; }
        if (*p == '*') { char c[2] = { (char)pqdelmax(q).key, 0 }; strcat(out, c); p++; continue; }
        pqinsert(q, (Item){ *p, 0 }); p++;
    }
}
static void expr(PQ *q, char *out)
{
    term(q, out);
    while (*p == '+' || *p == ' ') {
        if (*p == ' ') { p++; continue; }
        p++;
        PQ r; pqinit(&r); term(&r, out); pqjoin(q, &r);
    }
}

/* 9.3/9.4: a stack (or queue) is a PQ whose key is the insertion time */
static PQ S; static long clk;
static void push(int v) { pqinsert(&S, (Item){ ++clk, v }); }        /* newest = largest */
static int pop(void) { return pqdelmax(&S).val; }
static PQ Qu;
static void put(int v) { pqinsert(&Qu, (Item){ -(++clk), v }); }     /* oldest = largest */
static int get(void) { return pqdelmax(&Qu).val; }

int main(void)
{
    const char *seq = "P R I O * R * * I * T * Y * * * Q U E * * * U * E";
    PQ q; pqinit(&q);
    printf("9.1 deletions: ");
    for (const char *c = seq; *c; c++) {
        if (*c == ' ') continue;
        if (*c == '*') printf("%c ", (char)pqdelmax(&q).key); else pqinsert(&q, (Item){ *c, 0 });
    }
    printf("\n    left in the queue: "); pqshow(&q); printf("\n");

    char out[64] = "";
    p = "( ( ( P R I O * ) + ( R * I T * Y * ) ) * * * ) + ( Q U E * * * U * E )";
    pqinit(&q); expr(&q, out);
    printf("9.2 deletions in order: %s\n    final contents: ", out); pqshow(&q); printf("\n");

    pqinit(&S); for (int i = 1; i <= 5; i++) push(i * 10);
    printf("9.3 stack via PQ, push 10..50 then pop x3: ");
    for (int i = 0; i < 3; i++) printf("%d ", pop());
    push(99); printf("| push 99, pop: %d\n", pop());
    pqinit(&Qu); for (int i = 1; i <= 5; i++) put(i * 10);
    printf("9.4 queue via PQ, put 10..50 then get x3: ");
    for (int i = 0; i < 3; i++) printf("%d ", get());
    put(99); printf("| put 99, get: %d\n", get());

    /* 9.5: track "max inserted so far" and see it go stale */
    pqinit(&q); long maxsofar = 0;
    const char *ins = "PRIO"; for (int i = 0; ins[i]; i++) { pqinsert(&q, (Item){ ins[i], 0 }); if (ins[i] > maxsofar) maxsofar = ins[i]; }
    char d = (char)pqdelmax(&q).key;
    printf("9.5 insert P R I O, delete max -> %c; 'max so far' still says %c, true max is ", d, (char)maxsofar);
    { PQ c = q; printf("%c\n", (char)pqdelmax(&c).key); }
    return 0;
}
