تمارين محلولة · الفصل 10 (Radix Sorting) · تمارين 10.1–10.43

في الـ radix sorting بطّلنا نقارن مفاتيح كاملة، وبقينا نبصّ على «حتت» منها: bits أو bytes. تمارين الفصل ده بتدرّبك على حاجتين: إزاي تقطّع المفتاح لـ digits صح وبسرعة، وإزاي تعدّ الشغل بالـ **bytes اللي اتبصّ عليها** مش بالمقارنات. وفي الآخر مثال شرير (A وAA وAAA…) بيوريك إمتى الـ radix مابيكسبش.

:::goal هدف الدرس:
تعرف تستخرج digit من رقم بالـ shift والـ mask، وتتتبّع binary quicksort وLSD بالإيد، وتحسب تكلفة MSD و3-way radix quicksort على ملفات خاصة.
:::

:::tip عدّ صح
في الفصل ده «مقارنة» مش وحدة القياس الصح، لأن مقارنة strings طويلة ممكن تبصّ على 100 byte. الوحدة هي `digit()`: كل مرة البرنامج بيبصّ على byte. وكل الأرقام تحت متعدّة كده.
:::

* [src/10-radix-sort/exercises/ex10\_01\_digits.c](src:src/10-radix-sort/exercises/ex10_01_digits.c)
* [src/10-radix-sort/exercises/ex10\_09\_binary\_quick.c](src:src/10-radix-sort/exercises/ex10_09_binary_quick.c)
* [src/10-radix-sort/exercises/ex10\_35\_lsd\_msd.c](src:src/10-radix-sort/exercises/ex10_35_lsd_msd.c)

### تمرين 10.1–10.4: الـ digits

:::problem
(10.1) كام digit في رقم 32-bit لو اعتبرناه رقم radix 256؟ اشرح إزاي تطلّع كل digit. ونفس السؤال لـ radix 2<sup>16</sup>. (10.2) لـ N = 10³ و10⁶ و10⁹، إيه أصغر byte size يخلّي أي رقم من 0 لـ N يتمثّل في word فيها 4 bytes؟ (10.3) اعمل less() باستعمال الـ digit abstraction. (10.4) قارن تكلفة استخراج الـ digits بالـ shift وبالقسمة.
:::

:::think
32 bit ÷ 8 = 4 digits، و32 ÷ 16 = 2. والـ digit رقم b (من الشمال) = `(a >> (bitsbyte·(bytesword−1−b))) & (R−1)`. وفي 10.2: محتاجين ⌈lg(N+1)⌉ bit، ومقسومين على 4 bytes. وفي 10.4 خلّي بالك: الـ compiler هيحوّل `/256` لـ shift لوحده، فلازم الـ base يبقى `volatile` عشان القسمة تتعمل بجد.
:::

:::solution
```
10.1 0xDEADBEEF in radix 256 (4 digits): 222 173 190 239
     in radix 65536 (2 digits): 57005 48879
10.2 N=      1000 needs 10 bits -> bytes of 3 bits (4 x 3 = 12 >= 10)
10.2 N=   1000000 needs 20 bits -> bytes of 5 bits (4 x 5 = 20 >= 20)
10.2 N=1000000000 needs 30 bits -> bytes of 8 bits (4 x 8 = 32 >= 30)
10.3 less() via digits agrees with < on all test pairs: yes
10.4 50000000 digit extractions: shift/mask 0.062 s (812 M/s), divide/mod 0.178 s (281 M/s), ratio 2.9x
```

DE AD BE EF بالـ hex هي بالظبط 222 173 190 239، وفي radix 2<sup>16</sup> الـ digits هي DEAD وBEEF. وفي 10.4 الـ shift أسرع حوالي 3 مرات على الجهاز ده. عشان كده الكتاب بيختار R قوة لـ 2.
:::

### تمرين 10.9 و10.10: binary quicksort ضد quicksort

:::problem
(10.9) قارن عدد الـ exchanges في binary quicksort والـ quicksort العادي، للملف `001 011 101 110 000 001 010 111 110 010`. (10.10) ليه ترتيب الـ subfile الأصغر الأول مش مهم في binary quicksort زي ما كان مهم في الـ quicksort العادي؟
:::

:::think
binary quicksort بيعمل partition على أول bit: الـ i بيدوّر على 1 من الشمال، والـ j على 0 من اليمين، ويبدّلهم. ومابيحتاجش partitioning element. وفي 10.10: عمق الـ recursion عمره ما هيعدّي عدد الـ bits، لأن كل مستوى بياكل bit واحد. يعني الـ stack محدود بـ w مهما كانت الداتا.
:::

