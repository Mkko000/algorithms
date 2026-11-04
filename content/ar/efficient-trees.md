المراجع: Thareja ف10 (10.1 BST، 10.2 عملياتها، 10.3 threaded، 10.4 AVL، 10.5 red-black، 10.6 splay) · Weiss ف4 (4.3 BST بما فيه 4.3.6 الحالة المتوسطة، 4.4 AVL، 4.5 splay، 4.6 traversals، 12.2 red-black و11.5 تحليل splay) · Lipschutz ف7 (7.6–7.7 header nodes وthreads، 7.8–7.10 BST، 7.11–7.14 AVL) · Bandyopadhyay &amp; Dey ف12 (12.4–12.6) · Pal &amp; Halder ف8 (threaded، BST، heap، AVL، red-black)

الدرس ده بيكمّل [درس الشجر الثنائي](lesson:binary-trees) (الرقم 44 في الكورس): هنا الأشجار اللي بتتحسّن عشان البحث يفضل **O(log n)** دايمًا، أو بتسرّع الـ traversal، أو بتتأقلم مع نمط الاستعمال. ملاحظة تجنّب التكرار: Sedgewick ف12–13 شرحوا الـ BST بواجهة symbol-table ([BST](lesson:bst) و[الحذف والـ join](lesson:bst-ops)) وشرحوا [splay بالنزول من فوق (top-down)](lesson:balanced) و[2-3-4 والـ red-black](lesson:rb-trees). هنا النسخة اللي في الكتب التانية: **كود المؤشرات المعتاد، الـ bottom-up، وحالات الكتاب بأسمائها** (LL/RR/LR/RL وR0/R1/R-1/L0/L1/L-1، وحالات الـ red-black، وzig/zig-zig/zig-zag) عشان تحل بيها أي سؤال من الكتب.

:::goal هدف الدرس:
تنفّذ كل عمليات الـ BST بإيدك، وتعرف ليه الـ BST العادي ممكن يبقى chain، وتصلّح ميزان AVL بالـ rotations الأربعة، وتتعامل مع حالات الـ red-black، وتعمل splay خطوة بخطوة، وتبني threaded tree. كل الكود في `src/21-efficient-trees/` فيه assert لكل نتيجة، والرسومات اللي بتوصّف نتايج بتتأكد في بايثون من نفس الأرقام اللي بيطلّعها برنامج C.
:::

## 1) الـ BST: التعريف والعمليات (Thareja 10.1–10.2)

**Binary search tree**: شجرة ثنائية فيها لكل عقدة N: كل المفاتيح في شمالها **أصغر** من مفتاحها، وكل اللي في يمينها **أكبر** (أو يساوي، في طريقة الكتاب اللي بتحط المكرر يمين). النتيجة المباشرة: **in-order traversal بيطلّع المفاتيح مرتبة**. البحث بيمشي من الجذر شمال/يمين زي binary search، وتكلفته = عمق العقدة.

::code src="src/21-efficient-trees/bst_ops.c" region="basic"

### الحذف: ثلاث حالات

(1) **ورقة**: نشيلها. (2) **ابن واحد**: الابن بياخد مكان أبوه. (3) **ابنين**: نبدّل المفتاح بالـ **predecessor** (أكبر مفتاح في الشمال) أو الـ **successor** (أصغر مفتاح في اليمين) وبعدين نحذف العقدة اللي اتنقل منها (وهي بالضرورة حالة 1 أو 2، لأن أكبر عقدة في الشمال ملهاش ابن يمين). في الكود ده بنستعمل الـ predecessor عشان تتطابق نتايج تمارين الكتاب.

::figure ds21-bstdel

::code src="src/21-efficient-trees/bst_ops.c" region="delete"

::code src="src/21-efficient-trees/bst_ops.c" region="measure"

### كام BST ممكن من نفس المفاتيح؟

نفس n مفتاح ممكن تدّي أشجار مختلفة حسب ترتيب الإدخال. عدد الأشكال الممكنة (من غير ما نهتم بترتيب الإدخال) = كاتالان C(n). مع {7, 9, 11}: **5** أشجار (Thareja Q19)، والكود بيولّدها كلها:

