تمارين محلولة · الفصل 9 (Priority Queues and Heapsort) · تمارين 9.1–9.60

الفصل التاسع فيه ADT جديد (الـ priority queue)، وطريقتين لتنفيذه: الـ heap والـ binomial queue، وsort طالع منهم (heapsort). والتمارين نصّها تتبّع بالإيد. وده مش ممل: لو عرفت تتتبّع fixUp وfixDown من غير ما تبصّ في الكود، يبقى فهمت الفصل. والنص التاني أسئلة «فين ممكن يقع العنصر ده؟» اللي بتختبر فهمك لشكل الـ heap.

:::goal هدف الدرس:
تتتبّع أي sequence من insert وdelete-max على heap، وتعرف حدود أماكن العناصر جوه الـ heap، وتشوف ليه heapsort مش stable، وتبني binomial queue بنفس منطق الجمع بالـ binary.
:::

:::tip جملتين بيحلّوا نص التمارين
(1) أي عنصر في الـ heap أصغر من كل اللي فوقه، يعني لو عمقه d يبقى فيه d عنصر أكبر منه على الأقل. (2) أي عنصر أكبر من كل اللي تحته، يعني لو الـ subtree بتاعه فيه s node يبقى فيه s − 1 عنصر أصغر منه على الأقل.
:::

* [src/09-heapsort/exercises/ex9\_01\_pq\_basics.c](src:src/09-heapsort/exercises/ex9_01_pq_basics.c)
* [src/09-heapsort/exercises/ex9\_17\_heaps.c](src:src/09-heapsort/exercises/ex9_17_heaps.c)
* [src/09-heapsort/exercises/ex9\_54\_binomial.c](src:src/09-heapsort/exercises/ex9_54_binomial.c)

### تمرين 9.1 و9.2: insert وdelete-max وjoin

:::problem
(9.1) الحرف معناه insert، والنجمة معناها delete the maximum، في الـ sequence `P R I O * R * * I * T * Y * * * Q U E * * * U * E`. إيه القيم اللي بترجع من الـ deletes؟ (9.2) زوّد علامة + بمعنى join، والأقواس بتحدد الـ PQ اللي اتبنت جوّاها. إيه محتوى الـ PQ بعد `( ( ( P R I O * ) + ( R * I T * Y * ) ) * * * ) + ( Q U E * * * U * E )`؟
:::

:::think
في 9.1 امشي واحدة واحدة واكتب المحتوى الحالي جنب كل خطوة: P → PR → PRI → PRIO → (\*) R تطلع → POI → … . وفي 9.2 قيّم من جوه لبرّه: كل قوس هو PQ لوحده، وعلامة الـ + بتحطّهم مع بعض. والنجوم اللي بعد القوس بتشتغل على الـ PQ المدموجة.
:::

:::solution
```
9.1 deletions: R R P O T Y I I U Q E U
    left in the queue: E
9.2 deletions in order: RRTYPOIUQEU
    final contents: IE
```

في 9.2: القوس الأول بيبقّي P I O، والتاني بيبقّي I بس. والـ join بيدّي P O I I، والتلات نجوم بيشيلوا P وO وI، فيفضل I. وفي الآخر join مع (Q U E \* \* \* U \* E) اللي بيبقّي E. يعني النتيجة **I E**.
:::

### تمرين 9.3–9.5: stack وqueue من PQ، وفكرة «الـ max اللي شفناه»

:::problem
(9.3) إزاي تستعمل الـ priority queue ADT عشان تعمل stack؟ (9.4) وqueue؟ (9.5) انتقد الفكرة دي: عشان find the maximum تبقى في وقت ثابت، ليه مانحفظش أكبر قيمة دخلت لحد دلوقتي ونرجّعها؟
:::

:::think
الـ PQ بتطلّع «الأهم». فلو خلّينا الأهمية هي الوقت، هتبقى stack (الأحدث أهم) أو queue (الأقدم أهم، يعني الأولوية بالسالب). وفي 9.5 السؤال: إيه اللي بيحصل بعد delete the maximum؟
:::

:::solution
```
9.3 stack via PQ, push 10..50 then pop x3: 50 40 30 | push 99, pop: 99
9.4 queue via PQ, put 10..50 then get x3: 10 20 30 | put 99, get: 40
9.5 insert P R I O, delete max -> R; 'max so far' still says R, true max is P
```

