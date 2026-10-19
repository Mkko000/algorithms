/* ============================================================
 *  ex11_01_networks.c  —  Exercises 11.1, 11.5-11.10, 11.16, 11.17, 11.22, 11.23
 *  Chapter 11: Special-Purpose Sorting Methods — Batcher and networks
 *
 *  11.1  Shuffle and unshuffle E A S Y Q U E S T I O N.
 *  11.5  How does the 3-element straight-line program sort each of the six
 *        permutations of 1 2 3?
 *  11.6  A straight-line program that sorts four elements.
 *  11.7  0-1 principle: a network that sorts every 0-1 input sorts everything
 *        (checked here exhaustively for small networks).
 *  11.8-11.10 Merge A E Q S U Y | E I N O S T (and A E S Y | E I N O Q S T U,
 *        and 1 0 0 1 1 1 0 0 | 0 0 0 1 0 1 0 0) with Program 11.2.
 *  11.16 Sorting networks for 4, 5 and 6 elements with few comparators.
 *  11.17 Number of parallel steps of a straight-line program (labeling).
 *  11.22/11.23 Comparators in Batcher's odd-even sorting networks.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 11.1-11.3, Exercises 11.1-11.23
 *  Build:   gcc -std=c99 -Wall -O2 ex11_01_networks.c -o ex11_01
 * ============================================================ */
#include <stdio.h>
#include <string.h>

typedef char Item;
static Item aux[64];
#define compexch(A, B) if ((B) < (A)) { Item t = A; A = B; B = t; }

static void shuffle(Item a[], int l, int r)          /* Program 11.1 */
{
    int i, j, m = (l + r) / 2;
    for (i = l, j = 0; i <= r; i += 2, j++) { aux[i] = a[l + j]; aux[i + 1] = a[m + 1 + j]; }
    for (i = l; i <= r; i++) a[i] = aux[i];
}
static void unshuffle(Item a[], int l, int r)
{
    int i, j, m = (l + r) / 2;
    for (i = l, j = 0; i <= r; i += 2, j++) { aux[l + j] = a[i]; aux[m + 1 + j] = a[i + 1]; }
    for (i = l; i <= r; i++) a[i] = aux[i];
}
static int top; static Item *base; static int baseN;
static void pr(const char *lab) { printf("     %-18s", lab); for (int i = 0; i < baseN; i++) printf("%c", base[i]); printf("\n"); }
static void mergeTD(Item a[], int l, int r)            /* Program 11.2 */
{
    int i, m = (l + r) / 2, me = top; top = 0;
    if (r == l + 1) compexch(a[l], a[r]);
    if (r < l + 2) return;
    unshuffle(a, l, r); if (me) pr("unshuffle");
    mergeTD(a, l, m); mergeTD(a, m + 1, r); if (me) pr("merge halves");
    shuffle(a, l, r); if (me) pr("shuffle");
    for (i = l + 1; i < r; i += 2) compexch(a[i], a[i + 1]);
    if (me) pr("compare-exchange");
}
static void merge_show(const char *s)
{
    static Item a[64]; baseN = (int)strlen(s); memcpy(a, s, baseN); base = a;
    pr("input"); top = 1; mergeTD(a, 0, baseN - 1);
}
/* Program 11.2 needs two halves of the same power-of-2 size: pad each sorted
   half with sentinels '~' (larger than every key), merge, drop the sentinels */
static void merge_padded(const char *L, const char *R)
{
    int nl = (int)strlen(L), nr = (int)strlen(R), h = 1;
    while (h < nl || h < nr) h += h;
    static Item a[64]; memset(a, '~', 2 * h);
    memcpy(a, L, nl); memcpy(a + h, R, nr);
    base = a; baseN = 2 * h; pr("padded input"); top = 1; mergeTD(a, 0, 2 * h - 1);
    printf("     %-18s%.*s\n", "drop sentinels", nl + nr, a);
}

