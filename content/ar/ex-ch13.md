تمارين محلولة · الفصل 13 (Balanced Trees) · تمارين 13.8–13.81

الفصل 13 كله إجابات مختلفة لسؤال واحد: إزاي نمنع الـ BST إنها تبقى «list» في أسوأ حالة؟ عندنا 4 إجابات: العشوائية (randomized BST وskip lists)، والـ amortization (splay)، والضمان (2-3-4 وred-black). وتمارين الفصل أغلبها «ارسم الشجرة بعد الـ inserts دي»، وده التدريب اللي هيخلّيك تفهم بجد إيه اللي بيحصل جوه كل طريقة.

:::goal هدف الدرس:
تبني بالإيد randomized BST وsplay tree و2-3-4 tree وred-black tree وskip list لنفس المفاتيح، وتقارن شكلهم، وتعرف حدود ارتفاع الـ 2-3-4 trees، وتعدّ الـ links في الـ rotation.
:::

:::tip red-black = 2-3-4 متنكّرة
كل ما تشوف red-black tree، اقفل عينك وادمج كل node حمرا مع أبوها. اللي هيطلع 2-3-4 tree، وكل أوراقها على نفس العمق. وأي سؤال عن red-black جاوب عليه في عالم الـ 2-3-4 الأول.
:::

* [src/13-balanced-trees/exercises/ex13\_08\_rand\_splay.c](src:src/13-balanced-trees/exercises/ex13_08_rand_splay.c)
* [src/13-balanced-trees/exercises/ex13\_39\_234\_rb.c](src:src/13-balanced-trees/exercises/ex13_39_234_rb.c)
* [src/13-balanced-trees/exercises/ex13\_75\_skiplist.c](src:src/13-balanced-trees/exercises/ex13_75_skiplist.c)

### تمرين 13.8: randomized BST بـ «عملة» بايظة

:::problem
ارسم الـ randomized BST اللي بتطلع لما ندخّل E A S Y Q U T I O N بالترتيب ده في شجرة فاضية، بافتراض إن عندنا randomization function بايظة بتختار root insertion كل ما حجم الشجرة يبقى فردي.
:::

:::think
Program 13.2: في كل node بنعدّي عليها، احتمال 1/(N+1) نعمل root insertion في الـ subtree دي، وإلا ننزل عادي. هنا القرار ثابت: لو حجم الـ subtree الحالية فردي، المفتاح الجديد يبقى جذرها (بـ root insertion). ولو زوجي، ننزل خطوة ونسأل نفس السؤال تحت. وخلّي بالك: السؤال بيتسأل في **كل** مستوى، مش في الجذر بس.
:::

:::solution
```
13.8 randomized BST, root insertion whenever the (sub)tree size is odd:
     +E  E
     +A  A(-,E)
     +S  A(-,S(E,-))
     +Y  Y(A(-,S(E,-)),-)
     +Q  Y(Q(A(-,E),S),-)
     +U  U(Q(A(-,E),S),Y)
     +T  U(Q(A(-,E),T(S,-)),Y)
     +I  I(A(-,E),U(Q(-,T(S,-)),Y))
     +O  I(A(-,E),O(-,U(Q(-,T(S,-)),Y)))
     +N  N(I(A(-,E),-),O(-,U(Q(-,T(S,-)),Y)))
     height 6
```

مثلًا في +Q: الشجرة حجمها 4 (زوجي)، فننزل للشمال (A(-,S(E)) حجمها 3، فردي)، فـ Q تبقى جذر الـ subtree دي. والعملة البايظة دي بتخلّي الجذر يتغير في نص الـ inserts. والنتيجة مش وحشة (ارتفاع 6)، بس مش «random» خالص: أي حد عارف القاعدة يقدر يبني ترتيب يخلّيها وحشة.
:::

### تمرين 13.25 و13.26: splay insertion والـ double rotations

