/* ============================================================
 *  ex5_22_ruler_hanoi.c  —  Exercises 5.21-5.24
 *  Chapter 5: Recursion and Trees — solved exercises
 *
 *  5.21 Prove that Program 5.7 (towers of Hanoi) is optimal: any
 *       solution needs at least 2^N - 1 moves.  (Checked here by an
 *       exhaustive breadth-first search for small N.)
 *  5.22 Recursive program: length of the i-th mark in a ruler with
 *       2^n - 1 marks.
 *  5.23 Find a property of i that gives the direction of the i-th move.
 *  5.24 Fill an array with all the moves, nonrecursively (as Program 5.9).
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 5.7-5.9, Exercises 5.21-5.24
 *  Build:   gcc -std=c99 -Wall -O2 ex5_22_ruler_hanoi.c -o ex5_22
 * ============================================================ */
#include <stdio.h>
#include <string.h>

/* 5.22: middle mark has length n; left and right halves are rulers of n-1 */
static int mark(int i, int n)
{
    int mid = 1 << (n - 1);
    if (i == mid) return n;
    return i < mid ? mark(i, n - 1) : mark(i - mid, n - 1);
}

/* Program 5.7 with +1 = shift right (cyclically), -1 = shift left */
static int moves[1 << 12][2], nm;
static void hanoi(int N, int d) { if (N == 0) return; hanoi(N - 1, -d); moves[nm][0] = N; moves[nm][1] = d; nm++; hanoi(N - 1, -d); }

/* 5.21: BFS over all 3^N states, state = peg of every disk */
static int bfs(int N)
{
    static int dist[59049], q[59049];
    int S = 1; for (int i = 0; i < N; i++) S *= 3;
    memset(dist, -1, sizeof dist);
    int h = 0, t = 0; dist[0] = 0; q[t++] = 0;
    int goal = S - 1;                                   /* all disks on peg 2 */
    while (h < t) {
        int s = q[h++], peg[12], x = s;
        for (int i = 0; i < N; i++) { peg[i] = x % 3; x /= 3; }   /* disk i (0 smallest) */
        int top[3] = { -1, -1, -1 };
        for (int i = N - 1; i >= 0; i--) top[peg[i]] = i;          /* smallest disk on each peg */
        for (int a = 0; a < 3; a++) for (int b = 0; b < 3; b++) {
            if (a == b || top[a] < 0 || (top[b] >= 0 && top[b] < top[a])) continue;
            int p3 = 1; for (int i = 0; i < top[a]; i++) p3 *= 3;
            int ns = s + (b - a) * p3;
            if (dist[ns] < 0) { dist[ns] = dist[s] + 1; q[t++] = ns; }
        }
    }
    return dist[goal];
}

int main(void)
{
    printf("5.22 ruler n=4 :");
    for (int i = 1; i < 16; i++) printf(" %d", mark(i, 4));
    printf("\n     mark(i) = 1 + (number of trailing 0 bits of i):");
    int ok = 1;
    for (int i = 1; i < (1 << 10); i++) { int z = 0, x = i; while (!(x & 1)) { x >>= 1; z++; } if (mark(i, 10) != z + 1) ok = 0; }
    printf(" %s for n = 10\n", ok ? "true" : "FALSE");

    nm = 0; hanoi(4, 1);
    printf("5.23 hanoi(4): ");
    for (int i = 0; i < nm; i++) printf("%+d ", moves[i][0] * moves[i][1]);
    printf("\n     disk moved at step i = ruler mark(i); disk k always moves in direction %s\n",
           "(-1)^(N-k) * d  (odd and even disks go opposite ways)");
    int dirok = 1;
    for (int N = 1; N <= 12; N++) {
        nm = 0; hanoi(N, 1);
        for (int i = 0; i < nm; i++) {
            int k = moves[i][0], want = ((N - k) % 2 == 0) ? 1 : -1;
            int z = 0, x = i + 1; while (!(x & 1)) { x >>= 1; z++; }
            if (moves[i][1] != want || k != z + 1) dirok = 0;
        }
    }
    printf("     verified for N = 1..12: %s\n", dirok ? "yes" : "NO");

    /* 5.24: the same moves without recursion, straight from the rule */
    int N = 4; printf("5.24 nonrecursive:");
    for (int i = 1; i < (1 << N); i++) {
        int z = 0, x = i; while (!(x & 1)) { x >>= 1; z++; }
        int k = z + 1, d = ((N - k) % 2 == 0) ? 1 : -1;
        printf(" %+d", k * d);
    }
    printf("\n");

    for (int n = 1; n <= 10; n++) printf("5.21 N=%2d: BFS shortest = %4d, 2^N-1 = %4d\n", n, bfs(n), (1 << n) - 1);
    return 0;
}