::code src="src/21-efficient-trees/bst_ops.c" region="allbst"

الجذر 7: `7(.,9(.,11))` و`7(.,11(9,.))`. الجذر 9: `9(7,11)`. الجذر 11: `11(7(.,9),.)` و`11(9(7,.),.)`.

### أداء الـ BST: لازم نقيس، مش نخمّن

الأداء كله يتوقف على **الارتفاع**. لو المفاتيح اتضافت **مرتبة** الشجرة بتبقى chain والبحث O(n): متوسط العمق (n−1)/2. لو الترتيب **عشوائي** المتوسط حوالي **1.39 log₂ n** (Weiss 4.3.6: العمق الداخلي المتوسط O(log n) والمتوسط 2 ln n ≈ 1.39 log₂ n). الكود بيقيس ده بالفعل:

::code src="src/21-efficient-trees/bst_analysis.c" region="depth"

::output src="src/21-efficient-trees/bst_analysis.c" lines="10"

الصيغة الدقيقة للمتوسط (الجذر عمقه 0) هي 2(1+1/n)·Hₙ − 4 ≈ 11.03 لـ n = 1024، وده قريب من المقاس 11.09. و1.39·log₂ n = 13.9 هي القيمة التقريبية الكبيرة (بدون الحد الثابت −2.85). أما المدخلات المرتبة فبتدّي 511 = (n−1)/2 بالظبط.

### الحذف الكسول (lazy deletion) — Weiss 4.3.6

لو الحذف نادر نحط علامة «محذوف» على العقدة بدل ما نشيلها. فائدته: بساطة، وإعادة الإدخال لنفس المفتاح مجانية. عيبه: الشجرة بتكبر بعقد ميتة، فنعمل **purge** (إعادة بناء) لما عدد الميتين يعدّي نصف الشجرة مثلًا.

::code src="src/21-efficient-trees/bst_analysis.c" region="lazy"

## 2) الـ Threaded Binary Trees (Thareja 10.3، Lipschutz 7.6–7.7)

شجرة ثنائية بـ n عقدة فيها **n + 1 مؤشر NULL** (كل عقدة ليها مؤشرين: 2n مؤشر، منها n − 1 بتشاور على أبناء). بدل ما نسيبهم فاضيين نخلّيهم **threads** بتشاور على جيران الـ in-order: الـ NULL اليمين → الـ **successor** (one-way threading)، والـ NULL الشمال → الـ **predecessor** (two-way أو fully threaded). علامة (flag) لكل مؤشر بتقول «ابن» ولا «thread». الفايدة: traversal من غير stack ولا recursion.

::figure ds21-thread

::code src="src/21-efficient-trees/threaded.c" region="arrays"

::output src="src/21-efficient-trees/threaded.c" lines="50"

اللي فوق هو جدول Thareja Q16 بالظبط (الـ `T` بجوار الرقم = thread). لاحظ إن الـ pre-order threading بيشاور على جيران الـ **pre-order** (5 → 3 مثلًا) مش الـ in-order.

### النسخة بالمؤشرات مع Header node

Lipschutz 7.6: بنضيف عقدة **header** بتخلّي الـ thread الشمال للعقدة الأولى والـ thread اليمين للعقدة الأخيرة يشاوروا عليها، فمفيش NULL استثناء في أي مكان والـ traversal بيقف لما نرجع للـ header.

::code src="src/21-efficient-trees/threaded.c" region="pointer"

:::tip تكلفة الـ threads
الإدخال بقى أعقد (لازم نحدّث الـ threads بتاعة الجيران)، بس الـ traversal O(n) بـ O(1) مساحة: وده بالظبط نفس فكرة Morris في الدرس السابق، لكن هنا الـ links دايمة بدل المؤقتة.
:::

## 3) الـ AVL Trees (Thareja 10.4، Weiss 4.4، Lipschutz 7.11–7.14)

