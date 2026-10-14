/* ============================================================
 *  hanoi.c  —  Towers of Hanoi (divide and conquer)
 *  Chapter 5: Recursion and Trees
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 5.7
 *  Build:   gcc -std=c99 -Wall -O2 hanoi.c -o hanoi
 * ============================================================ */
#include <stdio.h>

void hanoi(int n, char from, char to, char aux) {
    if (n == 0) return;
    hanoi(n - 1, from, aux, to);
    printf("move disk %d : %c -> %c\n", n, from, to);
    hanoi(n - 1, aux, to, from);
}

int main(void) {
    hanoi(3, 'A', 'C', 'B');   /* 2^3 - 1 = 7 moves */
    return 0;
}
