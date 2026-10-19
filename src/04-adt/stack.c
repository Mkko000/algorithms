/* ============================================================
 *  stack.c  —  Pushdown stack ADT: array + linked-list versions
 *              Plus a postfix-expression evaluator (classic client).
 *  Chapter 4: Abstract Data Types
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 4.1, 4.2, 4.3, 4.4, 4.5
 *  Build:   gcc -std=c99 -Wall -O2 stack.c -o stack
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* ---- array stack ---- */
#define MAXN 100
static int a_st[MAXN], a_N;
void AStack_init(void) { a_N = 0; }
int  AStack_empty(void){ return a_N == 0; }
void AStack_push(int v){ a_st[a_N++] = v; }
int  AStack_pop(void)  { return a_st[--a_N]; }

/* ---- linked-list stack ---- */
typedef struct snode *slink;
struct snode { int item; slink next; };
static slink head;
void LStack_init(void) { head = NULL; }
int  LStack_empty(void){ return head == NULL; }
void LStack_push(int v){
    slink t = malloc(sizeof *t); t->item = v; t->next = head; head = t;
}
int  LStack_pop(void){
    int v = head->item; slink t = head->next; free(head); head = t; return v;
}

/* Evaluate a postfix expression of single digits + operators + - * /.
   Example: "5 9 8 + 4 6 * * 7 + *"  =  2075  (Sedgewick's classic). */
int eval_postfix(const char *s) {
    AStack_init();
    for (int i = 0; s[i]; i++) {
        if (isdigit((unsigned char)s[i])) AStack_push(s[i] - '0');
        if (s[i] == '+') AStack_push(AStack_pop() + AStack_pop());
        if (s[i] == '*') AStack_push(AStack_pop() * AStack_pop());
        if (s[i] == '-') { int b = AStack_pop(); AStack_push(AStack_pop() - b); }
        if (s[i] == '/') { int b = AStack_pop(); AStack_push(AStack_pop() / b); }
    }
    return AStack_pop();
}

/* Convert infix (no precedence, fully parenthesized or using + * only
   with Sedgewick's left-to-right convention) to postfix. Simplified
   Dijkstra shunting-yard for + and * with parentheses. */
void infix_to_postfix(const char *in, char *out) {
    LStack_init();
    int k = 0;
    for (int i = 0; in[i]; i++) {
        char c = in[i];
        if (isdigit((unsigned char)c)) out[k++] = c;
        else if (c == '(') LStack_push(c);
        else if (c == ')') {
            while (!LStack_empty() && (c = LStack_pop()) != '(') out[k++] = c;
        } else if (c == '+' || c == '*' || c == '-' || c == '/') {
            /* pop higher-or-equal precedence ops first */
            while (!LStack_empty()) {
                char top = head->item;   /* peek without pop */
                int prec_top = (top=='*'||top=='/') ? 2 : (top=='+'||top=='-') ? 1 : 0;
                int prec_c   = (c  =='*'||c  =='/') ? 2 : 1;
                if (prec_top >= prec_c) out[k++] = LStack_pop();
                else break;
            }
            LStack_push(c);
        }
    }
    while (!LStack_empty()) out[k++] = LStack_pop();
    out[k] = '\0';
}

int main(void) {
    const char *pf = "598+46**7+*";
    printf("postfix \"%s\" = %d\n", pf, eval_postfix(pf));

    char out[64];
    infix_to_postfix("(5*((9+8)*(4*6)+7))", out);
    printf("infix -> postfix: %s\n", out);
    printf("eval that: %d\n", eval_postfix(out));
    return 0;
}
