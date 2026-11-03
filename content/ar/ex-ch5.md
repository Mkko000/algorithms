تمارين محلولة · الفصل 5 (Recursion and Trees) · تمارين 5.1–5.90

الفصل الخامس أكبر فصل في الجزء الأول، وتمارينه فيها كل حاجة: recursion عادي، وdivide-and-conquer، وdynamic programming، وخصائص الشجر. اخترنا منهم اللي بيعلّمك حاجة: إمتى الـ recursion بيعمق جدًا، وإزاي تحوّل recursion لـ formula، وإزاي تحسب حاجة على شجرة في لفّة واحدة.

:::goal هدف الدرس:
تكتب recursive functions صح من أول مرة (base case، والمشكلة بتصغر)، تحوّل الـ recursion الأُسّي لـ DP، وتثبت وتقيس خصائص الشجر.
:::

:::tip سؤالين قبل أي recursion
(1) إيه أصغر حالة، وإجابتها إيه؟ (2) هل كل استدعاء بيقرّب للحالة دي فعلًا؟ لو مش متأكد من التاني، يبقى عندك infinite recursion مستخبّي. وده بالظبط موضوع تمرين 5.40 تحت.
:::

* [src/05-recursion-trees/exercises/ex5\_01\_recursion.c](src:src/05-recursion-trees/exercises/ex5_01_recursion.c)
* [src/05-recursion-trees/exercises/ex5\_22\_ruler\_hanoi.c](src:src/05-recursion-trees/exercises/ex5_22_ruler_hanoi.c)
* [src/05-recursion-trees/exercises/ex5\_37\_dp.c](src:src/05-recursion-trees/exercises/ex5_37_dp.c)
* [src/05-recursion-trees/exercises/ex5\_86\_trees.c](src:src/05-recursion-trees/exercises/ex5_86_trees.c)

### تمرين 5.1 و5.2: lg(N!) وN! mod M

:::problem
(5.1) اكتب برنامج recursive يحسب lg(N!). (5.2) عدّل Program 5.1 عشان يحسب N! mod M من غير overflow. جرّبه بـ M = 997 وN = 10³…10⁶، عشان تشوف نظامك بيتعامل إزاي مع الـ recursion العميق.
:::

:::think
lg(N!) = lg N + lg((N−1)!)، يعني نفس شكل Program 5.1 بس بنجمع لوغاريتمات بدل ما نضرب، فمفيش overflow. وفي mod: (a·b) mod M = ((a mod M)·(b mod M)) mod M، فخلّي كل نتيجة وسطانية أقل من M. بس قبل ما تشغّل، فكّر: N! لأي N ≥ 997 فيها 997 كعامل. يعني الإجابة 0! التمرين الحقيقي هنا هو عمق الـ recursion.
:::

:::solution
```c
double lgfact(int n) { return n <= 1 ? 0 : log2(n) + lgfact(n-1); }
int factmod(int n, int m)
  { return n == 0 ? 1 % m : (long)n % m * factmod(n-1, m) % m; }
```

```
5.1 lg(10!) = 21.7911 (log2(3628800) = 21.7911), lg(100!) = 524.8
5.2   1000! mod 997 =   0  (recursion depth 1001)
5.2 100000! mod 997 =   0  (recursion depth 100001)
    996! mod 997 = 996 by Wilson's theorem = p-1
```

عمق 100 ألف اشتغل عادي، لأن كل frame صغير. بس عند المليون، على كتير من الأنظمة (stack حجمه 8MB) البرنامج هيقع بـ stack overflow. ودي النقطة اللي سادجوك عايزك تشوفها: recursion عمقه N خطر لـ N كبير، حتى لو الخوارزمية صح. (والأمان إن الـ divide-and-conquer عمقه lg N بس.)
:::

### تمرين 5.3 و5.4: الـ puzzle (3N+1)

:::problem
(5.3) اكتب تسلسل قيم الـ argument لما Program 5.2 بيتنادى لكل رقم من 1 لـ 9. (5.4) لاقي الـ N \< 10⁶ اللي Program 5.2 بيعمل فيه أكبر عدد استدعاءات.
:::

:::think
Program 5.2: لو N زوجي نادي على N/2، ولو فردي نادي على 3N+1. ومحدش أثبت إنه بيخلص لكل N (دي Collatz conjecture)! فمفيش formula. الحل الوحيد إنك تجرّب المليون كلهم. وده سريع، لأن متوسط عدد الخطوات حوالي 100.
:::

