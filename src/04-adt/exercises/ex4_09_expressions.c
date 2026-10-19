/* ============================================================
 *  ex4_09_expressions.c  —  Exercises 4.9-4.12, 4.15, 4.16
 *  Chapter 4: Abstract Data Types — solved exercises
 *
 *  4.9  Convert ( 5 * ( ( 9 * 8 ) + ( 7 * ( 4 + 6 ) ) ) ) to postfix.
 *  4.10 Show the stack contents while Program 4.2 evaluates
 *       5 9 * 8 7 4 6 + * 2 1 3 * + * + *
 *  4.11/4.12 Add - and /, unary minus and $ (square root), in floating
 *       point: (-(-1) + $((-1) * (-1) - (4 * (-1)))) / 2  must print 1.618034
 *  4.15 Convert a postfix expression to infix with a pushdown stack.
 *  4.16 One module, two stacks: a stack of numbers and a stack of operators.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 4.2-4.3, Exercises 4.9-4.16
 *  Build:   gcc -std=c99 -Wall -O2 ex4_09_expressions.c -o ex4_09 -lm
 * ============================================================ */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* ---- Program 4.3: fully parenthesized infix -> postfix ---- */
static void infix2postfix(const char *a, char *out)
{
    char st[64]; int n = 0, k = 0;
    for (const char *p = a; *p; p++) {
        if (*p == ')') { out[k++] = st[--n]; out[k++] = ' '; }
        if (*p == '+' || *p == '*' || *p == '-' || *p == '/') st[n++] = *p;
        if (isdigit((unsigned char)*p)) {
            while (isdigit((unsigned char)*p)) out[k++] = *p++;
            out[k++] = ' '; p--;
        }
    }
    out[k] = 0;
}

/* ---- Program 4.2 with a stack trace (4.10) ---- */
static long eval_postfix(const char *a, int trace)
{
    long s[64]; int n = 0;
    for (const char *p = a; *p; p++) {
        if (*p == ' ') continue;
        if (*p == '+') { n--; s[n - 1] += s[n]; }
        else if (*p == '*') { n--; s[n - 1] *= s[n]; }
        else if (*p == '-') { n--; s[n - 1] -= s[n]; }
        else if (*p == '/') { n--; s[n - 1] /= s[n]; }
        else { long v = 0; while (isdigit((unsigned char)*p)) v = 10 * v + (*p++ - '0'); p--; s[n++] = v; }
        if (trace) { printf("  %c :", *p); for (int i = 0; i < n; i++) printf(" %ld", s[i]); printf("\n"); }
    }
    return s[0];
}

/* ---- 4.15: postfix -> fully parenthesized infix, stack of strings ---- */
static void postfix2infix(const char *a, char *out)
{
    static char st[32][256]; int n = 0;
    for (const char *p = a; *p; p++) {
        if (*p == ' ') continue;
        if (strchr("+-*/", *p)) {
            char tmp[256];
            snprintf(tmp, sizeof tmp, "(%s %c %s)", st[n - 2], *p, st[n - 1]);
            n -= 2; strcpy(st[n++], tmp);
        } else {
            int k = 0; while (isdigit((unsigned char)*p)) st[n][k++] = *p++;
            st[n++][k] = 0; p--;
        }
    }
    strcpy(out, st[0]);
}

/* ---- 4.11/4.12/4.16: two stacks (numbers, operators), precedence,
   unary '-' (stored as 'n') and '$' (square root), floating point.  ---- */
static double nums[64]; static int nn;
static char ops[64];    static int no;
static int prec(char c) { return c == 'n' || c == '$' ? 3 : (c == '*' || c == '/') ? 2 : (c == '+' || c == '-') ? 1 : 0; }
static void apply(void)
{
    char c = ops[--no];
    if (c == 'n') { nums[nn - 1] = -nums[nn - 1]; return; }
    if (c == '$') { nums[nn - 1] = sqrt(nums[nn - 1]); return; }
    double b = nums[--nn], *a = &nums[nn - 1];
    if (c == '+') *a += b; else if (c == '-') *a -= b; else if (c == '*') *a *= b; else *a /= b;
}
static double eval_infix(const char *s)
{
    nn = no = 0;
    int expectOperand = 1;
    for (const char *p = s; *p; p++) {
        if (*p == ' ') continue;
        if (isdigit((unsigned char)*p) || *p == '.') {
            double v = 0, f = 0.1; int frac = 0;
            for (; isdigit((unsigned char)*p) || *p == '.'; p++)
                if (*p == '.') frac = 1; else if (!frac) v = 10 * v + (*p - '0'); else { v += f * (*p - '0'); f /= 10; }
            p--; nums[nn++] = v; expectOperand = 0;
        } else if (*p == '(') { ops[no++] = '('; expectOperand = 1; }
        else if (*p == ')') { while (ops[no - 1] != '(') apply(); no--; expectOperand = 0; }
        else {
            char c = *p;
            if (c == '-' && expectOperand) c = 'n';
            if (c == 'n' || c == '$') { ops[no++] = c; continue; }      /* prefix: just push */
            while (no && ops[no - 1] != '(' && prec(ops[no - 1]) >= prec(c)) apply();
            ops[no++] = c; expectOperand = 1;
        }
    }
    while (no) apply();
    return nums[0];
}

int main(void)
{
    char out[256];
    infix2postfix("( 5 * ( ( 9 * 8 ) + ( 7 * ( 4 + 6 ) ) ) )", out);
    printf("4.9  postfix: %s  = %ld\n", out, eval_postfix(out, 0));
    printf("4.10 stack while evaluating 5 9 * 8 7 4 6 + * 2 1 3 * + * + *\n");
    long v = eval_postfix("5 9 * 8 7 4 6 + * 2 1 3 * + * + *", 1);
    printf("     value %ld\n", v);
    postfix2infix("5 9 * 8 7 4 6 + * 2 1 3 * + * + *", out);
    printf("4.15 infix: %s\n", out);
    printf("4.11 (8 - 2) / 3 - 1 = %g\n", eval_infix("(8 - 2) / 3 - 1"));
    printf("4.12 (-(-1) + $((-1) * (-1) - (4 * (-1)))) / 2 = %f\n",
           eval_infix("(-(-1) + $((-1) * (-1) - (4 * (-1)))) / 2"));
    printf("4.16 5 * (((9 + 8) * (4 * 6)) + 7) = %g\n", eval_infix("5 * (((9 + 8) * (4 * 6)) + 7)"));
    return 0;
}