شجرة BST بحيث **فرق ارتفاع الشمال واليمين ≤ 1 عند كل عقدة**. **Balance factor** BF = ارتفاع الشمال − ارتفاع اليمين (تعريف Thareja، Weiss بيستعمله بالعكس أحيانًا) ∈ {−1, 0, +1}. الارتفاع O(log n): لو S(h) أقل عدد عقد لـ AVL بارتفاع h فـ **S(h) = S(h−1) + S(h−2) + 1** (شجرة بشمال من h−1 ويمين h−2، زي فيبوناتشي) فبيطلع S(h) = 1, 2, 4, 7, 12, 20, 33, 54 ... وبالتالي h ≤ 1.44 log₂(n+2) − 1.33. الكود بيتأكد من الأرقام دي بالحساب وبالبناء.

::figure ds21-rot

### الإدخال والـ rotations الأربعة

بنضيف عادي زي BST، وبعدين بنطلع لفوق في الطريق لحد أول عقدة (critical node) |BF| = 2. نوع الاختلال بيحدّد العلاج:

<table class="comp-table">
<tr><th>الحالة</th><th>متى</th><th>العلاج</th></tr>
<tr><td><b>LL</b></td><td>BF = +2 وابن الشمال BF ≥ 0 (إدخال شمال-شمال)</td><td>rotation يمين واحدة على الـ critical</td></tr>
<tr><td><b>RR</b></td><td>BF = −2 وابن اليمين BF ≤ 0</td><td>rotation شمال واحدة</td></tr>
<tr><td><b>LR</b></td><td>BF = +2 وابن الشمال BF = −1</td><td>شمال على الابن، ثم يمين على الـ critical</td></tr>
<tr><td><b>RL</b></td><td>BF = −2 وابن اليمين BF = +1</td><td>يمين على الابن، ثم شمال على الـ critical</td></tr>
</table>

::code src="src/21-efficient-trees/avl.c" region="avlcore"

::code src="src/21-efficient-trees/avl.c" region="avlinsert"

في الإدخال **rotation واحدة (أو double) بتكفي دايمًا** لإصلاح الشجرة كلها، لأن بعد الإصلاح الارتفاع بيرجع زي ما كان قبل الإدخال. مثال Thareja Q18: ابني AVL من `16, 27, 9, 11, 36, 54, 81, 63, 72`. الخطوات (الـ rotations الثلاثة: RR على 27 ثم RL على 54 ثم RR على 36):

::figure ds21-avl18

### الحذف: أسماء الكتاب R0 / R1 / R−1 / L0 / L1 / L−1

بنحذف كـ BST، وبعدين بنطلع لفوق. لو الحذف من **اليمين** للـ critical node A فالأخ هو B (ابن A الشمال): اسم الحالة R + BF(B). لو من **الشمال** فـ L + BF(B) (B ابن A اليمين). **R0 / L0**: rotation واحدة وارتفاع الشجرة الفرعية **مابيقلش** (فبنقف). **R1 / L1**: rotation واحدة وبعدها الارتفاع **بيقل** فبنكمّل نشوف الأجداد. **R−1 / L−1**: double rotation. على عكس الإدخال، الحذف ممكن يحتاج rotation على كل مستوى لحد الجذر (O(log n) rotation).

::code src="src/21-efficient-trees/avl.c" region="avldelete"

::output src="src/21-efficient-trees/avl.c" lines="12"

:::oops تفصيلة في تمرين Q13
لما نحذف 63 (له ابنين) وبنستعمل الـ predecessor فالمفتاح اللي بيتنقل هو 54 مش 63، فالـ rotation بتتسجّل على العقدة اللي اتغيّرت بعد الاستبدال. ده اللي بيفسّر ليه الـ log بيقول L0@54. لو استعملنا الـ successor ممكن تطلع شجرة نهائية مختلفة (وكلاها AVL صحيح)، فخد بالك من الطريقة اللي السؤال بيطلبها.
:::

## 4) الـ Red-Black Trees (Thareja 10.5، Weiss 12.2)

BST كل عقدة فيها لون (أحمر/أسود)، والقواعد الأربعة: (1) الجذر **أسود**. (2) كل الـ NIL leaves (الأطراف الخارجية) **سوداء**. (3) العقدة الحمراء **ملهاش ابن أحمر** (مفيش حمرتين ورا بعض). (4) كل المسارات من أي عقدة لأي NIL تحتها فيها **نفس عدد السواد** (black-height). النتيجة: أطول مسار ≤ 2× أقصر مسار، فالارتفاع ≤ **2 log₂(n+1)**. أبسط من AVL في الحذف والإدخال (ثابت عدد rotations: حد أقصى 2 للإدخال و3 للحذف) والتوازن أقل صرامة.

