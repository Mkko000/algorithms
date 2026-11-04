تمارين محلولة · الفصل 11 (Special-Purpose Sorting Methods) · تمارين 11.1–11.48

الفصل الـ 11 فيه عالمين: الأول sorting networks (Batcher)، وده ترتيب «من غير if»: سلسلة compare-exchange ثابتة مهما كانت الداتا. والتاني external sorting، والداتا هنا مش هتكفّي الذاكرة والتكلفة الحقيقية هي عدد الـ passes على الديسك. التمارين بتمشي في الاتنين، وفيها مفاجأة صغيرة: Program 11.2 مابيشتغلش على مثال التمرين 11.8 زي ما هو.

:::goal هدف الدرس:
تتتبّع shuffle/unshuffle وBatcher merge بالإيد، وتثبت إن network بيرتّب باستعمال مبدأ 0-1، وتعدّ الـ comparators والـ parallel steps، وتحسب عدد الـ passes والـ runs في الـ external sort.
:::

:::tip مبدأ 0-1 هو سلاحك
عشان تثبت إن network فيه N خط بيرتّب، مش محتاج تجرّب الـ N! permutation. كفاية الـ 2<sup>N</sup> input اللي كلهم 0 و1 (Property 11.1). ولـ N = 6 ده 64 بدل 720، ولـ N = 16 ده 65536 بدل 20 تريليون.
:::

* [src/11-special-sorts/exercises/ex11\_01\_networks.c](src:src/11-special-sorts/exercises/ex11_01_networks.c)
* [src/11-special-sorts/exercises/ex11\_35\_external.c](src:src/11-special-sorts/exercises/ex11_35_external.c)

### تمرين 11.1 و11.5: shuffle، وstraight-line program لـ 3 عناصر

:::problem
(11.1) إيه نتيجة الـ shuffle والـ unshuffle للمفاتيح E A S Y Q U E S T I O N؟ (11.5) وريني إزاي الـ straight-line program اللي في الكتاب بيرتّب كل permutation من الـ 6 بتوع 1 و2 و3.
:::

:::think
الـ shuffle زي ما بتفنّط الكوتشينة: أول واحد من النص الشمال، وبعده أول واحد من اليمين، وهكذا. يعني EASYQU وESTION بيبقوا E E A S S T Y I Q O U N. والـ unshuffle عكسه: المواقع الزوجية تروح الشمال والفردية تروح اليمين. وفي 11.5 البرنامج compexch(0,1) وcompexch(1,2) وcompexch(0,1): أول اتنين بيزقّوا الأكبر للآخر زي bubble، والتالت بيرتّب أول اتنين.
:::

:::solution
```
11.1 shuffle   EASYQUESTION -> EEASSTYIQOUN
     unshuffle EASYQUESTION -> ESQETOAYUSIN
     shuffle(unshuffle(x))  -> EASYQUESTION
11.5 compexch(0,1) compexch(1,2) compexch(0,1) on the six permutations:
     123 -> 123 -> 123 -> 123
     132 -> 132 -> 123 -> 123
     213 -> 123 -> 123 -> 123
     231 -> 231 -> 213 -> 123
     312 -> 132 -> 123 -> 123
     321 -> 231 -> 213 -> 123
```

الـ 321 و231 بس هما اللي محتاجين التلات خطوات كلهم. وده المعنى الحقيقي لـ «straight-line»: البرنامج بيعمل التلات عمليات حتى لو الملف مترتب أصلًا (أول صف).
:::

### تمرين 11.6، 11.7 و11.16: networks صغيرة ومبدأ 0-1

:::problem
(11.6) اكتب straight-line program بيرتّب 4 عناصر. (11.7) اثبت Property 11.1 (مبدأ 0-1). (11.16) اعمل sorting networks لـ 4 و5 و6 عناصر بأقل عدد comparators ممكن.
:::

:::think
لـ 4: رتّب الزوجين (0,1) و(2,3)، وبعدين قارن الأصغرين (0,2) والأكبرين (1,3)، فيطلع الأصغر والأكبر في مكانهم، وفاضل comparator واحد (1,2) للنص. يعني 5. وهل 4 كفاية؟ لأ: 4 comparators بيدّوا 2⁴ = 16 مسار على الأكتر، والـ permutations 24. وفي 11.7 فكرة الإثبات: لو network فشل في input x، خد العنصر اللي اتحط في مكان غلط وحوّل كل حاجة ≥ قيمته لـ 1 وكل اللي أقل لـ 0. الـ comparators بتتعامل مع الـ 0/1 بنفس الطريقة بالظبط (الـ compexch بيحترم أي دالة monotone)، فهيفشل في الـ 0-1 input ده كمان.
:::

