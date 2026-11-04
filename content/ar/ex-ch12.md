تمارين محلولة · الفصل 12 (Symbol Tables and Binary Search Trees) · تمارين 12.17–12.77

الـ symbol table هو أكتر ADT هتستعمله في حياتك: insert وsearch، وبس. الفصل ده بيبدأ بالطرق البسيطة (arrays وlists)، وبعدين binary search، وبعدين الـ BST وعملياتها الحلوة: root insertion وdelete وjoin. تمارين الفصل أغلبها «ارسم الشجرة»، وده أحسن تدريب على الـ BST، ومعاها شوية حسابات تكلفة بتوريك إمتى كل طريقة بتكسب.

:::goal هدف الدرس:
تبني BST بالإيد بسرعة (standard وroot insertion)، وتعدّ تكلفة البناء والبحث، وتعرف كام ترتيب بيدّي نفس الشجرة، وتعمل delete وjoin من غير ما تتلخبط.
:::

:::tip الشجرة بتتحدد بأول مفتاح
أول مفتاح يبقى الجذر للأبد (في الـ insertion العادي). وأي مفتاح بعده بيروح شمال أو يمين حسب مقارنته بالجذر، وده بيحصل في الـ subtree كمان. عشان كده عدد الترتيبات اللي بتدّي نفس الشجرة = N! ÷ (حاصل ضرب أحجام كل الـ subtrees).
:::

* [src/12-bst/exercises/ex12\_17\_elementary.c](src:src/12-bst/exercises/ex12_17_elementary.c)
* [src/12-bst/exercises/ex12\_44\_bst.c](src:src/12-bst/exercises/ex12_44_bst.c)
* [src/12-bst/exercises/ex12\_70\_root\_ops.c](src:src/12-bst/exercises/ex12_70_root_ops.c)

### تمرين 12.17: الطرق الأربعة البسيطة

:::problem
كام مقارنة محتاجين عشان نحط المفاتيح E A S Y Q U E S T I O N في جدول فاضي، لو الـ ADT متنفّذ بكل طريقة من الأربعة: array أو list، مترتب أو لأ؟ افترض إن فيه search لكل مفتاح، وبعده insert لو الـ search ماعرفش يلاقيه (زي تمرين 12.15).
:::

:::think
غير المترتب: الـ miss بيعدّي على الجدول كله (n مقارنة)، والـ hit بيقف عند المفتاح. والفرق بين الـ array والـ list هنا في مكان الإضافة: الـ array بيضيف في الآخر، والـ list في الأول، وده بيغيّر مكان E وS لما نيجي ندوّر عليهم تاني. المترتب: الـ search بيقف عند أول مفتاح ≥ v، فالـ miss بيكلّف حوالي n/2 بس. وبما إن الـ insert بيستعمل المكان اللي الـ search لقاه، مابيكلّفش مقارنات زيادة.
:::

:::solution
```
12.17 compares for search-insert of EASYQUESTION:
     unordered array: 49
     unordered list : 55   (new items go to the front)
     ordered array  : 36
     ordered list   : 36
```

المترتب كسب لأن الـ misses (10 من 12) أرخص. والـ array والـ list المترتبين متساويين بالظبط، لأن ترتيب الفحص واحد. والفرق الحقيقي بينهم في تكلفة الـ insert نفسه (تحريك عناصر في الـ array، وتغيير link في الـ list)، ودي مش مقارنات.
:::

### تمرين 12.34 و12.35: binary search ضد sequential

:::problem
(12.34) لاقي قيم N اللي binary search فيها بيبقى أسرع 10 و100 و1000 مرة من sequential search. اتوقّعها بالتحليل، وبعدين اتأكد بالتجربة. (12.35) افترض إن الـ insertions في symbol table حجمه N بتتعمل زي insertion sort، والـ search بـ binary search، والـ searches أكتر من الـ insertions بـ 1000 مرة. قدّر نسبة الوقت اللي بيروح في الـ insertions، لـ N = 10³ لحد 10⁶.
:::

