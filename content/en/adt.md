Book sections: 4.1 Abstract Objects and Collections of Objects, 4.2 Pushdown Stack ADT (Program 4.1), 4.3 Examples of Stack ADT Clients (Programs 4.2 and 4.3), 4.4 Stack ADT Implementations (Programs 4.4 and 4.5)

Chapter 4 is a turning point in the book. Up to here we asked "do I store this in an array or a list?". From here on we ask a more important question: **what operations do I need?** The storage becomes a detail behind a curtain. That is what an abstract data type is.

:::goal The goal of this lesson:
Understand an ADT as a "contract" between an interface, an implementation and a client, know the pushdown stack well, run two classic clients (evaluating postfix and converting infix to postfix), and compare the array implementation with the list implementation.
:::

## 4.1 Abstract objects and collections of objects

**Definition:** an ADT is a data type (values + operations) **that is accessed only through an interface**. Any program that uses it is called a "client", and the program that defines the operations is called an "implementation".

Three files in C:

* **The interface** `STACK.h`: only the names of the functions and their types.
* **The implementation** `STACK.c`: the real code (an array? a list? nobody knows).
* **The client** `postfix.c`: does `#include "STACK.h"` and uses the operations.

The items themselves are abstract too: Sedgewick uses an `Item.h` containing `typedef int Item;` along with `less` and `eq`. Change that line to `char *` or a struct, and the stack keeps working. This repeats throughout the book: every sorting algorithm works on `Item`.

Most important ADTs deal with **collections** of objects, and they have two basic operations: **insert** a new item, and **delete** an item. What separates one ADT from another is **which item gets removed**: the stack removes the newest, the queue removes the oldest, the priority queue removes the largest, and the symbol table removes the one with a given key.

## 4.2 The pushdown stack

LIFO: last in, first out. A pile of plates, or the "Back" button in a browser.

```c listing="Program 4.1"
/* Program 4.1: STACK.h */
void STACKinit(int);
int  STACKempty(void);
void STACKpush(Item);
Item STACKpop(void);
```

Notice: the interface never says "array" anywhere. Even `STACKinit(int)` gives the maximum size only as a **hint**; a list implementation can ignore it.

## 4.3 Stack clients

### Evaluating postfix (Program 4.2)

The expression `5 * ( ( ( 9 + 8 ) * ( 4 * 6 ) ) + 7 )` written in postfix is `5 9 8 + 4 6 * * 7 + *`. Walk from the left: a number means push. An operator means pop two, compute, and push the result.

<table>
<tr><th>What we read</th><th>The stack afterwards</th></tr>
<tr><td>5 9 8</td><td>5 9 8</td></tr>
<tr><td>+</td><td>5 17</td></tr>
<tr><td>4 6</td><td>5 17 4 6</td></tr>
<tr><td>*</td><td>5 17 24</td></tr>
<tr><td>*</td><td>5 408</td></tr>
<tr><td>7 +</td><td>5 415</td></tr>
<tr><td>*</td><td><b>2075</b></td></tr>
</table>

No parentheses and no precedence: the order of writing carries all the information. Old HP calculators, the JVM and PostScript are all built on this.

::figure postfix

### Converting infix to postfix (Program 4.3)

The book takes a **fully parenthesized** expression and does something very simple: numbers go straight to the output, operators go onto a stack, and every `)` pops the operator on top. An opening parenthesis is ignored. So it is a second stack, this one of characters.

```c listing="Program 4.3"
if (a[i] == ')')  printf("%c ", STACKpop());
if (a[i] == '+' || a[i] == '*') STACKpush(a[i]);
if (a[i] >= '0' && a[i] <= '9') printf("%c ", a[i]);
```

Our file stretches the idea a little: it handles precedence (multiplication before addition) like Dijkstra's famous shunting yard algorithm, so `5*(((9+8)*(4*6))+7)` comes out as `598+46**7+*` and evaluates to 2075.

* [src/04-adt/stack.c](src:src/04-adt/stack.c)

:::tip The two clients together make a tiny compiler
Program 4.3 converts, Program 4.2 evaluates. That is exactly a miniature compiler: parse into an intermediate form, then execute. And both use one ADT.
:::

## 4.4 Two stack implementations

```c listing="Program 4.4"
/* Program 4.4: array */             /* Program 4.5: linked list */
static Item *s; static int N;         static link head;
void STACKinit(int maxN)              void STACKinit(int maxN)
  { s = malloc(maxN*sizeof(Item));      { head = NULL; }
    N = 0; }
void STACKpush(Item item)             void STACKpush(Item item)
  { s[N++] = item; }                    { head = NEW(item, head); }
Item STACKpop(void)                   Item STACKpop(void)
  { return s[--N]; }                    { Item item = head->item; link t = head->next;
                                          free(head); head = t; return item; }
```

<table class="comp-table">
<tr><th></th><th>Array</th><th>Linked list</th></tr>
<tr><td>push / pop</td><td>O(1), faster in practice</td><td>O(1), but malloc and free every time</td></tr>
<tr><td>Space</td><td>the maximum size reserved up front</td><td>just what the items need + a pointer each</td></tr>
<tr><td>Drawback</td><td>you must know the maximum (or grow by doubling, lesson 04)</td><td>extra space for every item</td></tr>
</table>

The nice part is that the client (postfix) **did not change a single line** between the two implementations. Our file has both side by side.

:::oops Two errors the examples keep quiet about
Popping an empty stack (underflow) and pushing onto a full array (overflow). The book says plainly that it leaves them out for simplicity. In real code: either check and return an error, or document that the client is responsible.
:::

:::quiz
Which file knows that the stack is implemented with an array?

1. [ ] The client
2. [ ] The interface STACK.h
3. [x] Only the implementation STACK.c

> That is the full meaning of "abstract": the interface and the client do not know the representation.
:::

:::quiz
What is the value of the postfix expression `3 4 + 2 *`?

1. [ ] 11
2. [x] 14
3. [ ] 10

> 3 4 + gives 7, then 7 2 \* gives 14.
:::

:::quiz
In Program 4.3 (fully parenthesized), when does an operator go to the output?

1. [ ] As soon as we read it
2. [x] When we read the closing parenthesis that belongs to it
3. [ ] At the very end of the expression

> The operator waits on the stack until its second operand is finished, and the closing parenthesis announces that.
:::

:::quiz
What separates the stack ADT from the queue and the priority queue?

1. [ ] How items are inserted
2. [x] The rule for choosing which item to remove
3. [ ] The type of the items

> They all have insert and delete; the difference is "who comes out": the newest, the oldest, or the largest.
:::

:::quiz
When do you prefer the list implementation over the array?

1. [ ] When speed matters most
2. [x] When you don't know the maximum size, or you have many stacks whose sizes change
3. [ ] Always

> A list takes exactly what it holds. An array is faster but needs a ceiling or resizing.
:::
