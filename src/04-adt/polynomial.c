/* ============================================================
 *  polynomial.c  —  First-class polynomial ADT (array of coefficients)
 *  Chapter 4: Abstract Data Types
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 4.21-4.23
 *  Build:   gcc -std=c99 -Wall -O2 polynomial.c -o polynomial
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct { int N; double *a; } *Poly;

Poly POLYterm(double coeff, int exp) {
    Poly t = malloc(sizeof *t);
    t->a = calloc(exp + 1, sizeof(double));
    t->N = exp + 1;
    t->a[exp] = coeff;
    return t;
}
Poly POLYadd(Poly p, Poly q) {
    int n = p->N > q->N ? p->N : q->N;
    Poly t = malloc(sizeof *t);
    t->a = calloc(n, sizeof(double)); t->N = n;
    for (int i = 0; i < p->N; i++) t->a[i] += p->a[i];
    for (int i = 0; i < q->N; i++) t->a[i] += q->a[i];
    return t;
}
Poly POLYmult(Poly p, Poly q) {
    Poly t = malloc(sizeof *t);
    t->N = p->N + q->N - 1;
    t->a = calloc(t->N, sizeof(double));
    for (int i = 0; i < p->N; i++)
        for (int j = 0; j < q->N; j++)
            t->a[i + j] += p->a[i] * q->a[j];
    return t;
}
double POLYeval(Poly p, double x) {
    double t = 0.0;
    for (int i = p->N - 1; i >= 0; i--) t = t * x + p->a[i];   /* Horner */
    return t;
}
void POLYshow(Poly p) {
    for (int i = p->N - 1; i >= 0; i--)
        if (p->a[i] != 0.0) printf("%+.1f x^%d ", p->a[i], i);
    putchar('\n');
}

int main(void) {
    /* (1 + 2x) * (3 + 4x) = 3 + 10x + 8x^2 */
    Poly a = POLYterm(1, 0); a = POLYadd(a, POLYterm(2, 1));
    Poly b = POLYterm(3, 0); b = POLYadd(b, POLYterm(4, 1));
    Poly c = POLYmult(a, b);
    printf("product: "); POLYshow(c);
    printf("eval at 2: %.1f\n", POLYeval(c, 2.0));

    /* Program 4.21: binomial coefficients as the coefficients of (x+1)^N */
    int N = 10;
    Poly x = POLYadd(POLYterm(1, 1), POLYterm(1, 0));   /* x + 1 */
    Poly y = x;
    for (int i = 1; i < N; i++) y = POLYmult(x, y);
    printf("(x+1)^%d coefficients:", N);
    for (int i = 0; i < y->N; i++) printf(" %.0f", y->a[i]);
    putchar('\n');
    printf("(x+1)^%d at x=1: %.0f (= 2^%d)\n", N, POLYeval(y, 1.0), N);
    return 0;
}
