تمارين محلولة · الفصل 6 (Elementary Sorting Methods) · تمارين 6.5–6.74

تمارين الفصل السادس شكلها سهل («اتتبّع selection sort على الملف ده»)، بس جواها أسئلة مهمة جدًا: مين stable ومين لأ ولييه، وإمتى الـ elementary sort بيكسب، وإزاي shellsort ممكن يبقى كارثة لو اخترت الـ increments غلط. وكل الأرقام هنا متعدّة بالبرامج، مش متخمّنة.

:::goal هدف الدرس:
تتتبّع الـ sorts الأساسية بإيدك على أي ملف، وتحكم على الـ stability بمثال مضاد، وتعرف تختار الـ sort المناسب حسب شكل الداتا.
:::

:::tip طريقة الحكم على الـ stability
عشان تثبت إن sort **مش** stable، مثال واحد كفاية: مفتاحين متساويين نط واحد فيهم فوق التاني. وعشان تثبت إنه stable، لازم تبيّن إن ولا خطوة في الكود بتعدّي عنصر فوق عنصر مساوي له.
:::

* [src/06-sorting/exercises/ex6\_05\_compexch.c](src:src/06-sorting/exercises/ex6_05_compexch.c)
* [src/06-sorting/exercises/ex6\_11\_elementary.c](src:src/06-sorting/exercises/ex6_11_elementary.c)
* [src/06-sorting/exercises/ex6\_33\_shellsort.c](src:src/06-sorting/exercises/ex6_33_shellsort.c)
* [src/06-sorting/exercises/ex6\_57\_index\_kic.c](src:src/06-sorting/exercises/ex6_57_index_kic.c)

### تمرين 6.5 و6.6: compare-exchange بس

:::problem
(6.5) اكتب كل الـ sequences اللي فيها تلات عمليات compare-exchange وبترتّب 3 عناصر (Program 6.1). (6.6) اديني sequence من 5 عمليات compare-exchange بترتّب 4 عناصر.
:::

:::think
كل عملية هي اختيار زوج (i, j). لـ 3 عناصر فيه 3 أزواج، يعني 3³ = 27 sequence بس. جرّبهم كلهم على كل الـ 6 permutations. ولـ 4 عناصر فيه 6 أزواج، يعني 6⁵ = 7776 sequence × 24 permutation، وده لسه صغير. والسؤال الأحلى: هل 4 عمليات بتكفي لـ 4 عناصر؟ فكّر: 4 عمليات كل واحدة بنتيجتين = 16 سيناريو على الأكتر، والـ permutations 24. يعني مستحيل.
:::

:::solution
```
6.5: sequences of 3 compare-exchanges that sort 3 elements:
    1:2 0:2 0:1      0:2 0:1 1:2
    0:1 1:2 0:1      1:2 0:1 1:2
    0:2 1:2 0:1      0:1 0:2 1:2
   total: 6 sequences
6.6: e.g.  0:1 2:3 1:3 0:2 1:2     (12 sequences in all)
    four compare-exchanges suffice for 4 elements? no (5 is optimal)
```

الـ 6 بتوع 6.5 كلهم بيعملوا نفس الفكرة: أول عمليتين بيحطّوا الأكبر (أو الأصغر) في طرفه، والتالتة بترتّب الاتنين الباقيين. وفي 6.6: رتّب زوجين (0:1، 2:3)، وبعدين قارن الأكبرين (1:3) والأصغرين (0:2)، وبعدين النص (1:2). دي «sorting network» صغيرة، وهترجع لها في الفصل 11.
:::

### تمرين 6.11، 6.15 و6.20: الثلاثة على EASYQUESTION

:::problem
وريني بنفس طريقة Figures 6.2 و6.3 و6.4 إزاي selection sort وinsertion sort وbubble sort بيرتّبوا الملف `E A S Y Q U E S T I O N`.
:::

:::think
في selection: كل سطر بيحط الأصغر في الخانة i. يعني السطر i دايمًا شكله «أصغر i+1 عنصر بالترتيب + الباقي». في insertion (Program 6.3): خد بالك إن أول خطوة هي لفّة الـ sentinel اللي بتجيب A للأول. في bubble: كل لفّة من اليمين للشمال.
:::

:::solution
::figure ex6-sel

::figure ex6-ins

