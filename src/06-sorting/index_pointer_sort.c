/* ============================================================
 *  index_pointer_sort.c  —  Sorting other types of data, and
 *                           sorting WITHOUT moving the records
 *  Chapter 6: Elementary Sorting Methods (Sections 6.7, 6.8)
 *
 *  6.7: one sort routine, many data types, by isolating the Item
 *       (key(), less(), exch()) in an interface — here a record type
 *       {name, grade}; Programs 6.9-6.12.
 *  6.8: when records are BIG, sort small things instead:
 *       - index sort:   permute an index array a[] so data[a[i]] is in order
 *       - pointer sort: permute an array of pointers to the records
 *       - then (optionally) rearrange the records in place, following
 *         the permutation's cycles, moving each record ONCE (Program 6.14).
 *  Several "views" of the same data (by name, by grade) can coexist.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 6.9-6.14 (Sections 6.7-6.8)
 *  Build:   gcc -std=c99 -Wall -O2 index_pointer_sort.c -o index_pointer_sort
 * ============================================================ */
#include <stdio.h>
#include <string.h>

typedef struct { char name[16]; int grade; char filler[200]; } record;   /* "big" record */
#define N 6
static record data[N] = {{"Wilson",63,""},{"Johnson",86,""},{"Jones",87,""},
                         {"Smith",90,""},{"Washington",84,""},{"Thompson",65,""}};

/* index sort: insertion sort on the index array, keys looked up through it */
void index_sort(int a[], int (*lessf)(int, int)) {
    for (int i = 1; i < N; i++) { int v = a[i], j = i; while (j > 0 && lessf(v, a[j-1])) { a[j] = a[j-1]; j--; } a[j] = v; }
}
static int by_name(int i, int j)  { return strcmp(data[i].name, data[j].name) < 0; }
static int by_grade(int i, int j) { return data[i].grade < data[j].grade; }

/* pointer sort */
void pointer_sort(record *p[]) {
    for (int i = 1; i < N; i++) { record *v = p[i]; int j = i; while (j > 0 && strcmp(v->name, p[j-1]->name) < 0) { p[j] = p[j-1]; j--; } p[j] = v; }
}

/* Program 6.14: in-place rearrangement following cycles; each record moves once */
int moves;
void insitu(record d[], int a[]) {
    for (int i = 0; i < N; i++) {
        record v = d[i]; int j, k;
        for (k = i; a[k] != i; k = a[j], a[j] = j) { j = k; d[k] = d[a[k]]; moves++; }
        d[k] = v; a[k] = k;
    }
}

int main(void) {
    int byname[N], bygrade[N]; record *p[N];
    for (int i = 0; i < N; i++) { byname[i] = bygrade[i] = i; p[i] = &data[i]; }
    index_sort(byname, by_name);
    index_sort(bygrade, by_grade);
    pointer_sort(p);
    printf("%-3s %-11s %-6s | by name (index) | by grade (index) | by name (pointer)\n", "i", "data", "grade");
    for (int i = 0; i < N; i++)
        printf("%-3d %-11s %-6d | %-15s | %-16s | %s\n", i, data[i].name, data[i].grade,
               data[byname[i]].name, data[bygrade[i]].name, p[i]->name);
    insitu(data, bygrade);
    printf("after in-place rearrangement by grade (%d record moves):", moves);
    for (int i = 0; i < N; i++) printf(" %s(%d)", data[i].name, data[i].grade);
    putchar('\n');
    return 0;
}