:::problem
(13.25) ارسم الـ splay BST اللي بتطلع من إدخال E A S Y Q U T I O N بالـ splay insertion. (13.26) كام tree link لازم يتغير في الـ double rotation؟ وكام بيتغير فعلًا في كل double rotation في Program 13.5؟
:::

:::think
الـ splay insertion: المفتاح الجديد بيطلع للجذر، بس بخطوتين خطوتين. لو هو وأبوه في نفس الاتجاه (zig-zig)، بنعمل rotate للجد الأول وبعدين للأب. ولو في اتجاهين مختلفين (zig-zag)، بنعمل rotate للأب الأول وبعدين للجد. وفي 13.26 بص على اللي اتغير بعد العملية: 3 nodes اترتّبوا من جديد، والـ subtrees بتاعتهم اتنقلت. خلّينا نعدّ كل pointer assignment بيغيّر قيمة فعلًا.
:::

:::solution
```
13.25 splay insertion:
     +E  E
     +A  A(-,E)
     +S  S(E(A,-),-)
     +Y  Y(S(E(A,-),-),-)
     +Q  Q(E(A,-),S(-,Y))
     +U  U(S(Q(E(A,-),-),-),Y)
     +T  T(S(Q(E(A,-),-),-),U(-,Y))
     +I  I(E(A,-),S(Q,T(-,U(-,Y))))
     +O  O(I(E(A,-),-),S(Q,T(-,U(-,Y))))
     +N  N(I(E(A,-),-),O(-,S(Q,T(-,U(-,Y)))))
     height 6
13.26 zig-zig: 4.00 links changed each   zig-zag: 5.00 links changed each   (+1 for the parent's link)
```

شوف +Q: الشجرة كانت «list» نازلة شمال (Y-S-E-A)، والـ Q اتحطت تحت الـ E، والـ splay قسم الـ list نصين: Q(E(A), S(Y)). ده سحر الـ splay: كل بحث في مسار طويل بيقصّره للنص تقريبًا. وفي 13.26: الـ double rotation محتاج يغيّر 4 links جوّه + link الأب. الـ zig-zig في Program 13.5 بيعمل كده بالظبط (4). أما الـ zig-zag فبيعمل 5، لأن `h->l` بيتكتب مرتين: مرة في `h->l = rotL(h->l)`، ومرة في `rotR(h)`. يعني فيه كتابة واحدة ممكن نوفّرها.
:::

### تمرين 13.39–13.42: 2-3-4 trees

:::problem
(13.39) ارسم الـ balanced 2-3-4 tree اللي بتطلع من إدخال E A S Y Q U T I O N بطريقة الـ top-down. (13.40) نفس السؤال بطريقة الـ bottom-up. (13.41/13.42) إيه أقل وأكبر ارتفاع ممكن لـ 2-3-4 tree فيها N مفتاح؟
:::

:::think
top-down: وانت نازل، أي 4-node تقابلها اعملها split على طول (الوسطاني يطلع لأبوه، وده مضمون إن عنده مكان، لأننا عملنا split لأي 4-node فوقه). bottom-up: انزل لحد تحت وحط المفتاح، ولو الورقة كانت 4-node اعملها split، والـ split ده ممكن يطلع لفوق ويعمل split تاني. وفي 13.41/13.42: شجرة ارتفاعها h لو كلها 2-nodes فيها 2<sup>h</sup> − 1 مفتاح، ولو كلها 4-nodes فيها 4<sup>h</sup> − 1، وأي عدد بين الاتنين ممكن.
:::

:::solution
::figure ex13-234

```
13.39 top-down 2-3-4:
     +Y  [E]([A] [SY])
     +U  [ES]([A] [Q] [UY])
     +N  [EOS]([A] [IN] [Q] [TUY])
13.40 bottom-up 2-3-4: the same trees at every step for these keys
     the shortest disagreement found: KIAHFCDEG
       top-down  [F]([C]([A] [DE]) [I]([GH] [K]))
       bottom-up [CFI]([A] [DE] [GH] [K])
13.41/13.42 min height = ceil(log4(N+1)), max height = floor(lg(N+1)):
     N=     10: min  2  max  3   (a random top-down tree: 2)
     N=   1000: min  5  max  9   (a random top-down tree: 8)
     N=1000000: min 10  max 19   (a random top-down tree: 16)
```

