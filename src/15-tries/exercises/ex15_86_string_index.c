/* ============================================================
 *  ex15_86_string_index.c  —  Exercises 15.83, 15.86, 15.87, 15.89
 *  Chapter 15: Radix Search — text-string indexes (Section 15.5)
 *
 *  15.83 Explain why the idea of improving binary search using the same
 *        basic principle on which TSTs are based (comparing characters
 *        rather than strings) is not effective.
 *  15.86 Write an efficient program to determine the longest repeated
 *        sequence in a huge text string.
 *  15.87 Write an efficient program to determine the 10-character sequence
 *        that occurs most frequently in a huge text string.
 *  15.89 Describe a text string of N characters for which a TST-based
 *        string index will perform particularly badly. Estimate the cost
 *        of building an index for the same string with a BST.
 *
 *  بالعربي: (15.86) أطول جزء متكرر في نص ضخم. (15.87) أكتر 10 حروف ورا بعض
 *  بتتكرر. (15.83) ليه فكرة الـ TST (قارن حرف حرف) مش بتفيد الـ binary
 *  search؟ (15.89) نص يخلّي الـ TST string index يتعب.
 *
 *  THINKING STEPS
 *   1. A string index = all suffixes of the text, in sorted order (that is
 *      what a trie / TST / patricia of the suffixes gives by an inorder
 *      walk). Here we build it the cheapest way: an array of N suffix
 *      positions sorted with a character-comparing compare (a "suffix
 *      array"). Same information as the TST index, much less memory.
 *   2. 15.86: two suffixes that share a long prefix are NEIGHBOURS in
 *      sorted order. So the longest repeat = max over i of
 *      lcp(suffix[i], suffix[i+1]). One pass after the sort.
 *   3. 15.87: all occurrences of a 10-char sequence are a contiguous run
 *      of suffixes that agree on the first 10 characters. Count runs.
 *   4. 15.83: binary search compares the search key against a middle key.
 *      Comparing "character by character" does not shrink the interval
 *      any faster: after a mismatch on char d you still discard only half
 *      of the keys. TSTs win because they REUSE the d matched characters
 *      (they never compare them again); binary search can only reuse them
 *      if it tracks lcp with both interval ends (that is the suffix-array
 *      "lcp trick"), which is extra bookkeeping, not the TST idea itself.
 *      We measure the character comparisons of plain binary search here.
 *   5. 15.89: text "aaaa...a": every suffix is a prefix of the longer ones.
 *      The TST becomes one spine of height N and building it walks
 *      N(N+1)/2 characters. A BST is worse: suffixes arrive longest first,
 *      each new one is smaller than all, so the BST is a left list
 *      (N^2/2 string compares) and each compare reads the whole shorter
 *      suffix: about N^3/6 character compares. We count both.
 *
 *  Run: ./ex15_86_string_index [textfile]   (default: built-in texts)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Section 15.5, Exercises 15.83-15.89
 *  Build:   gcc -std=c99 -Wall -O2 ex15_86_string_index.c -o ex15_86
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *T; static int N;

static int cmpSuf(const void *a, const void *b)
{
    const char *x = T + *(const int *)a, *y = T + *(const int *)b;
    while (*x && *x == *y) { x++; y++; }
    return (unsigned char)*x - (unsigned char)*y;
}
static int lcp(int i, int j) { int k = 0; while (T[i + k] && T[i + k] == T[j + k]) k++; return k; }

static int *suffixArray(const char *text, int n)
{
    T = text; N = n;
    int *sa = malloc(n * sizeof *sa);
    for (int i = 0; i < n; i++) sa[i] = i;
    qsort(sa, n, sizeof *sa, cmpSuf);
    return sa;
}
static void longestRepeat(int *sa, int n)          /* 15.86 */
{
    int best = 0, at = 0;
    for (int i = 0; i + 1 < n; i++) { int c = lcp(sa[i], sa[i + 1]); if (c > best) { best = c; at = i; } }
    printf("     15.86 longest repeated sequence: %d chars, at positions %d and %d:\n       \"", best, sa[at], sa[at + 1]);
    for (int k = 0; k < best && k < 70; k++) { char c = T[sa[at] + k]; putchar(c == '\n' || c == '\r' || c == '\t' ? ' ' : c); }
    printf("%s\"\n", best > 70 ? "..." : "");
}
static void mostFrequent(int *sa, int n, int L)    /* 15.87 */
{
    int best = 0, at = -1;
    for (int i = 0; i < n; ) {
        int j = i + 1;
        int len = 0; while (len < L && T[sa[i] + len]) len++; if (len < L) { i++; continue; }
        while (j < n && lcp(sa[i], sa[j]) >= L) j++;
        if (j - i > best) { best = j - i; at = sa[i]; }
        i = j;
    }
    printf("     15.87 most frequent %d-character sequence: \"", L);
    for (int k = 0; k < L; k++) { char c = T[at + k]; putchar(c == '\n' || c == '\r' || c == '\t' ? ' ' : c); }
    printf("\" occurs %d times\n", best);
}

/* 15.83: binary search over the sorted suffixes, counting character compares */
static int bsearchCount(int *sa, int n, const char *key, long *cc)
{
    int lo = 0, hi = n - 1, kl = (int)strlen(key);
    while (lo <= hi) {
        int m = (lo + hi) / 2; const char *x = T + sa[m]; int d = 0;
        while (d < kl && x[d] == key[d]) { d++; (*cc)++; }
        (*cc)++;
        if (d == kl) return m;
        if ((unsigned char)key[d] < (unsigned char)x[d]) hi = m - 1; else lo = m + 1;
    }
    return -1;
}