/* networks as comparator lists */
typedef struct { int i, j; } CE;
static int sorts01(const CE *c, int m, int n)          /* 0-1 principle check */
{
    for (int x = 0; x < (1 << n); x++) {
        int v[32]; for (int k = 0; k < n; k++) v[k] = (x >> k) & 1;
        for (int k = 0; k < m; k++) if (v[c[k].j] < v[c[k].i]) { int t = v[c[k].i]; v[c[k].i] = v[c[k].j]; v[c[k].j] = t; }
        for (int k = 1; k < n; k++) if (v[k - 1] > v[k]) return 0;
    }
    return 1;
}
static int sorts_perms(const CE *c, int m, int n)      /* direct check on all n! permutations */
{
    int p[8]; for (int k = 0; k < n; k++) p[k] = k;
    for (;;) {
        int v[8]; memcpy(v, p, sizeof v);
        for (int k = 0; k < m; k++) if (v[c[k].j] < v[c[k].i]) { int t = v[c[k].i]; v[c[k].i] = v[c[k].j]; v[c[k].j] = t; }
        for (int k = 1; k < n; k++) if (v[k - 1] > v[k]) return 0;
        int i = n - 2; while (i >= 0 && p[i] > p[i + 1]) i--;
        if (i < 0) return 1;
        int j = n - 1; while (p[j] < p[i]) j--;
        int t = p[i]; p[i] = p[j]; p[j] = t;
        for (int x = i + 1, y = n - 1; x < y; x++, y--) { t = p[x]; p[x] = p[y]; p[y] = t; }
    }
}
static int stages(const CE *c, int m, int n)           /* 11.17 labeling */
{
    int lab[64] = { 0 }, mx = 0;
    for (int k = 0; k < m; k++) {
        int s = (lab[c[k].i] > lab[c[k].j] ? lab[c[k].i] : lab[c[k].j]) + 1;
        lab[c[k].i] = lab[c[k].j] = s; if (s > mx) mx = s;
    }
    (void)n; return mx;
}
/* Batcher's odd-even merge sort network for n = 2^k (bottom-up, Program 11.3) */
static int batcher(CE *c, int N)
{
    int m = 0;
    for (int p = 1; p < N; p += p)
        for (int k = p; k > 0; k /= 2)
            for (int j = k % p; j + k < N; j += (k + k))
                for (int i = 0; i < k; i++)
                    if (j + i + k < N && (j + i) / (p + p) == (j + i + k) / (p + p)) c[m++] = (CE){ j + i, j + i + k };
    return m;
}

