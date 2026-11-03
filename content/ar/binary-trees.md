المراجع: Thareja ف9 (9.1–9.5: مصطلحات، binary trees، تمثيلها، traversals، expression trees، Huffman، tournament trees، general trees، forests) · Weiss ف4 (4.1 Preliminaries بما فيه 4.1.1 first-child/next-sibling و4.1.2 الـ directory listing، 4.2 Binary Trees و4.2.2 Expression Trees، 4.6 Tree Traversals Revisited) · Lipschutz ف7 (7.1–7.5: binary trees، التمثيل، traversals بالـ stack) · Bandyopadhyay &amp; Dey ف12 (12.1–12.3) · Pal &amp; Halder ف8 (Terminology, Strictly/Extended/Complete/Full/Skewed/Expression/Balanced، Representation، Traversal)

الدرس ده بيمسك الشجر من الزاوية اللي الكتب الخمسة اتفقوا عليها: **إيه هي الـ binary tree بالظبط، إزاي تتخزن في الذاكرة، وإزاي نمشي عليها بأربع طرق**. ملاحظة: درس [«الشجر: تعريفات وخصائص ولفّ»](lesson:trees) بتاع Sedgewick اتكلم عن الـ recursion على الشجر والـ traversals الأساسية، وهنا مش هنعيدها: هنركّز على اللي ماكانش موجود (التمثيل في array بالـ LEFT/DATA/RIGHT، الـ traversals الغير recursive بالـ stack وبـ **Morris** من غير stack خالص، بناء الشجرة من traversalين، شجر التعبيرات، Huffman، الـ general trees).

:::goal هدف الدرس:
تقدر تحل أي سؤال شجرة من الكتب دي: ترسم من array، تطلّع الـ 4 traversals، تبني الشجرة من اتنين منهم، تحوّل infix لشجرة وتقرأها prefix وpostfix، وتبني شجرة Huffman بإيدك. كل الكود في `src/20-trees/` متجرَّب بـ AddressSanitizer، وفيه **assert** لكل نتيجة رقمية في الدرس.
:::

## 1) المصطلحات (Thareja 9.1، Pal 8.1، B&amp;D 12.1)

الشجرة (tree) مجموعة عقد (nodes) فيها عقدة واحدة اسمها **root** والباقي موزّع على أشجار فرعية (sub-trees) منفصلة، كل واحدة تعريفها نفس التعريف (يعني التعريف **recursive**). الشجرة اللي فيها n عقدة ليها **n − 1 edge** بالظبط: كل عقدة غير الجذر ليها أب واحد، يعني حافة واحدة لفوق.

::figure ds20-terms

<table class="comp-table">
<tr><th>المصطلح</th><th>المعنى</th><th>في الشكل (Fig 9.3)</th></tr>
<tr><td>parent / child</td><td>العقدة اللي فوق / اللي تحت على نفس الحافة</td><td>2 و3 أبناء 1</td></tr>
<tr><td>siblings</td><td>أبناء نفس الأب</td><td>4 و5 إخوات</td></tr>
<tr><td>leaf (terminal)</td><td>مالهاش أبناء (درجتها 0)</td><td>5, 8, 9, 10, 11, 12</td></tr>
<tr><td>internal node</td><td>ليها ابن على الأقل</td><td>1, 2, 3, 4, 6, 7</td></tr>
<tr><td>ancestor / descendant</td><td>أي عقدة على المسار لفوق / لتحت</td><td>أجداد 11: 6, 3, 1</td></tr>
<tr><td>degree</td><td>عدد أبناء العقدة (في الـ binary tree 0 أو 1 أو 2)</td><td>درجة 7 = 1</td></tr>
<tr><td>level</td><td>عدد الحواف من الجذر. <b>الجذر level 0</b> (Thareja)</td><td>level 12 = 3</td></tr>
<tr><td>height</td><td><b>عدد العقد</b> على أطول مسار من الجذر لورقة (شجرة بجذر بس ارتفاعها 1 عند Thareja)</td><td>4</td></tr>
<tr><td>forest</td><td>مجموعة أشجار منفصلة (شيل الجذر من شجرة تبقى forest)</td><td>أبناء الجذر: forest من اتنين</td></tr>
</table>

