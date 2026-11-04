تمارين محلولة · الفصل 15 (Radix Search) · تمارين 15.1–15.89

الفصل 15 بيقلب طريقة تفكيرنا في البحث: بدل ما نقارن المفتاح كله بمفتاح تاني، بنبص عليه **حتة حتة**، بت ببت أو حرف بحرف، وكل حتة بتقول لنا نروح فين. وعندنا أربع أشكال لنفس الفكرة: الـ DST، والـ trie، والـ patricia، والـ TST. أغلب تمارين الفصل «ارسم اللي هيطلع»، وده مقصود، لأن الفرق بين الأربعة مش هيبان غير وإنت بترسم بإيدك. وبعدين شوية تجارب على مفاتيح عشوائية نتأكد بيها إن الكلام النظري صح، وفي الآخر string indexes على نص حقيقي.

:::goal هدف الدرس:
ترسم DST وtrie وpatricia وTST لنفس المفاتيح وتعرف الفرق في الشكل والمساحة، وتشوف إيه اللي بيحصل للمفاتيح المتكررة في كل واحدة، وتعمل sort وselect وprefix search، وتقارن الـ trie والـ patricia بالـ BST والـ red-black على مليون مفتاح، وتلاقي أطول جزء متكرر في كتاب كامل.
:::

:::tip اكتب البتات الأول
قبل أي رسمة اكتب كل مفتاح بالبتات تحت بعض: E = 00101، A = 00001، S = 10011… (الحرف رقم i = i بالـ binary على 5 بت، زي Figure 15.1). الرسمة بعد كده نقل: 0 شمال و1 يمين. ولو مفتاحين ليهم نفس البداية، البتات اللي تحت بعض هتوريك هيفترقوا فين بالظبط.
:::

* [src/15-tries/exercises/ex15\_01\_dst.c](src:src/15-tries/exercises/ex15_01_dst.c)
* [src/15-tries/exercises/ex15\_11\_trie.c](src:src/15-tries/exercises/ex15_11_trie.c)
* [src/15-tries/exercises/ex15\_33\_patricia.c](src:src/15-tries/exercises/ex15_33_patricia.c)
* [src/15-tries/exercises/ex15\_48\_tst.c](src:src/15-tries/exercises/ex15_48_tst.c)
* [src/15-tries/exercises/ex15\_86\_string\_index.c](src:src/15-tries/exercises/ex15_86_string_index.c)

### تمرين 15.1 و15.4: ارسم الـ DST

:::problem
(15.1) ارسم الـ DST اللي هتطلع لما ندخّل E A S Y Q U T I O N بالترتيب ده في شجرة فاضية، بكود الـ 5 بت بتاع Figure 15.1. (15.4) نفس الكلام للمفاتيح 01010011 00000111 00100001 01010001 11101100 00100001 10010101 01001010.
:::

:::think
الـ DST شكلها BST: مفتاح واحد في كل node. الفرق إننا مش بنقارن. عند عمق w بنبص على البت رقم w من المفتاح اللي بندوّر عليه: لو 0 نروح شمال، لو 1 نروح يمين، ولما نلاقي مكان فاضي نحط المفتاح فيه. يعني E تقعد في الجذر، والـ A (00001) بت 0 بتاعها 0 فتروح شمال، والـ S (10011) تروح يمين، والـ Y (11001) تروح يمين من E، وبعدين بت 1 بتاعها 1 فتروح يمين من S. وفي 15.4 خلّي بالك إن 00100001 مكتوب **مرتين**.
:::

:::solution
::figure ex15-dst

```
15.1 DST for E A S Y Q U T I O N (5-bit code, A = 00001):
       (root)   depth 0: E = 00101
       0        depth 1: A = 00001
       01       depth 2: I = 01001
       011      depth 3: O = 01111
       0111     depth 4: N = 01110
       1        depth 1: S = 10011
       10       depth 2: Q = 10001
       101      depth 3: U = 10101
       1010     depth 4: T = 10100
       11       depth 2: Y = 11001
     10 nodes, height 4 (never more than w = 5)
15.4 DST for the eight 8-bit keys (in that order):
     insert 01010011 -> depth 0
     insert 00000111 -> depth 1
     insert 00100001 -> depth 2
     insert 01010001 -> depth 2
     insert 11101100 -> depth 1
     insert 00100001 -> depth 3   (duplicate of key 3: lands right below it)
     insert 10010101 -> depth 2
     insert 01001010 -> depth 3
       (root)   depth 0: 01010011
       0        depth 1: 00000111
       00       depth 2: 00100001
       001      depth 3: 00100001
       01       depth 2: 01010001
       010      depth 3: 01001010
       1        depth 1: 11101100
       10       depth 2: 10010101
     8 nodes, height 3
```

الـ path اللي جنب كل node هو البتات اللي مشيناها لحد ما وصلنا. وفي 15.4 النسخة التانية من 00100001 نزلت تحت الأولى على طول (path 001). ده لأن Program 15.1 مابيقارنش بالمساواة وهو بيدخّل، فالمفتاح المكرر بيمشي في نفس الطريق وينزل خطوة كمان. ودي بداية الكلام في 15.5.
:::

### تمرين 15.2، 15.3 و15.5: ترتيب الإدخال والتكرار