:::think
الـ search hit بالـ sequential بيكلّف N/2 في المتوسط، وبالـ binary حوالي lg N − 1. فالسرعة = (N/2)/(lg N − 1)، ونحلّها لكل هدف. وفي 12.35: الـ insert بيحرّك N/2 عنصر، و1000 search بيكلّفوا 1000 lg N مقارنة. فالنسبة (N/2) ÷ (N/2 + 1000 lg N). بس ده لو تحريك عنصر وتكلفة مقارنة واحدة متساويين، وده هنشوف إنه مش صحيح.
:::

:::solution
```
12.34    10x predicted at N =    118; measured: sequential 59.5, binary 5.98, ratio 9.9
        100x predicted at N =   1993; measured: sequential 997.0, binary 9.98, ratio 99.9
       1000x predicted at N =  27494; measured: sequential 13747.5, binary 13.81, ratio 995.6
12.35 N=   1000: model (N/2 moves vs 1000 lg N compares)   4.8%   measured time   0.0%
      N=  10000: model (N/2 moves vs 1000 lg N compares)  27.3%   measured time   0.2%
      N= 100000: model (N/2 moves vs 1000 lg N compares)  75.1%   measured time   2.4%
      N=1000000: model (N/2 moves vs 1000 lg N compares)  96.2%   measured time  15.2%
```

12\.34: التحليل والتجربة متطابقين تقريبًا: حوالي 120 و2000 و27500. 12.35 فيها درس: الموديل بيقول إن الـ insertions هتاكل 96% من الوقت لـ N = مليون، بس الوقت الحقيقي 15% بس. السبب إن `memmove` بيحرّك آلاف العناصر بـ instructions vector سريعة جدًا، والـ binary search كل خطوة فيه cache miss وbranch مش متوقّع. بس الاتجاه نفسه صح: كل ما N تكبر، الـ insertion بياكل نسبة أكبر، وفي الآخر هيغلب.
:::

### تمرين 12.44–12.46 و12.53/12.54: ارسم الـ BST

:::problem
(12.44) ارسم الـ BST اللي بيطلع لما ندخّل E A S Y Q U T I O N بالترتيب ده في شجرة فاضية. (12.45) نفس السؤال لـ E A S Y Q U E S T I O N. (12.46) كام مقارنة محتاجين، لو فيه search لكل مفتاح وبعده insert لكل miss؟ (12.53/12.54) احسب ارتفاع الشجرة ومتوسط تكلفة الـ search hit.
:::

:::think
E جذر. A \< E شمال. S \> E يمين. Y \> S يمين الـ S. Q بين E وS، يعني شمال الـ S. وهكذا: كل مفتاح بيمشي من الجذر لحد ما يلاقي null. ولما المفتاح يساوي، Program 12.7 بيوديه يمين. والارتفاع = أطول مسار، والمتوسط = (مجموع أعماق كل الـ nodes) ÷ N.
:::

:::solution
::figure ex12-bst

```
12.44 BST of EASYQUTION: E(A,S(Q(I(-,O(N,-)),-),Y(U(T,-),-)))   height 6
12.45 BST of EASYQUESTION: E(A,S(Q(E(-,I(-,O(N,-))),-),Y(U(S(-,T),-),-)))
      height 7, internal path length 48, average hit cost 4.00 (12.53/12.54)
12.46 search+insert-on-miss: 53 compares (plain insertion of all 12: 36)
      the second E and second S are hits, so they are not inserted again
```

الصيغة `X(L,R)` معناها node فيها X وشمالها L ويمينها R، والـ `-` يعني null. لاحظ في 12.46 إن الـ search قبل الـ insert بيضاعف الشغل تقريبًا (53 بدل 36)، لأن الـ miss بيعدّي المسار والـ insert بيعدّيه تاني. ودي بالظبط حجة تمرين 12.48 (STsearchinsert في عملية واحدة).
:::

### تمرين 12.47: كام ترتيب بيدّي نفس الشجرة؟

:::problem
لو دخّلنا المفاتيح بالترتيب A S E R H I N G C في شجرة فاضية، هتطلع الشجرة اللي فوق في Figure 12.6. اديني 10 ترتيبات تانية بتطلّع نفس الشجرة.
:::

:::think
القاعدة الوحيدة: كل node لازم تدخل قبل كل اللي تحتها. وغير كده، ترتيب الـ subtrees المختلفة مع بعض حر. فلكل node، بنختار إزاي نخلط ترتيبات الـ subtree الشمال واليمين: C(a+b, a) طريقة. وبنضرب ده في كل الـ nodes.
:::

