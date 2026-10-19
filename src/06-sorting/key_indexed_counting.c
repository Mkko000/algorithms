/* ============================================================
 *  key_indexed_counting.c  —  Sort when keys are small integers 0..R-1
 *  Chapter 6: Elementary Sorting Methods (Section 6.10)
 *
 *  Program 6.17: count the keys, turn counts into starting positions
 *  (prefix sums), distribute into aux[], copy back. Property 6.12:
 *  linear time when the number of distinct keys R is O(N). Each item
 *  moves twice and the method is STABLE — shown here on records with
 *  section numbers 0..3 as keys (equal keys keep their input order).
 *  This stability is what LSD radix sort (Chapter 10) relies on.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.), Program 6.17
 *  Build:   gcc -std=c99 -Wall -O2 key_indexed_counting.c -o key_indexed_counting
 * ============================================================ */
#include <stdio.h>
#include <string.h>

#define R 26   /* keys are 'a'..'z' mapped to 0..25 */

void key_indexed(char a[], int n) {
    int count[R + 1] = {0};
    char aux[64];
    for (int i = 0; i < n; i++) count[a[i] - 'a' + 1]++;
    for (int r = 0; r < R; r++) count[r + 1] += count[r];   /* prefix sums = starts */
    for (int i = 0; i < n; i++) aux[count[a[i] - 'a']++] = a[i];
    for (int i = 0; i < n; i++) a[i] = aux[i];
}

int main(void) {
    char a[] = "easysortingexample";
    int n = (int)strlen(a);
    key_indexed(a, n);
    printf("%s\n", a);

    /* stability: records (section, name), sort by section only */
    struct { int key; const char *name; } rec[] = {
        {2,"Anderson"},{3,"Brown"},{3,"Davis"},{1,"Garcia"},{0,"Harris"},
        {3,"Jackson"},{2,"Johnson"},{1,"Jones"},{0,"Martin"},{2,"Martinez"}}, out[10];
    int M = 10, cnt[5] = {0};
    for (int i = 0; i < M; i++) cnt[rec[i].key + 1]++;
    for (int r = 0; r < 4; r++) cnt[r + 1] += cnt[r];
    for (int i = 0; i < M; i++) out[cnt[rec[i].key]++] = rec[i];
    printf("by section (stable):");
    for (int i = 0; i < M; i++) printf(" %d:%s", out[i].key, out[i].name);
    putchar('\n');
    return 0;
}
