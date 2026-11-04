Book sections: 6.1 Rules of the Game (Program 6.1), 6.2 Selection Sort (Program 6.2), 6.3 Insertion Sort (Program 6.3), 6.4 Bubble Sort (Program 6.4), 6.5 Performance Characteristics of Elementary Sorts (Properties 6.1 to 6.6)

Why study N² algorithms when we know N log N ones exist? Sedgewick has three convincing reasons: (1) they are the **foundation** of the better algorithms: shellsort is an evolved insertion sort, and quicksort falls back on insertion sort for small files. (2) On small or nearly sorted files they **really are faster**. (3) They teach you the "rules of the game" we will use to measure everything.

:::goal The goal of this lesson:
Know the rules of the game (less, exch, compexch, stability, sorting in place), write selection, insertion (with its improvements) and bubble sort, and know from the numbers when each one is best.
:::

## 6.1 The rules of the game

Every algorithm in chapters 6 to 11 is written in the same shape: `sort(Item a[], int l, int r)` sorts `a[l..r]`, using only three operations on the items:

```c
typedef int Item;
#define key(A) (A)
#define less(A, B) (key(A) < key(B))
#define exch(A, B) { Item t = A; A = B; B = t; }
#define compexch(A, B) if (less(B, A)) exch(A, B)
```

Program 6.1 is the simplest possible sort: a driver that reads N numbers (or generates them at random) and sorts them with compexch in a double loop. Why the macros? So that when we change the type of the items (numbers, strings, records) we change one line. And so that we can easily **count** compares and exchanges.

The chapter's terms:

* **Internal or external:** the data is in memory or on disk (lesson 26).
* **In place:** a small amount of extra space (constant or log N), as opposed to needing a second array of size N.
* **Stable:** items with equal keys keep their original order. It matters if you sort by name and then by city and want the names to stay sorted within each city.
* **Adaptive:** the time drops when the input is nearly sorted.

We measure the cost by: the number of **compares**, the number of **exchanges** (or moves), and the extra space.

::trace selection

::trace insertion

## 6.2 Selection sort (Program 6.2)

Find the smallest item and put it first. Then find the smallest of the rest and put it second. And so on.

```c listing="Program 6.2"
void selection(Item a[], int l, int r)
  { int i, j;
    for (i = l; i < r; i++)
      { int min = i;
        for (j = i+1; j <= r; j++)
          if (less(a[j], a[min])) min = j;
        exch(a[i], a[min]); } }
```

Its big advantage: **every item moves exactly once**, only N−1 exchanges. Its drawback: **it is blind to its input**; an already sorted array takes exactly the same time.

::figure sel-trace

* [src/06-sorting/selection\_sort.c](src:src/06-sorting/selection_sort.c)

## 6.3 Insertion sort (Program 6.3)

Like sorting a hand of cards: you push each new card to the left until it reaches its place. The simple Program 6.1 did compexch all the way. Sedgewick makes **three improvements**:

1. **Stop early:** as soon as it finds a smaller item, it stops, instead of going all the way to the start.
2. **Moves instead of exchanges:** take the item into `v`, shift the larger items one step to the right, and put it into the empty slot. One move instead of the 3 operations in each exch.
3. **A sentinel:** put the smallest item in `a[l]` first, so the inner loop does not need to check `j > l`.

```c listing="Program 6.1"
void insertion(Item a[], int l, int r)
  { int i;
    for (i = r; i > l; i--) compexch(a[i-1], a[i]);   /* sentinel */
    for (i = l+2; i <= r; i++)
      { int j = i; Item v = a[i];
        while (less(v, a[j-1]))
          { a[j] = a[j-1]; j--; }
        a[j] = v; } }
```

::figure ins-trace

* [src/06-sorting/insertion\_sort.c](src:src/06-sorting/insertion_sort.c)

## 6.4 Bubble sort (Program 6.4)

Walk from right to left, and exchange any two neighbors that are out of order. After the first pass, the smallest item has "risen" to the front of the array like a bubble. Repeat.

```c listing="Program 6.4"
void bubble(Item a[], int l, int r)
  { int i, j;
    for (i = l; i < r; i++)
      for (j = r; j > i; j--)
        compexch(a[j-1], a[j]); }
```

