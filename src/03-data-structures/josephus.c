/* ============================================================
 *  josephus.c  —  The Josephus problem with a circular linked list
 *  Chapter 3: Elementary Data Structures
 *
 *  N people in a circle; every Mth is eliminated until one remains.
 *  A circular list makes "walk around the ring and delete" natural.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 3.9
 *  Build:   gcc -std=c99 -Wall -O2 josephus.c -o josephus
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct node *link;
struct node { int item; link next; };

int josephus(int n, int m) {
    link t = malloc(sizeof *t), x;
    t->item = 1; t->next = t;      /* one-node circular list */
    x = t;
    for (int i = 2; i <= n; i++) {  /* grow the ring 2..n */
        x->next = malloc(sizeof *x->next);
        x = x->next;
        x->item = i; x->next = t;
    }
    while (x != x->next) {          /* until a single node is left */
        for (int i = 1; i < m; i++) x = x->next;   /* count m-1 hops */
        link dead = x->next;
        printf("%d ", dead->item);
        x->next = dead->next;
        free(dead);
    }
    int survivor = x->item;
    free(x);
    putchar('\n');
    return survivor;
}

int main(void) {
    int n = 9, m = 5;
    printf("N=%d M=%d  elimination order: ", n, m);
    printf("survivor = %d\n", josephus(n, m));
    return 0;
}
