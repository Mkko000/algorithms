Book sections: 5.4 Trees (Definitions 5.1 to 5.5, Property 5.4), 5.5 Mathematical Properties of Binary Trees (Properties 5.5 to 5.9), 5.6 Tree Traversal (Programs 5.14 to 5.16), 5.7 Recursive Binary Tree Algorithms (Programs 5.17 to 5.20)

The tree is the structure that stays with us to the end of the book: a heap is a tree, a BST is a tree, a trie is a tree, and a B tree is a tree. More important: every recursion we have written **is** a tree, the tree of calls. So if you understand trees well here, half of the book ahead gets easier.

:::goal The goal of this lesson:
Know the different kinds of trees and their recursive definitions, learn the mathematical properties of binary trees (and why they matter for analysis), walk a tree in the four ways (recursively and without recursion), and write recursive algorithms on trees: counting, height, printing, the tournament tree, and the parse tree.
:::

## 5.4 Kinds of trees

The book goes from the most specific to the most general:

* **Definition 5.1, binary tree:** either an external node (empty), or an internal node connected to an ordered pair of binary trees (left and right).
* **Definition 5.2, M ary tree:** the same idea, but an internal node has M ordered children.
* **Definition 5.3, ordered tree:** a node (the root) connected to a **sequence** of disjoint trees (called a forest). The number of children is not fixed, and there are no "external nodes".
* **Definition 5.4, rooted unordered tree:** the root is connected to a **set** of trees; the order of the children does not matter. These are the union find trees from lesson 02.
* **Definition 5.5, graph:** nodes, plus edges that connect pairs of distinct nodes. A **free tree** is a graph with no root that meets any one of four equivalent conditions: N−1 edges and no cycles; N−1 edges and connected; exactly one simple path between any two nodes; connected, but removing any edge disconnects it.

Terms that should feel natural to you: parent, child, sibling, leaf (a node with no children), the **level** of a node = its depth (the root is 0), the **height** of a tree = the largest level, and **path length** = the sum of the levels of the nodes.

### Property 5.4: every forest is a binary tree

There is a one to one correspondence between binary trees and ordered forests: make each node's left link point to its **first child**, and its right link point to **its next sibling** ("left child, right sibling"). So any tree with a varying number of children can be stored with nodes that have just two pointers.

## 5.5 Mathematical properties of binary trees

<table class="comp-table">
<tr><th>Property</th><th>Statement</th><th>Why it matters to us</th></tr>
<tr><td>5.5</td><td>N internal nodes give <b>N+1</b> external nodes</td><td>in a BST there are N+1 "places" where a search miss can end</td></tr>
<tr><td>5.6</td><td><b>2N</b> links: N−1 to internal nodes, N+1 to external nodes</td><td>almost half the pointers are NULL, which is wasted space (one motivation for Patricia, lesson 36)</td></tr>
<tr><td>5.7</td><td>external path length = internal + <b>2N</b></td><td>links the cost of a search hit to a search miss in a BST</td></tr>
<tr><td>5.8</td><td>height ≥ ⌊lg N⌋ and ≤ N−1</td><td>the best tree is balanced, and the worst is a list</td></tr>
<tr><td>5.9</td><td>internal path length ≥ N lg(N/4) and ≤ N(N−1)/2</td><td>the average search cost lies between lg N and N</td></tr>
</table>

::figure complete-tree

The proof of 5.5 by induction is lovely: an empty tree has 0 internal nodes and 1 external node. A tree with a root and two subtrees holding k and N−1−k internal nodes has (k+1) + (N−k) = N+1 external nodes. And the proof of 5.7: every time you turn an external node at depth d into an internal one, the internal length grows by d, while the external length loses d and gains two nodes at depth d+1, so it grows by d+2. The difference grows by 2 with every node.

Our file checks this on a real tree:

```
N=15 height=3  internal path=34 external path=64  (ext-int=30 = 2N)
```

## 5.6 Walking the tree

Program 5.14: a three line recursion, and the only difference between the kinds is **when you visit the node**:

```c listing="Program 5.14"
void traverse(link h, void (*visit)(link))
  { if (h == NULL) return;
    (*visit)(h);              /* preorder  : قبل الأبناء */
    traverse(h->l, visit);
    traverse(h->r, visit); }  /* inorder: بينهم · postorder: بعدهم */
```

```
preorder:    E D B A C H F G I
inorder:     A B C D E F G H I      ← BST: الـ inorder مرتّب
postorder:   A C B D G F I H E
```

::figure traversal

### With an explicit stack (Program 5.15) and with a queue (Program 5.16)

Preorder without recursion: push the root; while the stack is not empty, pop and visit, then push **the right child first** and then the left (so the left comes out first). The result is exactly the same: `preorder_nr: E D B A C H F G I`.

And the surprise: **replace the stack with a queue** in the same code, and you get level order: level by level from top to bottom. That same idea is the difference between DFS and BFS in the next lesson.

```c
void traverse(link h, void (*visit)(link))
  { QUEUEput(h);
    while (!QUEUEempty())
      { (*visit)(h = QUEUEget());
        if (h->l != NULL) QUEUEput(h->l);
        if (h->r != NULL) QUEUEput(h->r); } }
```

* [src/05-recursion-trees/tree\_traversal.c](src:src/05-recursion-trees/tree_traversal.c)

## 5.7 Recursive algorithms on binary trees

### Counting and height (Program 5.17)

```c listing="Program 5.17"
int count(link h)
  { if (h == NULL) return 0;
    return count(h->l) + count(h->r) + 1; }
int height(link h)
  { int u, v;
    if (h == NULL) return -1;
    u = height(h->l); v = height(h->r);
    if (u > v) return u+1; else return v+1; }
```

Notice `height(NULL) = -1`, so that a tree with one node has height 0.

### A quick printout (Program 5.18)

Reverse inorder (right, root, left) with indentation according to depth prints the tree rotated 90 degrees. The `*` marks are the external nodes. A great debugging tool for any tree in the book.

### The tournament tree (Program 5.19)

The divide and conquer max from lesson 11, but instead of throwing the intermediate results away, **we build a tree from them**: every node = the larger value of its two children. The root is the winner. This leads into heaps and priority queues.

```
tournament on {1 7 3 9 4 2 8 5}: winner=9
level order: 9 9 8 7 9 4 8 1 7 3 9 4 2 8 5
```

### The parse tree (Program 5.20)

The same prefix evaluation as in lesson 11, but instead of computing, we build a tree: the operator is the root, and the two operands are its children. Inorder on it gives the infix expression, and postorder gives postfix. Every compiler does this.

* [src/05-recursion-trees/binary\_tree\_algos.c](src:src/05-recursion-trees/binary_tree_algos.c)

:::oops A common mistake with height
People write `height(NULL) = 0` and then wonder why a single node tree has height 1 and the formulas don't fit. Fix your definition and stick to it: the book counts **edges** from the root.
:::

:::quiz
A binary tree has 100 internal nodes. How many external (NULL) nodes does it have?

1. [ ] 100
2. [x] 101
3. [ ] 200

> Property 5.5: always N+1.
:::

:::quiz
The internal path length of a tree with 10 nodes is 19. What is the external path length?

1. [ ] 29
2. [x] 39
3. [ ] 21

> Property 5.7: external = internal + 2N = 19 + 20.
:::

:::quiz
You replaced the stack in the non recursive preorder with a queue. What comes out?

1. [ ] inorder
2. [ ] postorder
3. [x] level order

> The queue finishes each level before the next one.
:::

:::quiz
How do you store a tree whose nodes have a varying number of children using just two pointers per node?

1. [ ] It is impossible
2. [x] The left pointer to the first child, and the right pointer to the next sibling
3. [ ] An array of children in each node

> Property 5.4: the correspondence between forests and binary trees.
:::

:::quiz
Which traversal of an expression parse tree produces postfix?

1. [ ] preorder
2. [ ] inorder
3. [x] postorder

> The operands first (the two subtrees), and the operator after them.
:::