:::problem
(15.2) اديني ترتيب إدخال للمفاتيح A B C D E F G يطلّع DST متوازنة تمامًا وفي نفس الوقت BST سليمة. (15.3) اديني ترتيب يطلّع DST متوازنة تمامًا وكل node فيها أصغر من كل الـ nodes اللي تحتها. (15.5) ينفع نحط records بمفاتيح متكررة في الـ DST زي ما بنعمل في الـ BST؟ اشرح.
:::

:::think
أول حاجة: بكود الـ 5 بت، A لحد G كلهم من 00001 لـ 00111، يعني أول بتين صفر عند الكل. فكلهم هيروحوا شمال مرتين، ومستحيل الشجرة تطلع متوازنة. السؤال ليه معنى بس لو اشتغلنا على آخر 3 بتات (A = 001 … G = 111). وبعد كده 7! = 5040 ترتيب بس، فنجرّبهم كلهم ونفحص الشرط. لما مجموعة الاحتمالات تكون صغيرة كده، التجربة الكاملة دي بتبقى برهان. ولـ 15.5: كل النسخ من نفس المفتاح ليها نفس البتات، يعني كلها هتمشي في نفس الطريق.
:::

:::solution
```
15.2/15.3 with the 5-bit code every key A..G starts with 00, so the root's right link
     stays empty: 'DBFACEG' gives height 4, not 2. We branch on the last 3 bits (w = 3):
     15.2: 80 of 5040 orders give a perfectly balanced DST that is also a BST; first: DBACFEG
       (root)   depth 0: D = 100
       0        depth 1: B = 010
       00       depth 2: A = 001
       01       depth 2: C = 011
       1        depth 1: F = 110
       10       depth 2: E = 101
       11       depth 2: G = 111
     15.3: 0 of 5040 orders give a perfectly balanced DST with every node smaller than its subtree.
     impossible: the root must be A, then bit 0 sends only B, C left and D E F G right (2 vs 4).
     best we can do with the heap condition is height 3, e.g. ABCDEFG:
       (root)   depth 0: A = 001
       0        depth 1: B = 010
       01       depth 2: C = 011
       1        depth 1: D = 100
       10       depth 2: E = 101
       11       depth 2: F = 110
       111      depth 3: G = 111
15.5 duplicates: insert the same 3-bit key D = 100 again and again (w = 3):
     copy 1 of D goes to depth 0 (path )
     copy 2 of D goes to depth 1 (path 1)
     copy 3 of D goes to depth 2 (path 10)
     copy 4 of D goes to depth 3 (path 100)
     copy 5 of D: NO room, all 3 bits used up (a DST holds at most w+1 = 4 copies)
```

15\.2: فيه 80 ترتيب ينفعوا، وأولهم بالترتيب الأبجدي D B A C F E G. الفكرة إن D (100) في الجذر، وB (010) وF (110) هما جذور الشجرتين الصغيرين، فالترتيب ده بيظبط ترتيب البتات وترتيب المفاتيح مع بعض.

15\.3: **مستحيل**، والبرنامج جرّب الـ 5040 ترتيب ومالقاش ولا واحد. والسبب بسيط: الجذر لازم يبقى A لأنه أصغر واحد. باقي الستة بيتقسموا على بت 0: B وC بس (01x) بيروحوا شمال، وD E F G (1xx) بيروحوا يمين. يعني 2 قصاد 4، ومستحيل يبقوا 3 و3. وأحسن حاجة تقدر توصلها بشرط الـ heap ده هي ارتفاع 3. ولو جرّبت ترقيم تاني (A = 000)، هتلاقيه مستحيل برضه.

15\.5: ينفع، بس بحدود. كل النسخ بتقعد على مسار واحد، وكل نسخة بتنزل مستوى. ولما البتات تخلص (بعد w+1 نسخة) مفيش مكان تاني، وكمان ضمان «الارتفاع ≤ عدد البتات» بيبوظ. فالحل الطبيعي إنك تحط كل الـ records اللي ليها نفس المفتاح في list متعلّقة في node واحدة.
:::

### تمرين 15.11، 15.12، 15.13 و15.24: الـ trie

:::problem
(15.11) ارسم الـ trie لما ندخّل E A S Y Q U T I O N. (15.12) إيه اللي بيحصل لو استعملنا Program 15.3 عشان ندخّل record مفتاحه موجود أصلًا؟ (15.13) ارسم الـ trie للمفاتيح الـ 8 بت بتوع 15.4. (15.24) اكتب برنامج يحسب عدد الـ nodes في الـ trie بتاع مجموعة مفاتيح، عن طريق إنه يرتّبهم ويقارن كل مفتاحين جنب بعض.
:::

:::think
في الـ trie المفاتيح كلها في الأوراق، والـ node الداخلية مجرد «بص على البت ده». الورقة بتاعة كل مفتاح بتقعد عند أول عمق تبقى فيه البادئة بتاعته مش متشاركة مع حد. يعني **شكل الشجرة بيتحدد بالمجموعة نفسها، مش بترتيب الإدخال**. ولـ 15.24: كل node داخلية = بادئة مشتركة بين مفتاحين على الأقل. ولو رتّبنا المفاتيح، المفاتيح اللي ليها نفس البادئة هتبقى جنب بعض، فكل البادئات المشتركة موجودة في الـ lcp (أطول بادئة مشتركة) بين كل جارين. والجار رقم i بيضيف بس البادئات اللي أطول من lcp الجار اللي قبله.
:::

:::solution
::figure ex15-trie