:::solution
```
12.47 tree of ASERHINGC: A(-,S(E(C,R(H(G,I(-,N)),-)),-))
      orderings that give it: 18 of 9! = 362880; ten of them:
      ASERHGCIN ASERHCGIN ASERHIGCN ASERHICNG ASERHCIGN
      ASERHICGN ASERHIGNC ASECRHGIN ASERCHIGN ASECRHING
```

الشجرة دي «رفيعة» جدًا، عشان كده عندها 18 ترتيب بس. لازم A الأول، وبعدها S، وبعدها E (كل واحد فيهم أب لكل اللي بعده). والحرية كلها في مكان الـ C (6 أماكن ممكنة)، وفي الـ G مع I وN (3 أماكن)، يعني 6 × 3 = 18. وكل الـ 10 ترتيبات اللي فوق متجرّبة بالبرنامج وبتدّي نفس الشجرة بالظبط.
:::

### تمرين 12.55 و12.57: شجرة binary search، وكل الأشكال

:::problem
(12.55) اديني ترتيب لإدخال مفاتيح EASYQUESTION بحيث الشجرة اللي تطلع تبقى «equivalent» لـ binary search، يعني سلسلة المقارنات في البحث عن أي مفتاح هي نفسها سلسلة المقارنات في binary search. (12.57) ارسم كل أشكال الـ BST المختلفة اللي ممكن تطلع لما ندخّل N مفتاح في شجرة فاضية، لـ 2 ≤ N ≤ 5.
:::

:::think
binary search بيبصّ الأول على a\[(l+r)/2\]. فلازم ده يبقى الجذر، يعني يدخل الأول، وبعدين نعمل نفس الكلام للنص الشمال واليمين (preorder للـ medians). بس خلّي بالك من المفاتيح المكررة: binary search بيقف عند أول E يقابلها، أما الـ BST فبيحط الـ E التانية في مكان تاني. فهنشتغل على الـ 10 مفاتيح المختلفة. وفي 12.57 عدد الأشكال هو أرقام Catalan: 2، 5، 14، 42.
:::

:::solution
::figure ex12-root

```
12.55 insert in the order OEAINTQSUY -> O(E(A,I(-,N)),T(Q(-,S),U(-,Y)))
      same probe sequence as binary search for all 10 keys: yes
12.57 N=2: 2 shapes (Catalan number), 2 orderings
     N=3: 5 shapes (Catalan number), 6 orderings:  (.(.(..))) 1/6  (.((..).)) 1/6  ((..)(..)) 2/6  ((.(..)).) 1/6  (((..).).) 1/6
     N=4: 14 shapes (Catalan number), 24 orderings; most likely shape 3/24, least likely 1/24
     N=5: 42 shapes (Catalan number), 120 orderings; most likely shape 8/120, least likely 1/120
```

في 12.57 الأشكال مش متساوية الاحتمال: لـ N = 3 الشكل المتوازن بيطلع بضعف احتمال أي شكل تاني (2 من 6)، لأن فيه ترتيبين بيدّوه (B A C وB C A). وده سبب إن الـ BST العشوائية متوازنة «تقريبًا»: الأشكال الكويسة ليها ترتيبات أكتر. وملحوظة: الشجرة اللي فوق (12.70) مش شجرة 12.55، دي شجرة الـ root insertion اللي جاية.
:::

### تمرين 12.70، 12.71 و12.74: root insertion

:::problem
(12.70) ارسم الـ BST اللي بيطلع من إدخال EASYQUESTION بطريقة الـ root insertion. (12.71) اديني ترتيب لـ 10 مفاتيح (من A لـ J) يخلّي بناء الشجرة بالـ root insertion ياخد أكبر عدد مقارنات، وقول العدد. (12.74) قيس متوسط تكلفة البحث عن الـ N/10 مفتاح اللي دخلوا آخر حاجة، بالـ insertion العادي وبالـ root insertion.
:::

:::think
الـ root insertion = حط المفتاح في مكانه العادي تحت، وبعدين طلّعه لفوق بـ rotations لحد ما يبقى الجذر. فآخر مفتاح هو الجذر دايمًا. وفي 12.71 الترتيب المترتب مابيكلّفش حاجة تقريبًا (كل جديد بيروح يمين الجذر على طول)، فالأسوأ لازم يبقى حاجة تانية. ومع 10! = 3.6 مليون ترتيب بس، نقدر نجرّبهم كلهم.
:::

