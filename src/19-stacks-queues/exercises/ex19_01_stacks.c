/* ============================================================
 *  ex19_01_stacks.c  —  Solved exercises, Chapter 19 (Stacks)
 *
 *  Thareja ch.7 "Stacks":  Review Questions 8-14 (the written-answer ones, 1-7 and 15-16, are
 *  answered on the course page) and Programming Exercises 1-10.
 *  The conversion engine (convert, to_postfix, eval...) is expr_convert.c, included below
 *  so nothing is duplicated; main() of that file is renamed out of the way.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Ch. 7 Exercises
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 ex19_01_stacks.c -o ex19_01 -lm
 * ============================================================ */
#define main expr_convert_main
#include "../expr_convert.c"
#undef main
#define main stack_apps_main
#include "../stack_apps.c"
#undef main

#include <math.h>

//>>RQ8
/* ---- RQ 8 and 9: infix -> postfix and infix -> prefix (the OCR of the book words both as "postfix";
        we give BOTH forms for every expression, so whichever the question meant is answered) ---- */
static void conv(const char *infix, const char *post_want, const char *pre_want)
{
    toks in, pf, pr; char b1[256], b2[256];
    assert(tokenize(infix, &in) > 0);
    assert(to_postfix(&in, &pf) > 0 && to_prefix_reverse(&in, &pr) > 0);
    join(&pf, b1, sizeof b1); join(&pr, b2, sizeof b2);
    printf("  %-30s postfix: %-26s prefix: %s\n", infix, b1, b2);
    assert(strcmp(b1, post_want) == 0 && strcmp(b2, pre_want) == 0);
}
static void rq8_rq9(void)
{
    conv("A - B + C",                   "A B - C +",                     "+ - A B C");
    conv("A * B + C / D",               "A B * C D / +",                 "+ * A B / C D");
    conv("(A - B) + C * D / E - C",     "A B - C D * E / + C -",         "- + - A B / * C D E C");
    conv("(A * B) + (C / D) - (D + E)", "A B * C D / + D E + -",         "- + * A B / C D + D E");
    conv("((A - B) + D / ((E + F) * G))", "A B - D E F + G * / +",       "+ - A B / D * + E F G");
    conv("(A - 2 * (B + C) / D * E) + F", "A 2 B C + * D / E * - F +",   "+ - A * / * 2 + B C D E F");
    conv("14 / 7 * 3 - 4 + 9 / 2",      "14 7 / 3 * 4 - 9 2 / +",        "+ - * / 14 7 3 4 / 9 2");
}
//<<RQ8

//>>RQ10
/* ---- RQ 10 and 11: postfix / prefix -> infix.  Scan, push strings, and each operator glues "(x op y)". ---- */
static void to_infix(const char *expr, int prefix, const char *want)
{
    toks t; assert(tokenize(expr, &t) > 0);
    char *r = convert(&t, prefix ? 2 : 1);
    printf("  %-18s -> %s\n", expr, r);
    assert(r && strcmp(r, want) == 0);
    free(r);
}
static void rq10_rq11(void)
{
    puts("RQ10 (postfix -> infix):");
    to_infix("A B + C * D -", 0, "(((A + B) * C) - D)");
    to_infix("A B C * + D -", 0, "((A + (B * C)) - D)");
    puts("RQ11 (prefix -> infix):");
    to_infix("* - + A B C D", 1, "(((A + B) - C) * D)");
    to_infix("+ - a * B C D", 1, "((a - (B * C)) + D)");
}
//<<RQ10

//>>RQ12
/* ---- RQ 12: 10 + ((7 - 5) + 10) / 2   ->  postfix, then evaluate with a visible stack ---- */
static double eval_trace(const toks *pf)
{
    double st[MAXT]; int sp = 0;
    for (int i = 0; i < pf->n; i++) {
        const char *t = pf->t[i];
        if (!is_op(t)) st[sp++] = atof(t);
        else { double y = st[--sp], x = st[--sp]; st[sp++] = apply(t[0], x, y); }
        printf("    read %-3s stack: [", t);
        for (int k = 0; k < sp; k++) printf("%s%g", k ? " " : "", st[k]);
        puts("]");
    }
    assert(sp == 1);
    return st[0];
}
static void rq12(void)
{
    toks in, pf; char buf[128];
    tokenize("10 + ((7 - 5) + 10) / 2", &in); to_postfix(&in, &pf);
    join(&pf, buf, sizeof buf);
    printf("  postfix: %s\n", buf);
    assert(strcmp(buf, "10 7 5 - 10 + 2 / +") == 0);
    double v = eval_trace(&pf);
    printf("  value = %g\n", v);
    assert(v == 16);
}
//<<RQ12