:::oops اختلاف الكتب في الارتفاع
Thareja وPal &amp; Halder بيعدّوا الارتفاع بعدد **العقد** (شجرة العقدة الوحيدة = 1) وLipschutz كمان (depth = عدد العقد). Weiss وSedgewick بيعدّوا **الحواف** (العقدة الوحيدة = 0). نفس الشجرة، رقم مختلف بواحد. الدرس والكود هنا بيمشوا على Thareja وبيقولوا ده صراحةً في الأماكن اللي فرقها بيظهر (مثلًا «أكبر عدد مستويات في BST فيه 100 عقدة»: 100 مستوى مرقمة 0 لـ 99).
:::

## 2) أنواع الـ binary trees وخصائصها

**Binary tree**: كل عقدة ليها **على الأكتر ابنين**، ومميّزين: شمال ويمين (فالشجرتين «A ابنها الشمال B» و«A ابنها اليمين B» مختلفتين). الأنواع (Thareja 9.2، Pal 8.2–8.7):

::figure ds20-types

<table class="comp-table">
<tr><th>النوع</th><th>التعريف</th><th>أسماء تانية</th></tr>
<tr><td><b>Full</b></td><td>كل عقدة ليها 0 أو 2 ابن</td><td>strictly binary (Pal) · 2-tree · <b>extended</b> لما نكمّل الناقص بعقد خارجية</td></tr>
<tr><td><b>Complete</b></td><td>كل المستويات ممتلئة ما عدا الأخير، والأخير ممتلئ من الشمال</td><td>اللي بيتخزن في array من غير فراغات (الـ heap)</td></tr>
<tr><td><b>Skewed</b></td><td>كل عقدة ليها ابن واحد بس: chain (left-skewed أو right-skewed)</td><td>أسوأ حالة: الارتفاع = n</td></tr>
<tr><td><b>Balanced</b></td><td>فرق الارتفاع بين الشمال واليمين ≤ 1 عند كل عقدة (AVL)</td><td>height-balanced</td></tr>
<tr><td><b>Extended</b></td><td>الشجرة بعد ما نضيف «عقدة خارجية» (مربع) مكان كل NULL: كل عقدة داخلية ليها بالظبط ابنين</td><td>2-tree</td></tr>
</table>

### الخصائص (وكلها متحقّق منها في الكود)

* أقصى عدد عقد في level r هو **2<sup>r</sup>** (الجذر r = 0). فمستوى 3 فيه 8، ومستوى 4 فيه 16، ومستوى 12 فيه 4096.
* الشجرة بارتفاع h (بعدد العقد) فيها **على الأقل h وعلى الأكتر 2<sup>h</sup> − 1** عقدة.
* الشجرة بـ n عقدة ارتفاعها **على الأقل ⌈log₂(n+1)⌉ وعلى الأكتر n**.
* الـ complete tree بـ n عقدة ارتفاعها بالظبط ⌈log₂(n+1)⌉. مثال: 27 عقدة: المستويات 0–3 فيها 15، والأخير (level 4) فيه **12**، الارتفاع 5.
* في الـ extended tree اللي فيها n عقدة داخلية: عدد العقد الخارجية = **n + 1**، و**E = I + 2n** (E مجموع أطوال المسارات للخارجية وI للداخلية).
* الـ full tree: عدد الأوراق = عدد العقد الداخلية + 1.

::code src="src/20-trees/tree_core.c" region="node"

::code src="src/20-trees/tree_core.c" region="parse"

الـ `parse` بتخلّيك تكتب أي شكل من الكتاب في سطر: `"A(B(D,E),C(F,G))"` و`.` معناها فاضي. كل اختبارات الدرس بتستعملها.

::code src="src/20-trees/tree_core.c" region="measures"

::code src="src/20-trees/tree_core.c" region="shape"

`similar` بتقول إن الشجرتين ليهم نفس **الشكل** (بغض النظر عن القيم)، و`copies` بتقول نفس الشكل **والمحتوى** (Thareja بيسمّيهم similar وcopies، وده سؤال Fill-in رقم 6). `is_complete` بتعمل BFS وبتتأكد إن أول NULL مايجيش بعده عقدة حقيقية.

## 3) تمثيل الشجرة في الذاكرة

### أ) التمثيل الرابط (linked) بثلاث arrays

Thareja 9.2.3: ثلاث arrays `LEFT[]` و`DATA[]` و`RIGHT[]` و`ROOT` و`AVAIL` (رأس قايمة الخانات الفاضية). الخانة i هي «عقدة»؛ `LEFT[i]` و`RIGHT[i]` أرقام خانات (أو −1 لـ NULL). ده هو الـ pointer بس بـ index. تمرين Q14 و Q15 من الفصل بيطلب تحويل في الاتجاهين وهنحلّه.

