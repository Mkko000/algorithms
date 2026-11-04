تمارين محلولة · الفصل 14 (Hashing) · تمارين 14.5–14.43

الـ hashing بيكسب على كل الأشجار في الـ search المتوسط، بس ليه تمنين: الـ worst case ممكن يبقى كارثة، والجدول لازم يتقاس صح. أغلب تمارين الفصل «حط المفاتيح دي في جدول بالطريقة دي»، وده اللي بيخلّيك تحس بالـ clustering. ومعاها أسئلة «كام في أسوأ حالة؟» و«إمتى نكبّر الجدول؟». والـ hash في كل الأمثلة هو 11k mod M، وk = رقم الحرف (A = 1).

:::goal هدف الدرس:
تحط مفاتيح بالإيد في separate chaining وlinear probing وdouble hashing، وتشوف الـ clustering بعينك، وتختبر الـ hash function بالـ χ²، وتختار امتى وبكام تكبّر الجدول.
:::

:::tip احسب الـ hashes الأول
قبل ما تبدأ أي تتبّع، اكتب سطر فيه h(key) لكل مفتاح. ساعتها هتشوف الـ collisions من قبل ما تحصل (مثلًا E وY وT وO كلهم 0 لما M = 5)، والتتبّع يبقى نقل وخلاص.
:::

* [src/14-hashing/exercises/ex14\_17\_chaining.c](src:src/14-hashing/exercises/ex14_17_chaining.c)
* [src/14-hashing/exercises/ex14\_25\_probing.c](src:src/14-hashing/exercises/ex14_25_probing.c)
* [src/14-hashing/exercises/ex14\_40\_dynamic.c](src:src/14-hashing/exercises/ex14_40_dynamic.c)

### تمرين 14.17 و14.18: separate chaining بـ M = 5

:::problem
(14.17) إيه محتوى الـ hash table بعد ما ندخّل E A S Y Q U T I O N بالترتيب ده في جدول فاضي فيه M = 5 lists، بـ separate chaining وlists مش مترتبة؟ استعمل الـ hash function 11k mod M للحرف رقم k. (14.18) نفس السؤال بـ lists مترتبة. هل الإجابة بتعتمد على ترتيب الإدخال؟
:::

:::think
E = 5 → 55 mod 5 = 0. A = 1 → 11 mod 5 = 1. وهكذا لكل مفتاح. وبما إن 11 mod 5 = 1، يبقى h = k mod 5 ببساطة. Program 14.3 بيضيف في **أول** الـ list، فالـ list غير المترتبة بتطلع بالعكس: آخر واحد دخل في الأول. والمترتبة شكلها متحدد بالمجموعة نفسها، مش بالترتيب.
:::

:::solution
```
14.17 hash values 11k mod 5: E=0 A=1 S=4 Y=0 Q=2 U=1 T=0 I=4 O=0 N=4
     unordered lists (insert at the front, Program 14.3):
       0: O T Y E
       1: U A
       2: Q
       3:
       4: N I S
14.18 ordered lists:
       0: E O T Y
       1: A U
       2: Q
       3:
       4: I N S
     inserting in reverse order gives the same table: yes
```

List رقم 0 فيها 4 من 10، وList رقم 3 فاضية خالص. يعني 5 lists لـ 10 مفاتيح والتوزيع بعيد عن المثالي، بس ده طبيعي مع أرقام صغيرة كده. وفي 14.18 الجواب «لأ»: الـ list المترتبة ليها شكل واحد بس لأي مجموعة مفاتيح، وجرّبنا الترتيب المعكوس فطلع نفس الجدول بالظبط.
:::

### تمرين 14.16 و14.24: أسوأ حالة

:::problem
(14.16) الـ insert لـ N مفتاح في جدول فاضي ممكن ياخد قد إيه في أسوأ حالة، بـ separate chaining و(i) lists مش مترتبة، (ii) lists مترتبة؟ (14.24) ونفس السؤال لو بنستعمل linear probing.
:::

