/* ============================================================
 *  knapsack.c  —  Knapsack (unlimited copies of each item type, as in the
 *                book): recursive vs. top-down and bottom-up DP
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
static long r_calls, m_calls;
int knap_r(int cap) {
    int max = 0;
    r_calls++;
    for (int i = 0; i < N; i++)
        if (size[i] <= cap) {
            int t = knap_r(cap - size[i]) + value[i];
            if (t > max) max = t;
        }
    return max;
}

/* Program 5.13: top-down DP (memoization). Save each value as the last
   action, check saved values as the first action. itemKnown[] records
   which item achieved the max so the solution can be reconstructed. */
#define MAXC 1000
static int maxKnown[MAXC], itemKnown[MAXC], known[MAXC];
int knap_m(int cap) {
    int i, space, max, maxi = -1, t;
    m_calls++;
    if (known[cap]) return maxKnown[cap];
    for (i = 0, max = 0; i < N; i++)
        if ((space = cap - size[i]) >= 0)
            if ((t = knap_m(space) + value[i]) > max) { max = t; maxi = i; }
    known[cap] = 1; maxKnown[cap] = max; itemKnown[cap] = maxi;
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
    printf("knapsack top-down  (cap=%d): %d\n", cap, knap_m(cap));
    printf("knapsack bottom-up (cap=%d): %d\n", cap, knap_dp(cap));
    printf("items used:");
    for (int c = cap; c > 0 && itemKnown[c] >= 0; c -= size[itemKnown[c]])
        printf(" %c(size %d, value %d)", 'A' + itemKnown[c], size[itemKnown[c]], value[itemKnown[c]]);
    printf("\n\ncalls: recursive vs top-down memo\n");
    for (int c = 10; c <= 40; c += 10) {
        r_calls = m_calls = 0;
        for (int i = 0; i < MAXC; i++) known[i] = 0;
        knap_r(c); knap_m(c);
        printf("  cap=%2d  recursive=%9ld  memo=%4ld\n", c, r_calls, m_calls);
    }
    return 0;
}