:::solution
```
11.6/11.16 networks (0-1 check / all-permutation check / parallel steps):
     N=4:  5 comparators  sorts / sorts / 3 steps
     N=5:  9 comparators  sorts / sorts / 6 steps
     N=6: 12 comparators  sorts / sorts / 6 steps
     any 4-comparator network for N=4? no (4! = 24 > 2^4 outcomes)
11.7 for the 12 networks 'N=6 minus one comparator', the 0-1 test and the 720-permutation test agree: yes
```

الـ 5 و9 و12 هما الأرقام المثالية المعروفة لـ N = 4 و5 و6. وكل network اتشيّك بالطريقتين: الـ 2<sup>N</sup> input بتوع 0-1، وكل الـ N! permutation، والاتنين اتفقوا. ولـ 11.7 عملنا اختبار أقوى: شيلنا comparator واحد من network الـ 6 بكل الـ 12 طريقة، فطلع عندنا networks «بايظة»، وبرضه الاختباران اتفقوا: لما الـ 0-1 بيقول «بايظ»، فيه permutation بتفضحه فعلًا.
:::

### تمرين 11.8–11.10: Batcher's odd-even merge

:::problem
(11.8) وريني إزاي المفاتيح `A E Q S U Y E I N O S T` بتتدمج بـ Program 11.2. (11.9) نفس السؤال للمفاتيح `A E S Y E I N O Q S T U`. (11.10) ونفس السؤال لـ `1 0 0 1 1 1 0 0 0 0 0 1 0 1 0 0`.
:::

:::think
Program 11.2 بيعمل unshuffle، وبعدين merge لكل نص recursively، وبعدين shuffle، وفي الآخر compare-exchange للأزواج (1,2) و(3,4) و… . بس خلّي بالك: الـ recursion بيفترض إن كل نص بعد الـ unshuffle عبارة عن نصين مترتبين **بنفس الحجم**. وده بيتحقق دايمًا بس لو الحجم قوة لـ 2. والـ 12 مش قوة لـ 2، و11.9 أصلًا نصّه 4 والتاني 8.
:::

:::solution
::figure ex11-batcher

```
     check on the 16-key example of Figure 11.2:
     input             AEGGIMNRABEELMPX
     compare-exchange  AABEEEGGILMMNPRX
11.8 Batcher merge (Program 11.2) of AEQSUY | EINOST, as is (N = 12):
     compare-exchange  AENOQESIUSYT          <- NOT sorted
     padded to 8 + 8:
     drop sentinels    AEEINOQSSTUY
11.9 AESY | EINOQSTU (unequal halves), padded to 8 + 8:
     shuffle           AEEINOQSSUTY~~~~
     compare-exchange  AEEINOQSSTUY~~~~
11.10 1001110000010100 (halves NOT sorted):
     compare-exchange  0000001010111010
```

ملاحظة أمينة: لو شغّلت Program 11.2 زي ما هو على الـ 12 مفتاح بتوع 11.8، الناتج **مش مترتب**. أول unshuffle بيدّي نصين حجم كل واحد 6، فيهم 3 + 3، والـ 3 بتتقسم 2 + 1، والتركيب بيبوظ. والحل المضمون: كمّل كل نص بـ sentinel أكبر من كل المفاتيح لحد الحجم 8، وبعدين اعمل الـ merge، وشيل الـ sentinels. ودي نفس الحيلة اللي بتحل 11.9 (4 + 8). وفي 11.10 النصين نفسهم مش مترتبين، فالـ merge بيطلّع حاجة مش مترتبة. الـ network «بيدمج»، مش «بيرتّب».
:::

### تمرين 11.17، 11.22 و11.23: عدّ الـ comparators والخطوات

:::problem
(11.17) اكتب برنامج يحسب عدد الـ parallel steps لأي straight-line program: كل خط بياخد label، والـ comparator بيدّي لخطّيه label = (أكبر label فيهم) + 1. (11.22) إيه عدد الـ comparators في networks Batcher لـ 2 ≤ N ≤ 32، لو الـ N اللي مش قوة لـ 2 بناخد لها أول N خط من network القوة اللي أكبر منها؟ (11.23) لـ N = 2<sup>n</sup>، استنتج صيغة مضبوطة، وراجعها على Figure 11.7 (1، 3، 9، 25، 65).
:::

