/* ============================================================
 *  ex4_24_adts.c  —  Exercises 4.19, 4.24, 4.28, 4.42, 4.48, 4.49
 *  Chapter 4: Abstract Data Types — solved exercises
 *
 *  4.19 Replace "test if empty" by count() (array stack).
 *  4.24 One array, two pushdown stacks: one grows from the start, one
 *       from the end; full only when the whole array is used.
 *  4.28 Union-find ADT: return the number of nodes connected to a node.
 *  4.42 Random queue: get removes a uniformly random item, O(1) each.
 *  4.48 Stack without duplicates, ignore-the-new-item policy.
 *  4.49 Stack without duplicates, forget-the-old-item policy.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 4.4, 4.8, Sections 4.6-4.7
 *  Build:   gcc -std=c99 -Wall -O2 ex4_24_adts.c -o ex4_24
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

/* ---- 4.24 two stacks in one array (4.19 count included) ---- */
#define MAX 8
static int A[MAX], top0 = 0, top1 = MAX;          /* stack0: A[0..top0-1], stack1: A[top1..MAX-1] */
static int push(int s, int v) { if (top0 == top1) return 0; if (s == 0) A[top0++] = v; else A[--top1] = v; return 1; }
static int pop(int s) { return s == 0 ? A[--top0] : A[top1++]; }
static int count(int s) { return s == 0 ? top0 : MAX - top1; }

/* ---- 4.28 weighted UF, size of a node's component ---- */
static int id[16], sz[16];
static int find(int x) { while (x != id[x]) x = id[x] = id[id[x]]; return x; }
static void unite(int p, int q) { int i = find(p), j = find(q); if (i == j) return; if (sz[i] < sz[j]) { int t = i; i = j; j = t; } id[j] = i; sz[i] += sz[j]; }
static int compsize(int p) { return sz[find(p)]; }

/* ---- 4.42 random queue: swap a random item to the end, remove it ---- */
static int RQ[100], rqn = 0;
static unsigned long long rs = 7;
static unsigned rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (unsigned)(rs >> 11); }
static void rqput(int v) { RQ[rqn++] = v; }
static int rqget(void) { int i = rnd() % rqn, v = RQ[i]; RQ[i] = RQ[--rqn]; return v; }

/* ---- 4.48 / 4.49 brute-force no-duplicate stacks ---- */
static int S[32], sn;
static void push_ignore(int v) { for (int i = 0; i < sn; i++) if (S[i] == v) return; S[sn++] = v; }
static void push_forget(int v)
{
    int i, j;
    for (i = 0; i < sn; i++) if (S[i] == v) break;
    if (i < sn) { for (j = i; j < sn - 1; j++) S[j] = S[j + 1]; sn--; }   /* remove the old copy */
    S[sn++] = v;
}
static void showS(const char *t) { printf("%s", t); for (int i = 0; i < sn; i++) printf(" %d", S[i]); printf("   (top = right)\n"); }

int main(void)
{
    int pushes = 0;
    for (int k = 0; k < 5; k++) pushes += push(0, k);
    for (int k = 0; k < 5; k++) pushes += push(1, 100 + k);
    printf("4.24 pushed %d of 10 into an array of %d: count0=%d count1=%d (full)\n", pushes, MAX, count(0), count(1));
    int a0 = pop(0), a1 = pop(1);
    printf("     pop0=%d pop1=%d -> count0=%d count1=%d\n", a0, a1, count(0), count(1));

    for (int i = 0; i < 16; i++) { id[i] = i; sz[i] = 1; }
    int pr[][2] = { {3,4},{4,9},{8,0},{2,3},{5,6},{2,9},{5,9},{7,3},{4,8},{5,6},{0,2},{6,1} };  /* Figure 1.1 */
    for (int k = 0; k < 12; k++) unite(pr[k][0], pr[k][1]);
    printf("4.28 after the pairs of Figure 1.1: size(0)=%d size(7)=%d size(10)=%d\n", compsize(0), compsize(7), compsize(10));

    int hist[5] = { 0 };
    for (int t = 0; t < 100000; t++) {
        rqn = 0; for (int k = 0; k < 5; k++) rqput(k);
        hist[rqget()]++;
    }
    printf("4.42 first get from a random queue of 0..4 (100000 trials):");
    for (int k = 0; k < 5; k++) printf(" %d:%d", k, hist[k]);
    printf("\n");

    int seq[] = { 1, 2, 3, 2, 4, 1, 5 };
    sn = 0; for (int k = 0; k < 7; k++) push_ignore(seq[k]); showS("4.48 ignore new, push 1 2 3 2 4 1 5:");
    sn = 0; for (int k = 0; k < 7; k++) push_forget(seq[k]); showS("4.49 forget old, push 1 2 3 2 4 1 5:");
    return 0;
}