```
15.11 trie for E A S Y Q U T I O N (5-bit code):
       leaf  000      depth 3: A = 00001
       leaf  001      depth 3: E = 00101
       leaf  010      depth 3: I = 01001
       leaf  01110    depth 5: N = 01110
       leaf  01111    depth 5: O = 01111
       leaf  1000     depth 4: Q = 10001
       leaf  1001     depth 4: S = 10011
       leaf  10100    depth 5: T = 10100
       leaf  10101    depth 5: U = 10101
       leaf  11       depth 2: Y = 11001
     11 internal nodes + 10 leaves, height 5
15.24 nodes from the sorted keys: 21 (built trie: 21)
     the same keys inserted backwards give: 11 internal, height 5, path length 39 (vs 39): the shape depends only on the key set
15.12 Program 15.3 on a second S: split() sees equal bits at 3, 4, ... and never stops
     (past bit 4 it reads garbage / overflows the stack). With a guard at the last bit it
     made 2 useless internal nodes before giving up (guard fired: yes).
     Fix: compare keys before splitting; our insertR() does, so S is simply ignored.
15.13 note: 00100001 is already there (it appears twice), so it is skipped
15.13 trie for the 8-bit keys:
       leaf  000      depth 3: 00000111
       leaf  001      depth 3: 00100001
       leaf  0100     depth 4: 01001010
       leaf  0101000  depth 7: 01010001
       leaf  0101001  depth 7: 01010011
       leaf  10       depth 2: 10010101
       leaf  11       depth 2: 11101100
     9 internal nodes + 7 leaves, height 7
15.24 nodes from the sorted keys: 16 (built trie: 16)
```

جرّبنا ندخّل نفس المفاتيح بالعكس، وطلع نفس عدد الـ nodes ونفس الارتفاع ونفس طول المسارات (39). ده الفرق الكبير بين الـ trie والـ BST/DST: الـ trie مالهاش «ترتيب وحش».

15\.12: الـ split بيعمل node داخلية لكل بت المفتاحين متفقين عليه. ولو المفتاحين متساويين، هيفضلوا متفقين على طول، فالـ split هيفضل ينزل لحد ما البتات تخلص وبعدها يقرا زبالة أو الـ stack يضرب. الحل إنك تقارن المفتاحين **قبل** الـ split. ولو مقارنتش وعملت بس guard عند آخر بت، هتفضل nodes داخلية ملهاش لازمة متعلّقة في الشجرة. وفي 15.13 المفتاح 00100001 مكرر، والبرنامج بيلاقيه ويتجاهله من غير ما يعمل ولا node.

15\.24: الحساب من المصفوفة المترتبة بس طلّع 21 و16، ونفس الأرقام اللي جت من الـ trie اللي اتبنت فعلًا. يعني مش محتاج تبني الـ trie عشان تعرف حجمه.
:::

### تمرين 15.14، 15.18 و15.19: sort وprefix ومليون مفتاح

:::problem
(15.18) اعمل الـ sort لـ symbol table مبني على trie. (15.19) اطبع كل المفاتيح اللي أول t بت منها زي مفتاح معيّن. (15.14) اعمل تجارب تقارن فيها الارتفاع وعدد الـ nodes وطول المسارات (internal path length) لـ trie اتبنى من N مفتاح عشوائي 32 بت، بنفس المقاييس لـ BST عادية ولـ red-black tree على نفس المفاتيح، لـ N = 10³ لحد 10⁶.
:::

:::think
في كل مستوى الشمال 0 واليمين 1، يعني الشمال أصغر من اليمين. فلو لفّينا على الأوراق من الشمال لليمين، هتطلع المفاتيح مترتبة من غير ولا مقارنة. و«نفس أول t بت» معناها subtree: انزل t مستوى بالبتات بتاعة المفتاح، واطبع كل اللي تحت. بس خلّي بالك: لو قابلت ورقة قبل ما توصل لعمق t، لازم تتأكد من باقي بتاتها. وفي 15.14 هنقيس «متوسط عمق المفتاح» بدل الـ path length الخام، عشان الأرقام تتقارن بـ lg N.
:::

:::solution
```
15.18 sort (inorder over the leaves): A E I N O Q S T U Y
15.19 keys with the same first 2 bits as Q = 10001: Q S T U
     keys with the same first 3 bits as O = 01111: N O
15.14 N random 32-bit keys: height / nodes / average depth of a key
            N | trie (internal+leaves)       | BST                    | red-black
         1000 | h=21 nodes=   2499 avg 11.47 | h=24 nodes=   1000 avg 12.25 | h=11 nodes=   1000 avg  8.26
                internal/N = 1.499 (1/ln2 = 1.443), lg N = 9.97
        10000 | h=27 nodes=  24406 avg 14.61 | h=29 nodes=  10000 avg 15.96 | h=17 nodes=  10000 avg 11.78
                internal/N = 1.441 (1/ln2 = 1.443), lg N = 13.29
       100000 | h=31 nodes= 243886 avg 17.94 | h=35 nodes= 100000 avg 19.08 | h=22 nodes= 100000 avg 15.13
                internal/N = 1.439 (1/ln2 = 1.443), lg N = 16.61
      1000000 | h=32 nodes=2441992 avg 21.26 | h=47 nodes= 999910 avg 24.61 | h=27 nodes= 999910 avg 18.73 (dup keys skipped)
                internal/N = 1.442 (1/ln2 = 1.443), lg N = 19.93
```

