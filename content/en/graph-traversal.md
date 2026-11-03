Book sections: 5.8 Graph Traversal (Programs 5.21 and 5.22, Property 5.10), 5.9 Perspective

Imagine you are in a maze with a ball of string and a piece of chalk. Every time you enter a new passage you unroll the string, and you mark every room you reach. If you arrive at a marked room, you go back. When all the passages are used up, you roll up the string and return to the previous room. That is Trémaux's method from the 19th century, and it is exactly **depth first search**.

:::goal The goal of this lesson:
Write DFS recursively and with a stack, and BFS with a queue, know why both take V+E time, use BFS for the shortest path by number of edges, solve the connectivity problem from chapter 1 with DFS, and understand when union find is better.
:::

## 5.8 Walking the graph

We will use the book's graph (8 vertices, 10 edges) with adjacency lists from lesson 07:

```
  0: 2 5 7       4: 3 5 6 7
  1: 7           5: 0 3 4
  2: 0 6         6: 2 4
  3: 4 5         7: 0 1 4
```

### Recursive DFS (Program 5.21)

```c listing="Program 5.21"
void traverse(int k, void (*visit)(int))
  { link t;
    (*visit)(k); visited[k] = 1;
    for (t = adj[k]; t != NULL; t = t->next)
      if (!visited[t->v]) traverse(t->v, visit); }
```

The only difference from preorder on a tree (lesson 13): a `visited` array. A tree has no cycles, so it does not need one; a graph does, and without it we would loop forever. If the graph itself is a tree, DFS from the root **is** preorder. And the edges we walked to reach new vertices form a **spanning tree** of the graph.

```
recursive DFS from 0     : 0 2 6 4 3 5 7 1
  list nodes touched = 20 = 2E (E=10); plus V=8 heads -> V+E work
```

:::tip Property 5.10
DFS with adjacency lists takes time ∝ **V + E**: each vertex has a list head we look at once, and each edge has two nodes in the lists (in an undirected graph), each touched exactly once. And with an adjacency matrix? V², because for each vertex you have to scan a whole row.
:::

::figure dfs

### DFS with an explicit stack

The simple way: pop a vertex, and if it has not been visited, visit it and push all its neighbors. For the order to match the recursive one, you have to push the neighbors **in reverse**. The problem the book points out: the same vertex can be put on the stack several times. The fix? The **forget the old item** policy from lesson 09: the copy on top is the one that will be visited first, so the old one is useless.

```
stack DFS (push all)     : 0 2 6 4 3 5 7 1   (max stack 5)
```

### BFS (Program 5.22)

Replace the stack with a queue. Now you visit all the neighbors of the start first, then their neighbors, and so on, like a wave spreading out from a stone dropped in water.

```c listing="Program 5.22"
void traverse(int k, void (*visit)(int))
  { link t;
    QUEUEinit(V); QUEUEput(k);
    while (!QUEUEempty())
      if (visited[k = QUEUEget()] == 0)
        { (*visit)(k); visited[k] = 1;
          for (t = adj[k]; t != NULL; t = t->next)
            if (visited[t->v] == 0) QUEUEput(t->v); } }
```

```
BFS, duplicates allowed  : 0 2 5 7 6 3 4 1   (max queue 5)
BFS, ignore-the-new-item : 0 2 5 7 6 3 4 1   (max queue 4)
edges from 0 (shortest)  : 0:0 1:2 2:1 3:2 4:2 5:1 6:2 7:1
```

Here the opposite policy applies: **ignore the new item**. If a vertex is already in the queue, don't add it again (the older copy will come out first). The same order with a smaller queue.

And the big advantage of BFS: it visits vertices in order of **distance** (number of edges) from the start. So it gives you the shortest path in an unweighted graph. DFS does not guarantee that at all; it may reach a direct neighbor by way of a long detour.

::figure bfs

* [src/05-recursion-trees/dfs\_bfs.c](src:src/05-recursion-trees/dfs_bfs.c)

### Back to the problem from chapter 1

DFS solves the connectivity problem in linear time: loop over all the vertices, and every vertex not yet visited starts a new component.

```
components: {0 2 6 4 3 5 7 1 } {8 9 }  -> 2 components
```

So why did we study union find at all? The book answers: DFS needs **the whole graph in memory** (space ∝ E), while union find needs space ∝ V only and works "online" on the edges as they arrive one by one. For a huge graph whose edges come from a stream, union find is still the better choice.

<table class="comp-table">
<tr><th></th><th>DFS</th><th>BFS</th></tr>
<tr><td>Structure</td><td>stack (or recursion)</td><td>queue</td></tr>
<tr><td>Shape of the search</td><td>goes as deep as it can, then backs up</td><td>a wave, level by level</td></tr>
<tr><td>Time (lists)</td><td>V + E</td><td>V + E</td></tr>
<tr><td>Shortest path by edges?</td><td>no</td><td>yes</td></tr>
<tr><td>Duplicate policy</td><td>forget the old</td><td>ignore the new</td></tr>
</table>

:::problem
DFS and BFS are exactly the same algorithm with a different generalized queue (lesson 09). If you use a random queue you get a random exploration. And with a priority queue whose priorities are distances, you get Dijkstra and Prim, which belong to the second volume of the series (Graph Algorithms).
:::

:::oops A practical warning
Recursive DFS on a graph with a million vertices in one long chain means a recursion depth of a million, which means a stack overflow. In production, write it with an explicit stack.
:::

## 5.9 The big picture

Chapter 5 is the cornerstone: recursion and trees are two sides of one coin. The tree of calls explains the running time of any recursive algorithm. And in everything to come (quicksort, mergesort, heaps, BSTs, tries) you will find yourself asking the same two questions: **what does the tree look like?** and **how tall is it?**. The book also stresses that any recursion can be removed with an explicit stack, and that this sometimes matters for performance, as we will do in quicksort (lesson 17).

:::quiz
What separates DFS on a graph from preorder on a tree?

1. [ ] DFS uses a queue
2. [x] A visited array, because of cycles
3. [ ] Preorder is slower

> A tree has no cycles, so we can never come back to a node we have seen.
:::

:::quiz
A graph with V = 1000 and E = 3000. Roughly how much work does DFS with adjacency lists do?

1. [ ] 10⁶
2. [x] About V + 2E ≈ 7000
3. [ ] Only 3000

> Every edge is in two lists. With an adjacency matrix it would be V² = a million.
:::

:::quiz
You want the fewest steps from one vertex to another in an unweighted graph. What do you use?

1. [ ] DFS
2. [x] BFS
3. [ ] They are the same

> BFS visits in order of distance, so the first time it reaches a vertex is along a shortest path.
:::

:::quiz
Edges arrive from a huge stream and you cannot store them all. You want to know about connectivity. What fits best?

1. [ ] DFS
2. [ ] BFS
3. [x] Union find

> Union find needs space ∝ V and processes each edge and throws it away. DFS and BFS need the whole graph.
:::