::code src="src/20-trees/tree_core.c" region="memrep"

### ب) التمثيل المتتابع (sequential) في array واحدة

الجذر في `TREE[1]`. لو أب في **K**: الشمال في **2K** واليمين في **2K+1**، والأب لعقدة في K هو **⌊K/2⌋**. مفيش pointers، ومحتاجين **2<sup>h</sup> − 1** خانة. مناسب للـ complete trees (مفيش فراغ) وسيء جدًا للـ skewed: شجرة بـ 20 عقدة chain ممكن تحتاج مليون خانة.

::code src="src/20-trees/tree_core.c" region="seqrep"

::figure ds20-mem

:::tip لو الـ index بيبدأ من 0
الأب في i: الشمال 2i+1 واليمين 2i+2. الكتاب (وكل الصيغ دي) بتبدأ من 1 عشان الحسبة تطلع أنضف.
:::

## 4) الـ Traversals: أربع طرق

أي تجوال بيمر على كل عقدة **مرة واحدة** في O(n). التلاتة العميقة (depth-first) بتختلف في **إمتى نزور الجذر** بالنسبة لأبنائه، والرابع (level-order / breadth-first) بيمشي مستوى مستوى:

<table class="comp-table">
<tr><th>الاسم</th><th>الترتيب</th><th>بنستعمله في</th></tr>
<tr><td><b>Pre-order</b></td><td>N ← L ← R</td><td>نسخ الشجرة، prefix من expression tree، listing الـ directories (Weiss)</td></tr>
<tr><td><b>In-order</b></td><td>L ← N ← R</td><td>في BST بيطلّع المفاتيح <b>مرتبة</b>؛ infix من expression tree</td></tr>
<tr><td><b>Post-order</b></td><td>L ← R ← N</td><td>حذف الشجرة (الأبناء الأول)، postfix، حساب حجم الـ directory</td></tr>
<tr><td><b>Level-order</b></td><td>مستوى مستوى بـ queue</td><td>إيجاد أقصر مسار، طباعة بالمستويات، فحص الـ complete</td></tr>
</table>

::code src="src/20-trees/tree_core.c" region="recursive"

::figure ds20-preorder

على الشجرة `A(B(D(G,H(.,L)),E),C(.,F(I,J(K,.))))` (Fig 9.19(a) في Thareja، Examples 9.6–9.8): pre = `A B D G H L E C F I J K`، in = `G D H L B E A C I F K J`، post = `G L H D E B I K J F C A`، level = `A B C D E F G H I J L K`. البرنامج بيتأكد منهم بـ assert.

::code src="src/20-trees/tree_core.c" region="level"

### من غير recursion: stack صريح (Lipschutz 7.4، Thareja 9.3)

الـ recursion نفسها stack (راجع [درس 43](lesson:stacks-queues-apps)). لو الشجرة عميقة (skewed بمليون عقدة) الـ call stack هيطفح. النسخة الـ iterative: pre-order بتدفع اليمين وبعدين الشمال. in-order بتنزل شمال لأقصى حد وبتدفع في السكة. post-order أصعب لأن العقدة بتتزار بعد **مرتين** رجوع: الحل بـ `last` (آخر عقدة اتزارت) عشان نعرف جاي من الشمال ولا من اليمين.

::code src="src/20-trees/tree_core.c" region="iterative"

### Morris: in-order بـ O(1) مساحة (من غير stack ومن غير recursion)

الفكرة (وهي أصل الـ threaded trees في الدرس الجاي): قبل ما ننزل شمال من عقدة cur، ندوّر على **predecessor** بتاعها (أقصى يمين في الشمال) ونخلّي `right` بتاعه يشاور مؤقتًا على cur. لما نرجع للعقدة عن طريق الـ thread نعرف إننا خلّصنا الشمال فنشيل الـ thread ونزور cur ونروح يمين. الشجرة بترجع زي ما كانت بالظبط، والبرنامج بيتأكد.

::code src="src/20-trees/tree_core.c" region="morris"

:::oops ماتستعملش Morris مع threads
لأنها بتعدّل الشجرة مؤقتًا: لو فيه thread آخر بيقرأ الشجرة في نفس الوقت، أو استثناء نصّ الـ traversal، الشجرة هتفضل بـ links مقلوبة. للاستعمال المتزامن استعمل stack.
:::

## 5) بناء الشجرة من traversalين