على EASYQUTION الطريقتين طلّعوا نفس الشجرة في كل خطوة، لأن مافيش مرة الطريقة الـ top-down عملت split «احتياطي» ماكانش هيحصل في الـ bottom-up. فدوّرنا على مثال بيفرّق، وأقصر واحد لقيناه KIAHFCDEG. الـ top-down عمل split للجذر \[CFI\] وهو نازل، رغم إن الـ G كانت هتلاقي مكان في \[H\] من غير أي split. يعني الـ top-down بيدفع splits زيادة، وده تمن إنه بيعدّي مرة واحدة من فوق لتحت.
:::

### تمرين 13.48–13.50: red-black trees

:::problem
(13.48) ارسم الـ red-black BST اللي بتطلع من إدخال E A S Y Q U T I O N بطريقة الـ top-down. (13.49) نفس السؤال بالـ bottom-up. (13.50) ارسم الـ red-black tree اللي بتطلع لما ندخّل الحروف من A لـ K بالترتيب، واشرح إيه اللي بيحصل بشكل عام لما نبني الشجرة بمفاتيح متزايدة.
:::

:::think
top-down (Program 13.6): أي node ليها ابنين حمر هي 4-node، فاقلب الألوان وانت نازل. ولو حصل اتنين حمر ورا بعض، صلّحهم بـ rotation واحدة (نفس الاتجاه) أو اتنين (zig-zag). bottom-up: حط ورقة حمرا. ولو أبوها أحمر، بص على العم: لو أحمر اقلب الألوان واطلع لفوق، ولو أسود اعمل rotation وخلاص. والنتيجة لازم تقابل الـ 2-3-4 بتاعة 13.39.
:::

:::solution
::figure ex13-rb

```
13.48 red-black, top-down (Program 13.6):
     +S  E(A*,S*)
     +Y  E(A,S(-,Y*))
     +U  E(A,S*(Q,Y(U*,-)))
     +O  E(A,S*(O(I*,Q*),U(T*,Y*)))
     +N  O(E*(A,I(-,N*)),S*(Q,U(T*,Y*)))
     red-black rules hold: yes, height 4
13.49 red-black, bottom-up:
     O(E*(A,I(-,N*)),S*(Q,U(T*,Y*)))
13.50 A..K ascending, top-down: D(B(A,C),F(E,H*(G,J(I*,K*))))   height 5
     N=   1000 ascending: height 16 (lg N = 10.0), rules ok
     N=1000000 ascending: height 30 (lg N = 19.9), rules ok
```

الـ \* معناها node حمرا. والطريقتين طلّعوا نفس الشجرة، وهي بالظبط ترجمة الـ 2-3-4 بتاعة 13.39: \[EOS\] = O ومعاها E\* وS\*، و\[TUY\] = U ومعاها T\* وY\*. وفي 13.50، المفاتيح المتزايدة بتعمل «سلسلة حمرا» على الجنب اليمين، والـ splits بتكسرها كل شوية. الارتفاع بيطلع حوالي 1.5 lg N (30 لمليون)، وده أقل من الحد النظري 2 lg N، بس أعلى من الشجرة العشوائية (~lg N).
:::

### تمرين 13.75، 13.76 و13.81: skip lists

:::problem
(13.75) ارسم الـ skip list اللي بتطلع من إدخال E A S Y Q U T I O N في list فاضية، لو randX بترجّع القيم 1، 3، 1، 1، 2، 2، 1، 4، 1، 1. (13.76) نفس السؤال للمفاتيح A E I N O Q S T U Y بنفس قيم randX. (13.81) كام رقم عشوائي محتاجين في المتوسط عشان نبني skip list بـ parameter t، باستعمال randX() اللي في Program 13.9؟
:::

