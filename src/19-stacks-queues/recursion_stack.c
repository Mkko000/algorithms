/* ============================================================
 *  recursion_stack.c  —  how recursion really works: simulate the call stack yourself
 *
 *  Every recursive call pushes a FRAME (arguments, locals, "where to continue"), every return
 *  pops it. Here we do that by hand with an explicit stack, for
 *     factorial, Fibonacci, Towers of Hanoi, Ackermann, binary-tree-style double recursion,
 *  and show tail-recursion elimination (gcd, power). Each simulation is checked against the
 *  recursive version.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Lipschutz ch.6.8-6.10 (recursion, Towers of Hanoi, implementation of recursive
 *                procedures by stacks), Thareja ch.7.4 (recursion), Weiss ch.1.3, Bandyopadhyay & Dey ch.6
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 recursion_stack.c -o recursion_stack
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//>>fact
/* recursive: 1 frame per level, n+1 frames deep at the bottom */
static long fact_rec(int n) { return n <= 1 ? 1 : n * fact_rec(n - 1); }

/* explicit stack: phase 1 "descend" pushes n, n-1, ..., 2 (the pending multiplications);
   phase 2 "return" pops them multiplying on the way back — exactly the order the real stack unwinds. */
static long fact_stack(int n, int *max_depth)
{
    int st[64], sp = 0;
    while (n > 1) st[sp++] = n--;                 /* "call fact(n-1), remember to multiply by n" */
    *max_depth = sp + 1;
    long result = 1;                               /* fact(1) */
    while (sp) result *= st[--sp];                 /* "return": result = n * result */
    return result;
}
/* tail recursion: the recursive call is the LAST thing done, so no frame has to be remembered;
   a compiler (or you) can turn it into a loop that reuses one frame. */
static long fact_tail(int n, long acc) { return n <= 1 ? acc : fact_tail(n - 1, acc * n); }
static long fact_loop(int n) { long acc = 1; while (n > 1) acc *= n--; return acc; }
static int gcd_rec(int a, int b)  { return b == 0 ? a : gcd_rec(b, a % b); }
static int gcd_loop(int a, int b) { while (b) { int t = a % b; a = b; b = t; } return a; }
//<<fact

//>>fib
static long fib_rec(int n) { return n < 2 ? n : fib_rec(n - 1) + fib_rec(n - 2); }
static long calls;                                   /* counts frames pushed */
static long fib_count(int n) { calls++; return n < 2 ? n : fib_count(n - 1) + fib_count(n - 2); }

/* Fibonacci with an explicit stack. A frame = (n, state). state 0: need to compute fib(n-1);
   state 1: have fib(n-1) in 'a', need fib(n-2); state 2: both done, return a+b.
   'ret' is the value being returned to the frame below (the "return register"). */
typedef struct { int n, state; long a; } frame;
static long fib_stack(int n0)
{
    frame st[128]; int sp = 0; long ret = 0;
    st[sp++] = (frame){n0, 0, 0};
    while (sp) {
        frame *f = &st[sp - 1];
        if (f->n < 2) { ret = f->n; sp--; continue; }               /* base case: return n */
        if (f->state == 0)      { f->state = 1; st[sp++] = (frame){f->n - 1, 0, 0}; }   /* call fib(n-1) */
        else if (f->state == 1) { f->a = ret; f->state = 2; st[sp++] = (frame){f->n - 2, 0, 0}; }   /* call fib(n-2) */
        else                    { ret = f->a + ret; sp--; }          /* return fib(n-1)+fib(n-2) */
    }
    return ret;
}
//<<fib

//>>hanoi
/* Towers of Hanoi: move n disks from 'from' to 'to' using 'via'. 2^n - 1 moves. */
static long hmoves; static int hlog[2048][2];
static void hanoi_rec(int n, int from, int to, int via)
{
    if (n == 0) return;
    hanoi_rec(n - 1, from, via, to);
    hlog[hmoves][0] = from; hlog[hmoves][1] = to; hmoves++;
    hanoi_rec(n - 1, via, to, from);
}
/* Same with an explicit stack of "tasks": either "solve (n,from,to,via)" or "do this one move".
   Push in REVERSE order of execution: move n-1 (via->to) last, the disk, then move n-1 (from->via) first. */