:::solution
```
5.3 N=3: 3 10 5 16 8 4 2 1   (8 calls)
5.3 N=7: 7 22 11 34 17 52 26 13 40 20 10 5 16 8 4 2 1   (17 calls)
5.3 N=9: 9 28 14 7 22 11 34 17 52 26 13 40 20 10 5 16 8 4 2 1   (20 calls)
5.4 N < 10^6 with the most calls: N = 837799, 525 calls
```

لاحظ إن 9 بتعدّي على 7، و7 بتعدّي على 5. فلو حفظت النتايج (DP) كنت هتوفّر كتير. وخد بالك: 3N+1 لـ N قريب من المليون بيعدّي حدود الـ int (32 bit) أحيانًا، فاستخدمنا `long`.
:::

### تمرين 5.5 و5.7: Euclid من غير recursion، وعمقه على Fibonacci

:::problem
(5.5) اكتب Euclid من غير recursion. (5.7) اديني عمق الـ recursion في Euclid لما الـ input رقمين Fibonacci ورا بعض (F<sub>N</sub> وF<sub>N+1</sub>).
:::

:::think
Program 5.3 فيه **tail recursion**: الاستدعاء آخر حاجة بتحصل، فيتحوّل لـ loop على طول. ولـ 5.7: F<sub>N+1</sub> mod F<sub>N</sub> = F<sub>N−1</sub> (لأن F<sub>N+1</sub> = F<sub>N</sub> + F<sub>N−1</sub> وF<sub>N−1</sub> \< F<sub>N</sub>). يعني كل خطوة بتنزل درجة واحدة بس في Fibonacci، وده أبطأ نزول ممكن.
:::

:::solution
```c
int gcd(int u, int v) { while (v) { int t = u % v; u = v; v = t; } return u; }
```

```
5.7 depth of gcd(F_{N+1}, F_N):  N=5 -> 5  N=15 -> 15  N=25 -> 25  N=35 -> 35  N=45 -> 45
```

العمق حوالي **N**. وبما إن F<sub>N</sub> ≈ φ<sup>N</sup>/√5، يبقى العمق ≈ log<sub>φ</sub> F<sub>N</sub> ≈ 1.44 lg F<sub>N</sub>. ودي أسوأ حالة لـ Euclid (نظرية Lamé). يعني Euclid عمره ما بيعدّي حوالي 1.44 lg(الرقم) خطوة.
:::

### تمرين 5.21–5.24: المسطرة وHanoi

:::problem
(5.21) أثبت إن حل Hanoi في Program 5.7 optimal، يعني أي حل محتاج 2<sup>N</sup> − 1 حركة على الأقل. (5.22) اكتب برنامج recursive يحسب طول العلامة رقم i في مسطرة فيها 2<sup>n</sup> − 1 علامة. (5.23) لاقي خاصية في i تحدد اتجاه الحركة رقم i. (5.24) املا array بكل الحركات (زي Program 5.9).
:::

:::think
5\.21: فكّر في **أكبر قرص**. لازم يتحرك مرة على الأقل. ولحظة ما يتحرك، كل الـ N−1 التانيين لازم يكونوا مكوّمين على الـ peg التالت (مفيش مكان تاني). وبعد آخر حركة ليه، لازم يرجعوا كلهم فوقه. يعني T(N) ≥ 2T(N−1) + 1. وده بالظبط الـ recurrence بتاع 2<sup>N</sup>−1. 5.22: العلامة اللي في النص طولها n، وكل نص مسطرة n−1. 5.23: اكتب الحركات واتفرّج.
:::

:::solution
::figure ex-hanoi

```
5.22 ruler n=4 : 1 2 1 3 1 2 1 4 1 2 1 3 1 2 1
     mark(i) = 1 + (number of trailing 0 bits of i): true for n = 10
5.23 hanoi(4): -1 +2 -1 -3 -1 +2 -1 +4 -1 +2 -1 -3 -1 +2 -1
     verified for N = 1..12: yes
5.24 nonrecursive: -1 +2 -1 -3 -1 +2 -1 +4 -1 +2 -1 -3 -1 +2 -1
5.21 N=10: BFS shortest = 1023, 2^N-1 = 1023
```

