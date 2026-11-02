/* ============================================================
 *  expr_convert.c  —  infix, postfix and prefix: conversions and evaluation
 *
 *  tokens: operands are runs of letters/digits ("A", "14"); operators + - * / % ^ ; ( )
 *  precedence: ^ (right-associative)  >  * / %  >  + -     (all others left-associative)
 *
 *    infix  -> postfix        shunting-yard with an operator stack
 *    infix  -> prefix         (1) reverse the tokens, swap ( ), run the algorithm, reverse back
 *                             (2) postfix first, then a stack of strings  -- both are checked equal
 *    postfix/prefix -> infix  stack of fully parenthesised strings (convert())
 *    evaluate postfix (left to right) and prefix (right to left) with one operand stack
 *
 *  Verified on 5000 random expressions against an independent recursive-descent evaluator.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja ch.7 (7.5-7.6), Lipschutz ch.6.6 (Polish notation), Weiss ch.3.3.3,
 *                Bandyopadhyay & Dey ch.6 (application of stacks)
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 expr_convert.c -o expr_convert -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>

#define MAXT 128
#define TLEN 16

//>>tok
typedef struct { char t[MAXT][TLEN]; int n; } toks;

/* split "(A-B)+C*10" into  ( A - B ) + C * 10   returns -1 on a bad character or overflow */
static int tokenize(const char *s, toks *o)
{
    o->n = 0;
    for (; *s; ) {
        if (isspace((unsigned char)*s)) { s++; continue; }
        if (o->n >= MAXT) return -1;
        if (isalnum((unsigned char)*s)) {
            int k = 0;
            while (isalnum((unsigned char)*s)) { if (k < TLEN - 1) o->t[o->n][k++] = *s; s++; }
            o->t[o->n][k] = 0;
        } else if (strchr("+-*/%^()", *s)) { o->t[o->n][0] = *s++; o->t[o->n][1] = 0; }
        else return -1;
        o->n++;
    }
    return o->n;
}
static int is_op(const char *t)    { return t[1] == 0 && strchr("+-*/%^", t[0]) != NULL; }
static int prec(char c)            { return c == '^' ? 3 : (c == '*' || c == '/' || c == '%') ? 2 : 1; }
static int right_assoc(char c)     { return c == '^'; }
static void join(const toks *t, char *out, size_t cap)
{
    out[0] = 0;
    for (int i = 0; i < t->n; i++) { if (i) strncat(out, " ", cap - strlen(out) - 1); strncat(out, t->t[i], cap - strlen(out) - 1); }
}
//<<tok

//>>postfix
/* shunting-yard. An operator waits on the stack until an operator of LOWER precedence
   (or equal, for left-associative ones) arrives and tells it "your operands are complete". */
static int to_postfix(const toks *in, toks *out)
{
    char stack[MAXT]; int sp = 0; out->n = 0;
    for (int i = 0; i < in->n; i++) {
        const char *t = in->t[i];
        if (t[0] == '(' ) stack[sp++] = '(';
        else if (t[0] == ')') {
            while (sp && stack[sp - 1] != '(') { out->t[out->n][0] = stack[--sp]; out->t[out->n++][1] = 0; }
            if (!sp) return -1;                              /* unmatched ')' */
            sp--;                                            /* discard '(' */
        } else if (is_op(t)) {
            char c = t[0];
            while (sp && stack[sp - 1] != '(' &&
                   (prec(stack[sp - 1]) > prec(c) || (prec(stack[sp - 1]) == prec(c) && !right_assoc(c)))) {
                out->t[out->n][0] = stack[--sp]; out->t[out->n++][1] = 0;
            }
            stack[sp++] = c;
        } else { strcpy(out->t[out->n++], t); }              /* operand goes straight to the output */
    }
    while (sp) { if (stack[sp - 1] == '(') return -1; out->t[out->n][0] = stack[--sp]; out->t[out->n++][1] = 0; }
    return out->n;
}
//<<postfix

//>>prefix
/* method 1: reverse + swap parentheses + shunting-yard with the associativity test FLIPPED
   (because after reversing, left-associative operators group from the other side) */
static int to_prefix_reverse(const toks *in, toks *out)
{
    toks rev; rev.n = in->n;
    for (int i = 0; i < in->n; i++) {
        const char *t = in->t[in->n - 1 - i];
        strcpy(rev.t[i], t);
        if (t[0] == '(') strcpy(rev.t[i], ")"); else if (t[0] == ')') strcpy(rev.t[i], "(");
    }
    char stack[MAXT]; int sp = 0; toks tmp; tmp.n = 0;
    for (int i = 0; i < rev.n; i++) {
        const char *t = rev.t[i];
        if (t[0] == '(' && t[1] == 0) stack[sp++] = '(';
        else if (t[0] == ')' && t[1] == 0) {
            while (sp && stack[sp - 1] != '(') { tmp.t[tmp.n][0] = stack[--sp]; tmp.t[tmp.n++][1] = 0; }
            if (!sp) return -1;
            sp--;
        } else if (is_op(t)) {
            char c = t[0];
            while (sp && stack[sp - 1] != '(' &&
                   (prec(stack[sp - 1]) > prec(c) || (prec(stack[sp - 1]) == prec(c) && right_assoc(c)))) {
                tmp.t[tmp.n][0] = stack[--sp]; tmp.t[tmp.n++][1] = 0;
            }
            stack[sp++] = c;
        } else strcpy(tmp.t[tmp.n++], t);
    }
    while (sp) { if (stack[sp - 1] == '(') return -1; tmp.t[tmp.n][0] = stack[--sp]; tmp.t[tmp.n++][1] = 0; }
    out->n = tmp.n;
    for (int i = 0; i < tmp.n; i++) strcpy(out->t[i], tmp.t[tmp.n - 1 - i]);
    return out->n;
}

