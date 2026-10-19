/* ============================================================
 *  strings.c  —  Strings as arrays of chars: the library functions
 *                written by hand, plus sorting strings via pointers
 *  Chapter 3: Elementary Data Structures (Section 3.6)
 *
 *  - Table 3.2: strlen, strcpy, strcmp, strcat in array form and in
 *    pointer form. Writing them once makes you understand why
 *    "strlen in a loop condition" turns O(N) code into O(N^2).
 *  - Program 3.17: read words into ONE big buffer, keep an array of
 *    pointers to them, and sort the POINTERS with qsort (the strings
 *    themselves never move).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Section 3.6, Table 3.2, Programs 3.15, 3.17
 *  Build:   gcc -std=c99 -Wall -O2 strings.c -o strings
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* array-index versions */
int my_strlen(const char a[]) { int i; for (i = 0; a[i] != 0; i++) ; return i; }
void my_strcpy(char a[], const char b[]) { int i; for (i = 0; (a[i] = b[i]) != 0; i++) ; }
int my_strcmp(const char a[], const char b[]) {
    int i;
    for (i = 0; a[i] == b[i]; i++) if (a[i] == 0) return 0;
    return (unsigned char)a[i] - (unsigned char)b[i];
}
/* pointer versions (the terse C idiom) */
int p_strlen(const char *a) { const char *b = a; while (*b) b++; return (int)(b - a); }
void p_strcat(char *a, const char *b) { while (*a) a++; while ((*a++ = *b++)) ; }

/* the trap: strlen in the loop condition is evaluated every iteration */
long slow_count(const char *a, char c) {
    long n = 0;
    for (size_t i = 0; i < strlen(a); i++) if (a[i] == c) n++;   /* O(N^2) */
    return n;
}
long fast_count(const char *a, char c) {
    long n = 0;
    for (; *a; a++) if (*a == c) n++;                            /* O(N)   */
    return n;
}

/* Program 3.17: sort an array of strings by sorting pointers */
#define Nmax 1000
#define Mmax 10000
static char buf[Mmax]; static int M = 0;
static int compare(const void *i, const void *j) {
    return strcmp(*(char *const *)i, *(char *const *)j);
}

int main(void) {
    char s[64];
    my_strcpy(s, "algo"); p_strcat(s, "rithms");
    printf("strcpy+strcat -> \"%s\" (len %d / %d)\n", s, my_strlen(s), p_strlen(s));
    printf("strcmp(\"abc\",\"abd\") = %d\n", my_strcmp("abc", "abd"));

    const char *text = "now is the time for all good people to come to the aid of their party";
    char *a[Nmax]; int N = 0;
    char tmp[64]; const char *p = text; int k;
    while (sscanf(p, "%63s%n", tmp, &k) == 1 && N < Nmax) {
        a[N] = &buf[M]; my_strcpy(a[N], tmp); M += my_strlen(tmp) + 1; N++; p += k;
    }
    qsort(a, N, sizeof(char *), compare);
    printf("sorted words:"); for (int i = 0; i < N; i++) printf(" %s", a[i]); putchar('\n');

    static char big[20001];
    for (int i = 0; i < 20000; i++) big[i] = "ab"[i % 2];
    printf("count 'a' fast=%ld slow=%ld (same answer, slow is quadratic)\n",
           fast_count(big, 'a'), slow_count(big, 'a'));
    return 0;
}
