/* ============================================================
 *  ex18_01_linked_lists.c  —  Solved exercises, Chapter 18 (Linked list variants)
 *
 *  Thareja ch.6: Review Questions 5, 7-11 and Programming Exercises 1-39.
 *  PE 6, 8, 12(circular), 13, 25, 27, 36, 37-39 and 26 are solved in the module programs
 *  (circular_header.c, doubly_list.c, poly_list.c); this file solves all the others and
 *  re-checks the doubly-list ones with the NULL-terminated version the book uses.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. B. Thareja, "Data Structures Using C" (2nd ed.), Ch. 6 Exercises
 *  Build:   gcc -std=c99 -Wall -Wextra -O2 ex18_01_linked_lists.c -o ex18_01
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef struct node { int val; struct node *next; } node;

static node *mk(int v, node *next) { node *n = malloc(sizeof *n); if (!n) exit(1); n->val = v; n->next = next; return n; }
static node *arr2list(const int *a, int n) { node *h = NULL; for (int i = n - 1; i >= 0; i--) h = mk(a[i], h); return h; }
static int  list2arr(const node *h, int *o) { int k = 0; for (; h; h = h->next) o[k++] = h->val; return k; }
static void freel(node *h) { while (h) { node *t = h->next; free(h); h = t; } }
static int  eq(const node *h, const int *e, int n) { int b[64]; return list2arr(h, b) == n && (n == 0 || !memcmp(b, e, n * sizeof(int))); }

//>>PE1
/* ---- PE 1: remove every node whose value already appeared earlier ---- */
static node *dedup(node *h)
{
    for (node *p = h; p; p = p->next)
        for (node *q = p; q->next; ) {
            if (q->next->val == p->val) { node *d = q->next; q->next = d->next; free(d); } else q = q->next;
        }
    return h;
}
//<<PE1
//>>PE2
/* ---- PE 2 / 21: occurrences of a value;  PE 3: multiply by 10;  PE 4: non-zero;  PE 5/35: sorted ---- */
static int count_val(const node *h, int v) { int c = 0; for (; h; h = h->next) c += h->val == v; return c; }
static void scale(node *h, int k) { for (; h; h = h->next) h->val *= k; }
static int count_nonzero(const node *h) { int c = 0; for (; h; h = h->next) c += h->val != 0; return c; }
static int isSorted(const node *h) { for (; h && h->next; h = h->next) if (h->val > h->next->val) return 0; return 1; }
//<<PE2
//>>PE7
/* ---- PE 7: merge (= concatenate) two lists;  PE 9: merge two SORTED lists into a sorted one ---- */
static node *concat(node *a, node *b)
{
    if (!a) return b;
    node *p = a; while (p->next) p = p->next;
    p->next = b;
    return a;
}
static node *merge_sorted(node *a, node *b)
{
    node d = {0, NULL}, *t = &d;
    while (a && b) { node **s = (b->val < a->val) ? &b : &a; t->next = *s; *s = (*s)->next; t = t->next; }
    t->next = a ? a : b;
    return d.next;
}
//<<PE7
//>>PE10
/* ---- PE 10: delete first, last and middle node of a header list (h->next is the first data node) ---- */
static int del_after(node *p) { node *t = p->next; if (!t) return 0; p->next = t->next; free(t); return 1; }
static void header_delete_first_last_middle(node *hdr)
{
    int n = 0; for (node *p = hdr->next; p; p = p->next) n++;
    if (n == 0) return;
    int mid = (n - 1) / 2;                       /* index (0-based) of the middle node; lower middle if n is even */
    node *p = hdr; for (int i = 0; i < mid; i++) p = p->next;
    del_after(p);                                /* middle  */
    p = hdr; while (p->next && p->next->next) p = p->next;
    del_after(p);                                /* last (if the list had >=2 nodes this is a different node) */
    del_after(hdr);                              /* first   */
}
//<<PE10
//>>PE11
/* ---- PE 11: a new list holding every alternate element (1st, 3rd, 5th ...) of an existing one ---- */
static node *alternate_copy(const node *h)
{
    node *res = NULL, **tail = &res; int i = 0;
    for (; h; h = h->next, i++) if (i % 2 == 0) { *tail = mk(h->val, NULL); tail = &(*tail)->next; }
    return res;
}
//<<PE11
//>>PE14
/* ---- PE 14: delete the first / the last / all occurrences of a character (list of chars) ---- */
static node *del_first_occ(node *h, int c)
{
    node **pp = &h;
    while (*pp && (*pp)->val != c) pp = &(*pp)->next;
    if (*pp) { node *d = *pp; *pp = d->next; free(d); }
    return h;
}
static node *del_last_occ(node *h, int c)
{
    node **last = NULL;
    for (node **pp = &h; *pp; pp = &(*pp)->next) if ((*pp)->val == c) last = pp;
    if (last) { node *d = *last; *last = d->next; free(d); }
    return h;
}
static node *del_all_occ(node *h, int c)
{
    node **pp = &h;
    while (*pp) { if ((*pp)->val == c) { node *d = *pp; *pp = d->next; free(d); } else pp = &(*pp)->next; }
    return h;
}
//<<PE14
//>>PE15
/* ---- PE 15: reverse by recursion;  PE 33: reverse by relinking;  PE 20: first node to the end ---- */
static node *rev_rec(node *h) { if (!h || !h->next) return h; node *r = rev_rec(h->next); h->next->next = h; h->next = NULL; return r; }
static node *rev_iter(node *h) { node *p = NULL; while (h) { node *n = h->next; h->next = p; p = h; h = n; } return p; }
static node *first_to_last(node *h)
{
    if (!h || !h->next) return h;
    node *first = h, *p = h; h = h->next;
    while (p->next) p = p->next;
    p->next = first; first->next = NULL;
    return h;
}
//<<PE15
//>>PE16
/* ---- PE 16: one digit per node;  PE 17/23: sum and mean;  PE 18: min and max ---- */
static node *digits_list(long x)
{
    node *h = NULL;
    if (x < 0) x = -x;
    do { h = mk((int)(x % 10), h); x /= 10; } while (x);   /* least significant digit is pushed first */
    return h;
}
static void sum_mean(const node *h, double *sum, double *mean)
{
    int n = 0; *sum = 0;
    for (; h; h = h->next, n++) *sum += h->val;
    *mean = n ? *sum / n : 0;
}
static void min_max(const node *h, int *mn, int *mx)
{
    *mn = *mx = h->val;
    for (h = h->next; h; h = h->next) { if (h->val < *mn) *mn = h->val; if (h->val > *mx) *mx = h->val; }
}
//<<PE16
//>>PE24
/* ---- PE 24: delete the k-th node (k from 1);  PE 34: n-th from the end in one pass ---- */
static node *del_kth(node *h, int k)
{
    if (k < 1 || !h) return h;
    if (k == 1) { node *t = h->next; free(h); return t; }
    node *p = h;
    for (int i = 2; i < k && p->next; i++) p = p->next;
    if (p->next) del_after(p);
    return h;
}
static node *nth_from_end(node *h, int n)
{
    node *lead = h, *lag = h;
    for (int i = 0; i < n; i++) { if (!lead) return NULL; lead = lead->next; }
    while (lead) { lead = lead->next; lag = lag->next; }
    return lag;
}
//<<PE24

