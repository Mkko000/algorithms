/* ============================================================
 *  complex.c  —  Complex numbers: from a transparent struct to a
 *                first-class ADT (opaque handle)
 *  Chapter 4: Abstract Data Types (Section 4.8)
 *
 *  Program 4.13 is a client: compute the N-th roots of unity and check
 *  that each one raised to the N-th power is 1. Programs 4.14/4.15 give
 *  a struct-based "Complex" (client can see re and im). Programs
 *  4.16/4.17 hide the representation: the client only holds a pointer
 *  to an incomplete type, so the implementation could switch to polar
 *  form without recompiling clients. Cost: allocation per value and
 *  the question "who frees it?" — the classic first-class-ADT tradeoff.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 4.13-4.17
 *  Build:   gcc -std=c99 -Wall -O2 complex.c -o complex -lm
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.14159265358979323846

/* -------- version 1: transparent struct (Programs 4.14-4.15) -------- */
typedef struct { float Re; float Im; } Complex;
Complex COMPLEXinit(float Re, float Im) { Complex t; t.Re = Re; t.Im = Im; return t; }
float   Re(Complex z) { return z.Re; }
float   Im(Complex z) { return z.Im; }
Complex COMPLEXmult(Complex a, Complex b) {
    return COMPLEXinit(a.Re * b.Re - a.Im * b.Im, a.Re * b.Im + a.Im * b.Re);
}

/* -------- version 2: first-class ADT, opaque handle (4.16-4.17) -------- */
typedef struct complex *CPX;          /* in the .h file the struct is NOT defined */
struct complex { float Re; float Im; };
CPX  CPXinit(float re, float im) { CPX t = malloc(sizeof *t); t->Re = re; t->Im = im; return t; }
CPX  CPXmult(CPX a, CPX b) { return CPXinit(a->Re * b->Re - a->Im * b->Im, a->Re * b->Im + a->Im * b->Re); }
float CPXre(CPX z) { return z->Re; }
float CPXim(CPX z) { return z->Im; }
void CPXfree(CPX z) { free(z); }

int main(void) {
    int N = 8;
    printf("%d roots of unity (struct version):\n", N);
    for (int i = 0; i < N; i++) {                     /* Program 4.13 */
        float theta = (float)(2.0 * PI * i / N);
        Complex t = COMPLEXinit(cosf(theta), sinf(theta)), x = t;
        printf("%2d %6.3f %6.3f ", i, Re(t), Im(t));
        for (int j = 0; j < N - 1; j++) x = COMPLEXmult(t, x);
        printf("  ^%d = %6.3f %6.3f\n", N, Re(x), Im(x));
    }
    printf("first-class version, root 1 ^ %d:", N);
    CPX t = CPXinit(cosf((float)(2 * PI / N)), sinf((float)(2 * PI / N))), x = CPXinit(1, 0);
    for (int j = 0; j < N; j++) { CPX y = CPXmult(t, x); CPXfree(x); x = y; }  /* no leaks */
    printf(" %6.3f %6.3f\n", CPXre(x), CPXim(x));
    CPXfree(t); CPXfree(x);
    return 0;
}
