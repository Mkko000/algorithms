Solved exercises, chapter 4 (Abstract Data Types), exercises 4.6 to 4.49

The exercises in chapter 4 come in two kinds. The "trace it by hand" kind: stacks, queues and deques, and what comes out of a sequence of operations. And the "design an ADT" kind: two stacks in one array, a random queue, a stack that refuses duplicates. In between are the postfix exercises, which are the best part of the chapter.

:::goal The goal of this lesson:
Be able to trace any sequence of operations on a stack, queue or deque without mistakes, know how to decide when a particular order is "impossible", and design a small ADT that meets a specific performance requirement.
:::

:::tip Before you start
Take a sheet of paper and make a table: one column for the operation and one for the contents of the stack after it. In tracing exercises like these, the table prevents 90% of the mistakes.
:::

* [src/04-adt/exercises/ex4\_06\_sequences.c](src:src/04-adt/exercises/ex4_06_sequences.c)
* [src/04-adt/exercises/ex4\_09\_expressions.c](src:src/04-adt/exercises/ex4_09_expressions.c)
* [src/04-adt/exercises/ex4\_24\_adts.c](src:src/04-adt/exercises/ex4_24_adts.c)

### Exercises 4.6 and 4.31: the same sequence on a stack and on a queue

:::problem
A letter means push and an asterisk means pop in the sequence `E A S * Y * Q U E * * * S T * * * I O * N * * *`. What values do the pops return? (4.31) The same question for a FIFO queue (a letter means put and an asterisk means get).
:::

:::think
On the stack, every asterisk pops the **last** letter that went in and is still there. On the queue it removes the **oldest** letter. And one observation saves time: on the queue, the output order is exactly the input order, wherever the asterisks are (as long as there is no get from an empty queue). So the answer to 4.31 is just the letters in order!
:::

:::solution
```
4.6  stack pops: S Y E U Q T S A O N I E
4.31 queue gets: E A S Y Q U E S T I O N
```

Tracing the first part on the stack: E A S, then `*` pops S. Y, then `*` pops Y. Q U E, then `* * *` pop E U Q. What is left on the stack: E A. S T, then `* * *` pop T S A. And so on. The queue gives "EASYQUESTION". The question was built to spell a word.
:::

### Exercises 4.7 and 4.8: possible or impossible order?

:::problem
With the convention of 4.6, put asterisks into `E A S Y` so that the pops return (i) E A S Y, (ii) Y S A E, (iii) A S Y E, (iv) A Y E S, or prove that there is no way. (4.8) Give an algorithm that decides, for any two strings, whether asterisks can be put into the first one so that it produces the second.
:::

:::think
There is no real choice here. If the letter on top is the letter you need now, you must pop it (if you postpone it, it gets buried under other letters and cannot come out before them). If it is not, you must push the next letter. So it is **greedy**: and if the input runs out while the top is not what you need, it is impossible. That is also the solution to 4.8, in O(N) time.
:::

:::solution
```
4.7  EASY -> EASY : E*A*S*Y*
4.7  EASY -> YSAE : EASY****
4.7  EASY -> ASYE : EA*S*Y**
4.7  EASY -> AYES : impossible
```

Why is (iv) impossible? For A to come out first, E must be under A. Then, for Y to come out, S must go in first (S comes before Y in the input), so the stack holds E S Y. Y comes out, and the top is now S. But we need E, and E is under S. ∎ The general rule: if a, b and c go in in that order, they can never come out as c then a then b (the "3 1 2" pattern is forbidden). In (iv), Y, E and S are 4, 1 and 3, which is exactly that pattern.
:::

### Exercises 4.34 and 4.35: the deque

:::problem
An uppercase letter means put at the front, a lowercase letter means put at the end, `+` means get from the front, and `*` means get from the end, in `E A s + Y + Q U E * * + s t + * + I O * n + + *`. What comes out? (4.35) Put + and \* into `E a s Y` to produce (i) E s a Y, (ii) Y a s E, (iii) a Y s E, (iv) a s Y E, or prove that it is impossible.
:::