5\.23: القرص اللي بيتحرك في الخطوة i هو 1 + عدد الأصفار في آخر i بالـ binary (يعني علامة المسطرة)، والقرص k دايمًا بيتحرك في نفس الاتجاه: (−1)<sup>N−k</sup>. فالحل كله ممكن يتولّد في loop واحد من غير أي recursion (5.24). وفي 5.21 الإثبات كفاية، بس البرنامج كمان عمل BFS على كل الـ 3<sup>N</sup> وضع لـ N ≤ 10، واتأكّد إن أقصر حل فعلًا 2<sup>N</sup>−1.
:::

### تمرين 5.37 و5.38: Fibonacci

:::problem
(5.37) احسب F<sub>N</sub> mod M بمساحة ثابتة. (5.38) إيه أكبر N يخلّي F<sub>N</sub> يتمثّل في integer بـ 64 bit؟
:::

:::think
مش محتاج array. آخر رقمين كفاية، زي bottom-up DP بس من غير جدول. واعمل mod في كل خطوة. ولـ 5.38: F<sub>N</sub> ≈ φ<sup>N</sup>/√5 \< 2<sup>63</sup>، يعني N \< (63 ln 2 + ln √5)/ln φ ≈ 92.4. يعني حوالي 92 لو signed، وواحد زيادة لو unsigned. والبرنامج بيتأكّد.
:::

:::solution
```
5.37 F_1000 mod 1000000007 = 517691607, F_100 mod 997 = 364
5.38 largest N: signed 64-bit F_92 = 7540113804746346429, unsigned 64-bit F_93 = 12200160415121876738
```

وProgram 5.10 (Fibonacci recursive عادي) كان هياخد حوالي F<sub>92</sub> ≈ 7.5×10¹⁸ استدعاء عشان يوصل لنفس الرقم ده. يعني مئات السنين.
:::

### تمرين 5.40–5.43: P<sub>N</sub> بالـ DP (وفخ في نص التمرين)

:::problem
(5.40) استخدم bottom-up DP عشان تحسب P<sub>N</sub> = ⌊N/2⌋ + P<sub>⌊N/2⌋</sub> + P<sub>⌈N/2⌉</sub>، لـ N ≥ 1 وP₀ = 0، وارسم N مقابل P<sub>N</sub> − N lg N/2. (5.41) نفس الكلام بـ top-down. (5.43) ارسم عدد الاستدعاءات في الـ top-down.
:::

:::think
طبّق السؤالين بتوع الـ tip: لـ N = 1، ⌈1/2⌉ = 1، فالـ recurrence بيقول P₁ = 0 + P₀ + P₁. يعني P₁ بيستدعي نفسه! لو كتبتها top-down حرفيًا، infinite recursion. فلازم base case تاني: P₁ = 0 (وده المعنى المقصود: مسألة حجمها 1 مابتتكلّفش). وبعد كده لاحظ إن الشكل ده هو نفسه عدد مقارنات الـ merge في أحسن حالة.
:::

:::solution
```
5.40 N, P_N, N lg N / 2, difference:
        16     32      32.0     0.0
        64    192     192.0     0.0
      1024   5120    5120.0     0.0
     N=1000: P=4932 vs 4982.9
5.43 top-down calls: N=23:15  N=64:13  N=1000:31  N=1024:21   (top-down == bottom-up: yes)
```

عند قوى 2 بالظبط P<sub>N</sub> = N lg N/2، وفي النص بينزل تحتها شوية (الفرق سالب وبيتذبذب، ولو رسمته هتلاقي شكل «سنّة منشار»). والـ top-down بيعمل O(lg N) استدعاء بس، لأن ⌊N/2⌋ و⌈N/2⌉ في كل مستوى بيبقوا على الأكتر رقمين مختلفين. فالـ memo بيلم كل حاجة.
:::

### تمرين 5.44 و5.45: C<sub>N</sub> بتاع quicksort

:::problem
احسب بالـ DP (bottom-up في 5.44، وtop-down في 5.45) C<sub>N</sub> = N + (1/N) Σ<sub>1≤k≤N</sub> (C<sub>k−1</sub> + C<sub>N−k</sub>) لـ N ≥ 1، وC₀ = 1.
:::

:::think
الحساب الساذج O(N) لكل C<sub>N</sub>، يعني O(N²) للكل. بس الحدّين في المجموع متماثلين: Σ(C<sub>k−1</sub> + C<sub>N−k</sub>) = 2 Σ<sub>j&lt;N</sub> C<sub>j</sub>. فخلّي running sum، وكل خطوة تبقى O(1). وده هو الـ recurrence بتاع متوسط مقارنات quicksort (الفصل 7)، فتوقّع ≈ 2N ln N.
:::