/* concatenate NULL-terminated pieces into a fresh heap string (nothing can be truncated) */
static char *catv(const char *first, ...)
{
    va_list ap; size_t len = 0;
    va_start(ap, first);
    for (const char *p = first; p; p = va_arg(ap, const char *)) len += strlen(p);
    va_end(ap);
    char *s = malloc(len + 1), *w = s;
    if (!s) { perror("malloc"); exit(1); }
    va_start(ap, first);
    for (const char *p = first; p; p = va_arg(ap, const char *)) { size_t k = strlen(p); memcpy(w, p, k); w += k; }
    va_end(ap);
    *w = 0;
    return s;
}
/* One stack-of-strings routine for the three string conversions:
     mode 0  postfix -> prefix           "op left right"
     mode 1  postfix -> infix            "(left op right)"
     mode 2  prefix  -> infix            same, but scan right to left
   returns a heap string (caller frees) or NULL if the input is malformed. */
static char *convert(const toks *pf, int mode)
{
    char *st[MAXT]; int sp = 0;
    for (int k = 0; k < pf->n; k++) {
        const char *t = pf->t[mode == 2 ? pf->n - 1 - k : k];
        if (!is_op(t)) { st[sp++] = catv(t, NULL); continue; }
        if (sp < 2) { while (sp) free(st[--sp]); return NULL; }
        char *y = st[--sp], *x = st[--sp];
        char *left = mode == 2 ? y : x, *right = mode == 2 ? x : y;   /* scanning backwards: first pop = LEFT */
        st[sp++] = mode == 0 ? catv(t, " ", left, " ", right, NULL)
                             : catv("(", left, " ", t, " ", right, ")", NULL);
        free(x); free(y);
    }
    if (sp != 1) { while (sp) free(st[--sp]); return NULL; }
    return st[0];
}
//<<prefix

//>>eval
static double apply(char op, double a, double b)
{
    switch (op) {
    case '+': return a + b;  case '-': return a - b;
    case '*': return a * b;  case '/': return a / b;
    case '%': return fmod(a, b);
    default:  return pow(a, b);
    }
}
/* postfix: scan LEFT to RIGHT. operand -> push; operator -> pop b, pop a, push a op b */
static int eval_postfix(const toks *pf, double *res)
{
    double st[MAXT]; int sp = 0;
    for (int i = 0; i < pf->n; i++) {
        const char *t = pf->t[i];
        if (is_op(t)) { if (sp < 2) return -1; double b = st[--sp], a = st[--sp]; st[sp++] = apply(t[0], a, b); }
        else st[sp++] = atof(t);
    }
    if (sp != 1) return -1;
    *res = st[0]; return 0;
}
/* prefix: scan RIGHT to LEFT. the first operand popped is the LEFT operand: a op b */
static int eval_prefix(const toks *pf, double *res)
{
    double st[MAXT]; int sp = 0;
    for (int i = pf->n - 1; i >= 0; i--) {
        const char *t = pf->t[i];
        if (is_op(t)) { if (sp < 2) return -1; double a = st[--sp], b = st[--sp]; st[sp++] = apply(t[0], a, b); }
        else st[sp++] = atof(t);
    }
    if (sp != 1) return -1;
    *res = st[0]; return 0;
}
//<<eval

/* ---- independent oracle: recursive descent  expr = term {(+|-) term}; term = pow {(*|/|%) pow};
        pow = atom [^ pow]  (right assoc); atom = number | ( expr ) ---- */
static const char *P;
static double r_expr(void);
static double r_atom(void)
{
    while (*P == ' ') P++;
    if (*P == '(') { P++; double v = r_expr(); while (*P == ' ') P++; P++; return v; }
    char *e; double v = strtod(P, &e); P = e; return v;
}
static double r_pow(void)
{
    double a = r_atom(); while (*P == ' ') P++;
    if (*P == '^') { P++; return pow(a, r_pow()); }
    return a;
}
static double r_term(void)
{
    double a = r_pow();
    for (;;) { while (*P == ' ') P++;
        if (*P == '*') { P++; a *= r_pow(); } else if (*P == '/') { P++; a /= r_pow(); }
        else if (*P == '%') { P++; a = fmod(a, r_pow()); } else return a; }
}
static double r_expr(void)
{
    double a = r_term();
    for (;;) { while (*P == ' ') P++;
        if (*P == '+') { P++; a += r_term(); } else if (*P == '-') { P++; a -= r_term(); } else return a; }
}