::code src="src/21-efficient-trees/rbtree.c" region="rbcore"

### الإدخال: العقدة الجديدة حمراء، والعلاج بيتحدد بالـ uncle

<table class="comp-table">
<tr><th>الحالة</th><th>الـ uncle</th><th>الفعل</th></tr>
<tr><td><b>I1</b></td><td>أحمر</td><td>لوّن الأب والـ uncle أسود والجد أحمر، وكمّل من الجد (ممكن المشكلة تطلع لفوق)</td></tr>
<tr><td><b>I2</b> (triangle)</td><td>أسود، والعقدة ابن «داخلي»</td><td>rotation على الأب فتتحول لـ I3</td></tr>
<tr><td><b>I3</b> (line)</td><td>أسود، والعقدة ابن «خارجي»</td><td>الأب أسود والجد أحمر، rotation على الجد، وبكده خلصنا</td></tr>
</table>

::code src="src/21-efficient-trees/rbtree.c" region="rbinsert"

::figure ds21-rb7

(الدواير السودا سُمك حلقتها زايد، الحمرا بلون أحمر وحافتها للأب أحمر.) إدخال 1..7 بالترتيب: الناتج `2b(1b,4r(3b,6b(5r,7r)))` وارتفاعه 4، في حين الـ BST العادي كان هيبقى chain بارتفاع 7. وبـ 1000 مفتاح مرتب الارتفاع الحقيقي 17 (الحد 20).

### الحذف: الـ «extra black»

حذف عقدة **حمراء** مفيهوش مشكلة. حذف **سودا** بيخلّي مكانها «سواد زايد» بيتحرك لفوق بـ 4 حالات حسب أخ العقدة w: **D1** الأخ أحمر (نلوّنه ونعمل rotation فتتحول لحالة تانية). **D2** الأخ أسود وابناه سود (نلوّن الأخ أحمر ونصعّد المشكلة للأب). **D3** الأخ أسود وابنه «القريب» أحمر (rotation تحوّلها لـ D4). **D4** الأخ أسود وابنه «البعيد» أحمر (rotation وخلصنا).

::code src="src/21-efficient-trees/rbtree.c" region="rbdelete"

::output src="src/21-efficient-trees/rbtree.c" lines="14"

:::tip مين أحسن: AVL ولا red-black؟
AVL أكتر توازنًا (ارتفاع ≤ 1.44 log n مقابل 2 log n) فالبحث أسرع شوية، بس بيحتاج حتى O(log n) rotations في الحذف. Red-black عدد rotations عنده ثابت تقريبًا فمناسب لأعباء الإدخال/الحذف الكتير (ولهذا بتستعمله المكتبات: std::map وjava.util.TreeMap).
:::

## 5) الـ Splay Trees (Thareja 10.6، Weiss 4.5 وتحليلها في 11.5)

شجرة BST **من غير أي معلومة توازن** (لا ارتفاع ولا لون): كل مرة بنوصل لعقدة (بحث/إدخال/حذف) بنرفعها للـ **جذر** بسلسلة خطوات splay. الفكرة: اللي اتستخدم قريب غالبًا هيتستخدم تاني. العملية الواحدة ممكن تبقى O(n)، لكن **amortised** O(log n) لكل عملية (تحليل Weiss 11.5 بالـ potential function Σ log size).

<table class="comp-table">
<tr><th>الخطوة</th><th>متى</th><th>الفعل</th></tr>
<tr><td><b>zig</b></td><td>الأب P هو الجذر</td><td>rotation واحدة على P</td></tr>
<tr><td><b>zig-zig</b></td><td>N وP الاتنين شمال (أو الاتنين يمين)</td><td>rotate الجد G <b>الأول</b> ثم P</td></tr>
<tr><td><b>zig-zag</b></td><td>N شمال وP يمين (أو العكس)</td><td>rotate P ثم G (زي AVL double)</td></tr>
</table>