//>>DL
/* ---- NULL-terminated doubly linked list (the book's version): PE 12, 13, 19, 22, 32 ---- */
typedef struct dn { int val; struct dn *prev, *next; } dn;
static dn *dpush_back(dn **head, int v)
{
    dn *n = malloc(sizeof *n); if (!n) exit(1);
    n->val = v; n->next = NULL; n->prev = NULL;
    if (!*head) { *head = n; return n; }
    dn *p = *head; while (p->next) p = p->next;
    p->next = n; n->prev = p;
    return n;
}
static dn *dlast(dn *h) { while (h->next) h = h->next; return h; }
static void dfree(dn *h) { while (h) { dn *t = h->next; free(h); h = t; } }
static int dlen(const dn *h) { int n = 0; for (; h; h = h->next) n++; return n; }
/* PE 19: first<->last, second<->second-last ... (swap the DATA, two pointers walking inwards) */
static void d_mirror_values(dn *h)
{
    if (!h) return;
    dn *i = h, *j = dlast(h);
    while (i != j && i->prev != j) { int t = i->val; i->val = j->val; j->val = t; i = i->next; j = j->prev; }
}
static void d_add(dn *h, int k) { for (; h; h = h->next) h->val += k; }          /* PE 22 */
static dn *d_concat(dn *a, dn *b)                                                /* PE 12 */
{
    if (!a) return b;
    if (!b) return a;
    dn *l = dlast(a); l->next = b; b->prev = l;
    return a;
}
static dn *d_first_to_last(dn *h)                                                /* PE 13 */
{
    if (!h || !h->next) return h;
    dn *f = h; h = h->next; h->prev = NULL;
    dn *l = dlast(h); l->next = f; f->prev = l; f->next = NULL;
    return h;
}
static dn *d_middle_to_front(dn *h)                                              /* PE 32 */
{
    int n = dlen(h); if (n < 3) return h;
    dn *m = h; for (int i = 0; i < n / 2; i++) m = m->next;
    m->prev->next = m->next; if (m->next) m->next->prev = m->prev;               /* cut it out ...    */
    m->prev = NULL; m->next = h; h->prev = m;                                    /* ... put it first  */
    return m;
}
static int deq(const dn *h, const int *e, int n)         /* also verifies the prev links */
{
    int i = 0; const dn *last = NULL;
    for (; h; last = h, h = h->next, i++) { if (i >= n || h->val != e[i]) return 0; if (h->prev != last) return 0; }
    return i == n;
}
//<<DL