الـ trie بيعدّي الـ BST العادية في كل حاجة. متوسط العمق 21.26 قصاد 24.61 عند المليون، والارتفاع عمره ما بيعدّي 32 (عدد البتات)، والـ BST وصلت 47. بس الـ red-black لسه أقصر (18.73)، لأن الـ trie مابيعرفش يتوازن، هو بس مابيقعش في أسوأ حالة. ولاحظ عدد الـ nodes: الـ trie فيه حوالي 1.44 N node داخلية فوق الـ N ورقة، يعني 1/ln 2، زي Property 15.4 بالظبط. والـ nodes الزيادة دي بالذات هي اللي الـ patricia جاية تشيلها.
:::

### تمرين 15.32–15.35 و15.44: الـ patricia

:::problem
(15.32) إيه اللي بيحصل لو استعملنا Program 15.5 عشان ندخّل record مفتاحه موجود؟ (15.33) ارسم الـ patricia لـ E A S Y Q U T I O N. (15.34) ارسمها للمفاتيح الـ 8 بت بالترتيب ده. (15.35) وارسمها لنفس المفاتيح بالترتيب المعكوس. (15.44) اثبت إن كل مفتاح في الـ patricia موجود على مسار البحث بتاعه هو نفسه.
:::

:::think
الـ patricia عبارة عن trie شلنا منه الـ nodes اللي ليها طريق واحد بس. كل node فيها مفتاح واحد ورقم البت اللي بتختبره، وأرقام البتات بتكبر وإحنا نازلين. الـ link اللي بيشاور على node رقم البت بتاعها مش أكبر (link «لفوق») بيلعب دور الورقة: لما نمشي فيه نقارن المفتاح كله مرة واحدة. ولما ندخّل مفتاح: ندوّر عليه، نوصل لمفتاح t، نلاقي أول بت i بيختلفوا فيه، وننزل تاني لحد ما نلاقي node بتختبر بت أكبر من i، أو link لفوق، ونحط node جديدة بتختبر بت i في المكان ده. ولو المفتاح زي t بالظبط، مفيش بت بيختلفوا فيه أصلًا.
:::

:::solution
::figure ex15-patricia

```
15.33 patricia for E A S Y Q U T I O N (5-bit code; head is the dummy key 00000):
       S  tests bit 0   0 -> I   1 -> Y
         I  tests bit 1   0 -> E   1 -> O
           E  tests bit 2   0 -> A   1 -> E (up)
             A  tests bit 4   0 -> head (up)   1 -> A (up)
           O  tests bit 2   0 -> I (up)   1 -> N
             N  tests bit 4   0 -> N (up)   1 -> O (up)
         Y  tests bit 1   0 -> U   1 -> Y (up)
           U  tests bit 2   0 -> Q   1 -> T
             Q  tests bit 3   0 -> Q (up)   1 -> S (up)
             T  tests bit 4   0 -> T (up)   1 -> U (up)
     sort (Program 15.6): A E I N O Q S T U Y
15.32 insert S again -> Program 15.5 finds S, sees no differing bit and returns: duplicates are ignored
15.44 keys met on their own search path: 10 of 10
15.34 patricia, keys in the given order:
       (00100001 is already there: Program 15.5 returns, nothing changes)
       11101100  tests bit 0   0 -> 01010011   1 -> 10010101
         01010011  tests bit 1   0 -> 00100001   1 -> 01001010
           00100001  tests bit 2   0 -> 00000111   1 -> 00100001 (up)
             00000111  tests bit 5   0 -> 00000000 (up)   1 -> 00000111 (up)
           01001010  tests bit 3   0 -> 01001010 (up)   1 -> 01010001
             01010001  tests bit 6   0 -> 01010001 (up)   1 -> 01010011 (up)
         10010101  tests bit 1   0 -> 10010101 (up)   1 -> 11101100 (up)
       (00000000 is the dummy head)
       tested bits: bit0 x1 bit1 x2 bit2 x1 bit3 x1 bit5 x1 bit6 x1
15.35 patricia, the same keys in reverse order:
       (00100001 is already there: Program 15.5 returns, nothing changes)
       10010101  tests bit 0   0 -> 01001010   1 -> 11101100
         01001010  tests bit 1   0 -> 00100001   1 -> 01010001
           00100001  tests bit 2   0 -> 00000111   1 -> 00100001 (up)
             00000111  tests bit 5   0 -> 00000000 (up)   1 -> 00000111 (up)
           01010001  tests bit 3   0 -> 01001010 (up)   1 -> 01010011
             01010011  tests bit 6   0 -> 01010001 (up)   1 -> 01010011 (up)
         11101100  tests bit 1   0 -> 10010101 (up)   1 -> 11101100 (up)
       (00000000 is the dummy head)
       tested bits: bit0 x1 bit1 x2 bit2 x1 bit3 x1 bit5 x1 bit6 x1
     same multiset of tested bits: yes (it is fixed by the key set; only the key placement differs)
```

15\.32: الـ STinsert بيلاقي المفتاح ويرجع على طول، فالـ patricia بتتجاهل التكرار من غير ما تقول. لو محتاج تخزّن records بنفس المفتاح، حطهم في list في الـ node.

