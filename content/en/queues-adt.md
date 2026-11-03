Book sections: 4.5 Creation of a New ADT (Programs 4.6 to 4.8), 4.6 FIFO Queues and Generalized Queues (Programs 4.9 to 4.11), 4.7 Duplicate and Index Items (Program 4.12)

In the last lesson we used a ready made ADT. Here we do two things: we **invent** a new ADT from a real problem (back to union find), and then we look at the whole family of queues, not just the ordinary one.

:::goal The goal of this lesson:
Turn the union find algorithm into a clean ADT, implement the FIFO queue with a circular array and with a list, know the generalized queues (random queue, deque), and decide how to handle duplicate items (ignore the new against forget the old).
:::

## 4.5 Building a new ADT: equivalence relations

In lesson 02 we wrote union find as one solid block of a program. Here Sedgewick asks: **what operations does the client actually need?** The answer is three functions (Program 4.7):

```c listing="Program 4.7"
void UFinit(int);
int  UFfind(int, int);    /* are p and q connected?  */
void UFunion(int, int);   /* connect them            */
```

The client (Program 4.6) is now two lines: if they are not connected, connect them and print the pair. And the implementation (Program 4.8) is exactly the weighted quick union from lesson 02, hidden inside a file.

```c
if (!UFfind(p, q)) { UFunion(p, q); printf(" %d %d\n", p, q); }
```

The gain? If tomorrow you find a faster implementation (path compression, say), you replace the file `UF.c` and nothing else. And the drawback the book admits: `UFfind` and `UFunion` each look for the roots on their own, so we walk the path twice. A clean design sometimes costs a little, and that is a deliberate engineering decision, not a mistake.

* [src/04-adt/equivalence\_adt.c](src:src/04-adt/equivalence_adt.c)

## 4.6 FIFO queues

First in, first out: `QUEUEput` at the end, `QUEUEget` from the front (Program 4.9).

### With a list (Program 4.10)

A `head` pointer to the item that will leave next, and a `tail` pointer to the item that arrived last. Insert after the tail, delete from the head. Both are O(1).

### With a circular array (Program 4.11)

With a plain array, the head keeps moving forward and the slots behind it are wasted. The fix: when you reach the end, wrap around to the start (`% N`). The hard question: when is the queue empty and when is it full? Both happen when head == tail! The book solves it by allocating one extra slot (N+1) and never filling it. Our file solves it with an item counter. Both are correct.

```c listing="Program 4.11"
void QUEUEput(Item item)
  { q[tail++] = item; tail = tail % N; }
Item QUEUEget(void)
  { head = head % N; return q[head++]; }
```

::figure circ-queue

* [src/04-adt/queue.c](src:src/04-adt/queue.c)

### Generalized queues

The same interface (insert and delete), with different rules for who comes out:

<table class="comp-table">
<tr><th>Kind</th><th>Which item does it remove?</th><th>Used for</th></tr>
<tr><td>Stack</td><td>the newest</td><td>recursion, DFS, evaluating expressions</td></tr>
<tr><td>FIFO queue</td><td>the oldest</td><td>BFS, print queues and networks</td></tr>
<tr><td>Random queue</td><td>a random item</td><td>simulation, sharing out work fairly</td></tr>
<tr><td>Deque</td><td>from either end</td><td>generalizes the stack and the queue together</td></tr>
<tr><td>Priority queue</td><td>the largest</td><td>scheduling, heapsort (lesson 21)</td></tr>
<tr><td>Symbol table</td><td>the one with a given key</td><td>dictionaries, indexes (the whole searching part)</td></tr>
</table>

A random queue is very easy with an array: to remove a random item, exchange a random slot with the last one and remove the last. O(1).

```c
Item RQget(void)
  { int i = rand() % N; Item t = q[i];
    q[i] = q[N-1]; N--; return t; }
```

```
random queue gets: 33 66 11 77 22 44 55 88
deque (-1 0 1 2): popFront=-1 popBack=2 popFront=0 popBack=1
```

## 4.7 Duplicate items and index items

A question we usually ignore: if you insert something that is already in the ADT, what happens? In many applications you do not want duplicates. Two policies:

* **ignore the new:** the item is already there? Ignore the new one.
* **forget the old:** the item is already there? Remove the old one and insert the new one (in its new place in the order).

Notice that the policy changes **the order** in which items come out. With the same input `3 5 3 7 5 1` on a stack:

```
ignore-new  bottom->top: 3 5 7 1
forget-old  bottom->top: 3 7 5 1
```

So how do we quickly know whether an item is already there? If the items are small integers (0 to N−1, "index items"), we keep an array `t[i]` holding 1 if i is present. That is Program 4.12: every operation is still O(1). And if the items are not small integers? Then we need a symbol table, which is exactly the subject of Part 4.

```c listing="Program 4.12"
void STACKpush(int item)
  { if (t[item] == 1) return;          /* ignore-new */
    s[N++] = item; t[item] = 1; }
int STACKpop(void)
  { N--; t[s[N]] = 0; return s[N]; }
```

* [src/04-adt/generalized\_queues.c](src:src/04-adt/generalized_queues.c)

:::oops The trap in forget the old
To remove the old item from the middle of a stack, you need to know where it is and shift everything above it, which is O(N) if you are not careful. Ignoring the new one is much cheaper, and that is why it is the common choice.
:::

:::quiz
Why did the book turn union find into an ADT?

1. [ ] To make it faster
2. [x] To separate the client from the implementation, so we can swap the algorithm without touching the client
3. [ ] Because C requires it

> The implementation may even be a little slower (we find the roots twice), but the design is cleaner.
:::

:::quiz
In a circular array, what does head == tail mean?

1. [ ] Definitely full
2. [ ] Definitely empty
3. [x] It is ambiguous, which is why we reserve an extra slot or count the items

> Both cases give the same condition if you fill the whole array.
:::

:::quiz
How do you remove a random item from a random queue in O(1)?

1. [ ] Shift every item after it
2. [x] Exchange it with the last item and decrease N
3. [ ] It cannot be done in O(1)

> The order inside a random queue does not matter anyway, so the exchange does no harm.
:::

:::quiz
A stack with the ignore the new policy; we insert `1 2 1 3`. What do the first and second pop return?

1. [ ] 1, then 3
2. [x] 3, then 2
3. [ ] 3, then 1

> The second 1 was ignored, so the stack holds 1 2 3. Out come 3, then 2.
:::
