/* ============================================================
 *  bst.c  —  Binary Search Tree symbol table
 *  Chapter 12: Symbol Tables and BSTs
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 12.7
 *  Build:   gcc -std=c99 -Wall -O2 bst.c -o bst
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char *Key;
typedef struct { Key key; int val; } Item;
static Item NULLitem = {NULL, -1};
#define key(A)  ((A).key)
#define less(A,B) (strcmp(A,B) < 0)
#define eq(A,B)   (strcmp(A,B) == 0)

typedef struct STnode *link;
struct STnode { Item item; link l, r; int N; };
static link head, z;

link NEW(Item item, link l, link r, int N) {
    link x = malloc(sizeof *x);
    x->item = item; x->l = l; x->r = r; x->N = N;
    return x;
}
void STinit(void) { head = (z = NEW(NULLitem, NULL, NULL, 0)); }
int  STcount(void){ return head->N; }

Item searchR(link h, Key v) {
    Key t = key(h->item);
    if (h == z) return NULLitem;
    if (eq(v, t)) return h->item;
    if (less(v, t)) return searchR(h->l, v);
    return searchR(h->r, v);
}
Item STsearch(Key v) { return searchR(head, v); }

link insertR(link h, Item item) {
    Key v = key(item), t = key(h->item);
    if (h == z) return NEW(item, z, z, 1);
    if (less(v, t)) h->l = insertR(h->l, item);
    else            h->r = insertR(h->r, item);
    (h->N)++; return h;
}
void STinsert(Item item) { head = insertR(head, item); }

void sortR(link h, void (*visit)(Item)) {
    if (h == z) return;
    sortR(h->l, visit);
    visit(h->item);
    sortR(h->r, visit);
}
void show(Item x) { if (x.key) printf("%s:%d ", x.key, x.val); }

int main(void) {
    const char *words[] = {"search","binary","tree","symbol","table","insert","delete"};
    STinit();
    for (int i = 0; i < 7; i++) {
        Item it; it.key = (char*)words[i]; it.val = i;
        STinsert(it);
    }
    printf("count = %d\n", STcount());
    printf("inorder: "); sortR(head, show); putchar('\n');
    Item hit = STsearch("tree");
    printf("search \"tree\" -> %d\n", hit.val);
    Item miss = STsearch("heap");
    printf("search \"heap\" -> %s\n", miss.key ? "hit" : "miss");
    return 0;
}