15\.34 و15.35: الشجرتين مختلفين في مكان كل مفتاح، بس **البتات اللي بتتختبر هي هي** (0 مرة، 1 مرتين، 2، 3، 5، 6). ده لأن البتات دي هي بالظبط الـ nodes الداخلية في الـ trie بتاع نفس المجموعة، والـ trie شكله بيتحدد بالمجموعة بس. والترتيب بيغيّر بس مين يقعد في أنهي node. ولاحظ إن البت 4 والبت 7 مش موجودين خالص: مفيش مفتاحين محتاجينهم عشان يفترقوا.

15\.44: الـ node بتاعة أي مفتاح k اتعملت على مسار البحث عن k، والـ link اللي بيرجع لها (link لنفسها أو لفوق) بيتاخد لما نختبر بت k نفسه. فالبحث عن k لازم يعدّي على الـ node بتاعته، وفي الآخر يرجع لها بـ link لفوق. والبرنامج اتأكد من ده للعشرة مفاتيح كلهم. ودي فكرة 15.45: ممكن تقارن المفتاح وإنت نازل وتوقف بدري لو لقيته.
:::

### تمرين 15.36، 15.38 و15.41: select وprefix ومليون مفتاح تاني

:::problem
(15.38) اعمل الـ select لـ symbol table مبني على patricia. (15.41) اطبع كل المفاتيح في الـ patricia اللي أول t بت منها زي مفتاح معيّن. (15.36) قارن ارتفاع الـ patricia وطول مساراتها بالـ BST والـ red-black على N مفتاح عشوائي 32 بت، لـ N = 10³ لحد 10⁶.
:::

:::think
Program 15.6 بيعمل sort: inorder ويعامل الـ links اللي لفوق كأنها أوراق. فالـ select هو نفس اللفة دي، بس بنعدّ ونقف عند رقم k. (في implementation بجد هتحط عدد المفاتيح اللي تحت كل node عشان تبقى lg N). وفي الـ prefix، الـ patricia بتنط على بتات، فلما نوصل لأول node بتختبر بت ≥ t، لازم نتأكد من مفتاح واحد بس تحتها إن أول t بت بتوعه زي المطلوب. لو طلع زيه، يبقى كل اللي تحت زيه.
:::

:::solution
```
15.38 select:  select(0) = A  select(3) = N  select(6) = S  select(9) = Y
15.41 keys with the same first 2 bits as Q (10001): Q S T U
      keys with the same first 3 bits as N (01110): N O
      keys with the same first 3 bits as G (00111): E
      keys with the same first 4 bits as M (01101): (none)
15.36 N random 32-bit keys: height and average depth (internal path length / N)
            N | patricia (avg bits/search)  | BST               | red-black
         1000 | h=13 avg  8.29 (10.27)      | h=26 avg 11.90    | h=14 avg  8.63     nodes=1000 (one per distinct key)
        10000 | h=17 avg 11.62 (13.62)      | h=29 avg 14.69    | h=18 avg 11.88     nodes=10000 (one per distinct key)
       100000 | h=21 avg 14.94 (16.94)      | h=42 avg 20.46    | h=23 avg 15.15     nodes=99997 (one per distinct key)
      1000000 | h=24 avg 18.26 (20.26)      | h=50 avg 25.15    | h=27 avg 18.60     nodes=999899 (one per distinct key)
```

بص على الـ M (01101): مفيش أي مفتاح بيبدأ بـ 0110، والـ patricia عرفت ده من مقارنة مفتاح واحد. وفي التجربة: الـ patricia فيها N node بالظبط (من غير الـ 1.44 N الزيادة بتوعة الـ trie)، والارتفاع 24 عند المليون، والـ red-black 27. ومتوسط العمق 18.26، أحسن من الـ red-black (18.60). وفي الآخر مقارنة مفتاح كاملة واحدة بس في كل بحث. ده Property 15.5 بأرقام حقيقية: patricia بتدّيك أداء الشجرة المتوازنة من غير أي كود توازن.
:::

### تمرين 15.48، 15.49، 15.58 و15.62: الـ multiway trie والـ TST

:::problem
(15.48) ارسم الـ existence trie بـ 27 فرع لما ندخّل الكلمات now is the time for all good people to come the aid of their party. (15.49) ارسم الـ existence TST لنفس الكلمات. (15.58) اكتب function تطبع كل المفاتيح في الـ TST اللي بتختلف عن مفتاح البحث في k مكان على الأكثر. (15.62) اطبع كل المفاتيح في R-way trie اللي أول t بايت منها زي مفتاح معيّن.
:::

:::think
في الـ 27-way trie، كل node فيها 27 link (26 حرف + علامة «الكلمة خلصت هنا»)، والكلمة نفسها هي المسار. المشكلة إن أغلب الـ links دي NULL. الـ TST بيحل ده: كل node فيها حرف واحد وتلات links بس. «أصغر» و«أكبر» بيفضلوا عند نفس الحرف (يعني نفس الموقع في الكلمة)، و«مساوي» بس هو اللي بيتقدّم للحرف اللي بعده. وفي 15.58 (Hamming distance ≤ k): عند كل node، لو الحرف زي حرف المفتاح انزل في «مساوي» بنفس k، ولو مش زيه انزل برضه بس بـ k − 1، وفي الحالتين لف على الشمال واليمين بنفس k. ولما k يبقى سالب اقف.
:::