:::solution
```
12.70 root insertion of EASYQUESTION: N(I(E(A(-,E),-),-),O(-,T(S(Q(-,S),-),U(-,Y))))   (31 compares)
12.71 max compares to build by root insertion: 33, e.g. EDCBAJIHGF; 8 of the 10! orders reach it
12.74 searches for the N/10 most recently inserted keys (average compares):
     N=   1000: standard  13.09 (sd 2.48)   root insertion   7.22 (sd 2.71)
     N=1000000: standard  27.69 (sd 4.92)   root insertion  22.94 (sd 5.24)
```

12\.71: الأسوأ هو «نازل وبعدين نازل»، EDCBA وبعدها JIHGF: كل مفتاح جديد أصغر من الجذر، فلازم يعدّي كل اللي قبله. والـ 33 دي أقل بكتير من الـ 45 بتاعة الـ insertion العادي على ملف مترتب، لأن الـ rotations بتعيد ترتيب الشجرة وبتقصّر المسارات. 12.74: لو الـ searches بتدوّر على الجديد، الـ root insertion بيوفّر حوالي 5 مقارنات لكل بحث، وده مكسب كبير لـ N صغيرة (7.2 بدل 13.1).
:::

### تمرين 12.76 و12.77: delete وjoin

:::problem
(12.76) ارسم الـ BST اللي بيطلع لما ندخّل E A S Y Q U T I O N وبعدين نمسح الـ Q. (12.77) ادخّل E A S Y في شجرة فاضية، وQ U E S T I O N في شجرة فاضية تانية، وبعدين اعمل join للاتنين.
:::

:::think
delete في Program 12.15: الـ node اللي هتتمسح بنحط مكانها join لولادها: أصغر واحد في **اليمين** بيطلع جذر بـ partition، والشمال بيتعلّق على شماله. والـ Q مالهاش يمين، فالشمال (I) بيطلع مكانها على طول. والـ join في Program 12.16: خد جذر الشجرة الأولى، واعمله root insertion في التانية (فيقسمها لأصغر وأكبر منه)، وبعدين اعمل join للشمال مع الشمال واليمين مع اليمين recursively.
:::

:::solution
```
12.76 BST of EASYQUTION:  E(A,S(Q(I(-,O(N,-)),-),Y(U(T,-),-)))
      after deleting Q:   E(A,S(I(-,O(N,-)),Y(U(T,-),-)))
12.77 EASY: E(A,S(-,Y))   QUESTION: Q(E(-,I(-,O(N,-))),U(S(-,T),-))
      join: E(A(-,E),S(Q(I(-,O(N,-)),S),Y(U(T,-),-)))
```

في 12.77 الجذر بتاع EASY (E) بقى جذر النتيجة، والـ E بتاعة QUESTION راحت تحت A على اليمين (لأن المتساوي بيروح يمين في الـ partition). والـ 12 مفتاح كلهم موجودين، والشجرة BST سليمة. والتكلفة كلها كانت root insertions، من غير ما نلمس الـ nodes غير اللي على المسارات.
:::

:::quiz
كام ترتيب للمفاتيح A B C بيدّي الشجرة المتوازنة (B جذر، A شمال، C يمين)؟

1. [ ] 1
2. [x] 2
3. [ ] 3

> B لازم الأول، وبعدها A وC بأي ترتيب: B A C أو B C A. يعني 3!/(3·1·1) = 2.
:::

:::quiz
في BST اتبنت بالـ root insertion، مين الجذر؟

1. [ ] أول مفتاح دخل
2. [x] آخر مفتاح دخل
3. [ ] الـ median

> كل insert بيطلّع المفتاح الجديد للجذر بـ rotations. ودي ميزته: الجديد قريب من فوق (تمرين 12.74).
:::

:::quiz
binary search أسرع من sequential search بـ 100 مرة تقريبًا عند N كام؟

1. [ ] حوالي 100
2. [x] حوالي 2000
3. [ ] حوالي مليون

> (N/2)/(lg N − 1) = 100 لما N ≈ 1993، والتجربة طلّعت 99.9.
:::