static void gen(char *out, int depth)                         /* random infix expression, no redundant structure */
{
    const char ops[] = "+-*/^%";
    int factors = 1 + rand() % 3;
    for (int i = 0; i < factors; i++) {
        if (i) { char o[3] = {ops[rand() % (depth > 1 ? 6 : 4)], 0, 0}; strcat(out, o); }
        if (depth > 0 && rand() % 3 == 0) { strcat(out, "("); gen(out, depth - 1); strcat(out, ")"); }
        else { char n[8]; snprintf(n, sizeof n, "%d", 1 + rand() % 9); strcat(out, n); }
    }
}

static void show(const char *infix)
{
    toks a, pf, pr; char buf[1024]; double v = 0, v2 = 0;
    tokenize(infix, &a);
    to_postfix(&a, &pf); to_prefix_reverse(&a, &pr);
    char *pre2 = convert(&pf, 0);                              /* second method: via postfix */
    join(&pf, buf, sizeof buf); printf("infix   %s\n  postfix: %s\n", infix, buf);
    join(&pr, buf, sizeof buf); printf("  prefix : %s\n", buf);
    assert(pre2 && strcmp(buf, pre2) == 0);
    free(pre2);
    char *full = convert(&pf, 1); printf("  full ( ): %s\n", full); free(full);
    int numeric = 1;
    for (const char *c = infix; *c; c++) if (isalpha((unsigned char)*c)) numeric = 0;
    if (numeric) {
        assert(!eval_postfix(&pf, &v) && !eval_prefix(&pr, &v2));
        printf("  value  : %g\n", v);
        assert(v == v2);
    }
}

int main(void)
{
    show("(A - B) + C * D / E - C");
    show("A * (B + C) / D ^ E ^ F");
    show("10 + ((7 - 5) + 10) / 2");

    toks t, pf; char s[1024]; double v;
    tokenize("A ^ B ^ C", &t); to_postfix(&t, &pf); join(&pf, s, sizeof s);
    assert(strcmp(s, "A B C ^ ^") == 0);                      /* ^ is right-associative */
    tokenize("A - B - C", &t); to_postfix(&t, &pf); join(&pf, s, sizeof s);
    assert(strcmp(s, "A B - C -") == 0);                      /* - is left-associative  */
    tokenize("(A + B", &t); assert(to_postfix(&t, &pf) == -1);
    tokenize("A + B)", &t); assert(to_postfix(&t, &pf) == -1);
    tokenize("1 +", &t);    assert(eval_postfix(&t, &v) == -1);          /* malformed input is detected, no crash */
    tokenize("+ 1", &t);    assert(eval_prefix(&t, &v) == -1);
    tokenize("1 2", &t);    assert(eval_postfix(&t, &v) == -1 && convert(&t, 1) == NULL);
    assert(tokenize("A $ B", &t) == -1);
    tokenize("* - + A B C D", &t);                                         /* Thareja RQ 11(a) */
    { char *r = convert(&t, 2); assert(strcmp(r, "(((A + B) - C) * D)") == 0); free(r); }
    tokenize("A B + C * D -", &t);                                         /* Thareja RQ 10(a) */
    { char *r = convert(&t, 1); assert(strcmp(r, "(((A + B) * C) - D)") == 0); free(r); }

    srand(11);
    int tested = 0;
    for (int round = 0; round < 5000; round++) {
        char e[512] = ""; gen(e, 3);
        toks a, post, pre; tokenize(e, &a);
        assert(to_postfix(&a, &post) > 0 && to_prefix_reverse(&a, &pre) > 0);
        char *pre2 = convert(&post, 0); char pre1[1024]; join(&pre, pre1, sizeof pre1);
        assert(pre2 && strcmp(pre1, pre2) == 0);                                           /* two methods agree */
        free(pre2);
        P = e; double want = r_expr();
        double a1, a2;
        assert(eval_postfix(&post, &a1) == 0 && eval_prefix(&pre, &a2) == 0);
        if (!isfinite(want) || fabs(want) > 1e15) continue;                                  /* skip 1/0, overflow */
        double tol = 1e-9 * (1 + fabs(want));
        assert(fabs(a1 - want) <= tol && fabs(a2 - want) <= tol);
        char *inf1 = convert(&post, 1), *inf2 = convert(&pre, 2);
        assert(inf1 && inf2 && strcmp(inf1, inf2) == 0);                                     /* both give the same full-paren infix */
        P = inf1; double back = r_expr();                                                    /* ... and it means the same */
        assert(fabs(back - want) <= tol);
        free(inf1); free(inf2);
        tested++;
    }
    printf("5000 random expressions: postfix == prefix == oracle (%d finite values compared)\n", tested);
    return 0;
}