::code src="src/21-efficient-trees/splay.c" region="splay"

::code src="src/21-efficient-trees/splay.c" region="splayops"

::figure ds21-splay

:::oops ليه الترتيب في zig-zig مهم؟
لو دورنا P الأول ثم N (بدل G ثم P) هنكون عملنا «rotation بسيطة مكررة» بترفع N من غير ما تقلل عمق الباقي، فمسار chain طويل مابيتقصّرش للنص، والـ amortised O(log n) بتضيع (ممكن ترجع O(n) لكل عملية). بعد search(1) على الـ chain المثال فوق، الارتفاع نزل من 7 لـ 5 وبعد search(2) لـ 4: كل splay بيقصّر المسار للنص تقريبًا.
:::

::output src="src/21-efficient-trees/splay.c" lines="14"

:::tip الـ splay مقابل الباقي
مناسب لما فيه locality (cache، تعرّض حديث). عيبه: كل **قراءة** بتعدّل الشجرة (مش آمنة للقراءة المتزامنة)، وممكن عملية واحدة تبقى بطيئة. ميزته: أبسط كود من AVL وred-black ومفيش مساحة زيادة.
:::

## 6) مقارنة سريعة

<table class="comp-table">
<tr><th></th><th>البحث (أسوأ حالة)</th><th>إدخال/حذف</th><th>مساحة زيادة</th><th>تستعمله في</th></tr>
<tr><td>BST عادي</td><td>O(n)</td><td>O(n)</td><td>—</td><td>بيانات عشوائية صغيرة</td></tr>
<tr><td>Threaded</td><td>O(n)</td><td>أعقد من BST</td><td>flag لكل مؤشر</td><td>traversal كتير من غير stack</td></tr>
<tr><td>AVL</td><td>O(log n) (≤ 1.44 log n)</td><td>O(log n)، حذف: rotation لكل مستوى</td><td>ارتفاع أو BF</td><td>بحث كتير، تعديل أقل</td></tr>
<tr><td>Red-black</td><td>O(log n) (≤ 2 log n)</td><td>O(log n)، rotations ثابتة</td><td>بت لون</td><td>مكتبات عامة (map/set)</td></tr>
<tr><td>Splay</td><td>O(n) لعملية، O(log n) amortised</td><td>amortised O(log n)</td><td>—</td><td>locality، caches</td></tr>
</table>

:::quiz
شجرة AVL بارتفاع 5 (عدد العقد في أقل شجرة AVL بالارتفاع ده)؟

1. [ ] 16
2. [x] 12
3. [ ] 20

> S(1)=1، S(2)=2، S(3)=4، S(4)=7، S(5)=7+4+1=12.
:::

:::quiz
أدخلنا 10، 30، 20 بالترتيب في AVL فاضي. أي حالة وإيه الجذر الناتج؟

1. [ ] RR والجذر 30
2. [x] RL والجذر 20
3. [ ] LR والجذر 20

> 30 يمين 10، 20 شمال 30: يمين ثم شمال = RL. rotation يمين على 30 ثم شمال على 10 فالجذر 20.
:::

:::quiz
عقدة جديدة حمراء أبوها أحمر وعمّها (uncle) أحمر. إيه الفعل؟

1. [ ] rotation على الجد
2. [x] تلوين الأب والعم أسود والجد أحمر وإكمال من الجد
3. [ ] نحذف الأب

> حالة I1: مفيش rotation أصلًا، وبس recolour. المشكلة ممكن تطلع للجد لو أبوه أحمر.
:::

:::quiz
في الـ zig-zig، بنعمل rotation على أنهي عقدة الأول؟

1. [ ] الأب P
2. [x] الجد G
3. [ ] العقدة N

> G الأول ثم P. العكس بيخلّي الـ amortised bound تضيع.
:::

:::quiz
threaded tree بـ 12 عقدة: كام مؤشر NULL كان فيها قبل ما نعمل threading؟

1. [ ] 12
2. [x] 13
3. [ ] 11

> n + 1 = 13: عدد المؤشرات 2n = 24 والأبناء n − 1 = 11 فالباقي 13.
:::
