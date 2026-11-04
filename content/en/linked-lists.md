Book sections: 3.3 Linked Lists (Definitions 3.2 and 3.3, Program 3.9), 3.4 Elementary List Processing (Programs 3.10 to 3.14, Table 3.1), 3.5 Memory Allocation for Lists

An array gives you a\[i\] in one step, but if you want to squeeze an item into the middle you have to shift everything else. A linked list flips the deal: **inserting and deleting become a matter of changing two pointers**, but to reach item number 500 you have to walk 500 steps. Choosing between them means choosing which operation will happen more often.

:::goal The goal of this lesson:
Write lists by hand without breaking them: insertion and deletion, the circular list (Josephus), reversal, insertion sort on a list, doubly linked lists, the head and tail conventions, and how to manage memory yourself with a free list.
:::

## 3.3 The linked list

**Definition 3.2:** a set of items where each item sits inside a "node" that also holds a pointer to a node. And a narrower definition (3.3): a list is either empty (null) or a pointer to a node holding an item and a pointer to a list. The second definition is recursive, and it makes recursive functions on lists feel very natural (lesson 11).

```c
typedef struct node *link;
struct node { Item item; link next; };

/* insert t after x */          /* delete the node after x */
t->next = x->next;               t = x->next;
x->next = t;                     x->next = t->next;  free(t);
```

Notice the order of the two lines in the insertion: if you swap them, `x->next` is lost before you copy it, and the rest of the list is cut off and leaks.

::figure list-insert

### The circular list: Josephus (Program 3.9)

N people in a circle, and every Mth person leaves until one is left. The last node points back to the first, so the counting wraps around by itself, and deleting is `x->next = x->next->next`. For N=9 and M=5 the survivor is 8. For 41 and 3 (the original story) the survivor is 31.

```c listing="Program 3.9"
while (x != x->next)
  { for (i = 1; i < M; i++) x = x->next;
    x->next = x->next->next; N--; }
printf("%d\n", x->item);
```

* [src/03-data-structures/josephus.c](src:src/03-data-structures/josephus.c)

::figure josephus

## 3.4 Elementary list processing

### Reversing a list (Program 3.10)

Three pointers: `r` (the part reversed so far), `y` (what comes next), and `x` (the current node). Each step flips one arrow.

```c listing="Program 3.10"
link reverse(link x)
  { link t, y = x, r = NULL;
    while (y != NULL)
      { t = y->next; y->next = r; r = y; y = t; }
    return r; }
```

::figure list-reverse

### Insertion sort on a list (Program 3.11)

Pull the nodes out of the original list one at a time, and insert each one in its place in a sorted output list (which has a dummy head). No item moves in memory; only pointers change.

* [src/03-data-structures/linked\_list.c](src:src/03-data-structures/linked_list.c)

### The head and tail conventions (Table 3.1)

<table class="comp-table">
<tr><th>Convention</th><th>Empty list</th><th>Advantage or drawback</th></tr>
<tr><td>circular, never empty</td><td>none</td><td>Josephus; the first node has to be handled on its own</td></tr>
<tr><td>a head pointer, the end is NULL</td><td><code>head = NULL</code></td><td>the simplest, but inserting at the front is a special case</td></tr>
<tr><td>a dummy head, the end is NULL</td><td><code>head-&gt;next = NULL</code></td><td>every insertion is "after a node", so no special cases</td></tr>
<tr><td>a dummy head and a dummy tail</td><td><code>head-&gt;next = z</code></td><td>deleting and inserting at both ends work the same way</td></tr>
</table>

There is no "right" convention; pick one and stick to it through the whole program. Most bugs come from mixing two.

### Doubly linked lists

Each node has `prev` and `next`. The price: an extra pointer and 4 pointer updates on every insertion. The gain: you can delete a node while holding a pointer to **that node itself** (not the one before it), and you can walk in both directions. That is exactly what we will need in the priority queue with handles (lesson 22).

```c
/* delete t, O(1), no search */
t->prev->next = t->next;
t->next->prev = t->prev;  free(t);
```

* [src/03-data-structures/doubly\_linked.c](src:src/03-data-structures/doubly_linked.c)

### A black box interface (Program 3.12)

Sedgewick takes an important step: he puts the list operations behind an interface (`newNode`, `freeNode`, `insertNext`, `deleteNext`, `Next`, `Item`), and the client (Josephus in Program 3.13) never touches the pointers itself. This leads straight to ADTs.

## 3.5 Managing memory yourself: the free list

`malloc` and `free` are general purpose and relatively slow. If your program creates and deletes nodes of the same size all the time, you can do better: **allocate all the nodes at once**, and keep the unused ones on a "free" list. `newNode` takes one off the front of the free list, and `freeNode` puts it back at the front. Both are O(1), with no call to the system at all (Program 3.14).

```c listing="Program 3.14"
void initNodes(int N)
  { freelist = malloc((N+1)*(sizeof *freelist));
    for (i = 0; i < N+1; i++) freelist[i].next = &freelist[i+1];
    freelist[N].next = NULL; }
link newNode(int i)  { link x = deleteNext(freelist); x->item = i; x->next = x; return x; }
void freeNode(link x) { insertNext(freelist, x); }
```

You can do the same thing with **indices into an array** instead of pointers (a cursor implementation): `next[i]` is the number of the next slot. Our file does both and checks that Josephus gives the same survivor.

* [src/03-data-structures/list\_memory.c](src:src/03-data-structures/list_memory.c)

:::problem
The book hints that a real allocator (`malloc`) has to cope with different sizes and fragmentation, and that other languages have garbage collection to gather up nodes nobody points to. The free list is the simplest version of that work, and it shows you why memory leaks happen: a node that is neither in the list nor on the free list is lost.
:::

:::oops The 3 most famous list bugs
(1) The two lines of an insertion in the wrong order, so the list gets cut. (2) Using `t->next` after `free(t)`. (3) Forgetting the case of an empty list or the last node, which is why dummy nodes exist.
:::

:::quiz
You want to add lots of items in the middle of a long list. Which structure suits you better?

1. [ ] An array
2. [x] A linked list
3. [ ] Both cost the same

> Insertion means changing two pointers, O(1) (if you already have the spot). In an array it is an O(N) shift.
:::

:::quiz
What does this code get wrong? `x->next = t; t->next = x->next;`

1. [ ] Nothing
2. [x] After the first line x-\>next is t, so t points to itself and the rest of the list is lost
3. [ ] It only leaks the memory of t

> You have to copy x-\>next into t-\>next first, and then make x point to t.
:::

:::quiz
What is the main advantage of a doubly linked list?

1. [ ] It uses less memory
2. [x] You can delete a node in O(1) while holding a pointer to it, and walk in both directions
3. [ ] Sorting it is faster

> In a singly linked list you need the node before it, and finding that can take N steps.
:::

:::quiz
Why build a free list instead of calling malloc and free for every node?

1. [ ] Because malloc does not exist in C99
2. [x] So that newNode and freeNode are O(1) with no system calls, when all the nodes have the same size
3. [ ] To avoid recursion

> Allocate once and reuse. Faster and predictable, but you have to know the maximum up front.
:::