/* 15.89: TST over suffixes, counting character steps while inserting */
typedef struct tn *tl;
struct tn { char c; tl l, m, r; };
static long steps;
static tl tIns(tl h, const char *s)
{
    tl *p = &h;
    for (;;) {
        steps++;
        if (!*p) { *p = calloc(1, sizeof **p); (*p)->c = *s; }
        tl x = *p;
        if (*s < x->c) p = &x->l;
        else if (*s > x->c) p = &x->r;
        else { if (!*s) return h; s++; p = &x->m; }
    }
}
static void tFree(tl h) { while (h) { tFree(h->l); tFree(h->r); tl m = h->m; free(h); h = m; } }

static void analyse(const char *name, const char *text, int n)
{
    printf("%s (N = %d characters)\n", name, n);
    int *sa = suffixArray(text, n);
    longestRepeat(sa, n);
    mostFrequent(sa, n, 10);
    free(sa);
}

static unsigned long long rs = 1586;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 16); }

int main(int argc, char **argv)
{
    /* 1) the classic sentence */
    const char *s = "now is the time for all good people to come the aid of their party";
    analyse("text 1: the sentence", s, (int)strlen(s));

    /* 2) a huge random text over a 4-letter alphabet with a planted repeat */
    int n = 1000000; char *big = malloc(n + 1);
    for (int i = 0; i < n; i++) big[i] = "acgt"[rnd() % 4];
    const char *plant = "gattacagattacacatgattaca";
    memcpy(big + 123456, plant, strlen(plant)); memcpy(big + 876543, plant, strlen(plant));
    big[n] = 0;
    analyse("text 2: random a/c/g/t with the same 24 letters planted at 123456 and 876543", big, n);
    printf("     (a random 4-letter text of 10^6 chars repeats about 2 log_4 N = 20 chars by chance)\n");

    /* 15.83 */
    { int *sa = suffixArray(big, n); long cc = 0; int found = 0;
      for (int q = 0; q < 1000; q++) { int p = rnd() % (n - 20); char key[21]; memcpy(key, big + p, 20); key[20] = 0; found += bsearchCount(sa, n, key, &cc) >= 0; }
      printf("15.83 binary search for 1000 substrings of length 20 in the sorted suffixes: %d found,\n"
             "      %.1f character compares per search for lg N = %.1f probes. Each probe re-reads the\n"
             "      characters that already matched; per probe we still only halve the interval.\n", found, cc / 1000.0, 19.93);
      free(sa); }

    /* 15.89 */
    printf("15.89 cost of a TST string index that inserts EVERY full suffix:\n");
    printf("      every character of every suffix is walked once, so the cost is always N(N+1)/2,\n"
           "      for ANY text. The all-'a' string is worse still for SEARCH: the TST is a single\n"
           "      spine of height N, and every search of a suffix of length k costs k character steps.\n");
    for (int m = 1000; m <= 8000; m *= 2) {
        char *a = malloc(m + 1); memset(a, 'a', m); a[m] = 0;
        char *r = malloc(m + 1); for (int i = 0; i < m; i++) r[i] = "acgt"[rnd() % 4]; r[m] = 0;
        tl h = NULL; steps = 0; for (int i = 0; i < m; i++) h = tIns(h, a + i);
        long sa_ = steps; int ha = 0; { tl x = h; while (x) { ha++; x = x->m; } } tFree(h);
        h = NULL; steps = 0; for (int i = 0; i < m; i++) h = tIns(h, r + i); long sr = steps; tFree(h);
        /* BST of the suffixes of "aaa...a": each new (shorter) suffix loses to every longer one already in the tree */
        long bst_chars = 0;
        for (int i = 1; i < m; i++) for (int j = 0; j < i; j++) bst_chars += (long)(m - i) + 1;
        printf("      N=%5d  all-'a' TST steps %9ld (N(N+1)/2 = %ld, height %d)\n"
               "               random  TST steps %9ld (same order: every suffix is fully stored)\n"
               "               all-'a' BST insert character compares ~ %ld (each new short suffix loses to every longer one)\n",
               m, sa_, (long)m * (m + 1) / 2, ha, sr, bst_chars);
        free(a); free(r);
    }
    printf("      fix used by the book (Figure 15.20): cut each suffix as soon as it becomes unique\n"
           "      among the set. For a random text that is about 2 log_sigma N characters per suffix\n"
           "      (the longest-repeat length above), so building is near-linear; only pathological\n"
           "      texts like all-'a' still force Theta(N^2).\n");

    /* optional: any file you give */
    if (argc > 1) {
        FILE *f = fopen(argv[1], "rb");
        if (f) {
            fseek(f, 0, SEEK_END); long L = ftell(f); fseek(f, 0, SEEK_SET);
            char *t = malloc(L + 1); size_t got = fread(t, 1, L, f); t[got] = 0; fclose(f);
            for (size_t i = 0; i < got; i++) if (!t[i]) t[i] = ' ';
            analyse(argv[1], t, (int)got); free(t);
        } else printf("cannot open %s\n", argv[1]);
    }
    free(big);
    return 0;
}