//>>RQ14
/* ---- RQ 14: draw the stack after each operation ---- */
static void show_stack(const char *what, const char *st, int n)
{
    printf("  %-22s [", what);
    for (int i = 0; i < n; i++) printf("%s%c", i ? " " : "", st[i]);
    printf("]%s\n", n ? "  <- top" : "");
}
static void rq14(void)
{
    char st[16]; int n = 0;
    for (char c = 'A'; c <= 'F'; c++) st[n++] = c;
    show_stack("(a) add A..F", st, n);          assert(n == 6 && st[5] == 'F');
    n -= 2;  show_stack("(b) delete two letters", st, n);   assert(n == 4 && st[3] == 'D');   /* F and E leave */
    st[n++] = 'G'; show_stack("(c) add G", st, n);          assert(n == 5 && st[4] == 'G');
    st[n++] = 'H'; show_stack("(d) add H", st, n);          assert(n == 6 && st[5] == 'H');
    n -= 4;  show_stack("(e) delete four letters", st, n);  assert(n == 2 && st[1] == 'B');   /* H G D C leave */
    st[n++] = 'I'; show_stack("(f) add I", st, n);          assert(n == 3 && strncmp(st, "ABI", 3) == 0);
}
//<<RQ14

//>>PE1
/* ---- PE 1: a stack as a linked list: push = insert at head, pop = delete at head, both O(1) ---- */
typedef struct snode { int val; struct snode *next; } snode;
static void ls_push(snode **top, int v)
{
    snode *n = malloc(sizeof *n);
    if (!n) { perror("malloc"); exit(1); }
    n->val = v; n->next = *top; *top = n;
}
static int ls_pop(snode **top, int *v)
{
    if (!*top) return 0;                          /* underflow */
    snode *t = *top; *v = t->val; *top = t->next; free(t); return 1;
}
//<<PE1

//>>PE2
/* ---- PE 2 and 3: "a+b" -> "ab+" and "a+b" -> "+ab" (single-letter operands, no spaces) ----
   the general engine handles any expression; here we strip the spaces so the result looks like the book's */
static void nospace(const toks *t, char *out)
{
    out[0] = 0;
    for (int i = 0; i < t->n; i++) strcat(out, t->t[i]);
}
static void pe2_pe3(void)
{
    toks in, pf, pr; char b[64];
    tokenize("a+b", &in);
    to_postfix(&in, &pf); nospace(&pf, b); printf("  a+b -> %s\n", b); assert(strcmp(b, "ab+") == 0);
    to_prefix_reverse(&in, &pr); nospace(&pr, b); printf("  a+b -> %s\n", b); assert(strcmp(b, "+ab") == 0);
}
//<<PE2

//>>PE4
/* ---- PE 4: a stack of student names: the element type is a string (copied in, so the caller's buffer may change) ---- */
#define NAMELEN 32
typedef struct { char name[CAP][NAMELEN]; int top; } name_stack;
static int ns_push(name_stack *s, const char *nm)
{
    if (s->top == CAP) return 0;
    size_t k = strlen(nm); if (k > NAMELEN - 1) k = NAMELEN - 1;      /* truncate long names, never overflow */
    memcpy(s->name[s->top], nm, k); s->name[s->top][k] = 0; s->top++; return 1;
}
static const char *ns_pop(name_stack *s) { return s->top ? s->name[--s->top] : NULL; }   /* valid until the next push */
//<<PE4

//>>PE6
/* ---- PE 6: F(x,y) = F(x-y, y) + 1 if y <= x, else 0.  Each call subtracts y and adds 1 -> F = floor(x / y).
        (y = 0 would never terminate, so the function guards it.) ---- */
static int F6(int x, int y) { assert(y > 0); return y <= x ? F6(x - y, y) + 1 : 0; }
/* ---- PE 7: F(n,r) = F(n-1,r) + F(n-1,r-1).  The book gives no base cases; the standard ones
        F(n,0) = F(n,n) = 1 turn it into Pascal's rule, i.e. the binomial coefficient C(n,r). ---- */
static long F7(int n, int r) { return (r == 0 || r == n) ? 1 : F7(n - 1, r) + F7(n - 1, r - 1); }
/* ---- PE 8: Lambda(n) = Lambda(n/2) + 1 for n > 1, Lambda(1) = 0  ->  floor(log2 n) ---- */
static int Lambda(int n) { return n > 1 ? Lambda(n / 2) + 1 : 0; }
/* ---- PE 9: F(M,N) = 1 if M = 0 or M >= N >= 1, else F(M-1,N) + F(M-1,N-1) ---- */
static long F9(int M, int N) { return (M == 0 || (M >= N && N >= 1)) ? 1 : F9(M - 1, N) + F9(M - 1, N - 1); }
/* ---- PE 10: reverse a string with recursion: reverse the middle, swap the ends ---- */
static void rev(char *s, int lo, int hi) { if (lo >= hi) return; char t = s[lo]; s[lo] = s[hi]; s[hi] = t; rev(s, lo + 1, hi - 1); }
//<<PE6

