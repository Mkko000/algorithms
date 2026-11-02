/* ============================================================
 *  cursor_list.c  —  linked lists WITHOUT pointers (the "cursor" implementation)
 *
 *  Some languages (old FORTRAN/BASIC, or embedded code with no malloc) have no pointers.
 *  The trick: one big array of nodes; a "pointer" is just an array index; index 0 means NULL.
 *  Slot 0 of the pool is the header of the FREE list, so allocating = pop from the free list,
 *  freeing = push onto it. Many lists can share one pool. This is also how a database
 *  or file system links records by record number.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Weiss "Data Structures and Algorithm Analysis" ch.3.2.8 (cursor implementation,
 *                C++ code translated to C); Lipschutz ch.5.3 (linked lists in memory: INFO/LINK arrays)
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 cursor_list.c -o cursor_list
 * ============================================================ */
#include <stdio.h>
#include <assert.h>

//>>pool
#define SPACE 16
typedef struct { int val; int next; } cnode;       /* next == 0  means "no node" */
static cnode pool[SPACE];

static void pool_init(void)
{
    for (int i = 0; i < SPACE; i++) pool[i].next = (i + 1) % SPACE;   /* 0 -> 1 -> ... -> SPACE-1 -> 0 */
    pool[0].val = 0;                                                  /* slot 0 = free-list header */
}
static int cursor_alloc(void)                  /* returns 0 when the pool is exhausted (OVERFLOW) */
{
    int p = pool[0].next;
    pool[0].next = pool[p].next;               /* unlink first free cell */
    return p;
}
static void cursor_free(int p)
{
    pool[p].next = pool[0].next;               /* push on the free list */
    pool[0].next = p;
}
//<<pool

//>>list
/* a list is a header cell; the header is allocated like any other cell */
static int list_make(void)
{
    int h = cursor_alloc();
    if (!h) return 0;
    pool[h].next = 0;
    return h;
}
static int list_empty(int h) { return pool[h].next == 0; }

static int list_find(int h, int v)             /* returns the cell index or 0 */
{
    int p = pool[h].next;
    while (p && pool[p].val != v) p = pool[p].next;
    return p;
}
static int list_find_prev(int h, int v)
{
    int p = h;
    while (pool[p].next && pool[pool[p].next].val != v) p = pool[p].next;
    return p;                                   /* pool[p].next is the target, or 0 if absent */
}
static int list_insert_after(int p, int v)      /* returns new cell or 0 on overflow */
{
    int c = cursor_alloc();
    if (!c) return 0;
    pool[c].val = v;
    pool[c].next = pool[p].next;
    pool[p].next = c;
    return c;
}
static int list_delete(int h, int v)            /* 1 if something was removed */
{
    int p = list_find_prev(h, v);
    int t = pool[p].next;
    if (!t) return 0;
    pool[p].next = pool[t].next;
    cursor_free(t);
    return 1;
}
//<<list

static int free_cells(void) { int n = 0; for (int p = pool[0].next; p; p = pool[p].next) n++; return n; }
static void show(const char *tag, int h)
{
    printf("%-10s", tag);
    for (int p = pool[h].next; p; p = pool[p].next) printf("%d ", pool[p].val);
    puts("");
}

int main(void)
{
    pool_init();
    assert(free_cells() == SPACE - 1);          /* cell 0 is the free-list header itself */
    int L1 = list_make(), L2 = list_make();     /* two lists sharing ONE pool */
    assert(L1 && L2 && L1 != L2 && list_empty(L1));

    int last = L1;
    for (int v = 10; v <= 40; v += 10) last = list_insert_after(last, v);
    last = L2;
    for (int v = 1; v <= 3; v++) last = list_insert_after(last, v);
    show("L1:", L1); show("L2:", L2);
    assert(free_cells() == SPACE - 1 - 2 - 4 - 3);

    int c = list_find(L1, 30);
    assert(c && pool[c].val == 30 && list_find(L1, 77) == 0);
    assert(list_insert_after(c, 35));
    show("L1 +35:", L1);
    assert(list_delete(L1, 20) && !list_delete(L1, 20));
    show("L1 -20:", L1);

    /* exhaust the pool: OVERFLOW is reported by a 0 index, not by crashing */
    int n = 0; while (list_insert_after(L2, 99)) n++;
    assert(n == 6 && free_cells() == 0);               /* 15 cells - 2 headers - 7 values = 6 left */
    printf("pool exhausted after %d extra inserts -> alloc returns 0 (OVERFLOW)\n", n);
    while (list_delete(L2, 99)) ;               /* give the cells back */
    assert(free_cells() == n);                  /* ... and they are reusable */
    puts("cursor lists ok");
    return 0;
}