::figure ex6-bub

```
selection: 66 compares, 11 exchanges, stable: no
insertion: 44 compares,               stable: yes
bubble:    66 compares, 31 exchanges, stable: yes
```
:::

### تمرين 6.14، 6.18، 6.22 و6.33: مين stable؟

:::problem
هل selection sort stable؟ insertion sort؟ bubble sort؟ shellsort؟
:::

:::think
بص على نوع الحركة في كل واحد. insertion وbubble بيحرّكوا العناصر خطوة خطوة بين **جيران**، وبيوقفوا عند المساوي (الشرط `less` مش `≤`). فمستحيل عنصر يعدّي جاره المساوي. أما selection وshellsort فبيعملوا **قفزات بعيدة**، والقفزة ممكن تعدّي فوق نسخة مساوية.
:::

:::solution
**selection: لأ.** في EASYQUESTION، السطر 2 بيبدّل الـ S اللي في الخانة 2 مع الـ E اللي في الخانة 6، فالـ S الأولى اتنقلت *بعد* الـ S التانية (اللي في 7). **insertion وbubble: أيوه**، والبرنامج اتأكّد. **shellsort: لأ**:

```
6.35 shellsort 13 4 1 on EASYQUESTION (E1/E2, S1/S2 = first/second copy):
    4-sorted: E1 A E2 N Q I O S2 T U S1 Y
    1-sorted: A E1 E2 I N O Q S2 S1 T U Y
6.33 stable? no (S2 ends before S1: the 4-sort jumped it over its twin)
```

الـ 4-sort رتّب كل متتابعة لوحدها (الخانات 2، 6، 10 فيهم S1 وE2 وO)، فـ S1 راحت للخانة 10، و S2 (في الخانة 7، متتابعة تانية) فضلت مكانها. والـ 1-sort الأخير stable، بس الضرر حصل خلاص.
:::

### تمرين 6.12 و6.13: selection sort بالأرقام

:::problem
(6.12) إيه أقصى عدد exchanges ممكن عنصر واحد يشارك فيه في selection sort؟ وإيه المتوسط؟ (6.13) اديني ملف فيه N عنصر بيخلّي الشرط `less(a[j], a[min])` ينجح أكبر عدد مرات (يعني `min` يتحدّث أكتر حاجة).
:::

:::think
6\.12: فيه N−1 exchange بالظبط، وكل exchange فيه عنصرين، فالمتوسط حوالي 2. والأقصى؟ عايز عنصر «يتزق» في كل لفّة. حط الأكبر في الأول: لفّة 0 هتبدّله مع الأصغر، فيروح للخانة 1. لفّة 1 هتبدّله تاني… 6.13: الـ min بيتحدّث كل ما يلاقي حاجة أصغر، يعني الأحسن إن الجزء الباقي يبقى تنازلي. بس خد بالك إن الـ exchanges بتبوّظ الترتيب ده بعد كل لفّة. فجرّب exhaustive على N صغير.
:::

:::solution
```
6.12 selection N=1000 random: max exchanges for one element 15, average 2.00
6.12 file 10 1 2 ... 9: key 10 takes part in 9 of the 9 exchanges (= N-1)
6.13 N=8: max min-updates 16 (reverse order gives 16), e.g. 8 7 6 5 4 1 3 2
6.13 N=9: max min-updates 20 (reverse order gives 20), e.g. 9 8 7 6 4 3 2 1 5
```

6\.12: الأقصى **N−1** (الملف N 1 2 … N−1)، والمتوسط 2(N−1)/N ≈ 2. 6.13: البحث على كل الـ 8! و9! permutation بيقول إن الأقصى **⌊N/2⌋·⌈N/2⌉ ≈ N²/4**، والملف المعكوس بيوصله. بس فيه ملفات تانية بتوصله كمان. والمغزى: حتى في أسوأ حالة، الـ updates (اللي هي assignment رخيص) ربع المقارنات بس.
:::

### تمرين 6.24 و6.30: bubble بخروج مبكّر، وshaker sort

:::problem
(6.24) اعمل تجارب تعرف بيها كام لفّة بنوفّر في المتوسط لما نضيف لـ bubble sort اختبار يوقّفه لو الملف اترتّب. (6.30) اعمل نسخة بتبدّل اتجاه اللفّات (شمال←يمين ويمين←شمال): shaker sort.
:::

