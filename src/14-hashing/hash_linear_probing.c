/* ============================================================
 *  hash_linear_probing.c  —  Figures 14.7 and 14.9 of the book, live:
 *                           linear probing vs. double hashing
 *  Chapter 14: Hashing (Sections 14.3-14.4)
 *
 *  Inserts the keys A S E R C H I N G X M P L into a table of size
 *  M = 13 using the book's hash values (h(k) = 7k mod 13 where k is the
 *  letter's position in the alphabet reproduces them exactly) and, for
 *  double hashing, the book's second hash values. After every insert
 *  the table is printed with the probed positions, so the clusters of
 *  linear probing are visible. Then the average number of probes for a
 *  search hit is computed for both tables.
 *   - Program 14.4: linear probing (try i, i+1, i+2, ... mod M)
 *   - Program 14.6: double hashing (try i, i+k, i+2k, ... with k = h2)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 14.4, 14.6
 *  Build:   gcc -std=c99 -Wall -O2 hash_linear_probing.c -o hash_linear_probing
 * ============================================================ */
#include <stdio.h>
#include <string.h>

#define M 13
static const char *keys = "ASERCHINGXMPL";
static const int h2book[] = {1, 3, 1, 5, 5, 5, 3, 3, 2, 3, 5, 4, 2};   /* Figure 14.9 */

static int hash(char c) { return 7 * (c - 'A' + 1) % M; }

static void show(const char *t, const char *probed) {
    printf("   ");
    for (int i = 0; i < M; i++) printf(" %c%c", probed[i] ? '*' : ' ', t[i] ? t[i] : '.');
    printf("\n");
}

static double build(int dbl) {
    char t[M]; memset(t, 0, M);
    int probesHit[26] = {0};
    printf("%s (M = %d), * = probed:\n    ", dbl ? "double hashing" : "linear probing", M);
    for (int i = 0; i < M; i++) printf(" %2d", i);
    printf("\n");
    for (int n = 0; keys[n]; n++) {
        char c = keys[n], probed[M]; memset(probed, 0, M);
        int i = hash(c), k = dbl ? h2book[n] : 1, p = 1;
        while (t[i]) { probed[i] = 1; i = (i + k) % M; p++; }
        probed[i] = 1; t[i] = c; probesHit[c - 'A'] = p;
        printf("%c h=%2d%s", c, hash(c), dbl ? "" : "");
        if (dbl) printf(" h2=%d", k);
        printf(": slot %2d after %d probe%s\n", i, p, p > 1 ? "s" : "");
        show(t, probed);
    }
    double s = 0; for (int n = 0; keys[n]; n++) s += probesHit[keys[n] - 'A'];
    return s / strlen(keys);
}

int main(void) {
    double a = build(0);
    printf("\n");
    double b = build(1);
    printf("\naverage probes for a search hit: linear %.2f, double %.2f (load factor %d/%d = %.2f)\n",
           a, b, (int)strlen(keys), M, (double)strlen(keys) / M);
    return 0;
}