:::think
أسوأ حالة إن كل المفاتيح تيجي على نفس الـ hash. ساعتها الـ list المترتبة (أو cluster الـ linear probing) لازم تتعدّي لحد آخرها مع كل insert: 1 + 2 + … + N ≈ N²/2. أما الـ list غير المترتبة فالـ insert في الأول مابيعملش ولا مقارنة. بس لو لازم نتأكد إن المفتاح مش موجود (search قبل الـ insert)، هنرجع لـ N²/2.
:::

:::solution
```
14.16 worst case (all N keys hash to the same list):
     N= 1000: unordered 0 compares (front insert; 499500 with a search-before-insert)   ordered 499500 (N^2/2 = 500000)
     N= 2000: unordered 0 compares (front insert; 1999000 with a search-before-insert)   ordered 1999000 (N^2/2 = 2000000)
     N= 4000: unordered 0 compares (front insert; 7998000 with a search-before-insert)   ordered 7998000 (N^2/2 = 8000000)
     N= 8000: unordered 0 compares (front insert; 31996000 with a search-before-insert)   ordered 31996000 (N^2/2 = 32000000)
14.24 worst case: every key hashes to the same slot:
     N= 1000:    500500 probes (N(N+1)/2 = 500500)
     N= 2000:   2001000 probes (N(N+1)/2 = 2001000)
     N= 4000:   8002000 probes (N(N+1)/2 = 8002000)
     N= 8000:  32004000 probes (N(N+1)/2 = 32004000)
```

الخلاصة: الـ hashing في أسوأ حالة = sequential search. والحماية الوحيدة إن الـ hash function «تبان عشوائية»، أو إننا نختارها عشوائي (universal hashing)، عشان مافيش حد يقدر يبني الملف الوحش ده عن قصد.
:::

### تمرين 14.5 و14.19: هل الـ hash function كويسة؟

:::problem
(14.5) اكتب برنامج يحسب الـ χ² للـ hash values بتاعة N مفتاح في جدول حجمه M: χ² = (M/N) Σ (f<sub>i</sub> − N/M)². لو الـ hash عشوائي، القيمة لازم تبقى M ± 2√M. (14.19) دخّل N رقم عشوائي في جدول حجمه N/100 بـ separate chaining، ولاقي أقصر وأطول list.
:::

:::think
كل list طولها المتوقع 100. والسؤال: الطولين المتطرفين بيبعدوا عن 100 قد إيه؟ الطول تقريبًا Poisson متوسطه 100 وانحرافه 10، فمع آلاف الـ lists المتطرفين هيبقوا على بعد 3–4 انحرافات. والـ χ² بيلخّص ده في رقم واحد. ونجرّب كمان hash وحش عن قصد (مفاتيح مضروبة في 10 وM = 100) عشان نشوف شكله.
:::

:::solution
```
14.19 N random keys, M = N/100 lists (average length 100):
     N=   1000: M=   10  shortest  81  longest 112   chi^2 =      8.4 (expect 10 +- 6)  (14.5)
     N=  10000: M=  100  shortest  80  longest 129   chi^2 =    106.6 (expect 100 +- 20)  (14.5)
     N= 100000: M= 1000  shortest  62  longest 135   chi^2 =   1023.7 (expect 1000 +- 63)  (14.5)
     N=1000000: M=10000  shortest  63  longest 138   chi^2 =  10076.1 (expect 10000 +- 200)  (14.5)
14.5 bad hash (keys multiples of 10, M = 100): chi^2 = 900096, far outside 100 +- 20
```

حتى مع hash ممتاز، أطول list أطول من المتوسط بحوالي 38%. ده العادي، ومش معناه إن الـ hash وحش. والـ χ² دايمًا جوه المدى. أما الـ hash الوحش فطلّع 900 ألف بدل 100: كل المفاتيح وقعت في 10 lists بس من الـ 100 (اللي رقمها مضاعف 10). والـ χ² كشفها في سطر واحد.
:::

### تمرين 14.25، 14.26، 14.31 و14.32: open addressing

:::problem
(14.25) إيه محتوى الـ hash table بعد ما ندخّل E A S Y Q U T I O N بالترتيب ده في جدول فاضي حجمه M = 16، بـ linear probing؟ استعمل 11k mod M للحرف رقم k. (14.26) نفس السؤال لـ M = 10. (14.31) نفس المفاتيح بـ double hashing وM = 16: أول probe بـ 11k mod M، والخطوة (k mod 3) + 1. (14.32) نفس السؤال لـ M = 10.
:::

