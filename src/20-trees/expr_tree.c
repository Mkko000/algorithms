/* ============================================================
 *  expr_tree.c  —  expression trees
 *
 *   * build a tree from POSTFIX (stack of subtrees), from PREFIX (recursion), from INFIX (precedence parser)
 *   * print infix (fully parenthesised and minimal), prefix and postfix by the three traversals
 *   * evaluate with variables bound to values
 *  Operators:  +  -  *  /  %  ^   (^ is right-associative, the others left-associative)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja 9.2.5 + Examples 9.2-9.4 + Exercises 4, 7, 8; Lipschutz 7.x (algebraic expressions);
 *                Weiss 4.2.2 (expression trees); Bandyopadhyay & Dey 12.2
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 expr_tree.c -o expr_tree -lm
 * ============================================================ */
#define TREE_NO_MAIN
#include "tree_core.c"
#include <math.h>
#include <ctype.h>

static int is_op(const char *s) { return strlen(s) == 1 && strchr("+-*/%^", s[0]) != NULL; }
static int prec(char c) { return c == '^' ? 3 : (c == '*' || c == '/' || c == '%') ? 2 : 1; }

//>>fromfix
/* POSTFIX:  operand -> push a leaf;  operator -> pop right, pop left, push the new node. */
static Node *from_postfix(const char *s)
{
    Node *st[64]; int sp = 0;
    char buf[256]; strncpy(buf, s, 255); buf[255] = 0;
    for (char *tok = strtok(buf, " "); tok; tok = strtok(NULL, " ")) {
        if (is_op(tok)) {
            Node *r = st[--sp], *l = st[--sp];
            st[sp++] = mk(tok, l, r);
        } else st[sp++] = mk(tok, NULL, NULL);
    }
    assert(sp == 1);
    return st[0];
}
/* PREFIX: read an operator, then build its two subtrees recursively. */
static Node *from_prefix_rec(char **toks, int *i, int n)
{
    assert(*i < n);
    const char *t = toks[(*i)++];
    if (!is_op(t)) return mk(t, NULL, NULL);
    Node *l = from_prefix_rec(toks, i, n);
    Node *r = from_prefix_rec(toks, i, n);
    return mk(t, l, r);
}
static Node *from_prefix(const char *s)
{
    char buf[256]; strncpy(buf, s, 255); buf[255] = 0;
    char *toks[64]; int n = 0;
    for (char *tok = strtok(buf, " "); tok; tok = strtok(NULL, " ")) { toks[n] = malloc(strlen(tok) + 1); strcpy(toks[n++], tok); }
    int i = 0; Node *t = from_prefix_rec(toks, &i, n);
    assert(i == n);
    for (int k = 0; k < n; k++) free(toks[k]);
    return t;
}
//<<fromfix

//>>frominfix
/* recursive-descent parser for single-token operands (letters or numbers) and ( ).
     expr   := term   { (+|-) term }
     term   := factor { (*|/|%) factor }
     factor := atom [ ^ factor ]            <- right recursion = right associativity
     atom   := name | number | '(' expr ')'                                                         */
static const char *ip;
static Node *p_expr(void);
static Node *p_atom(void)
{
    while (*ip == ' ') ip++;
    if (*ip == '(') { ip++; Node *e = p_expr(); while (*ip == ' ') ip++; assert(*ip == ')'); ip++; return e; }
    char lab[12]; int k = 0;
    while (isalnum((unsigned char)*ip)) lab[k++] = *ip++;
    assert(k > 0); lab[k] = 0;
    return mk(lab, NULL, NULL);
}
static Node *p_factor(void)
{
    Node *a = p_atom();
    while (*ip == ' ') ip++;
    if (*ip == '^') { ip++; Node *b = p_factor(); return mk("^", a, b); }
    return a;
}
static Node *p_term(void)
{
    Node *a = p_factor();
    for (;;) {
        while (*ip == ' ') ip++;
        if (*ip == '*' || *ip == '/' || *ip == '%') { char op[2] = { *ip++, 0 }; a = mk(op, a, p_factor()); }
        else return a;
    }
}
static Node *p_expr(void)
{
    Node *a = p_term();
    for (;;) {
        while (*ip == ' ') ip++;
        if (*ip == '+' || *ip == '-') { char op[2] = { *ip++, 0 }; a = mk(op, a, p_term()); }
        else return a;
    }
}
static Node *from_infix(const char *s) { ip = s; Node *t = p_expr(); while (*ip == ' ') ip++; assert(*ip == 0); return t; }
//<<frominfix

//>>printfix
/* fully parenthesised infix (every operator node gets brackets) */
static void infix_full(const Node *t, char *out)
{
    if (!is_op(t->lab) || !t->l) { strcat(out, t->lab); return; }
    strcat(out, "("); infix_full(t->l, out); strcat(out, t->lab); infix_full(t->r, out); strcat(out, ")");
}
/* minimal parentheses: a child needs brackets only if it binds weaker than its parent, or equally
   on the "wrong" side (left side of ^, right side of - / % and friends) */
static void infix_min(const Node *t, char *out)
{
    if (!is_op(t->lab) || !t->l) { strcat(out, t->lab); return; }
    char p = t->lab[0];
    const Node *kids[2] = { t->l, t->r };
    for (int k = 0; k < 2; k++) {
        const Node *c = kids[k];
        int need = 0;
        if (is_op(c->lab) && c->l) {
            char q = c->lab[0];
            if (prec(q) < prec(p)) need = 1;
            else if (prec(q) == prec(p)) need = (p == '^') ? (k == 0) : (k == 1);
        }
        if (need) strcat(out, "(");
        infix_min(c, out);
        if (need) strcat(out, ")");
        if (k == 0) strcat(out, t->lab);
    }
}
//<<printfix