int main(void)
{
    Item a[16]; strcpy(a, "EASYQUESTION");
    shuffle(a, 0, 11);   printf("11.1 shuffle   EASYQUESTION -> %.12s\n", a);
    strcpy(a, "EASYQUESTION");
    unshuffle(a, 0, 11); printf("     unshuffle EASYQUESTION -> %.12s\n", a);
    shuffle(a, 0, 11);   printf("     shuffle(unshuffle(x))  -> %.12s\n", a);

    printf("11.5 compexch(0,1) compexch(1,2) compexch(0,1) on the six permutations:\n");
    int P[6][3] = { { 1, 2, 3 }, { 1, 3, 2 }, { 2, 1, 3 }, { 2, 3, 1 }, { 3, 1, 2 }, { 3, 2, 1 } };
    for (int t = 0; t < 6; t++) {
        Item v[3] = { (Item)('0' + P[t][0]), (Item)('0' + P[t][1]), (Item)('0' + P[t][2]) };
        printf("     %c%c%c", v[0], v[1], v[2]);
        compexch(v[0], v[1]); printf(" -> %c%c%c", v[0], v[1], v[2]);
        compexch(v[1], v[2]); printf(" -> %c%c%c", v[0], v[1], v[2]);
        compexch(v[0], v[1]); printf(" -> %c%c%c\n", v[0], v[1], v[2]);
    }

    CE n4[] = { { 0, 1 }, { 2, 3 }, { 0, 2 }, { 1, 3 }, { 1, 2 } };
    CE n5[] = { { 0, 1 }, { 3, 4 }, { 2, 4 }, { 2, 3 }, { 0, 3 }, { 0, 2 }, { 1, 4 }, { 1, 3 }, { 1, 2 } };
    CE n6[] = { { 1, 2 }, { 4, 5 }, { 0, 2 }, { 3, 5 }, { 0, 1 }, { 3, 4 }, { 2, 5 }, { 0, 3 }, { 1, 4 }, { 2, 4 }, { 1, 3 }, { 2, 3 } };
    printf("11.6/11.16 networks (0-1 check / all-permutation check / parallel steps):\n");
    printf("     N=4: %2d comparators  %s / %s / %d steps\n", 5, sorts01(n4, 5, 4) ? "sorts" : "FAILS", sorts_perms(n4, 5, 4) ? "sorts" : "FAILS", stages(n4, 5, 4));
    printf("     N=5: %2d comparators  %s / %s / %d steps\n", 9, sorts01(n5, 9, 5) ? "sorts" : "FAILS", sorts_perms(n5, 9, 5) ? "sorts" : "FAILS", stages(n5, 9, 5));
    printf("     N=6: %2d comparators  %s / %s / %d steps\n", 12, sorts01(n6, 12, 6) ? "sorts" : "FAILS", sorts_perms(n6, 12, 6) ? "sorts" : "FAILS", stages(n6, 12, 6));
    {   /* can 4 comparators sort 4 elements?  try all 6^4 sequences */
        CE pairs[6] = { { 0, 1 }, { 0, 2 }, { 0, 3 }, { 1, 2 }, { 1, 3 }, { 2, 3 } }; int any = 0;
        for (int x = 0; x < 1296; x++) { CE c[4]; int y = x; for (int k = 0; k < 4; k++) { c[k] = pairs[y % 6]; y /= 6; } if (sorts01(c, 4, 4)) any = 1; }
        printf("     any 4-comparator network for N=4? %s (4! = 24 > 2^4 outcomes)\n", any ? "yes" : "no");
    }
    {   /* 11.7: a network that sorts all 0-1 inputs but we perturb it: dropping any one comparator */
        int agree = 1;
        for (int drop = 0; drop < 12; drop++) {
            CE c[12]; int m = 0; for (int k = 0; k < 12; k++) if (k != drop) c[m++] = n6[k];
            if (sorts01(c, m, 6) != sorts_perms(c, m, 6)) agree = 0;
        }
        printf("11.7 for the 12 networks 'N=6 minus one comparator', the 0-1 test and the 720-permutation test agree: %s\n", agree ? "yes" : "NO");
    }

    printf("     check on the 16-key example of Figure 11.2:\n");     merge_show("AEGGIMNRABEELMPX");
    printf("11.8 Batcher merge (Program 11.2) of AEQSUY | EINOST, as is (N = 12):\n");  merge_show("AEQSUYEINOST");
    printf("     padded to 8 + 8:\n");                                merge_padded("AEQSUY", "EINOST");
    printf("11.9 AESY | EINOQSTU (unequal halves), padded to 8 + 8:\n"); merge_padded("AESY", "EINOQSTU");
    printf("11.10 1001110000010100 (halves NOT sorted):\n");           merge_show("1001110000010100");

    printf("11.22 comparators in Batcher's network on the first N lines of the next power of 2 (0-1 checked for N <= 20):\n    ");
    static CE c[4096];
    for (int N = 2; N <= 32; N++) {
        int P2 = 1; while (P2 < N) P2 += P2;
        int m = batcher(c, P2), k = 0; CE d[4096];
        for (int t = 0; t < m; t++) if (c[t].j < N) d[k++] = c[t];
        if (N <= 20 && !sorts01(d, k, N)) printf("[N=%d FAILS]", N);     /* 0-1 check up to 2^20 inputs */
        printf(" %d:%d", N, k);
        if (N % 8 == 1) printf("\n    ");
    }
    printf("\n11.23 N = 2^n: sorting network comparators vs (n^2 - n + 4) 2^(n-2) - 1, and the last merge alone vs (n-1) 2^(n-1) + 1:\n");
    for (int n = 1; n <= 6; n++) {
        int N = 1 << n, half = n > 1 ? batcher(c, N / 2) : 0, m = batcher(c, N);
        long f = (long)(n * n - n + 4) * (1L << n) / 4 - 1;
        printf("     n=%d N=%2d: %3d (formula %3ld)   merge %3d (formula %3ld)   parallel steps %d\n", n, N, m, f, m - 2 * half, (long)(n - 1) * (1L << (n > 0 ? n - 1 : 0)) + 1, stages(c, m, N));
    }
    return 0;
}
