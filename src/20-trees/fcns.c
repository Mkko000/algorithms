/* ============================================================
 *  fcns.c  —  general trees by "first child / next sibling" (Weiss 4.1.1) and the two classic
 *             traversal applications of Weiss 4.1.2: the indented directory listing (PRE-order)
 *             and the size of every directory in blocks (POST-order)
 *
 *  A general tree node may have any number of children, so a fixed array of child pointers is wasteful.
 *  Instead every node keeps TWO pointers: its first child and its next sibling. (This is exactly the
 *  left-child / right-sibling BINARY tree of tree_core.c's general_to_binary.)
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: Weiss, Data Structures and Algorithm Analysis, 4.1.1 (Fig 4.3-4.5) and 4.1.2 (Fig 4.6-4.10)
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 fcns.c -o fcns
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//>>fcns
typedef struct TNode {
    char name[24];
    int own;                        /* the file's own size in blocks (0 for a plain directory) */
    struct TNode *first_child, *next_sibling;
} TNode;

static TNode *tn(const char *name, int own)
{
    TNode *t = calloc(1, sizeof *t);
    assert(strlen(name) < sizeof t->name);
    strcpy(t->name, name); t->own = own;
    return t;
}
static TNode *add_child(TNode *parent, TNode *c)          /* append at the END of the sibling chain, O(#children) */
{
    if (!parent->first_child) parent->first_child = c;
    else { TNode *s = parent->first_child; while (s->next_sibling) s = s->next_sibling; s->next_sibling = c; }
    return c;
}
static void tfree(TNode *t) { if (t) { tfree(t->first_child); tfree(t->next_sibling); free(t); } }

/* PRE-order with indentation (Weiss Fig 4.6): print the node, THEN its children one tab deeper.
   Total work is O(N): every node is visited once. */
static void list_all(const TNode *t, int depth, char *out)
{
    for (int i = 0; i < depth; i++) strcat(out, "  ");
    strcat(out, t->name); strcat(out, "\n");
    for (const TNode *c = t->first_child; c; c = c->next_sibling) list_all(c, depth + 1, out);
}
/* POST-order (Weiss Fig 4.9): a directory's size is known only AFTER all its children's sizes are known. */
static int size_of(const TNode *t, char *out)
{
    int s = t->own;
    for (const TNode *c = t->first_child; c; c = c->next_sibling) s += size_of(c, out);
    char line[64]; snprintf(line, sizeof line, "%-10s %4d\n", t->name, s); strcat(out, line);
    return s;
}
static int count_nodes(const TNode *t) { int n = 1; for (const TNode *c = t->first_child; c; c = c->next_sibling) n += count_nodes(c); return n; }
static int tree_height(const TNode *t)                     /* nodes on the longest path (single node = 1) */
{
    int h = 0;
    for (const TNode *c = t->first_child; c; c = c->next_sibling) { int x = tree_height(c); if (x > h) h = x; }
    return h + 1;
}
//<<fcns

#ifndef FCNS_NO_MAIN
int main(void)
{
    /* the Unix-like directory of Weiss Fig 4.5, with plausible block sizes */
    TNode *root = tn("/usr", 1);
    TNode *mark = add_child(root, tn("mark", 1));
    TNode *alex = add_child(root, tn("alex", 1));
    TNode *bill = add_child(root, tn("bill", 1));
    TNode *book = add_child(mark, tn("book", 1));
    add_child(mark, tn("course", 1)); add_child(mark, tn("junk", 6));
    add_child(book, tn("ch1.r", 3)); add_child(book, tn("ch2.r", 2)); add_child(book, tn("ch3.r", 4));
    add_child(alex, tn("junk", 8));
    add_child(bill, tn("work", 1)); add_child(bill, tn("course", 1));

    char out[2048] = "";
    list_all(root, 0, out);
    printf("%s", out);
    assert(strstr(out, "/usr\n  mark\n    book\n      ch1.r\n") == out);
    assert(count_nodes(root) == 13 && tree_height(root) == 4);

    char sz[2048] = "";
    int total = size_of(root, sz);
    printf("-- sizes (post-order: children first) --\n%s", sz);
    /* ch1..ch3 = 9, book = 1+9 = 10, course = 1, junk = 6, mark = 1+10+1+6 = 18, alex = 1+8 = 9,
       work=1, course=1, bill = 1+1+1 = 3, /usr = 1+18+9+3 = 31 */
    assert(total == 31 && strstr(sz, "mark         18") && strstr(sz, "book         10") && strstr(sz, "bill          3"));
    /* the post-order line for /usr is last: its size depends on everything else */
    assert(strstr(sz, "/usr         31\n") + strlen("/usr         31\n") == sz + strlen(sz));
    tfree(root);
    printf("fcns: all tests passed\n");
    return 0;
}
#endif