typedef struct { int kind, n, from, to, via; } task;       /* kind 0 = solve, 1 = move one disk */
static long hanoi_stack(int n, int from, int to, int via, int out[][2])
{
    task st[256]; int sp = 0; long m = 0;
    st[sp++] = (task){0, n, from, to, via};
    while (sp) {
        task t = st[--sp];
        if (t.kind == 1) { out[m][0] = t.from; out[m][1] = t.to; m++; continue; }
        if (t.n == 0) continue;
        st[sp++] = (task){0, t.n - 1, t.via, t.to, t.from};
        st[sp++] = (task){1, 0, t.from, t.to, 0};
        st[sp++] = (task){0, t.n - 1, t.from, t.via, t.to};
    }
    return m;
}
//<<hanoi

//>>ackermann
/* Ackermann: grows faster than any primitive recursive function. A(2,3)=9, A(3,3)=61, A(4,*) is astronomical.
   m = 0:  A = n+1;   n = 0:  A(m-1, 1);   else  A(m-1, A(m, n-1)).  */
static long ack_rec(long m, long n)
{
    if (m == 0) return n + 1;
    if (n == 0) return ack_rec(m - 1, 1);
    return ack_rec(m - 1, ack_rec(m, n - 1));
}
/* the stack of pending "m" values: A(m, n) with m>0 and n>0 becomes  push m-1, then compute A(m, n-1) */
static long ack_stack(long m, long n, long *max_stack)
{
    long *st = malloc(sizeof(long) * (1 << 20)); long sp = 0; *max_stack = 0;
    st[sp++] = m;
    while (sp) {
        m = st[--sp];
        if (m == 0) { n = n + 1; }
        else if (n == 0) { st[sp++] = m - 1; n = 1; }
        else { st[sp++] = m - 1; st[sp++] = m; n = n - 1; }
        if (sp > *max_stack) *max_stack = sp;
    }
    free(st);
    return n;
}
//<<ackermann

int main(void)
{
    int depth;
    for (int n = 0; n <= 15; n++) {
        assert(fact_rec(n) == fact_stack(n, &depth) && fact_rec(n) == fact_tail(n, 1) && fact_rec(n) == fact_loop(n));
    }
    fact_stack(10, &depth);
    printf("fact(10) = %ld  needs %d frames on the call stack\n", fact_rec(10), depth);
    for (int a = 1; a < 60; a++) for (int b = 0; b < 60; b++) assert(gcd_rec(a, b) == gcd_loop(a, b));

    for (int n = 0; n <= 20; n++) assert(fib_rec(n) == fib_stack(n));
    calls = 0; long f25 = fib_count(25);
    printf("fib(25) = %ld  using %ld recursive calls (the call stack is only about 25 frames deep)\n", f25, calls);
    assert(f25 == 75025 && calls == 242785);

    for (int n = 1; n <= 10; n++) {
        hmoves = 0; hanoi_rec(n, 0, 2, 1);
        static int out[2048][2];
        long m = hanoi_stack(n, 0, 2, 1, out);
        assert(m == hmoves && m == (1L << n) - 1);
        for (long i = 0; i < m; i++) assert(out[i][0] == hlog[i][0] && out[i][1] == hlog[i][1]);
    }
    hmoves = 0; hanoi_rec(3, 0, 2, 1);
    printf("hanoi n=3: %ld moves:", hmoves);
    for (int i = 0; i < hmoves; i++) printf(" %c->%c", 'A' + hlog[i][0], 'A' + hlog[i][1]);
    puts("");

    long mx;
    assert(ack_rec(2, 3) == 9 && ack_rec(3, 3) == 61);
    for (int m = 0; m <= 3; m++) for (int n = 0; n <= 6; n++) assert(ack_rec(m, n) == ack_stack(m, n, &mx));
    long a33 = ack_stack(3, 3, &mx);
    printf("A(3,3) = %ld, explicit stack never held more than %ld pending values\n", a33, mx);
    puts("recursion simulations match the recursive versions");
    return 0;
}
