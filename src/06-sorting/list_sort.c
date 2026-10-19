/* ============================================================
 *  list_sort.c  —  Sorting linked lists (selection sort on a list)
 *  Chapter 6: Elementary Sorting Methods (Section 6.9)
 *
 *  Arrays let us exchange a[i] and a[j]; lists do not have a[i].
 *  Program 6.16 removes the MAXIMUM node from the input list and
 *  pushes it onto the FRONT of the output list, so the output grows
 *  in increasing order with only pointer changes — no item ever moves.
 *  (Insertion sort for lists is in 03-data-structures/linked_list.c;
 *  mergesort for lists is in 08-mergesort/list_mergesort.c.)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 6.15-6.16
 *  Build:   gcc -std=c99 -Wall -O2 list_sort.c -o list_sort
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct node *link;
struct node { int item; link next; };
static link NEW(int item, link next) { link x = malloc(sizeof *x); x->item = item; x->next = next; return x; }

/* Find the node BEFORE the max in a list with dummy head h */
static link findmax(link h) {
    link t = h;
    for (link x = h; x->next != NULL; x = x->next)
        if (x->next->item > t->next->item) t = x;
    return t;
}
link listselection(link h) {        /* h is a dummy head */
    link out = NULL;
    while (h->next != NULL) {
        link max = findmax(h);
        link t = max->next;          /* unlink the max node */
        max->next = t->next;
        t->next = out; out = t;      /* push onto output */
    }
    h->next = out;
    return h;
}

int main(void) {
    link h = NEW(0, NULL);
    srand(2);
    for (int i = 0; i < 12; i++) h->next = NEW(rand() % 100, h->next);
    printf("input :"); for (link x = h->next; x; x = x->next) printf(" %d", x->item); putchar('\n');
    listselection(h);
    printf("sorted:"); for (link x = h->next; x; x = x->next) printf(" %d", x->item); putchar('\n');
    while (h) { link t = h->next; free(h); h = t; }
    return 0;
}