int main(void)
{
    puts("RQ 8 / 9:");  rq8_rq9();
    rq10_rq11();
    puts("RQ 12:");     rq12();
    puts("RQ 14:");     rq14();

    /* RQ 13 / PE 5: copy keeps order, compare detects differences (code is in stack_apps.c: is_copy, is_equal) */
    {
        istack a = {{0}, 0}, b = {{0}, 0}, c = {{0}, 0};
        for (int i = 1; i <= 5; i++) is_push(&a, i * 10);
        is_copy(&a, &b);
        assert(a.n == 5 && b.n == 5 && is_equal(&a, &b));          /* source untouched, destination is an exact copy */
        for (int i = 1; i <= 5; i++) assert(is_pop(&b) == (6 - i) * 10);   /* order preserved: 50 comes out first */
        for (int i = 1; i <= 4; i++) is_push(&c, i * 10);
        is_push(&c, 99);
        assert(!is_equal(&a, &c));                                  /* same size, one element differs */
        assert(is_pop(&c) == 99 && is_pop(&c) == 40);               /* c was restored untouched by is_equal */
        puts("RQ 13 / PE 5: copy preserves order; compare reports difference; both stacks restored");
    }

    /* PE 1 */
    {
        snode *top = NULL; int v;
        for (int i = 1; i <= 100000; i++) ls_push(&top, i);        /* no capacity: no overflow, only malloc failure */
        for (int i = 100000; i >= 1; i--) { assert(ls_pop(&top, &v) && v == i); }
        assert(!ls_pop(&top, &v) && top == NULL);
        puts("PE 1: linked stack, 100000 pushes/pops, underflow detected");
    }
    puts("PE 2 / 3:");  pe2_pe3();
    /* PE 4 */
    {
        name_stack s; s.top = 0; char buf[NAMELEN];
        const char *names[] = {"Mona", "Omar", "Salma", "Youssef"};
        for (int i = 0; i < 4; i++) { strcpy(buf, names[i]); assert(ns_push(&s, buf)); strcpy(buf, "XXXX"); }  /* copied, not aliased */
        assert(strcmp(ns_pop(&s), "Youssef") == 0 && strcmp(ns_pop(&s), "Salma") == 0);
        assert(strcmp(ns_pop(&s), "Omar") == 0 && strcmp(ns_pop(&s), "Mona") == 0 && ns_pop(&s) == NULL);
        puts("PE 4: stack of names pops Youssef, Salma, Omar, Mona (reverse of entry order)");
    }
    /* PE 6-10 */
    for (int x = 0; x < 60; x++) for (int y = 1; y < 12; y++) assert(F6(x, y) == x / y);
    printf("PE 6: F(17,5) = %d (= 17 div 5)\n", F6(17, 5));
    for (int n = 0; n <= 12; n++) for (int r = 0; r <= n; r++) {
        long c = 1; for (int i = 1; i <= r; i++) c = c * (n - r + i) / i;     /* multiplicative formula, exact at each step */
        assert(F7(n, r) == c);
    }
    printf("PE 7: F(6,2) = %ld, F(10,5) = %ld  (binomial coefficients)\n", F7(6, 2), F7(10, 5));
    assert(F7(6, 2) == 15 && F7(10, 5) == 252);
    for (int n = 1; n <= 1000; n++) { int l = 0; while ((1 << (l + 1)) <= n) l++; assert(Lambda(n) == l); }
    printf("PE 8: Lambda(1)=%d Lambda(2)=%d Lambda(8)=%d Lambda(1000)=%d  (floor of log2)\n", Lambda(1), Lambda(2), Lambda(8), Lambda(1000));
    puts("PE 9: F(M,N) table, M rows 0..6, N columns 0..6");
    for (int M = 0; M <= 6; M++) { printf("  "); for (int N = 0; N <= 6; N++) printf("%5ld", F9(M, N)); puts(""); }
    /* checks: row 0 is all 1, the "M >= N >= 1" triangle is all 1, and N <= 0 never meets the base case, so
       every one of the 2^M branches ends at M = 0:  F(M,0) = 2^M.  For N > M the values are Pascal-like sums. */
    for (int N = -3; N <= 6; N++) assert(F9(0, N) == 1);
    for (int M = 1; M <= 8; M++) { for (int N = 1; N <= M; N++) assert(F9(M, N) == 1); assert(F9(M, 0) == (1L << M)); }
    assert(F9(1, 4) == 2 && F9(2, 3) == 4 && F9(2, 5) == 4 && F9(3, 4) == 8);
    { char s[] = "recursion"; rev(s, 0, (int)strlen(s) - 1); assert(strcmp(s, "noisrucer") == 0);
      char e[] = ""; rev(e, 0, -1); assert(e[0] == 0);
      printf("PE 10: \"recursion\" reversed is \"%s\"\n", s); }
    puts("ex19_01_stacks ok");
    return 0;
}