:::solution
```c
sum += C[i-1];  C[i] = i + 2*sum/i;
```

```
5.44 C_N:  C_10=45.437  C_100=848.85  C_1024=13346.8   (2N ln N = 14195.7 at 1024)
5.45 top-down C_23 = 134.2460 (bottom-up 134.2460), 553 calls
```

قريب من 2N ln N (الحد الأدق 2(N+1)H<sub>N</sub> − حدود خطية). والـ top-down الساذج بيعمل 553 استدعاء لـ N = 23 (كل مستوى بيلف على كل اللي قبله)، رغم الـ memo. والـ bottom-up مع الـ running sum أحسن بكتير.
:::

### تمرين 5.68 و5.73: عدّ في الشجر

:::problem
(5.68) كام external node في M-ary tree فيها N internal node؟ واستخدم ده عشان تحسب الذاكرة. (5.73) شجرة Fibonacci ارتفاعها n \> 1 فيها شجرة Fibonacci ارتفاعها n−1 في subtree، وارتفاعها n−2 في التاني. اديني الارتفاع والـ external path length بدلالة N (عدد الـ nodes).
:::

:::think
5\.68: عدّ الـ links. كل internal node فيها M link، يعني MN link. وكل node ماعدا الجذر عليها link واحد داخل، فالـ links = (N + E) − 1. يبقى E = (M−1)N + 1. 5.73: N<sub>h</sub> = N<sub>h−1</sub> + N<sub>h−2</sub> + 1، فـ N<sub>h</sub> + 1 بيمشي زي Fibonacci.
:::

:::solution
::figure ex-fibtree

5\.68: E = **(M−1)N + 1**. والذاكرة N item + MN link = (M+1)N word، ومنهم (M−1)N + 1 link فاضيين (NULL). يعني الـ 3-ary tree بيضيّع حوالي ثلثي الـ links!

```c
5.73 Fibonacci trees:   h   N(nodes)  height* ext.path  N+1=F_{h+2}
                        5        12       5        50        13   EPL/((N+1) lg N) = 1.039
                       14       986      14     10225       987   EPL/((N+1) lg N) = 1.041
                       20     17710      20    260375     17711   EPL/((N+1) lg N) = 1.042
```

N + 1 = F<sub>h+2</sub>، يعني h ≈ log<sub>φ</sub>(√5 N) ≈ **1.44 lg N**. والـ external path length ≈ **1.04 N lg N** (من الجدول؛ وتحليليًا متوسط عمق الـ external node بيقرّب من (φ/√5)·h ≈ 0.72h). يعني حتى أسوأ شجرة «شبه متوازنة» بعيدة عن المثالي بـ 44% بس في الارتفاع، و4% في المتوسط.
:::

### تمرين 5.74 و5.75: شجرة divide-and-conquer

:::problem
(5.74) شجرة divide-and-conquer لـ N: جذرها N، وتحتها شجرة divide-and-conquer لـ ⌊N/2⌋ وواحدة لـ ⌈N/2⌉. ارسمها لـ 11 و15 و16 و23. (5.75) أثبت بالـ induction إن الـ path length بتاعها بين N lg N وN lg N + N.
:::

:::think
الأول افهم التعريف كويس: لو N عدد الـ nodes، يبقى 1 + ⌊N/2⌋ + ⌈N/2⌉ = N + 1. ده مستحيل. فالمقصود إن N عدد **الأوراق** (المسائل اللي حجمها 1)، زي Figure 5.6. والـ path length هنا هو مجموع أعماق الأوراق = مجموع أحجام كل المسائل اللي اتحلّت (ماعدا الجذر). Induction: L(N) = L(⌊N/2⌋) + L(⌈N/2⌉) + N.
:::

:::solution
::figure ex-dctree

الإثبات: L(1) = 0 والحدود صح. افترض إنها صح للأصغر، وخد a = ⌊N/2⌋ وb = ⌈N/2⌉. الحد الأدنى: L(N) ≥ a lg a + b lg b + N ≥ N lg(N/2) + N = N lg N، لأن x lg x convex فأقل قيمة ليها لما a = b = N/2. والحد الأعلى: L(N) ≤ a lg a + a + b lg b + b + N. واستخدم إن b ≤ (N+1)/2 والـ lg concave، وهتوصل لـ ≤ N lg N + N. ∎ والبرنامج اتأكّد لكل N ≤ 5000:

```
        11       39       38.1       49.1
        16       64       64.0       80.0
        23      106      104.0      127.0
      5000    61808    61438.6    66438.6
     bound holds for all N <= 5000: yes
```

وده بالظبط سبب إن mergesort بيعمل بين N lg N وN lg N + N مقارنة تقريبًا (الفصل 8).
:::

### تمرين 5.86–5.90: أوراق، وnodes بابن واحد، وinternal path length

:::problem
(5.86) برنامج يعدّ الأوراق في binary tree. (5.87) يعدّ الـ nodes اللي ليها ابن internal وابن external. (5.88) برنامج recursive يحسب internal path length (Definition 5.6). (5.89) كام استدعاء بيعمل؟ (5.90) خلّيه في وقت proportional لعدد الـ nodes.
:::

:::think
كل دول نفس القالب: `f(NULL)` = حالة بسيطة، و`f(h)` = حاجة عن h + f(يمين) + f(شمال). الفخ في 5.88: لو حسبت IPL = IPL(L) + IPL(R) + (عدد nodes تحت)، وبتعدّ الـ nodes بـ function تانية جوه كل استدعاء، هتبقى O(N²) في الشجرة المايلة. الحل: مرّر العمق كـ parameter، فكل node بتضيف عمقها بس.
:::

:::solution
```c
int leaves(link h)   { if (!h) return 0; if (!h->l && !h->r) return 1;
                       return leaves(h->l) + leaves(h->r); }
int onechild(link h) { if (!h) return 0;
                       return ((!h->l) != (!h->r)) + onechild(h->l) + onechild(h->r); }
long ipl(link h, int d) { if (!h) return 0; return d + ipl(h->l, d+1) + ipl(h->r, d+1); }
```

```
5.86 leaves(dc tree 11) = 11   5.87 one-child nodes = 0
5.88/5.89 IPL(dc tree 11) = 58 using 43 calls = 2N+1 (N=21): linear
     fib tree h=6: 20 nodes, 8 leaves, 5 one-child nodes
```

5\.89: الاستدعاءات = **2N + 1** بالظبط: واحد لكل node وواحد لكل external node (N+1 منهم، من 5.68). وإثباته بالـ induction: C(شجرة) = 1 + C(L) + C(R)، وC(فاضية) = 1. وشجرة الـ divide-and-conquer مفيهاش ولا node بابن واحد (كل node يا ورقة يا ليها ابنين)، أما شجرة Fibonacci فيها كتير.
:::

:::quiz
ليه Program 5.1 (factorial recursive) ممكن يقع عند N = مليون، وmergesort على مليون عنصر لأ؟

1. [ ] لأن الـ factorial بيعمل overflow
2. [x] لأن عمق الـ recursion في الـ factorial N، وفي mergesort lg N ≈ 20 بس
3. [ ] لأن mergesort مش recursive

> الـ stack بيشيل frame لكل استدعاء لسه ماخلصش. والعمق مش عدد الاستدعاءات الكلي هو اللي بيحدد.
:::

:::quiz
في Hanoi بـ 10 أقراص، أنهي قرص بيتحرك في الخطوة رقم 48؟

1. [ ] قرص 1
2. [ ] قرص 4
3. [x] قرص 5
4. [ ] قرص 6

> 48 = 110000 بالـ binary، فيها 4 أصفار في الآخر، فالقرص = 4 + 1 = 5.
:::

:::quiz
3-ary tree فيها 100 internal node. كام external node (NULL link)؟

1. [ ] 101
2. [x] 201
3. [ ] 300

> (M−1)N + 1 = 2×100 + 1 = 201.
:::

:::quiz
الـ recurrence P<sub>N</sub> = ⌊N/2⌋ + P<sub>⌊N/2⌋</sub> + P<sub>⌈N/2⌉</sub> مع P₀ = 0 بس. إيه المشكلة لو كتبته top-down؟

1. [ ] بطيء
2. [x] P₁ بيستدعي P₁ (لأن ⌈1/2⌉ = 1)، فـ infinite recursion
3. [ ] مفيش مشكلة

> لازم base case لـ N = 1، لأن المسألة مش بتصغر عند N = 1.
:::