:::think
كل مفتاح بياخد عدد levels = قيمة randX اللي جت له. E = 1، A = 3، S = 1، Y = 1، Q = 2، U = 2، T = 1، I = 4، O = 1، N = 1. وبعد كده ارسم كل level لوحده، فيه المفاتيح اللي levelها ≥ رقمه. وفي 13.81 بص على الكود نفسه: randX في Program 13.9 بتنادي rand() **مرة واحدة** بس، وبتقارن الرقم بحدود RAND\_MAX/t وRAND\_MAX/t² وهكذا.
:::

:::solution
```
13.75 EASYQUTION with levels 1 3 1 1 2 2 1 4 1 1:
       level 4: head           -> I
       level 3: head -> A      -> I
       level 2: head -> A      -> I           -> Q           -> U
       level 1: head -> A -> E -> I -> N -> O -> Q -> S -> T -> U -> Y
       28 key compares to build
13.76 AEINOQSTUY with the same levels:
       level 4: head                                    -> T
       level 3: head      -> E                          -> T
       level 2: head      -> E           -> O -> Q      -> T
       level 1: head -> A -> E -> I -> N -> O -> Q -> S -> T -> U -> Y
       19 key compares to build
13.81 t=2: Program 13.9 1.000 per insert;  coin flipping 1.998 (t/(t-1) = 2.000)
      t=4: Program 13.9 1.000 per insert;  coin flipping 1.335 (t/(t-1) = 1.333)
```

شكل الـ skip list بيعتمد على مين أخد أنهي level، مش على ترتيب الإدخال. في 13.76 نفس الـ levels راحت لمفاتيح تانية (E بقت 3 وT بقت 4)، والبناء كان أرخص (19 مقارنة)، لأن المفاتيح جاية مترتبة. وفي 13.81 الجواب «رقم واحد لكل insert» بالظبط، يعني N رقم للبناء كله. ولو كنا عملناها بالطريقة الساذجة (عملة 1/t لكل level)، كنا هنحتاج t/(t−1) رقم لكل insert.
:::

:::quiz
red-black tree فيها node سودا ومعاها ابنين حمر. دي تقابل إيه في الـ 2-3-4؟

1. [ ] 2-node
2. [ ] 3-node
3. [x] 4-node

> 3 مفاتيح «ملزوقين» بـ links حمرا = 4-node، وده اللي top-down بيعمله split بـ color flip.
:::

:::quiz
2-3-4 tree فيها مليون مفتاح. أكبر ارتفاع ممكن؟

1. [ ] 10
2. [x] 19
3. [ ] 40

> لو كل الـ nodes 2-nodes: ⌊lg(10⁶ + 1)⌋ = 19. ولو كلها 4-nodes: ⌈log₄(10⁶ + 1)⌉ = 10 (تمرين 13.41).
:::

:::quiz
top-down 2-3-4 insertion ضد bottom-up. الفرق؟

1. [ ] الـ bottom-up بيعمل splits أكتر
2. [x] الـ top-down ممكن يعمل split لـ 4-node ماكانش محتاج يتعمله split
3. [ ] دايمًا بيطلّعوا نفس الشجرة

> الـ top-down بيعمل split لأي 4-node وهو نازل، احتياطي. مثال KIAHFCDEG: عمل split للجذر، والـ bottom-up ماعملش.
:::

:::quiz
في splay tree، إيه اللي بيحصل لـ «list» طويلة لما ندوّر على آخرها؟

1. [ ] بتفضل زي ما هي
2. [x] العمق بيقل للنص تقريبًا
3. [ ] بتبقى متوازنة تمامًا

> الـ zig-zig بيطوي المسار: كل خطوتين بيطلعوا لفوق مع بعض، ومن هنا جه الـ amortized log N.
:::