:::solution
```
15.48 27-way existence trie for the sentence (end-of-string = [word]):
     43 nodes x 27 links = 1161 links (most NULL)
15.62 keys with the same first 2 characters as "th": the their
      keys with the same first 3 characters as "peo": people
15.49 existence TST for the same words (56 nodes, height 9; '=' next char, '<' '>' siblings, '$' end):
      n
       <i
         <f
           <a
             =l
               <i
                 =d
                   =$   [aid]
               =l
                 =$   [all]
             >c
               =o
                 =m
                   =e
                     =$   [come]
           =o
             =r
               =$   [for]
           >g
             =o
               =o
                 =d
                   =$   [good]
         =s
           =$   [is]
       =o
         =w
           =$   [now]
       >t
         <p
           <o
             =f
               =$   [of]
           =e
             <a
               =r
                 =t
                   =y
                     =$   [party]
             =o
               =p
                 =l
                   =e
                     =$   [people]
         =h
           =e
             =$   [the]
               >i
                 =r
                   =$   [their]
           >i
             =m
               =e
                 =$   [time]
             >o
               =$   [to]
15.58 keys within Hamming distance 1 of "the": the
      keys within Hamming distance 2 of "tome": come time to
```

طريقة قراية الـ TST: `=x` معناها الحرف اللي بعده في نفس الكلمة، و`<x` و`>x` حروف تانية بديلة في نفس الموقع، و`$` معناها «الكلمة خلصت هنا». مثلًا الجذر n، وعلى شماله i، وعلى شمال i جت f، وهكذا. الحروف الأولى للكلمات اتعملت منها BST صغيرة.

الأرقام: الـ trie فيه 43 node، كل واحدة بـ 27 link، يعني 1161 link أغلبهم NULL. والـ TST فيه 56 node بس بـ 3 links، يعني 168 link، أقل بحوالي 7 مرات. ولاحظ إن «the» اتكررت في الجملة ومحسوبة مرة واحدة: دي existence table. وفي 15.58: الكلمات اللي على بعد 2 من «tome» هي come وtime (حرف واحد مختلف)، وto (أقصر بحرفين، واحنا اعتبرنا كل حرف زيادة أو ناقص اختلاف).
:::

### تمرين 15.50–15.52: نفس المفاتيح بـ «بايتات» مختلفة

:::problem
(15.50) ارسم الـ 4-way trie للمفاتيح الـ 8 بت بتوع 15.4، باستعمال بايت = 2 بت. (15.51) ارسم الـ TST لنفس المفاتيح بـ 2 بت. (15.52) وبـ 4 بت.
:::

:::think
بنقسم الـ 8 بت لأرقام: بـ 2 بت كل مفتاح بيبقى 4 أرقام من 0 لـ 3 (01010011 = 1 1 0 3)، وبـ 4 بت بيبقى رقمين hex (01010011 = 5 3). وبعد كده نتعامل معاهم كأنهم حروف بالظبط. كل ما الـ «بايت» يكبر، الشجرة تقصر، بس الـ trie يحتاج links أكتر في كل node (R = 2^bits).
:::

:::solution
```
15.50 4-way trie (2-bit bytes) for the 8-bit keys (24 nodes):
       leaf 0013     : 00000111
       leaf 0201     : 00100001
       leaf 1022     : 01001010
       leaf 1101     : 01010001
       leaf 1103     : 01010011
       leaf 2111     : 10010101
       leaf 3230     : 11101100
15.52 TST with 4-bit bytes (20 nodes):
      5
       <0
         =7
           =$   [07]
         >2
           =1
             =$   [21]
           >4
             =a
               =$   [4a]
       =3
         <1
           =$   [51]
         =$   [53]
       >e
         <9
           =5
             =$   [95]
         =c
           =$   [ec]
```

الـ 4-way trie فيه 24 node (كل واحدة بـ 4 links، يعني 96 link). والـ TST بـ 2 بت 30 node (90 link)، وبـ 4 بت 20 node بس (60 link)، وأقصى عمق 3. يعني لما البايت يكبر، الـ TST بيكسب أكتر، لأنه مابيحجزش 16 link في كل node زي الـ 16-way trie. ولاحظ إن المفتاح المكرر 00100001 اتشال من الأول.
:::

### تمرين 15.76–15.78: string index للجملة

:::problem
(15.76) ارسم الـ DST بـ 26 فرع لما نبني text-string index من الكلمات now is the time for all good people to come the aid of their party. (15.77) ارسم الـ 26-way trie لنفس الـ index. (15.78) ارسم الـ TST لنفس الـ index بنفس ستايل Figure 15.20.
:::

:::think
في الـ string index، المفاتيح مش الكلمات. المفاتيح هي **كل اللي فاضل في النص** من أول كل كلمة (suffix)، وكل مفتاح بيمثّل مكان في النص. الـ DST بتخزّن مكان واحد في كل node وبتتفرّع بالحرف. أما الـ trie والـ TST فمش محتاجين المفتاح كله: بنقطع كل suffix عند أول حرف يخلّيه مختلف عن الباقيين، وده اللي Figure 15.20 بيعمله. وعشان نعرف نقطع فين: نرتّب الـ suffixes، وكل واحد محتاج (أكبر lcp مع جيرانه) + 1 حرف.
:::