9\.3/9.4 شغالين، بس بتكلفة الـ PQ (log N مع heap)، وده أغلى من stack حقيقي بـ O(1). يعني دي فكرة نظرية بتوضّح إن الـ PQ «أعمّ» من الاتنين. وفي 9.5 الفكرة بتقع أول ما نعمل delete: الـ max اتشال، وأكبر واحد بعده محتاج نعرفه، وده بالظبط الشغل اللي كنا عايزين نهرب منه.
:::

### تمرين 9.17 و9.18: شكل الـ heap

:::problem
(9.17) هل المصفوفة المترتبة تنازلي heap؟ (9.18) أكبر عنصر لازم يبقى في الموقع 1، وتاني أكبر لازم يبقى في 2 أو 3. لـ heap حجمه 15 ومفاتيحه مختلفة، فين ممكن (وفين مستحيل) يقع العنصر رقم k في الكبر، لـ k = 2، 3، 4؟
:::

:::think
9\.17: لكل k، الـ a\[k/2\] جاي قبل a\[k\] في المصفوفة، فهو أكبر منه أو يساويه. يعني أيوه. 9.18: العنصر رقم k عنده k − 1 عنصر أكبر منه بس. ولو هو على عمق d، يبقى الـ d اللي فوقه كلهم أكبر منه، فلازم d ≤ k − 1. والعكس: كل اللي تحته أصغر منه، فلازم حجم الـ subtree بتاعه يبقى ≤ N − k + 1. ونأكد بالتجربة على 2 مليون heap عشوائي.
:::

:::solution
```
9.17 descending array is a heap: yes
9.18 heap of size 15 (formula, then 2,000,000 random heaps to confirm):
     k=2 can: 2 3   cannot: the rest
     k=3 can: 2 3 4 5 6 7   cannot: the rest
     k=4 can: 2 3 4 5 6 7 8 9 10 11 12 13 14 15   cannot: the rest
```

الـ 4 ممكن يقع في أي حتة غير الجذر، حتى في ورقة على عمق 3: تلات أجداده هم بالظبط أكبر 3 عناصر. والعينات العشوائية لقت كل مكان «ممكن» فعلًا، وماخالفتش الشرط ولا مرة. وشرط حجم الـ subtree مابيأثّرش هنا لأن N = 15 كبيرة كفاية، بس بيفرق لـ k قريبة من N (9.19).
:::

### تمرين 9.21 و9.22: بناء heap وتتبّع العمليات

:::problem
(9.21) إيه الـ heap اللي بيطلع لما ندخّل E A S Y Q U E S T I O N في heap فاضي؟ (9.22) وريني الـ heaps اللي بتطلع من العمليات `P R I O * R * * I * T * Y * * * Q U E * * * U * E`.
:::

:::think
insert = حط الجديد في الآخر وطلّعه لفوق (fixUp) طول ما هو أكبر من أبوه. delete = بدّل الجذر مع الآخر، وصغّر الحجم، ونزّل الجذر الجديد (fixDown) ناحية الابن الأكبر.
:::

:::solution
::figure ex9-heap

```
   9.21  YTUSQNEASIOE
9.22 heaps after each operation:
   +P    P        *P    OI       *Y    II      *U    QE
   +R    RP       +I    OII      *I    I       *Q    E
   +I    RPI      *O    II       *I            *E
   +O    RPIO     +T    TII      +Q    Q       +U    U
   *R    POI      *T    II       +U    UQ      *U
   +R    RPIO     +Y    YII      +E    UQE     +E    E
   *R    POI
```

لاحظ في 9.21 إن الـ Y طلعت لفوق خالص في خطوتين، والـ T طلعت مكان الـ S. والمصفوفة دي مش مترتبة، بس كل أب ≥ ولاده.
:::

### تمرين 9.26: أقل حركة ممكنة في delete-max

:::problem
إيه أقل عدد مفاتيح لازم يتحرك في عملية delete the maximum في heap؟ اديني heap حجمه 15 بيحقق الأقل ده.
:::

:::think
delete-max بيبدّل a\[1\] مع a\[15\]، وبعدين fixDown. هل ممكن fixDown مايعملش حاجة خالص؟ لأ: الـ a\[15\] ورقة تحت a\[7\] تحت a\[3\]، فهي أصغر من a\[3\]، يعني لازم تنزل خطوة على الأقل (لو المفاتيح مختلفة). طب ممكن تقف بعد خطوة واحدة؟ أيوه، لو نزلت ناحية الشمال وولاد الموقع 2 أصغر منها.
:::