:::think
For the trace: write the deque as a horizontal row, and know which end each letter goes to. For 4.35 greedy is not enough, because there are now two choices for a get (front or end). So we **search** (DFS): at each step either put the next letter, or get from either end if that produces exactly the letter needed. That last condition cuts off nearly every branch, so in practice the search is very fast. That is the solution to 4.36.
:::

:::solution
```
4.34 deque gets: A Y s E E U t Q s O I n
4.35 EasY -> EsaY : E+as*+Y+
4.35 EasY -> YasE : impossible
4.35 EasY -> aYsE : Ea*sY+*+
4.35 EasY -> asYE : Ea*s*Y++
```

Why is (ii) impossible? Y is the last letter in the input, so for it to come out first, E, a and s must all be inside. The deque then holds E a s (E at the front, a and s added at the end). After Y we need a, and a is in the middle between E and s, while you can only reach the ends. ∎
:::

### Exercises 4.9 and 4.10: postfix

:::problem
(4.9) Convert `( 5 * ( ( 9 * 8 ) + ( 7 * ( 4 + 6 ) ) ) )` to postfix. (4.10) Like Figure 4.2, show the contents of the stack as Program 4.2 evaluates `5 9 * 8 7 4 6 + * 2 1 3 * + * + *`.
:::

:::think
Program 4.3 is very simple: a number goes straight out, an operator goes onto a stack, and every `)` pops an operator. So an operator comes out when its parenthesis closes. For evaluation: a number is pushed, and an operator takes the top two and pushes the result.
:::

:::solution
```
4.9  postfix: 5 9 8 * 7 4 6 + * + *   = 710
4.10   5 : 5            *  : 45 8 70
       9 : 5 9          2  : 45 8 70 2
       * : 45           1  : 45 8 70 2 1
       8 : 45 8         3  : 45 8 70 2 1 3
       7 : 45 8 7       *  : 45 8 70 2 3
       4 : 45 8 7 4     +  : 45 8 70 5
       6 : 45 8 7 4 6   *  : 45 8 350
       + : 45 8 7 10    +  : 45 358
                        *  : 16110
```

Notice that the stack only reached a depth of 6. The maximum depth is the largest number of "waiting results" at the same time.
:::

### Exercises 4.11, 4.12 and 4.16: subtraction, division and sqrt, with two stacks

:::problem
Add `-` and `/` (4.11), then unary minus and `$` (square root) with floating point (4.12). The example `(-(-1) + $((-1) * (-1)-(4 * (-1))))/2` must print **1.618034**. (4.16) Combine Programs 4.2 and 4.3 into one module with a stack of numbers and a stack of operators.
:::

:::think
The example is not fully parenthesized, so Program 4.3 alone is not enough. You need **precedence**: before pushing an operator, apply everything on the stack whose precedence is ≥ its own. The hard part: how do you tell subtraction `-` from negation `-`? Look at what came before it: if you were expecting an operand (at the start, after `(` or after an operator), it is unary. Store it as a different symbol (`n`) with a higher precedence.
:::

:::solution
```c
if (c == '-' && expectOperand) c = 'n';         /* unary minus */
if (c == 'n' || c == '$') { ops[no++] = c; continue; }
while (no && ops[no-1] != '(' && prec(ops[no-1]) >= prec(c)) apply();
ops[no++] = c;
```

```
4.11 (8 - 2) / 3 - 1 = 1
4.12 (-(-1) + $((-1) * (-1) - (4 * (-1)))) / 2 = 1.618034
4.16 5 * (((9 + 8) * (4 * 6)) + 7) = 2075
```

1\.618034 is the golden ratio (1+√5)/2, and the example was built to produce it. And the `>=` condition is what makes `8 - 2 - 1` evaluate from left to right: 5, not 7.
:::

### Exercise 4.15: from postfix to infix

:::problem
Write a program that converts postfix to infix using a pushdown stack.
:::

:::think
Exactly Program 4.2, except the stack holds **strings** instead of numbers. And instead of adding, an operator glues: "(" + left + op + right + ")".
:::