:::solution
::figure ex10-binq

```
10.9 binary quicksort: 000 001 001 010 010 011 101 110 110 111  13 exch calls, 7 real exchanges
     normal quicksort (Program 7.1): 11 exch calls, 10 real exchanges
10.10 sorted 20-bit file of 10^6 keys: recursion depth 19 (never more than the 20 bits)
```

Program 10.1 بينادي exch كتير على نفس الموقع (i = j) في آخر كل partition، فالعدّ الأمين هو «الـ exchanges الحقيقية»: 7 ضد 10. الـ binary quicksort كسب، لأنه بيبدّل بس اللي في الناحية الغلط بالنسبة للـ bit. وفي 10.10، حتى على ملف مترتب (اللي بيبوّظ الـ quicksort العادي) العمق 19.
:::

### تمرين 10.11: المستوى التاني على 0..170

:::problem
اشرح إيه اللي بيحصل في المستوى التاني من الـ partitioning (للـ subfile الشمال واليمين) لما binary quicksort يرتّب permutation عشوائية للأعداد من 0 لـ 170.
:::

:::think
170 محتاج 8 bits. أول partition على bit 7: من 0 لـ 127 شمال (128 رقم)، ومن 128 لـ 170 يمين (43 رقم). المستوى التاني على bit 6: الشمال بيتقسم 64/64 بالظبط. أما اليمين، فكل الأرقام من 128 لـ 170 فيها bit 6 = 0، لأن 128 + 64 = 192 \> 170.
:::

:::solution
```
10.11 0..170 (8 bits, 171 = 10101011): level 0 split 128 | 43
      level 1: left subfile splits 64 | 64,  right subfile splits 43 | 0
```

الـ partition على اليمين «فاضي»: بيعدّي على الـ 43 رقم كلهم ومايحرّكش حاجة، وبعدين ينزل للـ bit اللي بعده. والمسألة دي بتتكرر كل ما N ماتكونش قوة لـ 2. وهي اللي بيعالجها تمرين 10.12: اكتشف الـ bits اللي مابتفرقش وماتضيّعش وقت عليها.
:::

### تمرين 10.12: تخطّي الـ bits المتساوية في الأول

:::problem
اكتب برنامج يحدد في pass واحد عدد الـ bits اللي في الأول ومتساوية في كل المفاتيح، وبعدين ينادي binary quicksort معدّل يتجاهلها. وقارن الوقت بالنسخة العادية.
:::

:::think
اعمل OR لـ `a[i] ^ a[0]` على كل المفاتيح: أي bit فضل 0 في النتيجة يبقى متساوي في الكل. وعدد الأصفار في الأول هو اللي هنتخطاه. وبعدين ابدأ الـ sort من w = skip بدل 0.
:::

:::solution
```
10.12 32-bit keys whose top 12 bits are all equal (time in ms):
     leading equal bits found: 12
     N=   1000: standard 0.049   skip-leading 0.043   (per sort)
     N=  10000: standard 0.874   skip-leading 0.822   (per sort)
     N= 100000: standard 10.206   skip-leading 9.605   (per sort)
     N=1000000: standard 106.415   skip-leading 110.374   (per sort)
```

الكسب صغير: 5–10% لـ N صغيرة، ولما N = مليون خسرنا شوية بسبب الـ pass الزيادة. ليه؟ كل partition «فاضي» على bit متساوي بيعدّي على الملف مرة واحدة بسرعة جدًا من غير exchanges، يعني تكلفته زي تكلفة الـ pass بتاعنا بالظبط. الفكرة صح، بس مكسبها محدود: 12 pass رخيصين من حوالي 32.
:::

### تمرين 10.35: LSD على أول حرفين

:::problem
وريني نتيجة LSD radix sort على أول حرفين من المفاتيح `now is the time for all good people to come the aid of their party`.
:::

:::think
LSD بيبدأ من الحرف الأيمن (رقم 1)، وبعدين الحرف رقم 0. وكل pass لازم يبقى **stable** (key-indexed counting)، عشان ترتيب الـ pass الأول يفضل جوه المجموعات اللي حرفها الأول واحد. والكلمات اللي حرفها التاني فاضي (مافيش) بتتعامل كأنها 0، يعني أصغر حاجة.
:::

:::solution
```
10.35 LSD radix sort on the leading two characters:
     pass on char 1: party people of the the their time aid all now for good to come is
     pass on char 0: aid all come for good is now of party people the the their time to
```

