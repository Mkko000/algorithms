/* ============================================================
 *  ex3_57_strings.c  —  Exercises 3.56, 3.57, 3.60, 3.62, 3.69, 3.70
 *  Chapter 3: Elementary Data Structures — solved exercises
 *
 *  3.56 Print a table of each character in a string and its frequency.
 *  3.57 Check whether a string is a palindrome, ignoring blanks
 *       ("if i had a hifi" must succeed).
 *  3.60 Replace every run of more than one blank by exactly one blank.
 *  3.62 Find the longest run of blanks, examining as few characters as
 *       possible (faster when the run is longer).
 *  3.69/3.70 Adjacency matrix and adjacency lists built by Programs
 *       3.18/3.19 for the pairs 0-2 1-4 2-5 3-6 0-4 6-0 1-3.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Sections 3.6-3.7, Programs 3.18-3.19
 *  Build:   gcc -std=c99 -Wall -O2 ex3_57_strings.c -o ex3_57
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int palindrome(const char *s)
{
    int i = 0, j = (int)strlen(s) - 1;
    for (;;) {
        while (i < j && s[i] == ' ') i++;
        while (i < j && s[j] == ' ') j--;
        if (i >= j) return 1;
        if (s[i++] != s[j--]) return 0;
    }
}

static void squeeze(char *s)               /* in place, one pass */
{
    char *w = s;
    for (char *r = s; *r; r++)
        if (!(*r == ' ' && w > s && w[-1] == ' ')) *w++ = *r;
    *w = 0;
}

/* 3.62: if s[i+best] is not a blank, no run longer than best can start
   in i..i+best, so jump past it.  Otherwise measure the run around it. */
static int longest_blanks(const char *s, int n, long *looked)
{
    int best = 0, i = 0; *looked = 0;
    while (i + best < n) {
        int k = i + best;
        (*looked)++;
        if (s[k] != ' ') { i = k + 1; continue; }
        int l = k, r = k + 1;
        while (l > i && ((*looked)++, s[l - 1] == ' ')) l--;
        while (r < n && ((*looked)++, s[r] == ' ')) r++;
        if (r - l > best) best = r - l;
        i = r + 1;
    }
    return best;
}

int main(void)
{
    const char *t = "abracadabra";
    int freq[256] = { 0 };
    for (const char *p = t; *p; p++) freq[(unsigned char)*p]++;
    printf("3.56 \"%s\":", t);
    for (int c = 0; c < 256; c++) if (freq[c]) printf("  %c %d", c, freq[c]);
    printf("\n");

    const char *tests[] = { "if i had a hifi", "a man a plan", "never odd or even", "abc" };
    for (int k = 0; k < 4; k++) printf("3.57 \"%s\" -> %s\n", tests[k], palindrome(tests[k]) ? "palindrome" : "no");

    char sq[] = "too    many     blanks  here";
    squeeze(sq); printf("3.60 \"%s\"\n", sq);

    int n = 1000000;
    for (int run = 10; run <= 10000; run *= 10) {
        char *s = malloc(n + 1);
        for (int i = 0; i < n; i++) s[i] = (i % 7 == 3) ? ' ' : 'x';   /* single blanks */
        for (int i = 0; i < run; i++) s[100 + i] = ' ';
        s[n] = 0;
        long looked; int b = longest_blanks(s, n, &looked);
        printf("3.62 N=%d, longest run %5d: examined %7ld chars (%.1f%% of N)\n", n, b, looked, 100.0 * looked / n);
        free(s);
    }

    int pr[][2] = { {0,2},{1,4},{2,5},{3,6},{0,4},{6,0},{1,3} };
    int adj[7][7] = { { 0 } };
    for (int i = 0; i < 7; i++) adj[i][i] = 1;                  /* Program 3.18 sets the diagonal */
    for (int k = 0; k < 7; k++) adj[pr[k][0]][pr[k][1]] = adj[pr[k][1]][pr[k][0]] = 1;
    printf("3.69 adjacency matrix:\n");
    for (int i = 0; i < 7; i++) { printf("     "); for (int j = 0; j < 7; j++) printf("%d ", adj[i][j]); printf("\n"); }
    /* Program 3.19: insert each edge at the FRONT of both lists */
    int lst[7][7], len[7] = { 0 };
    for (int k = 0; k < 7; k++) {
        int a = pr[k][0], b = pr[k][1];
        memmove(&lst[a][1], &lst[a][0], len[a] * sizeof(int)); lst[a][0] = b; len[a]++;
        memmove(&lst[b][1], &lst[b][0], len[b] * sizeof(int)); lst[b][0] = a; len[b]++;
    }
    printf("3.70 adjacency lists:\n");
    for (int i = 0; i < 7; i++) { printf("     %d:", i); for (int j = 0; j < len[i]; j++) printf(" %d", lst[i][j]); printf("\n"); }
    return 0;
}