:::solution
```c
15.76-78 text (spaces removed) = "nowisthetimeforallgoodpeopletocometheaidoftheirparty", one suffix per word start
15.76 26-way DST string index (each node holds one text position):
     pos  0 "nowisthetime..."
     a\
       pos 15 "allgoodpeopl..."
       i\
         pos 37 "aidoftheirpa..."
     c\
       pos 30 "cometheaidof..."
     f\
       pos 12 "forallgoodpe..."
     g\
       pos 18 "goodpeopleto..."
     i\
       pos  3 "isthetimefor..."
     o\
       pos 40 "oftheirparty..."
     p\
       pos 22 "peopletocome..."
       a\
         pos 47 "party..."
     t\
       pos  5 "thetimeforal..."
       h\
         pos 34 "theaidofthei..."
         e\
           pos 42 "theirparty..."
       i\
         pos  8 "timeforallgo..."
       o\
         pos 28 "tocometheaid..."
15.77 26-way trie string index: 21 nodes for 15 positions; each leaf [prefix] -> position:
       [ai       ] -> pos 37  "aidoftheirpart..."
       [al       ] -> pos 15  "allgoodpeoplet..."
       [c        ] -> pos 30  "cometheaidofth..."
       [f        ] -> pos 12  "forallgoodpeop..."
       [g        ] -> pos 18  "goodpeopletoco..."
       [i        ] -> pos  3  "isthetimeforal..."
       [n        ] -> pos  0  "nowisthetimefo..."
       [o        ] -> pos 40  "oftheirparty..."
       [pa       ] -> pos 47  "party..."
       [pe       ] -> pos 22  "peopletocometh..."
       [thea     ] -> pos 34  "theaidoftheirp..."
       [thei     ] -> pos 42  "theirparty..."
       [thet     ] -> pos  5  "thetimeforallg..."
       [ti       ] -> pos  8  "timeforallgood..."
       [to       ] -> pos 28  "tocometheaidof..."
15.78 TST string index for the same cut suffixes, inserted in text order (35 nodes, height 8;
      inserted in sorted order the height would be 15: the first-level BST degenerates):
      n
       <i
         <f
           <a
             =l
               <i
                 =$   [ai]
               =$   [al]
             >c
               =$   [c]
           =$   [f]
           >g
             =$   [g]
         =$   [i]
       =$   [n]
       >t
         <p
           <o
             =$   [o]
           =e
             <a
               =$   [pa]
             =$   [pe]
         =h
           =e
             =t
               <a
                 =$   [thea]
                 >i
                   =$   [thei]
               =$   [thet]
           >i
             =$   [ti]
             >o
               =$   [to]
```

الـ 15 suffix اتمثلوا بـ 21 node في الـ trie، و35 node في الـ TST. وأطول بادئة احتجناها «thea» و«thei» و«thet»، لأن the متكررة: مرة «the time»، ومرة «the aid»، و«their». ولما تدوّر على أي كلمة في النص، تمشي في الحروف لحد ما توصل لورقة، والورقة بتقول لك المكان، وبعدين تقارن الباقي في النص نفسه.

وجرّبنا كمان ندخّل نفس الـ suffixes في الـ TST بالترتيب الأبجدي، فالارتفاع بقى 15 بدل 8. كل الحروف الأولى بقت سلسلة يمين ورا يمين، لأن الـ BST اللي في أول مستوى بتبوظ مع المدخلات المترتبة زي أي BST. فلو بتبني index من ملف مترتب، الأحسن تلخبطه الأول أو تدخّل بترتيب النص.
:::

### تمرين 15.83، 15.86، 15.87 و15.89: نص ضخم بجد

:::problem
(15.86) اكتب برنامج كويس يلاقي أطول جزء متكرر في نص ضخم. (15.87) اكتب برنامج يلاقي أكتر 10 حروف ورا بعض بتتكرر في نص ضخم. (15.83) اشرح ليه فكرة تحسين الـ binary search بنفس مبدأ الـ TST (نقارن حروف مش strings كاملة) مش بتفيد. (15.89) اوصف نص من N حرف يخلّي الـ TST string index أداؤه وحش جدًا، وقدّر تكلفة بناء index لنفس النص بـ BST.
:::

:::think
الـ index في الآخر هو كل الـ suffixes مترتبة، والـ trie أو الـ TST بيدّوك الترتيب ده بـ inorder. وأرخص طريقة نعمله بيها: مصفوفة فيها N رقم (أماكن) ونرتّبها بمقارنة حرف حرف. ده اسمه suffix array: نفس المعلومة بذاكرة أقل بكتير. ولما الـ suffixes تبقى مترتبة: أي اتنين بيشتركوا في بداية طويلة لازم يبقوا جنب بعض. فأطول تكرار = أكبر lcp بين جارين. وأي 10 حروف متكررة هتظهر كمجموعة suffixes ورا بعض متفقين في أول 10 حروف، فنعدّ المجموعات دي.
:::

