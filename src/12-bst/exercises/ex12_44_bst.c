/* ============================================================
 *  ex12_44_bst.c  —  Exercises 12.44-12.47, 12.53-12.55, 12.57
 *  Chapter 12: Symbol Tables and BSTs — binary search trees
 *
 *  12.44 The BST from inserting E A S Y Q U T I O N.
 *  12.45 The BST from inserting E A S Y Q U E S T I O N.
 *  12.46 Comparisons to build it (a search per key, insert on a miss).
 *  12.47 A S E R H I N G C gives the top tree of Figure 12.6; give ten other
 *        orderings that give the same tree (and count them all).
 *  12.53/12.54 Height and average search-hit cost of a given BST.
 *  12.55 An insertion order of E A S Y Q U E S T I O N whose BST is
 *        equivalent to binary search.
 *  12.57 All structurally different BSTs from N random insertions (N <= 5),
 *        with the probability of each shape.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 12.6-12.8, Exercises 12.44-12.57
 *  Build:   gcc -std=c99 -Wall -O2 ex12_44_bst.c -o ex12_44
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node *link;
struct node { int key; link l, r; };
static long C;
static link NEW(int k) { link x = malloc(sizeof *x); x->key = k; x->l = x->r = NULL; return x; }
static link insert(link h, int k)                      /* Program 12.7: equal keys go right */
{
    if (!h) return NEW(k);
    C++;
    if (k < h->key) h->l = insert(h->l, k); else h->r = insert(h->r, k);
    return h;
}
static link search(link h, int k) { while (h) { C++; if (k == h->key) return h; h = k < h->key ? h->l : h->r; } return NULL; }
static void show(link h) { if (!h) { printf("-"); return; } printf("%c", h->key); if (h->l || h->r) { printf("("); show(h->l); printf(","); show(h->r); printf(")"); } }
static int height(link h) { if (!h) return 0; int a = height(h->l), b = height(h->r); return 1 + (a > b ? a : b); }   /* 12.53: nodes on the longest path */
static long ipl(link h, int d) { return h ? d + ipl(h->l, d + 1) + ipl(h->r, d + 1) : 0; }
static int size(link h) { return h ? 1 + size(h->l) + size(h->r) : 0; }
static void freet(link h) { if (h) { freet(h->l); freet(h->r); free(h); } }
static int same(link a, link b) { if (!a || !b) return a == b; return a->key == b->key && same(a->l, b->l) && same(a->r, b->r); }
static void shape(link h, char *o) { if (!h) { strcat(o, "."); return; } strcat(o, "("); shape(h->l, o); shape(h->r, o); strcat(o, ")"); }
static long ways(link h)                               /* orderings giving this tree: n! / prod(subtree sizes) */
{
    if (!h) return 1;
    int a = size(h->l), b = size(h->r);
    long c = 1; for (int i = 1; i <= a; i++) c = c * (a + b - a + i) / i;    /* C(a+b, a) */
    return c * ways(h->l) * ways(h->r);
}

/* 12.55: insert the middle (as Program 12.6 computes it) first, then the halves */
static void bs_order(const char *k, int l, int r, char *out) { if (l > r) return; int m = (l + r) / 2; strncat(out, k + m, 1); bs_order(k, l, m - 1, out); bs_order(k, m + 1, r, out); }

