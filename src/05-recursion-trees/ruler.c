/* ============================================================
 *  ruler.c  —  Drawing a ruler: divide and conquer, recursive and not
 *  Chapter 5: Recursion and Trees (Section 5.2)
 *
 *  Program 5.8: rule(l, r, h) marks the middle with height h, then
 *  recursively rules both halves with height h-1 (inorder: left, mark,
 *  right). Changing the ORDER of the three steps (preorder/postorder)
 *  gives the same picture drawn in a different sequence.
 *  Program 5.9: the nonrecursive, bottom-up version: draw all marks of
 *  height 1, then all of height 2, ... — the "combine" view.
 *  Bonus: the mark height at position i is 1 + (number of trailing 0
 *  bits of i), the same pattern as Hanoi disk moves (Property 5.2).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 5.8, 5.9
 *  Build:   gcc -std=c99 -Wall -O2 ruler.c -o ruler
 * ============================================================ */
#include <stdio.h>
#include <string.h>

#define W 16
static int mark_at[W + 1];
static char order[256]; static int olen;

void mark(int x, int h) { mark_at[x] = h; olen += sprintf(order + olen, "%d ", x); }

void rule(int l, int r, int h) {                  /* Program 5.8 (inorder) */
    int m = (l + r) / 2;
    if (h > 0) { rule(l, m, h - 1); mark(m, h); rule(m, r, h - 1); }
}
void rule_pre(int l, int r, int h) {              /* preorder variant */
    int m = (l + r) / 2;
    if (h > 0) { mark(m, h); rule_pre(l, m, h - 1); rule_pre(m, r, h - 1); }
}
void rule_nr(int l, int r, int h) {               /* Program 5.9: bottom-up */
    for (int t = 1, j = 1; t <= h; j += j, t++)
        for (int i = 0; l + j + i <= r; i += j + j) mark(l + j + i, t);
}
void draw(const char *title) {
    printf("%s\n", title);
    for (int h = 4; h >= 1; h--) {
        printf("  ");
        for (int x = 1; x < W; x++) putchar(mark_at[x] >= h ? '|' : ' ');
        putchar('\n');
    }
    printf("  order: %s\n", order);
    memset(mark_at, 0, sizeof mark_at); olen = 0; order[0] = 0;
}
static int ctz_height(int i) { int h = 1; while ((i & 1) == 0) { i >>= 1; h++; } return h; }

int main(void) {
    rule(0, W, 4);    draw("recursive, inorder (Program 5.8):");
    rule_pre(0, W, 4); draw("recursive, preorder:");
    rule_nr(0, W, 4); draw("nonrecursive, bottom-up (Program 5.9):");
    printf("height(i) = 1 + trailing zeros of i:");
    for (int i = 1; i < W; i++) printf(" %d", ctz_height(i));
    putchar('\n');
    return 0;
}