بعد الـ pass الأول: كل اللي حرفه التاني a (party) الأول، وبعده e (people)، وبعده f (of)، وبعده h (the the their)، وهكذا. وبعد الـ pass التاني الكلمات مترتبة بأول حرفين. ولاحظ إن `the the their` فضلوا بنفس ترتيبهم الأصلي، لأن أول حرفين متساويين (th) والـ sort stable. ولأن الترتيب على أول حرفين بس، النتيجة مش ترتيب كامل: لو كان فيه «thaw» بعد «the» في الملف الأصلي، كانت هتفضل بعدها رغم إنها أصغر.
:::

### تمرين 10.39، 10.41 و10.43: ملفات خاصة

:::problem
(10.39) ملف فيه 1000 نسخة من كل رقم من 1 لـ 1000 (كل واحد في 32-bit word). إزاي تستغل المعلومة دي عشان تعمل radix sort سريع؟ (10.41) إيه عدد الـ bytes الكلي اللي بيتبصّ عليه في 3-way radix quicksort في أسوأ حالة، لـ bytestrings طولها ثابت؟ (10.43) كام byte بيتبصّ عليه في MSD و3-way radix quicksort لملف فيه N مفتاح: A، AA، AAA، AAAA، …؟
:::

:::think
10\.39: القيم 1000 بس، فاعتبر المفتاح كله «digit واحد» بـ R = 1001 واعمل key-indexed counting مرة واحدة، يعني 2N خطوة. 10.41: أسوأ حاجة لما كل المفاتيح متساوية: كل byte في كل مفتاح لازم يتبصّ عليه (مافيش طريقة تعرف إنهم متساويين غير كده)، وكل مستوى بيعدّي على الـ N. 10.43: المفتاح رقم i طوله i، وكل الـ prefixes متساوية، فالمستوى d فيه N − d مفتاح لسه شغّالين. يعني Σ(N − d) ≈ N²/2 «لمسة» لكل pass على الملف.
:::

:::solution
```
10.39 1000 copies of 1..1000: one counting pass, 2000000 key accesses (2N), sorted yes
10.41 N=1000 equal keys of 16 bytes: 34017 bytes examined = 2.00 x N(W+1)
      same size, random keys: 15594 bytes examined (15.59 per key)
10.43 keys A, AA, AAA, ... (N keys):
     N=   10: MSD       128 bytes (ok)   3-way radix quicksort       420 bytes (ok)   N^2 = 100
     N=  100: MSD     10298 bytes (ok)   3-way radix quicksort     10781 bytes (ok)   N^2 = 10000
     N= 1000: MSD   1002998 bytes (ok)   3-way radix quicksort   1006594 bytes (ok)   N^2 = 1000000
```

10\.41: أسوأ حالة حوالي 2N(W + 1)، يعني كل byte (بما فيهم الـ 0 اللي في الآخر) بيتبصّ عليه مرتين (scan + تشييك مساواة). ده خطي في حجم الداتا، ومافيش أحسن من كده بكتير. أما المفاتيح العشوائية فبتتحسم بعد كام byte بس (15.6 لكل مفتاح، يعني أقل من الـ 16 byte نفسهم). وفي 10.43 الاتنين تربيعيين (~N²)، بس ده مش عيب في الـ algorithm: حجم الداتا نفسه N²/2 حرف. يعني الـ radix بيبصّ على كل حرف حوالي مرتين، وده «linear في حجم الداتا» زي ما الكتاب بيقول.
:::

:::quiz
رقم 32-bit في radix 2<sup>16</sup>. كام digit، والـ digit الشمال بيتطلّع إزاي؟

1. [ ] 4 digits؛ `a & 0xFFFF`
2. [x] 2 digits؛ `(a >> 16) & 0xFFFF`
3. [ ] 16 digit؛ `a % 16`

> 32/16 = 2. والشمال هو الـ 16 bit العالية، فبنزقّها 16 خانة لليمين.
:::

:::quiz
ليه binary quicksort مش محتاج يرتّب الـ subfile الأصغر الأول؟

1. [ ] لأنه مابيستعملش recursion
2. [x] لأن العمق محدود بعدد الـ bits مهما كانت الداتا
3. [ ] لأن الـ subfiles دايمًا متساوية

> كل مستوى بياكل bit، فمع مفاتيح 32-bit العمق ≤ 32 (تمرين 10.10).
:::

:::quiz
ملف فيه مليون رقم كلهم من 1 لـ 1000. أسرع طريقة؟

1. [ ] 3-way radix quicksort
2. [ ] LSD بـ 4 passes على bytes
3. [x] key-indexed counting مرة واحدة بـ R = 1001

> مفيش داعي تقطّع المفتاح: هو أصلًا «digit» واحد قيمه قليلة، فـ 2N خطوة وخلاص (تمرين 10.39).
:::