:::think
linear probing: لو الخانة مليانة، جرّب اللي بعدها، ولو وصلت للآخر لف من الأول. double hashing: نفس الفكرة بس بنمشي بخطوة خاصة بكل مفتاح. ولـ M = 10 خلّي بالك: الخطوة 2 بتزور المواقع الزوجية بس (أو الفردية بس)، يعني نص الجدول. والجدول هنا هيتملي كله (10 مفاتيح في 10 خانات)، فممكن مفتاح مايلاقيش مكان. البرنامج بيتشيّك على ده.
:::

:::solution
::figure ex14-probe

```
14.25 linear probing, M = 16:  hashes E=7 A=11 S=1 Y=3 Q=11 U=7 T=12 I=3 O=5 N=10
     table: . S . Y I O . E U . N A Q T . .   (14 probes)
14.26 linear probing, M = 10:  hashes E=5 A=1 S=9 Y=5 Q=7 U=1 T=0 I=9 O=5 N=4
     table: T A U I N E Y Q O S   (19 probes)
14.31 double hashing, M = 16 (steps E=3 A=2 S=2 Y=2 Q=3 U=1 T=3 I=1 O=1 N=3):
     table: . S . Y I O . E U . N A T . Q .   (13 probes)
14.32 double hashing, M = 10:
     table: Q A U T I E O Y N S   (28 probes)
```

لـ M = 10 الجدول اتملى خالص، والـ probes قفزت: 19 للـ linear و28 للـ double. وفي 14.32 الدنيا عدّت بالحظ: كل مفتاح خطوته زوجية لقى مكان فاضي في «نصه» من الجدول. لو كان فيه مفتاح كمان بخطوة 2 ونصه اتملى، كان هيلف للأبد (أو البرنامج يقول «CANNOT be inserted»). عشان كده الكتاب بيصرّ إن الخطوة والـ M يبقوا relatively prime، والطريقة المضمونة إن M يبقى prime.
:::

### تمرين 14.28: تكلفة الـ search miss من الـ clusters

:::problem
اكتب برنامج يدخّل M/2 رقم عشوائي في جدول حجمه M بـ linear probing، وبعدين يحسب متوسط تكلفة الـ search miss في الجدول ده من أطوال الـ clusters، لـ M = 10³ لحد 10⁶.
:::

:::think
الـ miss اللي بيبدأ في خانة فاضية بيكلّف probe واحد. واللي بيبدأ جوه cluster بيكلّف (المسافة لآخر الـ cluster) + 1. فلو عدّينا على كل الخانات وجمعنا، هيطلع المتوسط بالظبط من غير ما نعمل searches. والنظرية (Property 14.4) بتقول (1 + 1/(1−α)²)/2 = 2.5 لما α = ½.
:::

:::solution
```
14.28 M/2 random keys (alpha = 1/2); search miss cost from clusters vs the formula (1 + 1/(1-a)^2)/2 = 2.5:
     M=   1000: average miss 2.378 probes (insert cost so far 1.520 per key)
     M=  10000: average miss 2.445 probes (insert cost so far 1.492 per key)
     M= 100000: average miss 2.504 probes (insert cost so far 1.505 per key)
     M=1000000: average miss 2.499 probes (insert cost so far 1.501 per key)
```

النظرية مظبوطة لما M تبقى كبيرة (2.499). وتكلفة الـ insert في المتوسط 1.5، ودي بالظبط (1 + 1/(1−α))/2 للـ hit. ولاحظ إن الـ miss أغلى من الـ hit بكتير حتى عند α = ½. ولو α وصلت 0.9، الـ miss هيوصل لـ 50 probe.
:::

### تمرين 14.40–14.43: الجدول اللي بيكبر لوحده

:::problem
(14.40) إيه محتوى الـ hash table بعد ما ندخّل E A S Y Q U T I O N في جدول حجمه الأول M = 4، وبيتضاعف كل ما يتملي النص، والـ collisions بالـ linear probing؟ استعمل 11k mod M. (14.41) هل أوفر إننا نضرب الحجم في 3 (بدل 2) لما يتملي النص؟ (14.42) هل أوفر إننا نضرب في 3 لما يتملي التلت؟ (14.43) هل أوفر إننا نضاعف لما يتملي التلات أرباع؟
:::

