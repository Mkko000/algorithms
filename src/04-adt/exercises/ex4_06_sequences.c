/* ============================================================
 *  ex4_06_sequences.c  —  Exercises 4.6-4.8, 4.31, 4.34-4.36
 *  Chapter 4: Abstract Data Types — solved exercises
 *
 *  4.6  Letter = push, * = pop in  E A S * Y * Q U E * * * S T * * * I O * N * * *
 *       Give the values returned by the pops.
 *  4.7  Insert asterisks in  E A S Y  so the pops return (i) E A S Y,
 *       (ii) Y S A E, (iii) A S Y E, (iv) A Y E S -- or prove impossible.
 *  4.8  Algorithm: can asterisks be added so sequence 1 produces sequence 2?
 *  4.31 Same as 4.6 with a FIFO queue (letter = put, * = get).
 *  4.34 Deque: UPPER = put at beginning, lower = put at end,
 *       + = get from beginning, * = get from end, in
 *       E A s + Y + Q U E * * + s t + * + I O * n + + *
 *  4.35 Insert + and * in  E a s Y  to get (i) E s a Y, (ii) Y a s E,
 *       (iii) a Y s E, (iv) a s Y E -- or prove impossible.
 *  4.36 Algorithm for the general deque question.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Sections 4.2 and 4.6, Exercises 4.6-4.8, 4.31-4.36
 *  Build:   gcc -std=c99 -Wall -O2 ex4_06_sequences.c -o ex4_06
 * ============================================================ */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ---- 4.6 stack and 4.31 queue on the same op string ---- */
static void run_stack(const char *ops)
{
    char s[64]; int n = 0;
    printf("4.6  stack pops: ");
    for (const char *p = ops; *p; p++)
        if (*p == '*') printf("%c ", s[--n]);
        else if (*p != ' ') s[n++] = *p;
    printf("\n");
}
static void run_queue(const char *ops)
{
    char q[64]; int head = 0, tail = 0;
    printf("4.31 queue gets: ");
    for (const char *p = ops; *p; p++)
        if (*p == '*') printf("%c ", q[head++]);
        else if (*p != ' ') q[tail++] = *p;
    printf("\n");
}

/* ---- 4.8: greedy.  Push input in order; pop whenever the top is the
   next wanted output.  If the input runs out and the top is not what we
   want, it is impossible.  Builds the op string when it succeeds.  ---- */
static int stack_ops(const char *in, const char *want, char *ops)
{
    char s[64]; int n = 0, k = 0, w = 0, len = (int)strlen(want);
    for (const char *p = in; ; ) {
        if (n && w < len && s[n - 1] == want[w]) { ops[k++] = '*'; n--; w++; continue; }
        if (!*p) break;
        ops[k++] = *p; s[n++] = *p++;
    }
    ops[k] = 0;
    return w == len && n == 0;
}

/* ---- 4.34 deque ---- */
static void run_deque(const char *ops)
{
    char d[128]; int lo = 64, hi = 64;          /* items in d[lo..hi-1] */
    printf("4.34 deque gets: ");
    for (const char *p = ops; *p; p++) {
        if (*p == ' ') continue;
        if (*p == '+') printf("%c ", d[lo++]);
        else if (*p == '*') printf("%c ", d[--hi]);
        else if (isupper((unsigned char)*p)) d[--lo] = *p;
        else d[hi++] = *p;
    }
    printf("\n");
}

/* ---- 4.36: depth-first search over "which get (if any) next".
   State = (position in input, deque contents, outputs produced).  The
   only choices are: put the next letter, get from the front, get from
   the back -- and a get is allowed only if it produces the next wanted
   letter, which prunes almost everything.                          ---- */
static const char *IN, *WANT;
static char OPS[128];
static int dfs(int i, int w, char *d, int lo, int hi, int k)
{
    int len = (int)strlen(WANT);
    if (w == len) { OPS[k] = 0; return IN[i] == 0 && lo == hi; }
    if (lo < hi && d[lo] == WANT[w]) {                 /* + */
        OPS[k] = '+'; if (dfs(i, w + 1, d, lo + 1, hi, k + 1)) return 1;
    }
    if (lo < hi && d[hi - 1] == WANT[w]) {             /* * */
        OPS[k] = '*'; if (dfs(i, w + 1, d, lo, hi - 1, k + 1)) return 1;
    }
    if (IN[i]) {                                       /* next put */
        char c = IN[i];
        OPS[k] = c;
        if (isupper((unsigned char)c)) { char save = d[lo - 1]; d[lo - 1] = c; int r = dfs(i + 1, w, d, lo - 1, hi, k + 1); d[lo - 1] = save; if (r) return 1; }
        else                           { char save = d[hi];     d[hi] = c;     int r = dfs(i + 1, w, d, lo, hi + 1, k + 1); d[hi] = save;     if (r) return 1; }
    }
    return 0;
}
static int deque_ops(const char *in, const char *want)
{
    char d[128]; IN = in; WANT = want;
    return dfs(0, 0, d, 64, 64, 0);
}

int main(void)
{
    const char *seq = "EAS*Y*QUE***ST***IO*N***";
    run_stack(seq);
    run_queue(seq);
    const char *t47[] = { "EASY", "YSAE", "ASYE", "AYES" };
    for (int i = 0; i < 4; i++) {
        char ops[64];
        int ok = stack_ops("EASY", t47[i], ops);
        printf("4.7  EASY -> %s : %s\n", t47[i], ok ? ops : "impossible");
    }
    run_deque("EAs+Y+QUE**+st+*+IO*n++*");
    const char *t35[] = { "EsaY", "YasE", "aYsE", "asYE" };
    for (int i = 0; i < 4; i++) {
        int ok = deque_ops("EasY", t35[i]);
        printf("4.35 EasY -> %s : %s\n", t35[i], ok ? OPS : "impossible");
    }
    return 0;
}