:::solution
::figure ex9-heapmin

```
9.26 fewest exchanges in delete-max (N=15, distinct keys): 2 (= 4 key moves with exch, 3 with insertion-style moves)
     example heap: 15 14 13 4 7 11 12 2 3 5 1 6 8 9 10
```

الجواب يعتمد على طريقة العدّ: exchange-ين = 4 حركات مفاتيح بـ exch، أو 3 حركات لو بنحرّك «زي insertion sort» (تمرين 9.23): الـ max يطلع، والـ 14 تطلع، والـ 10 تتحط في مكانها.
:::

### تمرين 9.27–9.29: heapsort

:::problem
(9.27) وريني إزاي heapsort بيرتّب E A S Y Q U E S T I O N. (9.28) وريني إن heapsort مش stable. (9.29) إيه نسبة الوقت اللي heapsort بيصرفه في مرحلة البناء، لـ N = 10³ لحد 10⁶؟
:::

:::think
مرحلة البناء (bottom-up) بتعمل fixDown من k = N/2 لحد 1. وبعدين كل خطوة: بدّل الجذر مع الآخر وfixDown. وفي 9.28 دوّر على أصغر مثال: مفتاحين B متساويين، والـ heap هيحط واحد منهم في الجذر ويرميه في الآخر الأول. وفي 9.29 البناء خطي (أقل من 2N مقارنة)، والترتيب 2N lg N، فالنسبة لازم تقل مع N.
:::

:::solution
```
9.27 heapsort on EASYQUESTION (Program 9.7):
   k=3   EAUYQSESTION
   k=2   EYUTQSESAION
   k=1   YTUSQSEEAION
         UTSSQNEEAIOY   heap size 11
         TSSOQNEEAIUY   heap size 10
         SQSOINEEATUY   heap size 9
         ...
         AEEINOQSSTUY   heap size 1
9.28 heapsort of B1 B2 A -> A3 B2 B1  (B2 before B1: not stable)
9.29 share of heapsort time spent building the heap:
     N=   1000: 14.8%     N= 100000: 8.5%
     N=  10000: 10.4%     N=1000000: 6.5%
```

9\.28: الملف B1 B2 A هو heap جاهز، فالـ B1 (الجذر) بتتبدّل مع آخر عنصر وتروح الموقع 3، وبعدين الـ B2 تروح الموقع 2. فتطلع B2 قبل B1. وفي 9.29 البناء رخيص: أقل من 15% حتى لـ N صغيرة، وبيقل لأن تكلفته خطية والباقي N lg N.
:::

### تمرين 9.38: أصغر 100 من مليون

:::problem
أنهي تنفيذ للـ priority queue تستعمله عشان تلاقي أصغر 100 رقم من 10⁶ رقم random؟ برّر.
:::

:::think
الغريبة إننا بنستعمل **max**-heap عشان نجيب الأصغر. نحتفظ بأصغر 100 لحد دلوقتي، والجذر بتاعهم هو «أكبر واحد فيهم»، يعني أول واحد هيطلع لو جه حد أصغر. وكل رقم جديد بنقارنه بالجذر بس، ولو مش أصغر منه بنرميه في مقارنة واحدة. ولأن الأرقام random، الحالة دي هي الغالبة.
:::

:::solution
```
9.38 100 smallest of 10^6 with a max-heap of 100: 1009988 compares (1.01 per element; a full sort needs ~20 per element)
```

حوالي مقارنة واحدة لكل عنصر! الرقم رقم i بيدخل الـ heap باحتمال 100/i بس، يعني حوالي 100 ln(10⁶/100) ≈ 920 مرة إدخال في المجموع، وكل واحدة بتكلّف حوالي 2 lg 100 ≈ 13 مقارنة. والمساحة 100 خانة بس.
:::

### تمرين 9.54، 9.56، 9.57 و9.60: الـ binomial queues

:::problem
(9.54) ارسم binomial queue حجمه 29. (9.56) إيه الـ binomial queue اللي بيطلع لما ندخّل EASYQUESTION؟ (9.57) ابني EASY وQUESTION لوحدهم، واعمل delete-max لكل واحد، وبعدين اعمل join. (9.60) اثبت إن الـ binomial tree اللي فيها 2<sup>n</sup> node فيها C(n, i) node في المستوى i.
:::