Sedgewick's opinion is blunt: bubble sort is **slower** than the other two in most cases. The most famous, but not the best. You can improve it by stopping when a pass makes no exchanges, or by walking in both directions (shaker sort), but it still will not beat insertion sort.

::figure bub-trace

* [src/06-sorting/bubble\_sort.c](src:src/06-sorting/bubble_sort.c)

## 6.5 Performance in numbers

<table class="comp-table">
<tr><th>Property</th><th>Statement</th></tr>
<tr><td>6.1</td><td>Selection: about <b>N²/2 compares</b> and <b>N exchanges</b>, always.</td></tr>
<tr><td>6.2</td><td>Insertion: about <b>N²/4</b> compares and N²/4 moves on average, twice that in the worst case.</td></tr>
<tr><td>6.3</td><td>Bubble: about <b>N²/2</b> compares and N²/2 exchanges on average and in the worst case.</td></tr>
<tr><td>6.4</td><td>Insertion and bubble are <b>linear</b> if every item has only a constant number of inversions.</td></tr>
<tr><td>6.5</td><td>Insertion is <b>linear</b> if only a constant number of items have more than a constant number of inversions.</td></tr>
<tr><td>6.6</td><td>Selection is <b>linear</b> if the items are large and the keys small.</td></tr>
</table>

An **inversion** is a pair of items in the wrong order. A sorted array has zero. A reversed one has N(N−1)/2. The key point: every move in insertion sort fixes exactly one inversion, so **the number of moves = the number of inversions**.

We measured it on N = 1000:

```
sort       input       compares exch/moves
selection  random        499500        999
selection  sorted        499500        999
insertion  random        250121     249113     ← N²/4 = 250000
insertion  sorted          1997          0     ← خطي!
insertion  reverse       500498     499500     ← الأسوأ: N²/2
insertion  10 swaps        1997          8     ← Property 6.5
bubble     random        499500     247782
bubble     sorted        499500          0
```

* [src/06-sorting/elementary\_compare.c](src:src/06-sorting/elementary_compare.c)

### Property 6.6: when exchanges are expensive

If every item is a record M times the size of its key (say 1KB with a 4 byte key), a compare costs 1 and an exchange costs M. Selection: N²/2 + N·M. If M is bigger than about N, the cost of exchanges N·M dominates, and selection is the only one whose number of exchanges is N. (And the better solution: don't move the records at all; sort pointers, lesson 16.)

### Stability

```
stability on keys 2a 1b 2c 1d 2e
  insertion: 1b 1d 2a 2c 2e   (stable)
  selection: 1b 1d 2c 2a 2e   (NOT stable)
```

Selection exchanges items that are far apart, so an item can jump over an equal one. Insertion and bubble only exchange neighbors, and they stop at an equal item, so they are stable.

:::oops A mistake in insertion sort
`while (j > l && less(v, a[j-1]))`: if you swap the order of the two conditions, `a[j-1]` can become `a[-1]`. That is exactly what the sentinel removes.
:::

:::quiz
Which algorithm makes exactly the same number of compares for every input?

1. [ ] Insertion
2. [x] Selection
3. [ ] Both

> Selection always makes N(N−1)/2 compares, because its inner loop has no early exit.
:::

:::quiz
An array that is sorted except for 3 items in the wrong places. Which simple algorithm is fastest?

1. [ ] Selection
2. [x] Insertion
3. [ ] Bubble (without the improvement)

> Property 6.5: when only a few items have many inversions, insertion sort is linear.
:::

:::quiz
What is the number of moves in insertion sort equal to?

1. [ ] Always N²/2
2. [x] The number of inversions in the input
3. [ ] N − 1

> Every move of a larger neighbor fixes exactly one pair that was out of order.
:::

:::quiz
You are sorting records of 10KB each with a 4 byte key, N = 100. Which simple algorithm suits you?

1. [ ] Bubble
2. [ ] Insertion
3. [x] Selection

> Property 6.6: selection moves each record once, and moving is the expensive part here.
:::

:::quiz
What is the sentinel in Program 6.3 for?

1. [ ] It makes the sort stable
2. [x] It removes a boundary check from the inner loop
3. [ ] It halves the number of exchanges

> The smallest item in a\[l\] guarantees that less(v, a\[j-1\]) becomes false before we run off the array.
:::
