Book sections: 4.8 First Class ADTs (Programs 4.13 to 4.20), 4.9 Application Based ADT Example (Programs 4.21 to 4.23), 4.10 Perspective

Every ADT we have built so far has a hidden problem: there is **only one stack** in the whole program, because the data lives in static variables inside the implementation file. So what if you need 10 queues? Or lots of complex numbers? Here we need a "first class" ADT: a type you can make many copies of, exactly like `int`.

:::goal The goal of this lesson:
Tell an ordinary ADT from a first class ADT, learn the "pointer to an incomplete struct" trick that hides the representation in C, run a simulation with many queues, build an ADT for polynomials, and understand why the book treats this chapter as the foundation of everything that follows.
:::

## 4.8 First class ADTs

**Definition:** a first class data type is a type you can create many variables of, pass to functions, and return from functions. `int` and `double` are first class. The stack from lesson 08 is not.

### The first example: complex numbers

Program 4.13 is a simple client: it computes the N roots of unity and checks that every root raised to the power N gives 1. The first implementation (4.14 and 4.15) is an exposed struct:

```c listing="Program 4.13"
typedef struct { float Re; float Im; } Complex;
Complex COMPLEXmult(Complex a, Complex b);
```

It works and it is fast, but the client can see `Re` and `Im` and can touch them. If tomorrow you want to store the polar form (r, θ), every client that touched the fields will break.

The second implementation (4.16 and 4.17) uses **the key trick of the chapter**:

```c
/* COMPLEX.h — the client sees only this */
typedef struct complex *Complex;    /* pointer to an incomplete type */
Complex COMPLEXinit(float, float);
float   Re(Complex);  float Im(Complex);
Complex COMPLEXmult(Complex, Complex);

/* COMPLEX.c — only here is the struct defined */
struct complex { float Re; float Im; };
```

The client holds a pointer to something whose insides it does not know. The compiler itself will stop it from writing `c->Re`. That is the closest thing in C to "private" in newer languages.

```
8 roots of unity (struct version):
 1  0.707  0.707   ^8 =  1.000  0.000
 3 -0.707  0.707   ^8 =  1.000  0.000 ...
first-class version, root 1 ^ 8:  1.000  0.000
```

* [src/04-adt/complex.c](src:src/04-adt/complex.c)

:::oops The price: who calls free?
Every `COMPLEXmult` returns a new object allocated with malloc. In an expression like `mult(mult(a,b),c)` nobody holds on to the intermediate result, so it leaks. The book admits that this is the big weakness of first class ADTs in C: you need an interface with a `destroy`, and the client has to stick to it. Languages with garbage collection take that worry off your hands.
:::

### The second example: many queues (Programs 4.18 to 4.20)

The same trick applied to the FIFO queue: `QUEUEinit` now **returns** a `Q`, and every operation takes the Q as its first argument.

```c
Q    QUEUEinit(int maxN);
void QUEUEput(Q, Item);
Item QUEUEget(Q);
```

The client (Program 4.19) simulates M tellers in a bank: each customer joins a random queue, and every so often a random teller serves a customer. That was impossible with the old queue.

```
served  5 from line 0
served  6 from line 2
line 0: 8 12 16 18
line 1: 1 3 4 7 10 17 19
line 2: 9 14
line 3: 0 2 11 13 15
```

* [src/04-adt/queue\_firstclass.c](src:src/04-adt/queue_firstclass.c)

## 4.9 An ADT from an application: polynomials

The book closes with a bigger example: you want to compute `(x+1)^N` and print the coefficients (which are the binomial coefficients, Pascal's triangle). The client (Program 4.21) speaks only the language of math:

```c listing="Program 4.21"
t = POLYterm(1, 1); x = POLYadd(t, POLYterm(1, 0));   /* x+1 */
for (i = 1, y = x; i < N; i++) y = POLYmult(x, y);
```

And the implementation (4.23) is an array of coefficients: `a[i]` = the coefficient of `x^i`. Addition is O(N), and multiplication is O(N²): every term in the first is multiplied by every term in the second. Evaluation uses Horner's method in `POLYeval`:

```c listing="Program 4.21"
for (t = 0.0, i = p->N; i >= 0; i--) t = t*x + p->a[i];
```

The book mentions an alternative: if the polynomial is "sparse" (like `x^1000000 + 1`), the array wastes an enormous amount of space, and a linked list holding only the nonzero terms is better. The same interface, two implementations, choose according to use. That is exactly the message of the chapter.

```
product: +8.0 x^2 +10.0 x^1 +3.0 x^0
eval at 2: 55.0
(x+1)^10 coefficients: 1 10 45 120 210 252 210 120 45 10 1
(x+1)^10 at x=1: 1024 (= 2^10)
```

* [src/04-adt/polynomial.c](src:src/04-adt/polynomial.c)

## 4.10 The big picture

Why does this chapter matter so much? Because the rest of the book follows this shape: **an interface, then clients, then competing implementations**.

* Sorting (chapters 6 to 11): the interface `sort(Item a[], l, r)` and `less`, with 10 implementations.
* The priority queue (chapter 9): the interface insert and delmax, with implementations from a list to a heap to a binomial queue.
* The symbol table (chapters 12 to 16): the interface insert and search, with implementations from an array to a BST to hashing to a B tree.

:::tip The takeaway
When you meet a problem, first write the interface the client needs. Then you can compare algorithms on common ground and swap between them without fear.
:::

:::quiz
What makes an ADT "first class"?

1. [ ] That it is fast
2. [x] That you can make many copies of it and pass and return them like any type
3. [ ] That it is written in a file of its own

> The stack from lesson 08 was a single global copy, so it is not first class.
:::

:::quiz
`typedef struct complex *Complex;` in the header, without defining the struct. What is the benefit?

1. [ ] It saves memory
2. [x] The client cannot reach the fields, so you can change the representation without breaking it
3. [ ] It speeds up multiplication

> An incomplete struct means the compiler forbids any `->` outside the implementation file.
:::

:::quiz
What is the biggest practical problem with first class ADTs in C?

1. [ ] Slow functions
2. [x] Memory management: intermediate objects that nobody frees
3. [ ] They do not work with pointers

> Every operation returns a new object made with malloc; you need a destroy function and discipline from the client.
:::

:::quiz
The polynomial `x^1000000 + 1`. What is the best representation?

1. [ ] An array of coefficients
2. [x] A list of only the nonzero terms
3. [ ] It makes no difference

> The array would allocate a million slots for just two terms.
:::