:::think
كل مرة الجدول يكبر، لازم نعمل rehash لكل مفتاح بالـ M الجديدة، فالمواقع بتتغير خالص. ولـ 14.41–14.43 فيه تمنين بيتعارضوا: الـ rehashing (كل ما نكبّر بمعامل أكبر، بنكبّر مرات أقل)، والـ probes (كل ما الجدول أفضى، الـ probe أرخص). والتالت هو الذاكرة. فلازم نقيس التلاتة مع بعض.
:::

:::solution
```
14.40 start M=4, double when half full:
     +E  M= 4: . . . E
     +A  M= 8: . . . A . . . E
          (grew 4 -> 8, everything rehashed with 11k mod 8)
     +S  M= 8: . S . A . . . E
     +Y  M=16: . S . Y . . . E . . . A . . . .
          (grew 8 -> 16, everything rehashed with 11k mod 16)
     +Q  M=16: . S . Y . . . E . . . A Q . . .
     +U  M=16: . S . Y . . . E U . . A Q . . .
     +T  M=16: . S . Y . . . E U . . A Q T . .
     +I  M=32: . . . I . . . U . . . A . . . . . S . Y . . . E . . . Q T . . .
          (grew 16 -> 32, everything rehashed with 11k mod 32)
     +O  M=32: . . . I . O . U . . . A . . . . . S . Y . . . E . . . Q T . . .
     +N  M=32: . . . I . O . U . . . A . . . . . S . Y . . . E . . N Q T . . .
14.41-14.43 N = 10^6 random keys; total probes (inserts + rehashing), final M, final load, avg search-miss probes:
     double at 1/2 (Program 14.7)   probes   3030432 (3.03 per key)  M= 2097152  load 0.48  miss 2.33
     triple at 1/2  (14.41)         probes   2237456 (2.24 per key)  M= 2125764  load 0.47  miss 2.28
     triple at 1/3  (14.42)         probes   2420294 (2.42 per key)  M= 6377292  load 0.16  miss 1.20
     double at 3/4  (14.43)         probes   5377835 (5.38 per key)  M= 2097152  load 0.48  miss 2.33
```

ملاحظة أمينة: الأرقام دي بتعتمد على مكان N = 10⁶ بالنسبة لآخر مرة الجدول كبر فيها، فخدها كاتجاه مش كقانون. 14.41: الضرب في 3 لما يتملي النص أوفر في الشغل الكلي، لأن عدد مرات الـ rehash أقل، والذاكرة في نفس الحدود. 14.42: الضرب في 3 لما يتملي التلت بيخلّي الـ search أرخص (1.20)، بس بياكل 3 أضعاف الذاكرة. 14.43: الاستنى لحد 3/4 هو الأغلى في الشغل (5.38 لكل مفتاح)، لأن الـ inserts اللي بتحصل وα بين ½ و¾ بتدفع probes كتير. يعني لـ linear probing، الاستنى لحد ¾ مش توفير.
:::

:::quiz
10 مفاتيح وM = 5 lists بـ separate chaining. أطول list في المثال كان فيها كام؟

1. [ ] 2
2. [x] 4
3. [ ] 10

> E وY وT وO كلهم h = 0 (تمرين 14.17). التوزيع على أعداد صغيرة كده مابيبقاش متساوي أبدًا.
:::

:::quiz
double hashing وM = 10 والخطوة لمفتاح معين 2. إيه المشكلة؟

1. [ ] مافيش مشكلة
2. [x] المفتاح هيزور 5 خانات بس من الـ 10
3. [ ] المفتاح هيزور كل الخانات مرتين

> gcd(2, 10) = 2، فالخطوات بتلف على نص الجدول بس. والحل إن M يبقى prime.
:::

:::quiz
linear probing وα = ½. متوسط الـ search miss؟

1. [ ] 1\.5
2. [x] 2\.5
3. [ ] 4

> (1 + 1/(1−½)²)/2 = 2.5، والتجربة طلّعت 2.499. والـ 1.5 ده متوسط الـ hit.
:::