:::think
network الـ sort لـ 2<sup>n</sup> = اتنين sort لـ 2<sup>n−1</sup> + merge. والـ merge M(n) = 2M(n−1) + (2<sup>n−1</sup> − 1)، وحلّها M(n) = (n−1)2<sup>n−1</sup> + 1، ودي بالظبط 1، 3، 9، 25، 65. يعني الشكل اللي في Figure 11.7 بيعدّ الـ merge networks. والـ sort S(n) = 2S(n−1) + M(n)، وحلّها S(n) = (n² − n + 4)·2<sup>n−2</sup> − 1.
:::

:::solution
```
11.22 comparators in Batcher's network on the first N lines of the next power of 2:
     2:1 3:3 4:5 5:9 6:12 7:16 8:19 9:28
     10:32 11:38 12:42 13:48 14:53 15:59 16:63 17:85
     18:90 19:98 20:103 21:112 22:119 23:127 24:132 25:140
     26:147 27:156 28:162 29:171 30:178 31:186 32:191
11.23 n=3 N= 8:  19 (formula  19)   merge   9 (formula   9)   parallel steps 6
      n=4 N=16:  63 (formula  63)   merge  25 (formula  25)   parallel steps 10
      n=5 N=32: 191 (formula 191)   merge  65 (formula  65)   parallel steps 15
      n=6 N=64: 543 (formula 543)   merge 161 (formula 161)   parallel steps 21
```

الصيغتين مظبوطين لحد N = 64. والـ parallel steps بتطلع n(n+1)/2، يعني 6 و10 و15 و21: كل merge بياخد n خطوة على التوازي. ودي أهم ميزة في Batcher: مليون عنصر بيترتّبوا في 210 خطوة parallel بس. ولاحظ إن network الـ N = 5 اللي في 11.16 فيه 6 خطوات، رغم إن الأمثل 5، لأن ترتيب الـ comparators بتاعنا مش أحسن ترتيب للتوازي.
:::

### تمرين 11.35–11.37: balanced multiway merging

:::problem
(11.35) وريني إزاي `E A S Y Q U E S T I O N W I T H P L E N T Y O F K E Y S` بتترتّب بـ 3-way balanced merging. (11.36) إيه تأثير مضاعفة عدد الأجهزة الخارجية على عدد الـ passes؟ (11.37) وإيه تأثير إن الذاكرة الداخلية تكبر 10 مرات؟
:::

:::think
الذاكرة بتشيل 3 records: نقرا 3، نرتّبهم، ونكتبهم run على جهاز من 3 بالدور. 28 مفتاح يعني 10 runs. وكل pass بيدمج 3 runs في واحد، فـ 10 → 4 → 2 → 1، يعني 3 passes. وبشكل عام: عدد الـ passes = 1 + ⌈log<sub>P</sub>(N/M)⌉. والـ P في الـ log base، والـ M جوه الـ log.
:::

:::solution
```
     initial runs (memory 3):
       dev 3: AES|INO|ENT|S
       dev 4: QUY|ITW|FOY
       dev 5: EST|HLP|EKY
     after merge pass 1:
       dev 0: AEEQSSTUY|S
       dev 1: HIILNOPTW
       dev 2: EEFKNOTYY
     after merge pass 2:
       dev 3: AEEEEFHIIKLNNOOPQSSTTTUWYYY
       dev 4: S
     after merge pass 3:
       dev 0: AEEEEFHIIKLNNOOPQSSSTTTUWYYY
11.36/11.37 passes = 1 + ceil(log_P(N/M)) for N = 10^9 records:
     M=1e+06:  P=2  -> 11  P=4  -> 6  P=8  -> 5  P=16 -> 4
     M=1e+07:  P=2  -> 8  P=4  -> 5  P=8  -> 4  P=16 -> 3
```

لاحظ الـ S اليتيمة: الـ run العاشر لوحده بيتنقل pass ورا pass لحد آخر merge، ودي تكلفة إن 10 مش قوة لـ 3. 11.36: مضاعفة الأجهزة (P → 2P) بتقسم الـ log على log₂(2P)/log₂P، فمن P = 2 لـ 4 الـ passes بتقل للنص تقريبًا (11 → 6)، وبعد كده الكسب بيقل. 11.37: الذاكرة × 10 بتطرح log<sub>P</sub>10 pass بس (حوالي 3 لـ P = 2، وواحد لـ P = 16). يعني الذاكرة مفيدة، بس الأجهزة (أو الـ P) أقوى.
:::