traversal واحد **مش بيحدد** الشجرة. اتنين بيحددوها في حالتين: **(pre + in)** أو **(post + in)**. السبب: pre بيقولك مين الجذر (أول عنصر)، وin بيقولك مين في الشمال ومين في اليمين (اللي قبل الجذر وبعده). نكرر على الجزئين. (pre + post) مش كفاية لما فيه عقدة بابن واحد: ماقدرش أعرف الابن شمال ولا يمين.

::code src="src/20-trees/tree_core.c" region="construct"

مثال (Thareja 9.4.5): in = `D B E A F C G` و pre = `A B D E C F G` بيطلّعوا شجرة جذرها A و post بتاعها `D E B F G C A`. ولو اديتها post = `D E B F G C A` مع نفس الـ in بتطلع **نفس الشجرة**. البرنامج بيعمل الاتنين ويقارنهم.

## 6) شجرة التعبير (Expression Tree) — Thareja 9.4، Weiss 4.2.2

الأوراق = معاملات والعقد الداخلية = عمليات. القراءة: **in-order** = infix (بتحتاج أقواس)، **pre-order** = prefix، **post-order** = postfix. والبناء من postfix بـ stack: معامل → اعمل له عقدة وادفعها. عملية → pop اتنين (الأول هو اليمين) وركّب عقدة ليهم وادفعها. ده بالظبط Weiss 4.2.2.

::figure ds20-exprtree

::code src="src/20-trees/expr_tree.c" region="fromfix"

من infix مباشرة: محلل recursive-descent بثلاث دوال للمستويات الثلاثة (expr ثم term ثم factor)، والـ `^` بتتجمّع من اليمين:

::code src="src/20-trees/expr_tree.c" region="frominfix"

::code src="src/20-trees/expr_tree.c" region="printfix"

الـ `infix_full` بتحط أقواس على كل عملية (آمنة دايمًا)، أما `infix_min` بتحط أقواس بس لما ضروري. القاعدة: ابن شمال يحتاج قوس لو أولويته **أقل** من أبوه، وابن يمين يحتاج قوس لو أولويته **أقل أو تساوي** (عشان الشمالي بيتجمّع من الشمال) — والعكس تمامًا للـ `^`. لذلك `a−(b−c)` بتحتفظ بالقوس و`(a−b)−c` بتفقده.

::code src="src/20-trees/expr_tree.c" region="evalt"

::output src="src/20-trees/expr_tree.c" lines="16"

## 7) كام شجرة ممكنة؟ والـ general trees

عدد الأشكال المختلفة لشجرة ثنائية بـ n عقدة هو **عدد كاتالان** C(n) = (2n)! / ((n+1)! n!): 1, 1, 2, 5, 14, 42, 132 … (والصيغة: C(n) = Σ C(i)·C(n−1−i) لأن كل عقدة جذر بتقسم الباقي لشمال ويمين). مع 3 عقد: 5 أشكال؛ مع 4: 14. الكود بيعدّها وبيطبعها.

::figure ds20-shapes

::code src="src/20-trees/tree_core.c" region="shapes"

### الشجرة العامة (General tree) → شجرة ثنائية

عقدة ممكن تبقى ليها أي عدد أبناء. بدل ما نحجز array pointers، كل عقدة بتحتفظ بـ **أول ابن** وبـ **الأخ التالي** (first-child / next-sibling، Weiss 4.1.1). ده نفسه التحويل لشجرة ثنائية: ابن أول = left، أخ تالي = right. والـ forest بتتحوّل بنفس الطريقة (الجذور إخوات).

::figure ds20-general

::code src="src/20-trees/tree_core.c" region="general"

### تطبيقا Weiss الكلاسيكيين: listing الـ directory وحجمه

الـ UNIX directory شجرة عامة. **pre-order** بيطبع الاسم قبل أولاده مع مسافة بادئة بعمق العقدة (Weiss Fig 4.6–4.7): O(N) لأن كل عقدة تتزار مرة. **post-order** بيحسب الحجم (Fig 4.8–4.10) لأن حجم الـ directory مش معروف إلا بعد حجم كل أولاده؛ ولذلك سطر `/usr` بيطلع **آخر** سطر.

::code src="src/20-trees/fcns.c" region="fcns"

::output src="src/20-trees/fcns.c" lines="30"

## 8) Huffman: أطول مسار موزون وأشجار الاختصار

### Weighted path length