//>>REC
/* ---- RQ 7-11, PE 28-31: a list of student / employee records ---- */
typedef struct rec { int id; char name[16]; double pay; struct rec *next; } rec;
static rec *rec_insert_sorted(rec *h, int id, const char *name, double pay)    /* kept sorted by id */
{
    rec *n = malloc(sizeof *n); if (!n) exit(1);
    n->id = id; snprintf(n->name, sizeof n->name, "%s", name); n->pay = pay;
    rec **pp = &h;
    while (*pp && (*pp)->id < id) pp = &(*pp)->next;
    n->next = *pp; *pp = n;
    return h;
}
static rec *rec_find(rec *h, int id) { for (; h; h = h->next) if (h->id == id) return h; return NULL; }
static rec *rec_delete(rec *h, int id)
{
    rec **pp = &h;
    while (*pp && (*pp)->id != id) pp = &(*pp)->next;
    if (*pp) { rec *d = *pp; *pp = d->next; free(d); }
    return h;
}
static void rec_free(rec *h) { while (h) { rec *t = h->next; free(h); h = t; } }
/* RQ 11: sort a list of NAMES by exchanging the links of a selection-sort (the names are never copied) */
typedef struct sn { char s[16]; struct sn *next; } sn;
static sn *names_sort(sn *h)
{
    sn *out = NULL;
    while (h) {
        sn **minp = &h;
        for (sn **pp = &h; *pp; pp = &(*pp)->next) if (strcmp((*pp)->s, (*minp)->s) < 0) minp = pp;
        sn *m = *minp; *minp = m->next;                     /* detach the smallest */
        sn **tail = &out; while (*tail) tail = &(*tail)->next;
        m->next = NULL; *tail = m;
    }
    return out;
}
/* RQ 10: split a list of letters into an upper-case list and a lower-case list */
static void split_case(const node *h, node **up, node **lo)
{
    node **tu = up, **tl = lo; *up = *lo = NULL;
    for (; h; h = h->next) {
        node *n = mk(h->val, NULL);
        if (isupper(h->val)) { *tu = n; tu = &n->next; } else { *tl = n; tl = &n->next; }
    }
}
//<<REC