int main(void)
{
    link t = NULL; const char *s1 = "EASYQUTION";
    for (int i = 0; s1[i]; i++) t = insert(t, s1[i]);
    printf("12.44 BST of EASYQUTION: "); show(t); printf("   height %d\n", height(t)); freet(t);

    t = NULL; C = 0; const char *s = "EASYQUESTION";
    for (int i = 0; s[i]; i++) t = insert(t, s[i]);
    long cins = C;
    printf("12.45 BST of EASYQUESTION: "); show(t); printf("\n");
    printf("      height %d, internal path length %ld, average hit cost %.2f (12.53/12.54)\n", height(t), ipl(t, 1), (double)ipl(t, 1) / size(t));
    freet(t);
    /* 12.46: search first, insert on a miss (Program 12.2's driver) */
    t = NULL; C = 0;
    for (int i = 0; s[i]; i++) { if (!search(t, s[i])) t = insert(t, s[i]); }
    printf("12.46 search+insert-on-miss: %ld compares (plain insertion of all 12: %ld)\n", C, cins);
    printf("      the second E and second S are hits, so they are not inserted again: ");
    show(t); printf("\n"); freet(t);

    link ref = NULL; const char *o = "ASERHINGC";
    for (int i = 0; o[i]; i++) ref = insert(ref, o[i]);
    printf("12.47 tree of ASERHINGC: "); show(ref); printf("\n      orderings that give it: %ld of 9! = 362880; ten of them:\n      ", ways(ref));
    {
        unsigned long long rs = 5; int found = 0; char seen[20][10]; int ns = 0;
        while (found < 10) {
            char p[10]; strcpy(p, o);
            for (int i = 8; i > 0; i--) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; int j = (int)(rs % (unsigned long long)(i + 1)); char c = p[i]; p[i] = p[j]; p[j] = c; }
            link x = NULL; for (int i = 0; i < 9; i++) x = insert(x, p[i]);
            int dup = !strcmp(p, o); for (int i = 0; i < ns; i++) if (!strcmp(seen[i], p)) dup = 1;
            if (same(x, ref) && !dup) { strcpy(seen[ns++], p); printf("%s ", p); found++; if (found == 5) printf("\n      "); }
            freet(x);
        }
        printf("\n");
    }
    freet(ref);

    char keys[] = "AEINOQSTUY", ord[16] = "";            /* the 10 distinct keys of EASYQUESTION */
    bs_order(keys, 0, 9, ord);
    t = NULL; for (int i = 0; ord[i]; i++) t = insert(t, ord[i]);
    printf("12.55 insert in the order %s -> ", ord); show(t); printf("\n");
    {   /* check: the probe sequence of the tree equals binary search's probes for every key */
        int ok = 1;
        for (int k = 0; k < 10; k++) {
            char p1[16] = "", p2[16] = ""; int l = 0, r = 9, n1 = 0, n2 = 0;
            while (l <= r) { int m = (l + r) / 2; p1[n1++] = keys[m]; if (keys[k] == keys[m]) break; if (keys[k] < keys[m]) r = m - 1; else l = m + 1; }
            for (link h = t; h; h = keys[k] < h->key ? h->l : h->r) { p2[n2++] = (char)h->key; if (keys[k] == h->key) break; }
            if (strcmp(p1, p2)) ok = 0;
        }
        printf("      same probe sequence as binary search for all 10 keys: %s\n", ok ? "yes" : "NO");
    }
    freet(t);

    printf("12.57 BST shapes from random insertion orders:\n");
    for (int n = 2; n <= 5; n++) {
        char shp[64][64]; long cnt[64] = { 0 }; int ns = 0, p[5]; long tot = 0;
        for (int i = 0; i < n; i++) p[i] = i;
        for (;;) {
            link x = NULL; for (int i = 0; i < n; i++) x = insert(x, p[i]);
            char b[64] = ""; shape(x, b); freet(x);
            int f = -1; for (int i = 0; i < ns; i++) if (!strcmp(shp[i], b)) f = i;
            if (f < 0) { strcpy(shp[ns], b); f = ns++; }
            cnt[f]++; tot++;
            int i = n - 2; while (i >= 0 && p[i] > p[i + 1]) i--;
            if (i < 0) break;
            int j = n - 1; while (p[j] < p[i]) j--;
            int tt = p[i]; p[i] = p[j]; p[j] = tt;
            for (int a = i + 1, b2 = n - 1; a < b2; a++, b2--) { tt = p[a]; p[a] = p[b2]; p[b2] = tt; }
        }
        printf("     N=%d: %d shapes (Catalan number), %ld orderings", n, ns, tot);
        if (n == 3) { printf(":"); for (int i = 0; i < ns; i++) printf("  %s %ld/6", shp[i], cnt[i]); }
        if (n >= 4) { long mx = 0, mn = 1 << 30; for (int i = 0; i < ns; i++) { if (cnt[i] > mx) mx = cnt[i]; if (cnt[i] < mn) mn = cnt[i]; } printf("; most likely shape %ld/%ld, least likely %ld/%ld", mx, tot, mn, tot); }
        printf("\n");
    }
    return 0;
}