:::think
bubble بيخلص لما آخر عنصر «تايه» يوصل مكانه، وكل لفّة بتحرّك العنصر الكبير خطوة واحدة بس لليمين. فلو فيه عنصر صغير قريب من الآخر، أو كبير قريب من الأول، هتحتاج لفّات كتير. وفي ملف عشوائي، غالبًا فيه عنصر كده. فتوقّع إن التوفير صغير. والـ shaker بيعالج المشكلة دي: الكبير بيتحرك بسرعة في لفّة الشمال←يمين.
:::

:::solution
```
6.24 bubble N=100:  early exit saves 10.1 of 99 passes on average
6.24 bubble N=1000: early exit saves 42.2 of 999 passes on average
6.30 random N=1000, both with early exit: bubble 498743 compares/248365 exch,
                                          shaker 378472 compares/248365 exch
```

التوفير حوالي √N لفّة بس (10 من 99، و42 من 999)، يعني مالوش قيمة في الملفات العشوائية. والـ shaker وفّر حوالي 24% من المقارنات. بس عدد الـ exchanges **نفسه بالظبط** (248365) في الاتنين، لأن أي sort بيبدّل جيران بس لازم يعمل exchange لكل inversion، لا أكتر ولا أقل.
:::

### تمرين 6.26، 6.27 و6.28: مين أسرع امتى؟

:::problem
مين أسرع واحد في الثلاث طرق لملف كل مفاتيحه متساوية (6.26)؟ ولملف معكوس (6.27)؟ (6.28) اديني ملف من 10 عناصر (A لـ J) bubble sort بيعمل فيه مقارنات أقل من insertion sort، أو أثبت إنه مش موجود.
:::

:::think
insertion (Program 6.3) adaptive: بيوقف أول ما يلاقي اللي قبله ≤ منه. selection وbubble (Program 6.4) مش adaptive: بيعملوا N(N−1)/2 مقارنة دايمًا. و6.28 فيها فخ: bubble ثابت عند 45 مقارنة لـ N = 10. فالسؤال الحقيقي: هل insertion ممكن يعدّي 45؟ أيوه، لأن لفّة الـ sentinel (N−1 مقارنة) بتتضاف على الـ inversions.
:::

:::solution
```
6.26 all keys equal, N=1000:    selection 499500c/999x  insertion 1997c/0x  bubble 499500c/0x
6.27 reverse order, N=1000:     selection 499500c/999x  insertion 500498c   bubble 499500c/499500x
6.28 file J I H G F E D C B A: bubble 45 compares, insertion 53 compares
```

6\.26: **insertion** بفرق كبير (2N مقارنة، خطي). 6.27: **selection**، لأنه بيعمل N exchange بس، والباقيين بيعملوا N²/2 حركة. 6.28: الملف المعكوس نفسه: insertion = 9 (sentinel) + 8 + 36 inversion = 53 \> 45. يعني «insertion دايمًا أحسن من bubble» مش صح 100%.
:::

### تمرين 6.36: shellsort بقوى 2 = كارثة

:::problem
لاقي وقت تشغيل shellsort بالـ increments 1 2 4 8 16 32 … على ملف فيه الأرقام 1…N في المواقع الفردية، وN+1…2N في المواقع الزوجية.
:::

:::think
كل increment زوجي (2، 4، 8، …) بيقارن مواقع ليها نفس الـ parity بس. والمواقع الفردية مترتبة أصلًا، والزوجية كمان! يعني كل اللفّات دي مابتعملش حاجة. والـ 1-sort الأخير بيلاقي الملف 1، N+1، 2، N+2، … وفيه حوالي N²/2 inversion. فالشغل كله على insertion sort عادي.
:::

:::solution
```
6.36 2N=  512: powers of 2 ->    36737 compares (N(N-1)/2 =    32640),  1 4 13 40... ->   3993
6.36 2N= 2048: powers of 2 ->   544257 compares (N(N-1)/2 =   523776),  1 4 13 40... ->  21727
6.36 2N= 4096: powers of 2 ->  2141185 compares (N(N-1)/2 =  2096128),  1 4 13 40... ->  44716
```

