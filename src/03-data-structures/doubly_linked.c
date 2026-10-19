/* ============================================================
 *  doubly_linked.c  —  Linked-list conventions and a doubly linked list
 *  Chapter 3: Elementary Data Structures (Sections 3.3, 3.4)
 *
 *  The book's Table 3.1 lists five conventions for head and tail:
 *   circular never-empty, head pointer + null tail, dummy head +
 *   null tail, dummy head + dummy tail, ... Each makes some code
 *   simpler and other code uglier. Here we use dummy head AND tail
 *   in a DOUBLY linked list, so delete(t) needs only t itself and
 *   no special cases at either end.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Section 3.4 (Table 3.1, Figures 3.13-3.14)
 *  Build:   gcc -std=c99 -Wall -O2 doubly_linked.c -o doubly_linked
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>

typedef struct dnode *dlink;
struct dnode { int item; dlink prev, next; };
static dlink head, tail;

void dl_init(void) {
    head = malloc(sizeof *head); tail = malloc(sizeof *tail);
    head->prev = NULL; head->next = tail; tail->prev = head; tail->next = NULL;
}
dlink dl_insert_after(dlink x, int v) {           /* O(1), 4 link updates */
    dlink t = malloc(sizeof *t); t->item = v;
    t->next = x->next; t->prev = x;
    x->next->prev = t; x->next = t;
    return t;
}
void dl_delete(dlink t) {                         /* O(1), no search needed */
    t->prev->next = t->next; t->next->prev = t->prev; free(t);
}
void dl_show(const char *msg) {
    printf("%-22s forward:", msg);
    for (dlink t = head->next; t != tail; t = t->next) printf(" %d", t->item);
    printf("   backward:");
    for (dlink t = tail->prev; t != head; t = t->prev) printf(" %d", t->item);
    putchar('\n');
}

int main(void) {
    dl_init();
    dlink keep[6];
    for (int i = 1; i <= 5; i++) keep[i] = dl_insert_after(tail->prev, i * 10);
    dl_show("after appends");
    dl_delete(keep[3]);          dl_show("delete 30 by handle");
    dl_insert_after(head, 5);    dl_show("insert 5 at front");
    dl_delete(keep[5]);          dl_show("delete last (50)");
    while (head->next != tail) dl_delete(head->next);
    dl_show("emptied");
    free(head); free(tail);
    return 0;
}
