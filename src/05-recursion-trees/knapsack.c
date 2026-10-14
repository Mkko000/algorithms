/* ============================================================
 *  knapsack.c  —  0/1 knapsack: recursive vs. dynamic programming
 *  Chapter 5: Recursion and Trees
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 5.12-5.13
 *  Build:   gcc -std=c99 -Wall -O2 knapsack.c -o knapsack
 * ============================================================ */
#include <stdio.h>

#define N 5
static int size[N]  = {3, 4, 7, 8, 9};
static int value[N] = {4, 5, 10, 11, 13};

/* Recursive: for capacity cap, try each item that fits. Exponential. */
int knap_r(int cap) {
    int max = 0;
    for (int i = 0; i < N; i++)
        if (size[i] <= cap) {
            int t = knap_r(cap - size[i]) + value[i];
            if (t > max) max = t;
        }
    return max;
}

/* Bottom-up DP: maxKnap[w] = best value for capacity w. */
int knap_dp(int cap) {
    int maxKnap[100] = {0};
    for (int w = 1; w <= cap; w++) {
        int max = 0;
        for (int i = 0; i < N; i++)
            if (size[i] <= w) {
                int t = maxKnap[w - size[i]] + value[i];
                if (t > max) max = t;
            }
        maxKnap[w] = max;
    }
    return maxKnap[cap];
}

int main(void) {
    int cap = 17;
    printf("knapsack recursive (cap=%d): %d\n", cap, knap_r(cap));
    printf("knapsack DP        (cap=%d): %d\n", cap, knap_dp(cap));
    return 0;
}