**تربيعي**: حوالي N²/2. والـ increments بتاعة Knuth على نفس الملف أسرع حوالي 48 مرة عند 4096. والدرس: الـ increments لازم «تخلط» الـ parity. وده سبب إن سادجوك بيحذّر من increments بينهم عوامل مشتركة.
:::

### تمرين 6.39: إيه أحسن بديل لـ 13؟

:::problem
اعمل تجارب تحدد قيمة x اللي تدّي أقل وقت تشغيل لملفات عشوائية (N = 10000)، لو حطّيت x مكان 13 في 1 4 13 40 121 364 1093 3280 9841.
:::

:::think
جرّب كل x من 5 لـ 39 على نفس الـ 5 ملفات (عشان المقارنة تبقى عادلة)، وعدّ المقارنات. وتوقّع إن المنحنى «مسطّح»، لأن increment واحد من 9 مش هيفرق كتير.
:::

:::solution
```
6.39 x : 5:262708 9:230202 13:234439 17:218444 21:229108 25:219301 29:223351 33:228088 37:239943
     best x = 15 with 217606 compares per file (x = 13 is the standard)
```

أحسن x في التجربة دي 15 (أقل من 13 بحوالي 7%)، بس الأرقام بتتذبذب (17 كويس و21 أوحش). يعني الفرق قريب من ضوضاء العيّنة. الأمانة تقتضي إننا نقول: 13 مش «سحري»، وأي قيمة بين 13 و25 تقريبًا كويسة. وهي أي حاجة بعيدة عن 4 و40.
:::

### تمرين 6.43: (h−1)(k−1)

:::problem
أثبت إن أي رقم ≥ (h−1)(k−1) ممكن يتكتب كـ combination خطية بمعاملات غير سالبة من h وk، لو h وk أوّليين فيما بينهم.
:::

:::think
التلميح في الكتاب: أول h مضاعف لـ k (0، k، 2k، …، (h−1)k) بواقي قسمة مختلفة على h (لو اتنين ليهم نفس الباقي، يبقى h بيقسم الفرق، وده مستحيل لأن gcd = 1). يعني كل باقي من 0 لـ h−1 بيظهر مرة واحدة. فأي v ≥ (h−1)k − (h−1) ليه مضاعف jk ≤ v بنفس الباقي، وv − jk مضاعف لـ h.
:::

:::solution
خد v ≥ (h−1)(k−1)، واختار j ∈ \[0, h−1\] بحيث jk ≡ v (mod h) (موجود ووحيد من التلميح). لو v ≥ jk، يبقى v = jk + ih بـ i ≥ 0، وخلصنا. ولو v \< jk، يبقى v − jk مضاعف *سالب* لـ h، يعني v ≤ jk − h ≤ (h−1)k − h = (h−1)(k−1) − 1 \< v، وده تناقض. ∎ والحد ده مضبوط: (h−1)(k−1) − 1 = hk − h − k عمره ما بيتكتب كده (ده رقم Frobenius). والبرنامج اتأكّد لكل h \< k ≤ 41:

```
6.43 every v >= (h-1)(k-1) representable: yes; (h-1)(k-1)-1 never is: yes  (h<k<=41)
```

وده بيستخدموه في تحليل shellsort: بعد h-sort وk-sort، كل عنصر بيبقى أكبر من اللي على بُعد ≥ (h−1)(k−1) منه، فالـ 1-sort بيلاقي inversions قليلة.
:::

### تمرين 6.58 و6.59: index sort وتطبيق الـ permutation

:::problem
(6.58) اكتب الـ index array اللي بينتج لما المفاتيح `E A S Y Q U E S T I O N` تترتّب بالـ index sort. (6.59) اكتب سلسلة نقلات الداتا اللازمة عشان نرتّب المفاتيح in place بعد كده (Program 6.14).
:::

:::think
a\[i\] = مكان المفتاح اللي ترتيبه i. وفي 6.59: الـ permutation بيتقسم **cycles**. ولكل cycle: احفظ أول قيمة، وانقل القيم على طول الـ cycle، وحط المحفوظة في الآخر. يعني كل cycle طوله L بيكلّف L+1 نقلة، والـ fixed points (a\[i\] = i) ببلاش.
:::

:::solution
::figure ex6-index

