/* Shared Item macros in the Sedgewick style.
 * Include this and define Item as needed, or use the int defaults. */
#ifndef COMMON_ITEM_H
#define COMMON_ITEM_H
typedef int Item;
#define key(A)    (A)
#define less(A,B) (key(A) < key(B))
#define eq(A,B)   (!less(A,B) && !less(B,A))
#define exch(A,B) { Item t = A; A = B; B = t; }
#define compexch(A,B) if (less(B,A)) exch(A,B)
#endif