لو للأوراق (العقد الخارجية) أوزان w<sub>i</sub> وعمق d<sub>i</sub>: **WEPL = Σ w<sub>i</sub>·d<sub>i</sub>**. الشجرة اللي تقلّلها لنفس مجموعة الأوزان هي شجرة Huffman. (Thareja Q16: الأشجار T1 وT2 وT3 بتطلع 77 و49 و36.)

::code src="src/20-trees/huffman.c" region="pathlen"

### خوارزمية Huffman

عايزين كود لكل حرف: **prefix-free** (ولا كود بداية كود تاني، فنفك التشفير من غير فواصل) وأقصر ما يمكن للحروف الكتيرة. الخوارزمية الجشعة (greedy): ابدأ بغابة أوراق بأوزانها، وكرّر: خد **أصغر شجرتين** وادمجهم تحت جذر جديد وزنه المجموع (الأصغر شمال)، لحد ما تبقى شجرة واحدة. الكود لكل حرف = مسار الجذر للورقة (0 شمال، 1 يمين). إثبات الأمثلية: أصغر وزنين لازم يبقوا إخوات في أعمق مستوى في أي شجرة مثالية (لو ماكانوش نبدّلهم مع اللي هناك ومفيش خسارة).

::figure ds20-huffman

::code src="src/20-trees/huffman.c" region="huff"

:::tip ليه الـ priority queue «مستقرة» مهمة؟
لما وزنين يتساووا ممكن يطلع أكتر من شجرة Huffman صحيحة، كلها بنفس WEPL بس بأكواد مختلفة. البرنامج بيدّي الأفضلية للأقدم عشان النتيجة تبقى ثابتة وتتطابق مع الرسم. الاتنين صح.
:::

::output src="src/20-trees/huffman.c" lines="20"

### Tournament (winner) trees

شجرة complete فيها الأوراق هي اللاعبين وكل عقدة داخلية هي **الفائز** من ابنيها: الجذر هو الأصغر في O(n) بنا. لما نغيّر قيمة ورقة نعيد المباريات على مسارها بس: O(log n). ده أساس الـ replacement selection في الـ external sorting.

::code src="src/20-trees/huffman.c" region="winner"

:::quiz
شجرة ثنائية بـ 27 عقدة، complete. كام عقدة في الـ level الأخير وكام الارتفاع؟

1. [ ] 13 و 5
2. [x] 12 و 5
3. [ ] 14 و 6

> المستويات 0–3 فيها 1+2+4+8 = 15، يبقى 27 − 15 = 12 في level 4 (سعته 16). الارتفاع ⌈log₂28⌉ = 5.
:::

:::quiz
in-order = `D B A E C` و pre-order = `A B D C E`. إيه الـ post-order؟

1. [ ] D E C B A
2. [x] D B E C A
3. [ ] مش ممكن نحدد

> A الجذر (أول pre). في in: `D B` شمال و`E C` يمين. pre الشمال `B D` → B فوق D شمال. pre اليمين `C E` → C فوق E شمال. post = `D B` + `E C` + `A`.
:::

:::quiz
ليه (pre-order + post-order) مش بيحدد الشجرة الثنائية؟

1. [ ] لأن الـ post مش بيقول الجذر
2. [x] لأن العقدة اللي ليها ابن واحد: ماقدرش أعرف هو شمال ولا يمين
3. [ ] بيحدد بس لو الشجرة complete

> الشجرتين `A(B,.)` و`A(.,B)` ليهم نفس pre (`A B`) ونفس post (`B A`). لو الشجرة full (كل عقدة 0 أو 2 ابن) فهو بيحدد.
:::

:::quiz
إيه الـ postfix اللي بيطلع من شجرة التعبير `(a+b*c)+((d*e+f)*g)` (Weiss Fig 4.14)؟

1. [ ] a b c + \* d e \* f + g \* +
2. [x] a b c \* + d e \* f + g \* +
3. [ ] \+ + a \* b c \* + \* d e f g

> الأخير هو الـ prefix. الأولى غلط لأن \* أقوى من + فـ b c \* بيتحسب الأول.
:::

:::quiz
إيه ميزة Morris traversal؟ وإيه عيبها؟

1. [ ] أسرع من الـ stack بكتير
2. [x] O(1) مساحة إضافية من غير stack ولا recursion، بس بتعدّل الـ pointers مؤقتًا (مش آمنة مع القراءة المتزامنة) وبتزور كل حافة مرتين
3. [ ] بتشتغل بس على BST

> الزمن لسه O(n) (كل حافة بتتمشى على الأكتر 3 مرات).
:::