```
   data[0] = data[1]  (A)
   data[1] = E (the saved key)                   cycle (0 1)
   data[2] = data[6]  (E)
   data[6] = data[4]  (Q)
   data[4] = data[11] (N)
   data[11] = data[3] (Y)
   data[3] = data[9]  (I)
   data[9] = data[8]  (T)
   data[8] = data[7]  (S)
   data[7] = S (the saved key)                   cycle (2 6 4 11 3 9 8 7)
   data[5] = data[10] (O)
   data[10] = U (the saved key)                  cycle (5 10)
   12 moves, result AEEINOQSSTUY
```

تلات cycles، طولهم 2 و8 و2، ومفيش fixed points. كل cycle طوله L بياخد L نقلة (L−1 نسخ + كتابة القيمة المحفوظة)، يعني 2 + 8 + 2 = **12 نقلة = N** (من غير عدّ الـ «احفظ» اللي في أول كل cycle). وفي العموم العدد N − (عدد الـ fixed points). والمهم: كل سجل بيتنقل **مرة واحدة** بس. ودي ميزة كبيرة لما السجلات تقيلة.
:::

### تمرين 6.72–6.74: key-indexed counting

:::problem
(6.72) اعمل نسخة متخصصة من key-indexed counting لملفات فيها 3 قيم بس (a وb وc). (6.73) لو استخدمنا insertion sort على ملف عشوائي فيه 3 قيم بس، الوقت خطي ولا تربيعي ولا بين الاتنين؟ (6.74) وريني إزاي key-indexed counting بيرتّب ABRACADABRA.
:::

:::think
6\.72: لو عرفت إن فيه 6 a و6 b، اكتب 6 a وبعدين 6 b وبعدين الباقي c. ومفيش داعي لمصفوفة تانية (بس ده بيشتغل للمفاتيح نفسها، مش لسجلات كاملة). 6.73: عدّ الـ inversions. أي زوج (i\<j) احتمال إنه inversion = P(x<sub>i</sub> \> x<sub>j</sub>) = 3/9 = 1/3. يعني حوالي (1/3)(N²/2) = N²/6.
:::

:::solution
::figure ex6-kic

```
6.72 counts a=6 b=6 c=5 -> aaaaaabbbbbbccccc
6.73 N= 1000:    171346 compares  (N^2/6 =    166667)
6.73 N= 4000:   2612287 compares  (N^2/6 =   2666667)
6.73 N=16000:  42633035 compares  (N^2/6 =  42666667)
```

6\.73: **تربيعي**، N²/6 بالظبط تقريبًا. يعني قلة القيم مابتنقذش insertion sort، بس key-indexed counting بيعمل الشغل في 2N خطوة. والفرق بين 42 مليون مقارنة و32 ألف خطوة هو قيمة إنك تعرف حاجة عن مفاتيحك.
:::

:::quiz
أنهي sort فيهم stable؟

1. [ ] selection sort
2. [x] insertion sort
3. [ ] shellsort

> insertion بيحرّك بين جيران وبيوقف عند المساوي. selection وshellsort بيعملوا قفزات بعيدة ممكن تعدّي نسخة مساوية.
:::

:::quiz
ملف فيه 10⁵ عنصر وكل المفاتيح متساوية. أسرع elementary sort؟

1. [ ] selection
2. [x] insertion
3. [ ] bubble (Program 6.4)

> insertion خطي هنا (حوالي 2N مقارنة)، والاتنين التانيين N²/2 مهما كانت الداتا.
:::

:::quiz
ليه shellsort بقوى 2 (… 8 4 2 1) ممكن يبقى تربيعي؟

1. [ ] لأن الـ increments صغيرة
2. [x] لأن كل الـ increments ماعدا 1 زوجية، فمابتقارنش أبدًا موقع فردي بموقع زوجي
3. [ ] لأن فيها increments كتير

> في تمرين 6.36 كل اللفّات قبل الأخيرة مابتعملش أي حاجة، والـ 1-sort بيدفع N²/2.
:::

:::quiz
bubble sort وshaker sort على نفس الملف. عدد الـ exchanges؟

1. [ ] shaker أقل
2. [x] متساوي بالظبط (= عدد الـ inversions)
3. [ ] bubble أقل

> كل exchange بين جيران بيصلّح inversion واحد بالظبط. الـ shaker بيوفّر مقارنات بس.
:::
