/* ============================================================
 *  linked_list.c  —  Singly linked lists: build, traverse, reverse,
 *                    insert, delete, and list insertion sort
 *  Chapter 3: Elementary Data Structures
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Programs 3.9-3.11
 *  Build:   gcc -std=c99 -Wall -O2 linked_list.c -o linked_list
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct node *link;
struct node { int item; link next; };

link new_node(int item, link next) {
    link x = malloc(sizeof *x);
    x->item = item; x->next = next;
    return x;
}

/* Build a list 1..n using a dummy head so insertion is uniform. */
link build(int n) {
    link head = new_node(-1, NULL), t = head;
    for (int i = 1; i <= n; i++) { t->next = new_node(i, NULL); t = t->next; }
    return head;   /* head is a dummy; real items start at head->next */
}

void print_list(link head) {
    for (link t = head->next; t != NULL; t = t->next) printf("%d ", t->item);
    putchar('\n');
}

/* In-place reversal: classic three-pointer dance (Program 3.10 idea). */
link reverse(link x) {
    link r = NULL;
    while (x != NULL) { link y = x->next; x->next = r; r = x; x = y; }
    return r;
}

/* List insertion sort (Program 3.11): pull nodes off the input list and
   splice each into the correct spot of a growing sorted list. */
link list_insertion_sort(link head) {
    link a = head->next;            /* the unsorted input */
    link out = new_node(-1, NULL);  /* dummy head of the sorted output */
    while (a != NULL) {
        link t = a; a = a->next;    /* detach first unsorted node t */
        link x = out;
        while (x->next != NULL && x->next->item < t->item) x = x->next;
        t->next = x->next; x->next = t;   /* splice t after x */
    }
    head->next = out->next;
    free(out);
    return head;
}

int main(void) {
    link h = build(8);
    printf("built:    "); print_list(h);

    h->next = reverse(h->next);
    printf("reversed: "); print_list(h);

    list_insertion_sort(h);
    printf("sorted:   "); print_list(h);
    return 0;
}