### تمرين 11.45–11.48: replacement selection

:::problem
(11.45) اديني الـ runs اللي بيطلّعها replacement selection بـ priority queue حجمها 4 للمفاتيح EASYQUESTION. (11.46) إيه تأثير replacement selection على ملف هو نفسه طالع من replacement selection؟ (11.47) اعرف بالتجربة متوسط عدد الـ runs بـ PQ حجمها 1000. (11.48) إيه أسوأ عدد runs بـ PQ حجمها M \< N؟
:::

:::think
طلّع الأصغر، وادخّل اللي بعده. لو الجديد ≥ اللي لسه طالع، يكمّل في نفس الـ run. ولو أصغر، يتعلّم «للـ run الجاي» (بنرتّب على (run, key)). على الملف العشوائي، كل عنصر جديد ليه فرصة كويسة يلحق الـ run الحالي، والنتيجة الشهيرة إن طول الـ run حوالي 2M. وأسوأ حالة: ملف نازل، لأن كل عنصر جديد أصغر من اللي طلع، فمابيلحقش أبدًا.
:::

:::solution
```
11.45 replacement selection, PQ of 4, on EASYQUESTION: 2 runs:  AEQSSTUY | EINO
11.46 run it again on its own output: 2 run(s):  AEQSSTUY | EINO
11.47 PQ of 1000, random files (average run length should be ~2M = 2000):
     N=   1000:    1 runs, average length 1000
     N=  10000:    6 runs, average length 1667
     N= 100000:   51 runs, average length 1961
     N=1000000:  501 runs, average length 1996
11.48 worst case: a file in DECREASING order:
     N=100000 M=  10: 10000 runs  (ceil(N/M) = 10000)
     N=100000 M=1000:   100 runs  (ceil(N/M) = 100)
```

11\.45: أول run طوله 8 (ضعف الـ PQ بالظبط!)، والباقي 4. 11.46: تشغيله تاني مابيكسبش حاجة. الـ runs زي ما هي، لأن عند كل حدود run الأكبر (Y) جه قبل الأصغر (E)، ومافيش PQ حجمها 4 تقدر تعدّي فرق بالطول ده. 11.47: الـ 2M ظهرت بوضوح (1996). 11.48: الملف النازل بيرجّعنا لـ N/M runs، زي ما لو كنا بنرتّب M ورا M من غير أي ذكاء. يعني replacement selection مابيضرّش أبدًا، وفي المتوسط بيقسم عدد الـ runs على 2.
:::

:::quiz
عشان تثبت إن network فيه 10 خطوط بيرتّب أي input، أقل عدد inputs محتاج تجرّبه؟

1. [ ] 10! = 3,628,800
2. [x] 2¹⁰ = 1024 (0 و1 بس)
3. [ ] 10² = 100

> مبدأ 0-1 (Property 11.1): لو بيرتّب كل الـ 0-1 inputs، يبقى بيرتّب أي حاجة.
:::

:::quiz
Program 11.2 (Batcher merge) على نصين حجم كل واحد 6. النتيجة؟

1. [ ] دايمًا صح
2. [x] ممكن تطلع غلط؛ لازم نكمّل لقوة لـ 2 (بـ sentinels مثلًا)
3. [ ] البرنامج بيقف بـ error

> على AEQSUY | EINOST طلع AENOQESIUSYT. والـ recursion محتاج نصين متساويين في كل مستوى.
:::

:::quiz
replacement selection بـ PQ حجمها M على ملف random. طول الـ run في المتوسط؟

1. [ ] M
2. [x] حوالي 2M
3. [ ] M lg M

> التجربة: 1996 لـ M = 1000. وده بيقسم عدد الـ runs على 2، وده ممكن يوفّر pass كامل.
:::

:::quiz
external sort لـ 10⁹ record بذاكرة 10⁶ وP = 2. إيه اللي يوفّر passes أكتر؟

1. [x] إننا نخلّي P = 4 (أجهزة أكتر)
2. [ ] إن الذاكرة تبقى 10⁷
3. [ ] الاتنين نفس الشيء

> P = 4: من 11 لـ 6 passes. والذاكرة × 10: من 11 لـ 8 بس.
:::