int main(void)
{
    int buf[64]; (void)buf;
    /* PE 1, 2, 3, 4, 5 */
    node *h = arr2list((int[]){3, 1, 3, 0, 2, 1, 3, 0}, 8);
    assert(count_val(h, 3) == 3 && count_nonzero(h) == 6 && !isSorted(h));
    scale(h, 10); assert(eq(h, (int[]){30,10,30,0,20,10,30,0}, 8));
    h = dedup(h); assert(eq(h, (int[]){30,10,0,20}, 4));
    freel(h);
    h = arr2list((int[]){1,2,2,5}, 4); assert(isSorted(h)); freel(h);

    /* PE 7, 9 */
    node *a = arr2list((int[]){1,4,9}, 3), *b = arr2list((int[]){2,3,10}, 3);
    node *m = merge_sorted(a, b); assert(eq(m, (int[]){1,2,3,4,9,10}, 6) && isSorted(m)); freel(m);
    a = arr2list((int[]){1,2}, 2); b = arr2list((int[]){3}, 1);
    m = concat(a, b); assert(eq(m, (int[]){1,2,3}, 3)); freel(m);

    /* PE 10: header list of 5 -> middle=3 gone, last=5 gone, first=1 gone */
    node *hdr = mk(0, arr2list((int[]){1,2,3,4,5}, 5));
    header_delete_first_last_middle(hdr); assert(eq(hdr->next, (int[]){2,4}, 2));
    header_delete_first_last_middle(hdr); assert(eq(hdr->next, NULL, 0));      /* middle=2 -> 4, last 4, nothing left */
    header_delete_first_last_middle(hdr); assert(hdr->next == NULL);
    freel(hdr);

    /* PE 11 */
    h = arr2list((int[]){1,2,3,4,5}, 5); node *alt = alternate_copy(h);
    assert(eq(alt, (int[]){1,3,5}, 3) && eq(h, (int[]){1,2,3,4,5}, 5)); freel(alt); freel(h);

    /* PE 14 on "banana" */
    h = arr2list((int[]){'b','a','n','a','n','a'}, 6);
    h = del_first_occ(h, 'a'); assert(eq(h, (int[]){'b','n','a','n','a'}, 5));
    h = del_last_occ(h, 'a');  assert(eq(h, (int[]){'b','n','a','n'}, 4));
    h = del_all_occ(h, 'n');   assert(eq(h, (int[]){'b','a'}, 2));
    h = del_all_occ(h, 'b');   h = del_all_occ(h, 'a'); assert(!h);

    /* PE 15, 20, 33 */
    h = arr2list((int[]){1,2,3,4}, 4);
    h = rev_rec(h);  assert(eq(h, (int[]){4,3,2,1}, 4));
    h = rev_iter(h); assert(eq(h, (int[]){1,2,3,4}, 4));
    h = first_to_last(h); assert(eq(h, (int[]){2,3,4,1}, 4));
    freel(h);

    /* PE 16 */
    h = digits_list(12345); assert(eq(h, (int[]){1,2,3,4,5}, 5)); freel(h);
    h = digits_list(0); assert(eq(h, (int[]){0}, 1)); freel(h);
    /* PE 17, 18, 23 */
    h = arr2list((int[]){4, 8, 15, 16, 23, 42}, 6);
    double s, mean; int mn, mx; sum_mean(h, &s, &mean); min_max(h, &mn, &mx);
    assert(s == 108 && mean == 18 && mn == 4 && mx == 42);
    printf("PE17/18: sum=%.0f mean=%.1f min=%d max=%d\n", s, mean, mn, mx);
    /* PE 24, 34 */
    assert(nth_from_end(h, 1)->val == 42 && nth_from_end(h, 6)->val == 4 && !nth_from_end(h, 7));
    h = del_kth(h, 3); assert(eq(h, (int[]){4,8,16,23,42}, 5));
    h = del_kth(h, 1); assert(eq(h, (int[]){8,16,23,42}, 4));
    h = del_kth(h, 99); assert(eq(h, (int[]){8,16,23,42}, 4));
    freel(h);

    /* doubly: PE 12, 13, 19, 22, 32 */
    dn *d = NULL; for (int i = 1; i <= 6; i++) dpush_back(&d, i);
    d_mirror_values(d);  assert(deq(d, (int[]){6,5,4,3,2,1}, 6));
    d_mirror_values(d);  assert(deq(d, (int[]){1,2,3,4,5,6}, 6));
    d_add(d, 10);        assert(deq(d, (int[]){11,12,13,14,15,16}, 6));
    d = d_first_to_last(d); assert(deq(d, (int[]){12,13,14,15,16,11}, 6));
    d = d_middle_to_front(d); assert(deq(d, (int[]){15,12,13,14,16,11}, 6));
    dn *e = NULL; dpush_back(&e, 0); d = d_concat(d, e); assert(deq(d, (int[]){15,12,13,14,16,11,0}, 7));
    dfree(d);
    dn *odd = NULL; for (int i = 1; i <= 5; i++) dpush_back(&odd, i);
    d_mirror_values(odd); assert(deq(odd, (int[]){5,4,3,2,1}, 5)); dfree(odd);       /* middle element stays */

    /* RQ 7-9, PE 28-31 */
    rec *r = NULL;
    r = rec_insert_sorted(r, 30, "Sara", 4500); r = rec_insert_sorted(r, 10, "Omar", 5200); r = rec_insert_sorted(r, 20, "Laila", 3900);
    assert(r->id == 10 && r->next->id == 20 && r->next->next->id == 30);
    assert(rec_find(r, 20) && strcmp(rec_find(r, 20)->name, "Laila") == 0 && !rec_find(r, 99));
    r = rec_delete(r, 10); r = rec_delete(r, 99); assert(r->id == 20 && !rec_find(r, 10));
    rec_free(r);
    /* RQ 10, 11 */
    node *letters = arr2list((int[]){'a','B','c','D','e'}, 5), *up, *lo;
    split_case(letters, &up, &lo);
    assert(eq(up, (int[]){'B','D'}, 2) && eq(lo, (int[]){'a','c','e'}, 3)); freel(up); freel(lo); freel(letters);
    sn *names = NULL; const char *nm[] = {"Zaki", "Adam", "Mona", "Bassem"};
    for (int i = 0; i < 4; i++) { sn *n = malloc(sizeof *n); snprintf(n->s, sizeof n->s, "%s", nm[i]); n->next = names; names = n; }
    names = names_sort(names);
    const char *exp[] = {"Adam", "Bassem", "Mona", "Zaki"}; int i = 0;
    for (sn *p = names; p; p = p->next, i++) assert(strcmp(p->s, exp[i]) == 0);
    assert(i == 4);
    while (names) { sn *t = names->next; free(names); names = t; }

    puts("ex18_01: Thareja ch.6 programming exercises verified");
    return 0;
}
