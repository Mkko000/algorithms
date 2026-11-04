Book sections: 3.6 Strings (Table 3.2, Programs 3.15 and 3.17), 3.7 Compound Data Structures (Programs 3.16 and 3.18 to 3.20)

So far we have arrays and lists. This lesson combines them: a string is an array of characters with special rules, and a graph is an array of lists or an array of arrays. Most of the "real" data structures in the world are combinations like these.

:::goal The goal of this lesson:
Understand strings in C properly (and the `strlen` trap), sort strings by sorting pointers, allocate a two dimensional array dynamically, represent a graph in two ways, and see how a "grid of lists" turns an N² problem into an almost linear one.
:::

## 3.6 Strings

In C a string is an array of `char` that ends with `'\0'`. No length is stored; to know the length you have to walk until you reach the zero. Table 3.2 in the book writes the library functions in two ways, and it is an exercise worth doing:

```c
/* بالفهارس */                         /* بالمؤشرات */
for (i = 0; a[i] != 0; i++) ;            b = a; while (*b++) ;
return i;               /* strlen */     return b - a - 1;

for (i = 0; (a[i] = b[i]) != 0; i++) ;   while (*a++ = *b++) ;   /* strcpy */

for (i = 0; a[i] == b[i]; i++)           /* strcmp */
  if (a[i] == 0) return 0;
return a[i] - b[i];
```

:::oops The strlen trap
`for (i = 0; i < strlen(a); i++)` looks innocent, but `strlen` walks the whole string **on every iteration**. The linear loop just became N². On a string of a million characters that is the difference between a millisecond and minutes. Compute the length once before the loop, or walk until `'\0'`.
:::

* [src/03-data-structures/strings.c](src:src/03-data-structures/strings.c)

### Searching for a pattern (Program 3.15)

Brute force search: for every position i in the text, try to match the pattern character by character. The worst case is N·M (a text like AAAA…AB and a pattern AAAB). In practice it usually stops early. The strings volume of Sedgewick's series gives smarter methods (KMP, Boyer and Moore); here what matters is understanding the basics.

* [src/03-data-structures/string\_search.c](src:src/03-data-structures/string_search.c)

### Sorting strings (Program 3.17)

The clever trick: read all the words into **one big buffer**, keep an array of **pointers** to the start of each word, and sort the pointers with `qsort` and a compare function that calls `strcmp`. The strings themselves never move; we move pointers that are 8 bytes each. It is the same "pointer sort" idea we will go deeper into in lesson 16.

```c listing="Program 3.17"
int compare(const void *i, const void *j)
  { return strcmp(*(char **)i, *(char **)j); }
...
qsort(a, N, sizeof(char *), compare);
```

## 3.7 Compound structures

### A dynamic two dimensional array (Program 3.16)

Want an r×c array whose size is decided at run time? Allocate an array of r pointers, each pointing to a row of c items. After that, `t[i][j]` works as usual.

```c listing="Program 3.16"
int **malloc2d(int r, int c)
  { int i; int **t = malloc(r * sizeof(int *));
    for (i = 0; i < r; i++) t[i] = malloc(c * sizeof(int));
    return t; }
```

### Graphs: adjacency matrix against adjacency lists (Programs 3.18 and 3.19)

A graph is V vertices and E edges. Two representations:

<table>
<tr><th></th><th>Adjacency Matrix</th><th>Adjacency Lists</th></tr>
<tr><td>Shape</td><td><code>adj[i][j] = 1</code> if there is an edge</td><td><code>adj[i]</code> = the list of neighbors of i</td></tr>
<tr><td>Space</td><td>V²</td><td>V + E</td></tr>
<tr><td>"Is there an edge u-w?"</td><td>O(1)</td><td>O(degree of u)</td></tr>
<tr><td>"Visit the neighbors of u"</td><td>always V</td><td>only the degree of u</td></tr>
<tr><td>When?</td><td>dense graph or small V</td><td>sparse graph (usually the real world)</td></tr>
</table>

In an undirected graph every edge is recorded twice (at i and at j). And in the lists, the order of the neighbors depends on the order of insertion, which affects the order in which DFS visits vertices (lesson 14).

* [src/03-data-structures/graph.c](src:src/03-data-structures/graph.c)

### A two dimensional array of lists: closest points again (Program 3.20)

Back to the problem "how many pairs of points are closer than d". The idea: divide the square into a G×G grid of cells, each with side d, and give each cell a **list** of the points that fall into it. Any two points closer than d must be in the same cell or a neighboring one. So each new point is compared only with the points in 9 cells.

```
$ ./point_closest 20000 0.005
  brute force: 15808 pairs closer than d, 199990000 distance computations
  grid lists : 15808 pairs closer than d,     44772 distance computations
```

The same answer with **4000 times** fewer calculations. And this is not a new algorithm; it is a smarter data structure. A small trick: we allocate an extra row and column around the edges so we never have to write boundary checks.

* [src/03-data-structures/point\_closest.c](src:src/03-data-structures/point_closest.c)

:::tip The general rule
Whenever you find yourself comparing "everything with everything", ask: is there a structure that lets me compare only with the things that could matter? The grid here, and the BST and hashing later, are all answers to that question.
:::

:::quiz
Why can `for (i=0; i<strlen(a); i++)` become N²?

1. [ ] Because there are many characters
2. [x] Because strlen is recomputed on every iteration and walks the whole string
3. [ ] Because C does not know the length of a string

> C really does not store the length, so every call to strlen is a loop. Compute it once.
:::

:::quiz
In Program 3.17, what moves during the sort?

1. [ ] The characters themselves
2. [x] Only the pointers to the words; the words stay put in the buffer
3. [ ] The whole buffer gets copied

> Exchanging two pointers is much cheaper than exchanging two long strings.
:::

:::quiz
A graph with V=100000 and E=200000. Which do you prefer?

1. [ ] An adjacency matrix
2. [x] Adjacency lists
3. [ ] They are the same

> The matrix has 10¹⁰ entries. The lists have ≈ V + 2E ≈ half a million nodes.
:::

:::quiz
In the grid of lists (Program 3.20), what is each point compared with?

1. [ ] Every point
2. [x] Only the points in its own cell and the 8 cells around it
3. [ ] The points in the same row

> Because the side of a cell = d, any point closer than d must be in a neighboring cell.
:::