:::solution
```
text 1: the sentence (N = 66 characters)
     15.86 longest repeated sequence: 5 chars, at positions 43 and 6:
       " the "
     15.87 most frequent 10-character sequence: " aid of th" occurs 1 times
text 2: random a/c/g/t with the same 24 letters planted at 123456 and 876543 (N = 1000000 characters)
     15.86 longest repeated sequence: 24 chars, at positions 123456 and 876543:
       "gattacagattacacatgattaca"
     15.87 most frequent 10-character sequence: "gtctgatgga" occurs 9 times
     (a random 4-letter text of 10^6 chars repeats about 2 log_4 N = 20 chars by chance)
15.83 binary search for 1000 substrings of length 20 in the sorted suffixes: 1000 found,
      117.9 character compares per search for lg N = 19.9 probes. Each probe re-reads the
      characters that already matched; per probe we still only halve the interval.
15.89 cost of a TST string index that inserts EVERY full suffix:
      every character of every suffix is walked once, so the cost is always N(N+1)/2,
      for ANY text. The all-'a' string is worse still for SEARCH: the TST is a single
      spine of height N, and every search of a suffix of length k costs k character steps.
      N= 1000  all-'a' TST steps    502499 (N(N+1)/2 = 500500, height 1001)
               random  TST steps    506818 (same order: every suffix is fully stored)
               all-'a' BST insert character compares ~ 167166000 (each new short suffix loses to every longer one)
      N= 2000  all-'a' TST steps   2004999 (N(N+1)/2 = 2001000, height 2001)
               random  TST steps   2015755 (same order: every suffix is fully stored)
               all-'a' BST insert character compares ~ 1335332000 (each new short suffix loses to every longer one)
      N= 4000  all-'a' TST steps   8009999 (N(N+1)/2 = 8002000, height 4001)
               random  TST steps   8033339 (same order: every suffix is fully stored)
               all-'a' BST insert character compares ~ 10674664000 (each new short suffix loses to every longer one)
      N= 8000  all-'a' TST steps  32019999 (N(N+1)/2 = 32004000, height 8001)
               random  TST steps  32072398 (same order: every suffix is fully stored)
               all-'a' BST insert character compares ~ 85365328000 (each new short suffix loses to every longer one)
      fix used by the book (Figure 15.20): cut each suffix as soon as it becomes unique
      among the set. For a random text that is about 2 log_sigma N characters per suffix
      (the longest-repeat length above), so building is near-linear; only pathological
      texts like all-'a' still force Theta(N^2).
```

```
$ ./ex15_86 sedgewick.txt
sedgewick.txt (N = 1580125 characters)
     15.86 longest repeated sequence: 250 chars, at positions 561107 and 168616:
       ", 1988. D. E. Knuth, The Art of Computer Programming. Volume 1: Fun­ ..."
     15.87 most frequent 10-character sequence: " implement" occurs 1184 times
```

جرّبناه كمان على الكتاب نفسه، 1.58 مليون حرف. أطول جزء متكرر طلع 250 حرف من قايمة المراجع: «1988. D. E. Knuth, The Art of Computer Programming. Volume 1: Fun…»، لأن المراجع مكتوبة مرتين في آخر الكتاب. وأكتر 10 حروف متكررة طلعت « implement»، 1184 مرة. وده منطقي جدًا لكتاب كل صفحة فيه بتقول «implement». الكلام ده كله اتعمل في حوالي 3 ثواني.

15\.83: الـ binary search عملت حوالي 118 مقارنة حرف عشان تلاقي 20 حرف. يعني في كل خطوة من الـ 20 خطوة بتعيد قراية الحروف اللي اتفقت قبل كده. والمقارنة حرف بحرف مابتخلّيش الـ interval يصغر أسرع من النص. الـ TST بيكسب لأنه **مابيرجعش** للحروف اللي اتفقت، بيسيبها وراه وينزل في «مساوي». والـ binary search مش هتعرف تعمل كده غير لو فضلت شايلة الـ lcp مع طرفين الـ interval، ودي حسابات زيادة، مش فكرة الـ TST.

15\.89: النص «aaaa…a». كل suffix بادئة من اللي أطول منه، فالـ TST بيبقى عمود واحد ارتفاعه N، والبناء بيكلّف N(N+1)/2 بالظبط. والـ BST أوحش بكتير: الـ suffixes بتيجي من الأطول للأقصر، وكل واحد أصغر من اللي قبله، فالشجرة بتبقى list شمال، وكل مقارنة بتقرا الـ suffix الأقصر كله. يعني حوالي N³/6 مقارنة حرف (8.5 × 10¹⁰ لـ N = 8000). وملاحظة مهمة: الـ TST اللي بيخزّن كل suffix كامل بيكلّف N²/2 مع **أي** نص، عشان كده بنقطع كل suffix أول ما يبقى مختلف عن الباقيين.
:::

:::quiz
نفس العشر مفاتيح اتدخلوا في trie مرة بترتيب ومرة بالعكس. إيه اللي هيتغير؟

1. [ ] الارتفاع
2. [ ] عدد الـ nodes الداخلية
3. [x] ولا حاجة

> شكل الـ trie بيتحدد بمجموعة المفاتيح بس. جرّبناها في 15.11 وطلع نفس الـ 11 node الداخلية ونفس الارتفاع.
:::

:::quiz
مليون مفتاح عشوائي في patricia. عدد الـ nodes قد إيه؟

1. [ ] حوالي 1.44 مليون
2. [x] مليون بالظبط (واحدة لكل مفتاح مختلف)
3. [ ] حوالي 2.44 مليون

> الـ patricia بتشيل الـ nodes اللي ليها طريق واحد، فكل node فيها مفتاح. الـ 2.44 مليون دول بتوع الـ trie (داخلي + أوراق).
:::

:::quiz
إزاي تلاقي أطول جزء متكرر في نص بعد ما ترتّب الـ suffixes؟

1. [ ] تقارن كل اتنين suffixes ببعض
2. [x] تبص على الـ lcp بين كل suffix واللي جنبه بس
3. [ ] تدوّر على أطول suffix

> أي اتنين بيشتركوا في بداية طويلة لازم يبقوا جنب بعض في الترتيب، فلفّة واحدة على الجيران كفاية.
:::
