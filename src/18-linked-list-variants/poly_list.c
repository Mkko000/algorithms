/* ============================================================
 *  poly_list.c  —  polynomials in x and y as a linked list of terms
 *
 *  Each node = (coefficient, x-exponent, y-exponent). The list is kept SORTED by
 *  (x-exp desc, y-exp desc) and never holds a zero coefficient, so add / multiply
 *  are merge-like single passes. Operations: add, multiply by number, multiply
 *  two polynomials, derivative, evaluate, print.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Thareja ch.6 RQ5 and PE26; Lipschutz ch.5 (polynomials), Weiss ch.3.2.3
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 poly_list.c -o poly_list -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

//>>term
typedef struct term { int c, ex, ey; struct term *next; } term;

/* order: bigger x-exponent first, then bigger y-exponent. returns >0 if (ax,ay) comes first */
static int before(int ax, int ay, int bx, int by) { return ax != bx ? ax > bx : ay > by; }

static term *new_term(int c, int ex, int ey)
{
    term *t = malloc(sizeof *t);
    if (!t) { perror("malloc"); exit(1); }
    t->c = c; t->ex = ex; t->ey = ey; t->next = NULL;
    return t;
}
/* append at the tail through a "pointer to the last next-field" (no special case for empty) */
static term **append(term **tail, int c, int ex, int ey)
{
    if (c == 0) return tail;
    *tail = new_term(c, ex, ey);
    return &(*tail)->next;
}
static void poly_free(term *p) { while (p) { term *t = p->next; free(p); p = t; } }
//<<term

//>>add
/* p + q : two-pointer merge. Equal exponents add their coefficients (and vanish if 0). */
static term *poly_add(const term *p, const term *q)
{
    term *res = NULL, **tail = &res;
    while (p && q) {
        if (p->ex == q->ex && p->ey == q->ey) { tail = append(tail, p->c + q->c, p->ex, p->ey); p = p->next; q = q->next; }
        else if (before(p->ex, p->ey, q->ex, q->ey)) { tail = append(tail, p->c, p->ex, p->ey); p = p->next; }
        else { tail = append(tail, q->c, q->ex, q->ey); q = q->next; }
    }
    for (; p; p = p->next) tail = append(tail, p->c, p->ex, p->ey);
    for (; q; q = q->next) tail = append(tail, q->c, q->ex, q->ey);
    return res;
}
/* k * p  (Thareja PE 26) */
static term *poly_scale(const term *p, int k)
{
    term *res = NULL, **tail = &res;
    for (; p; p = p->next) tail = append(tail, p->c * k, p->ex, p->ey);
    return res;
}
//<<add

//>>mul
/* p * q : multiply p by each term of q and add the partial products.
   with m and n terms it costs O(m*n) multiplications plus n merges of growing size. */
static term *poly_mul(const term *p, const term *q)
{
    term *res = NULL;
    for (; q; q = q->next) {
        term *part = NULL, **tail = &part;
        for (const term *t = p; t; t = t->next) tail = append(tail, t->c * q->c, t->ex + q->ex, t->ey + q->ey);
        term *sum = poly_add(res, part);
        poly_free(res); poly_free(part);
        res = sum;
    }
    return res;
}
/* d/dx : c*ex x^(ex-1) y^ey (terms with ex==0 vanish) */
static term *poly_dx(const term *p)
{
    term *res = NULL, **tail = &res;
    for (; p; p = p->next) if (p->ex > 0) tail = append(tail, p->c * p->ex, p->ex - 1, p->ey);
    return res;
}
static double poly_eval(const term *p, double x, double y)
{
    double s = 0;
    for (; p; p = p->next) s += p->c * pow(x, p->ex) * pow(y, p->ey);
    return s;
}
//<<mul

static void poly_print(const char *tag, const term *p)
{
    printf("%s", tag);
    if (!p) { puts("0"); return; }
    for (int first = 1; p; p = p->next, first = 0) {
        int c = p->c, a = abs(c);
        printf(first ? (c < 0 ? "-" : "") : (c < 0 ? " - " : " + "));
        if (a != 1 || (p->ex == 0 && p->ey == 0)) printf("%d", a);
        if (p->ex) { printf("x"); if (p->ex > 1) printf("^%d", p->ex); }
        if (p->ey) { printf("y"); if (p->ey > 1) printf("^%d", p->ey); }
    }
    puts("");
}
static term *build(const int (*a)[3], int n)          /* helper: rows of {c, ex, ey}, already sorted */
{
    term *res = NULL, **tail = &res;
    for (int i = 0; i < n; i++) tail = append(tail, a[i][0], a[i][1], a[i][2]);
    return res;
}
static int equal(const term *p, const term *q)
{
    for (; p && q; p = p->next, q = q->next) if (p->c != q->c || p->ex != q->ex || p->ey != q->ey) return 0;
    return !p && !q;
}

int main(void)
{
    /* Thareja RQ5:  7x^3y^2 - 8x^2y + 3xy + 11x - 4 */
    static const int A[][3] = {{7,3,2},{-8,2,1},{3,1,1},{11,1,0},{-4,0,0}};
    term *p = build(A, 5);
    poly_print("p = ", p);
    assert(fabs(poly_eval(p, 2, 3) - (7*8*9 - 8*4*3 + 3*2*3 + 22 - 4)) < 1e-9);

    static const int B[][3] = {{-7,3,2},{5,2,0},{4,0,0}};
    term *q = build(B, 3), *s = poly_add(p, q);
    poly_print("q = ", q);
    poly_print("p+q = ", s);                       /* the x^3y^2 terms cancel completely */
    static const int S[][3] = {{-8,2,1},{5,2,0},{3,1,1},{11,1,0}};
    term *es = build(S, 4);
    assert(equal(s, es));
    poly_free(es);

    term *k = poly_scale(p, 3);
    poly_print("3p = ", k);
    term *d = poly_dx(p);
    poly_print("dp/dx = ", d);                     /* 21x^2y^2 - 16xy + 3y + 11 */
    static const int D[][3] = {{21,2,2},{-16,1,1},{3,0,1},{11,0,0}};
    term *ed = build(D, 4);
    assert(equal(d, ed));
    poly_free(ed);

    /* (x + y + 1)^2 via multiplication, checked against the closed form at many points */
    static const int L[][3] = {{1,1,0},{1,0,1},{1,0,0}};
    term *l = build(L, 3), *sq = poly_mul(l, l);
    poly_print("(x+y+1)^2 = ", sq);
    for (int xi = -3; xi <= 3; xi++) for (int yi = -3; yi <= 3; yi++)
        assert(fabs(poly_eval(sq, xi, yi) - (xi + yi + 1) * (xi + yi + 1)) < 1e-9);
    term *prod = poly_mul(p, q);                    /* evaluation homomorphism: (pq)(a,b) = p(a,b) q(a,b) */
    for (int xi = -2; xi <= 2; xi++) for (int yi = -2; yi <= 2; yi++)
        assert(fabs(poly_eval(prod, xi, yi) - poly_eval(p, xi, yi) * poly_eval(q, xi, yi)) < 1e-6);
    puts("product and sum verified at 25+49 points");
    poly_free(p); poly_free(q); poly_free(s); poly_free(k); poly_free(d); poly_free(l); poly_free(sq); poly_free(prod);
    return 0;
}