//>>evalt
typedef struct { const char *name; double v; } Bind;
static double lookup(const Bind *b, int nb, const char *name)
{
    if (isdigit((unsigned char)name[0])) return atof(name);
    for (int i = 0; i < nb; i++) if (!strcmp(b[i].name, name)) return b[i].v;
    assert(!"unbound variable");
    return 0;
}
static double eval_tree(const Node *t, const Bind *b, int nb)
{
    if (!t->l) return lookup(b, nb, t->lab);
    double x = eval_tree(t->l, b, nb), y = eval_tree(t->r, b, nb);
    switch (t->lab[0]) {
        case '+': return x + y;
        case '-': return x - y;
        case '*': return x * y;
        case '/': return x / y;
        case '%': return fmod(x, y);
        default:  return pow(x, y);
    }
}
//<<evalt

#ifndef EXPR_NO_MAIN
static void expect(const Node *t, const char *infix_full_s, const char *pre_s, const char *post_s)
{
    char o[256] = ""; infix_full(t, o);
    Buf a = {{0}}, b = {{0}};
    pre(t, &a); post(t, &b);
    if (infix_full_s) assert(!strcmp(o, infix_full_s));
    if (pre_s) assert(!strcmp(a.s, pre_s));
    if (post_s) assert(!strcmp(b.s, post_s));
}
int main(void)
{
    /* Fig 9.13 and the three worked examples */
    Node *f13 = from_infix("(a-b)+(c*d)");
    expect(f13, "((a-b)+(c*d))", "+ - a b * c d", "a b - c d * +");
    Node *e92 = from_infix("((a+b)-(c*d))%((e^f)/(g-h))");
    expect(e92, "(((a+b)-(c*d))%((e^f)/(g-h)))", "% - + a b * c d / ^ e f - g h", "a b + c d * - e f ^ g h - / %");
    /* Example 9.3: given the tree, write the expression -> build it from its prefix and read it back */
    Node *e93 = from_prefix("^ + / a b * c d / % f g - h i");
    expect(e93, "(((a/b)+(c*d))^((f%g)/(h-i)))", NULL, "a b / c d * + f g % h i - / ^");
    /* Example 9.4: a + b / c * d - e  (precedence decides the tree) */
    Node *e94 = from_infix("a + b / c * d - e");
    expect(e94, "((a+((b/c)*d))-e)", "- + a * / b c d e", "a b c / d * + e -");
    /* Review Q4: postfix AB+C*D-  */
    Node *q4 = from_postfix("A B + C * D -");
    expect(q4, "(((A+B)*C)-D)", "- * + A B C D", "A B + C * D -");
    /* Review Q8: prefix -/ab*+bcd -> infix, tree */
    Node *q8 = from_prefix("- / a b * + b c d");
    expect(q8, "((a/b)-((b+c)*d))", "- / a b * + b c d", "a b / b c + d * -");
    /* Review Q7: tree  e + (D - ((a/b)*c))   with a=30 b=10 c=2 d=30 e=10 */
    Node *q7 = parse("+(e,-(d,*(/(a,b),c)))");
    expect(q7, "(e+(d-((a/b)*c)))", "+ e - d * / a b c", "e d a b / c * - +");
    Bind bd[] = { {"a",30}, {"b",10}, {"c",2}, {"d",30}, {"e",10} };
    assert(eval_tree(q7, bd, 5) == 34.0);
    char m[256] = ""; infix_min(q7, m);
    assert(!strcmp(m, "e+(d-a/b*c)"));

    /* Weiss Fig 4.14: (a + b * c) + ((d * e + f) * g) built from its POSTFIX form  a b c * + d e * f + g * +  */
    Node *w414 = from_postfix("a b c * + d e * f + g * +");
    expect(w414, "((a+(b*c))+(((d*e)+f)*g))", "+ + a * b c * + * d e f g", "a b c * + d e * f + g * +");
    { char m2[128] = ""; infix_min(w414, m2); assert(!strcmp(m2, "a+b*c+(d*e+f)*g")); printf("Weiss Fig 4.14 minimal infix: %s\n", m2); }
    free_tree(w414);

    /* associativity: minimal brackets must re-parse to the same tree */
    const char *tests[] = { "a-(b-c)", "(a-b)-c", "a^b^c", "(a^b)^c", "(a+b)*c", "a+b*c", "a/(b*c)", "a/b*c" };
    for (int i = 0; i < 8; i++) {
        Node *t = from_infix(tests[i]);
        char s[128] = ""; infix_min(t, s);
        Node *u = from_infix(s);
        assert(copies(t, u));
        printf("%-10s -> minimal %-10s\n", tests[i], s);
        free_tree(t); free_tree(u);
    }
    /* postfix <-> infix <-> prefix on the numeric expression */
    Node *n1 = from_infix("2^3^2 + 8 - 3 - 2");
    assert(eval_tree(n1, NULL, 0) == 512 + 8 - 3 - 2);
    printf("2^3^2 + 8 - 3 - 2 = %g\n", eval_tree(n1, NULL, 0));

    Node *all[] = { f13, e92, e93, e94, q4, q8, q7, n1 };
    for (int i = 0; i < 8; i++) free_tree(all[i]);
    printf("expr_tree: all tests passed\n");
    return 0;
}
#endif