:::solution
```
4.15 infix: ((5 * 9) * (8 + ((7 * (4 + 6)) * (2 + (1 * 3)))))
```

The result is fully parenthesized, which is exactly the input Program 4.3 accepts. So the two programs are inverses of each other.
:::

### Exercises 4.24 and 4.19: two stacks in one array

:::problem
Build an ADT that gives the client two pushdown stacks in one array: one starts at the beginning of the array, and the other at its end. (4.19) And replace "test if empty" with `count`.
:::

:::think
Stack 0 grows to the right from 0, and stack 1 grows to the left from MAX. They are full only when the two meet (`top0 == top1`). So if one is big and the other small, the whole space gets used. That would be impossible if you split the array into two halves.
:::

:::solution
```c
int push(int s, int v)
  { if (top0 == top1) return 0;                  /* full */
    if (s == 0) A[top0++] = v; else A[--top1] = v;
    return 1; }
int count(int s) { return s == 0 ? top0 : MAX - top1; }
```

```
4.24 pushed 8 of 10 into an array of 8: count0=5 count1=3 (full)
     pop0=4 pop1=102 -> count0=4 count1=2
```
:::

### Exercises 4.28 and 4.42: the size of a connected set, and the random queue

:::problem
(4.28) Modify the union find ADT to return the number of nodes connected to a given node. (4.42) Build a random queue ADT with an array, where get removes a random item and every operation takes constant time.
:::

:::think
4\.28: weighted union already keeps `sz[root]`! So the answer is `sz[find(p)]` for free. 4.42: removing from the middle of an array costs O(N) if you move what comes after it. The trick: order does not matter at all in a random queue, so exchange the chosen item with the last one, and remove the last.
:::

:::solution
```c
int compsize(int p) { return sz[find(p)]; }
int rqget(void) { int i = rand() % n, v = q[i]; q[i] = q[--n]; return v; }
```

```
4.28 after the pairs of Figure 1.1: size(0)=10 size(7)=10 size(10)=1
4.42 first get from a random queue of 0..4 (100000 trials): 0:20135 1:20086 2:19998 3:19815 4:19966
```

Each value came out about 20% of the time, so the choice really is uniform.
:::

### Exercises 4.48 and 4.49: a stack without duplicates

:::problem
Modify the array stack so it does not allow duplicates: (4.48) with the "ignore the new" policy, and (4.49) with the "forget the old" policy. Use brute force.
:::

:::think
Both search for the value on the stack (O(N)). The difference: "ignore the new" does nothing if it finds it. "Forget the old" removes the old copy from its place (sliding down what is above it) and puts the new one on top. So the second one lets the latest use decide the position, which is exactly the behavior of an LRU cache.
:::

:::solution
```
4.48 ignore new, push 1 2 3 2 4 1 5: 1 2 3 4 5   (top = right)
4.49 forget old, push 1 2 3 2 4 1 5: 3 2 4 1 5   (top = right)
```

In 4.51 the book asks for an O(1) version for items from 0 to M−1. The idea: an array `where[v]` holding v's position on the stack, or −1 if it is not there. Key indexed again.
:::

:::quiz
You push 1 2 3 in order (with pops at any time). Which pop order is impossible?

1. [ ] 3 2 1
2. [ ] 2 3 1
3. [x] 3 1 2
4. [ ] 1 3 2

> For 3 to come out first, 1 and 2 must be inside with 2 on top of 1. So after 3 must come 2, not 1.
:::

:::quiz
Why can a random queue do get in O(1) while removing from the middle of an ordinary stack cannot?

1. [ ] Because random is faster
2. [x] Because order inside a random queue does not matter, so we exchange the chosen item with the last one and remove it
3. [ ] Because it uses a linked list

> When order does not matter, exchanging with the last item saves sliding N items.
:::

:::quiz
In the infix evaluator, why do we apply operators when the precedence is ≥ (not just \>)?

1. [ ] For speed
2. [x] So that operators of equal precedence are evaluated from left to right (8−2−1 = 5)
3. [ ] It is unnecessary

> With \> alone, 8−2−1 would be computed as 8−(2−1) = 7, which is wrong.
:::