:::think
الـ binomial queue هو تمثيل N بالـ binary: لكل bit = 1 فيه شجرة B<sub>k</sub>. والـ insert زي ما تزوّد 1 على رقم binary: لو فيه B<sub>0</sub> اعمل carry وادمجهم B<sub>1</sub>، وهكذا. ودمج شجرتين B<sub>k</sub> = الجذر الأصغر يبقى ابن للأكبر. وفي 9.60: الـ B<sub>n</sub> عبارة عن B<sub>n−1</sub> متعلّق في B<sub>n−1</sub>، فالمستوى i في الجديدة = المستوى i في واحدة + المستوى i − 1 في التانية، يعني C(n−1, i) + C(n−1, i−1) = C(n, i). ده مثلث باسكال بالظبط.
:::

:::solution
::figure ex9-binom

```
9.54 N = 29 = binary 11101:   B4 (16 nodes)  B3 (8)  B2 (4)  B0 (1)
   9.56 after inserting EASYQUESTION (N = 12 = 1100):
      B3: Y(U(S(E) Q) E(A) S)
      B2: T(O(N) I)
   9.57 delete max: Y from EASY, U from QUESTION
   EASY after delete max (N=3 = 11):        B1: E(A)   B0: S
   QUESTION after delete max (N=7 = 111):   B2: T(O(N) I)   B1: S(E)   B0: Q
   join of the two (N=10 = 1010):
      B3: T(S(E(A) E) O(N) I)
      B1: S(Q)
9.60 B4: 1 4 6 4 1   C(4,i): 1 4 6 4 1
     B6: 1 6 15 20 15 6 1   C(6,i): 1 6 15 20 15 6 1
     equal for every n <= 12: yes
```

الـ join في 9.57 هو حرفيًا جمع 3 + 7 = 10 بالـ binary: 011 + 111. الـ B<sub>0</sub>s (S وQ) بيعملوا carry شجرة B<sub>1</sub> = S(Q). وفي B<sub>1</sub> عندنا E(A) وS(E) والـ carry، يعني تلاتة: واحدة بتفضل (S(Q))، والاتنين التانيين بيعملوا carry B<sub>2</sub>. والـ B<sub>2</sub> دي بتتدمج مع T(O(N) I) وتطلع B<sub>3</sub>. فالنتيجة 1010 = B<sub>3</sub> + B<sub>1</sub>.
:::

:::quiz
في heap فيه 15 مفتاح مختلف، فين ممكن يقع تالت أكبر مفتاح؟

1. [ ] 2 أو 3 بس
2. [x] أي موقع من 2 لـ 7
3. [ ] أي موقع غير 1

> عمقه لازم يبقى ≤ 2، لأن كل اللي فوقه أكبر منه، ومافيش غير 2 أكبر منه. والمستويات 1 و2 هي المواقع من 2 لـ 7.
:::

:::quiz
عايز أصغر 100 من مليون رقم. أحسن اختيار؟

1. [ ] min-heap فيه المليون كلهم وتشيل 100 مرة
2. [x] max-heap حجمه 100، وتقارن كل رقم جديد بالجذر
3. [ ] ترتّب المليون

> مساحة 100 بس، وتقريبًا مقارنة واحدة لكل رقم (تمرين 9.38). والـ min-heap الكبير محتاج مساحة N، وبناؤه بس بياخد حوالي 2N مقارنة.
:::

:::quiz
binomial queue فيه 10 عناصر. كام شجرة فيه وأحجامها إيه؟

1. [ ] 3 شجر: 4 و4 و2
2. [x] شجرتين: 8 و2
3. [ ] شجرة واحدة: 10

> 10 = 1010 بالـ binary، وكل bit = 1 = شجرة B<sub>k</sub> حجمها 2<sup>k</sup>.
:::

:::quiz
ليه heapsort مش stable؟

1. [ ] لأنه بيستعمل recursion
2. [x] لأن العنصر اللي في الجذر بيتنقل لآخر المصفوفة، ويعدّي فوق نسخ مساوية له
3. [ ] لأنه in-place

> B1 B2 A → A B2 B1 (تمرين 9.28): الـ B1 اتنقلت من الموقع 1 للموقع 3 على طول.
:::
