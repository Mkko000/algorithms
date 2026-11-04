المراجع: Thareja ف7 (7.1–7.5: stack، infix/postfix/prefix، recursion، Hanoi) وف8 (8.1–8.5: queue، circular، deque، priority، multiple queues) · Lipschutz ف6 (6.1–6.15: stacks، arithmetic expressions، quicksort بالـ stack، recursion، Ackermann، Fibonacci، Hanoi، queues، deques، priority queues) · Weiss ف3 (3.3 Stack ADT: balancing symbols، postfix، infix→postfix، function calls؛ 3.4 Queue ADT) · Bandyopadhyay &amp; Dey ف6–7 · Pal &amp; Halder ف6–7

الـ stack والـ queue الأساسيين اتشرحوا قبل كده في [درس 08](lesson:adt) و[درس 09](lesson:queues-adt) (الـ ADT والتنفيذ بالـ array والـ list). والريكيرشن في [درس 11](lesson:recursion). الدرس ده بيغطّي **اللي كل الكتب التانية بتقوله وماكانش موجود**: تحويل التعبيرات (infix / postfix / prefix) وتقييمها، التطبيقات الكلاسيكية للـ stack، محاكاة الـ call stack بإيدك، الـ queue الدايرية بتلات طرق، الـ deque بنوعيه، والـ priority queue بطريقتين.

:::goal هدف الدرس:
تقدر تحوّل أي تعبير بين الأشكال التلاتة بالخوارزمية (مش بالحفظ)، تبني stack من queue وعكسه، تفهم ليه الريكيرشن = stack، وتفرّق بين الفاضية والممتلئة في queue دايرية. كل الكود في `src/19-stacks-queues/` متجرَّب بـ AddressSanitizer، وبعضه متقارن بـ 5000 تعبير عشوائي أو 20000 عملية عشوائية ضد نموذج مرجعي.
:::

## 1) التعبيرات الحسابية: infix وpostfix وprefix

الكتابة اللي إحنا متعودين عليها **infix**: العملية بين المعاملين `A + B`، وعشان نفهمها محتاجين أولويات وأقواس. **Postfix** (Reverse Polish، RPN): العملية بعد المعاملين `A B +`. **Prefix** (Polish): قبلهم `+ A B`. السر: في الشكلين دول **مفيش أقواس ومفيش أولويات**، الترتيب وحده بيحدد المعنى. وده ليه تطبيق كبير: المترجمات (compilers) بتحوّل infix لـ postfix، والحاسبات اللي بالـ stack (HP RPN) بتقيّم postfix مباشرة.

<table class="comp-table">
<tr><th>infix</th><th>postfix</th><th>prefix</th></tr>
<tr><td><code>A + B * C</code></td><td><code>A B C * +</code></td><td><code>+ A * B C</code></td></tr>
<tr><td><code>(A + B) * C</code></td><td><code>A B + C *</code></td><td><code>* + A B C</code></td></tr>
<tr><td><code>A - B - C</code></td><td><code>A B - C -</code></td><td><code>- - A B C</code></td></tr>
<tr><td><code>A ^ B ^ C</code></td><td><code>A B C ^ ^</code></td><td><code>^ A ^ B C</code></td></tr>
</table>

:::tip الأولوية والـ associativity
الترتيب: `^` (أعلى، وبتتجمّع من اليمين: `2^3^2 = 2^9`) ثم `* / %` ثم `+ -` (الاتنين من الشمال: `8-3-2 = 3` مش 7). تجاهل الـ associativity هو أشهر غلطة في حلول التمارين.
:::

## 2) infix → postfix بالـ stack (shunting-yard)

الفكرة: المعاملات (`A`، `10`) بتروح للـ output على طول. العمليات بتستنى على stack لحد ما يجي عملية **أضعف** (أو مساوية لو شمالية) تقولها «معاملاتك اكتملت». القوس المفتوح بيتحط، وعند القوس المقفول بنفرّغ لحد المفتوح. وفي الآخر نفرّغ الباقي.

::figure ds-shunting

::code src="src/19-stacks-queues/expr_convert.c" region="tok"

::code src="src/19-stacks-queues/expr_convert.c" region="postfix"

:::oops الشرط اللي بيتغلط فيه
الـ `while` بتفرّغ لو `prec(top) > prec(c)` **أو** المساواة مع `c` شمالية. لو كتبت `>=` بس، الـ `^` بتبقى شمالية غلط: `A^B^C` تطلع `A B ^ C ^` بدل `A B C ^ ^`. الاختبار في البرنامج بيمسك ده.
:::

## 3) infix → prefix: بطريقتين

**الطريقة 1 (عكس + shunting + عكس):** اعكس التعبير وبدّل الأقواس، طبّق نفس الخوارزمية بس بـ **عكس شرط الـ associativity** (لأن بعد العكس العمليات الشمالية بقت بتتجمّع من الناحية التانية)، وبعدين اعكس الناتج. **الطريقة 2:** حوّل لـ postfix وبعدين من postfix لـ prefix بـ stack من **النصوص**: لكل عملية خد آخر نصّين x وy، وادفع `op x y`. نفس الطريقة بتديك infix مقوّس كامل: `(x op y)`. وتحويل prefix → infix بنفس الـ stack بس بنقرأ من اليمين لليسار.

::code src="src/19-stacks-queues/expr_convert.c" region="prefix"

دالة `convert` الواحدة بتغطّي التلات تحويلات (mode 0: postfix→prefix، 1: postfix→infix، 2: prefix→infix). هي بتبني نصوص على الـ heap بـ `catv` عشان ما نحطش حجم ثابت يقطع التعبير.

## 4) تقييم postfix وprefix

قراءة من الشمال: معامل → push. عملية → pop اتنين (**الأول اللي بيطلع هو المعامل اليمين**)، طبّق، push النتيجة. في الآخر لازم يفضل عنصر واحد بالظبط (غير كده التعبير غلط). ممكن نقيّم prefix بنفس الطريقة لو قرينا من اليمين (وساعتها أول pop هو اليسار).

::code src="src/19-stacks-queues/expr_convert.c" region="eval"

::output src="src/19-stacks-queues/expr_convert.c" lines="14"

البرنامج مش بس بيطبع أمثلة: بيولّد **5000 تعبير عشوائي**، ويحوّله لـ postfix وprefix، ويقيّمهم، ويقارن بمحلّل recursive-descent مستقل (oracle) يحسب القيمة من الـ infix مباشرة. اتطابقوا كلهم (4880 قيمة محدودة قورنت؛ الباقي قسمة على صفر).

## 5) الأقواس المتوازنة (balancing symbols)

أبسط تطبيق للـ stack وبيظهر في كل compiler. كل فاتح بيدفع مقفوله المتوقع، وكل مقفول لازم يطابق القمة. الأخطاء التلاتة: مقفول زيادة (stack فاضي)، مقفول غلط (القمة مش هو)، وفاتح ماتقفلش (stack مش فاضي في الآخر). الدالة بترجع **مكان أول غلطة** مش بس صح/غلط.

::code src="src/19-stacks-queues/stack_apps.c" region="brackets"

## 6) أكتر من stack في array واحدة

لو عندك stackين وكل واحد محجوز له نص المصفوفة، واحد منهم ممكن يفيض والتاني فاضي نص مساحته. الحل: واحد يبدأ من الأول (topA) والتاني من الآخر (topB) ويكبروا لبعض. الـ overflow بس لما `topA + 1 == topB`: يعني المصفوفة كلها اتملت فعلًا. (للـ queues الحل بيبقى بالـ circular أو بتقسيم ديناميكي لأن الـ queue بتتحرك للأمام.)

::figure ds-two-stacks

::code src="src/19-stacks-queues/stack_apps.c" region="two"

## 7) stack بـ getMin في O(1)

مع كل push بنخزّن كمان «أصغر قيمة لحد دلوقتي» في stack تاني. الـ pop بيشيل من الاتنين، فالـ min دايمًا فوق التاني. نفس الفكرة بتنفع لأي دالة تجميعية (max، gcd، sum).

::code src="src/19-stacks-queues/stack_apps.c" region="minstack"

## 8) queue من stackين، وstack من queueين

**queue من stackين:** enqueue بيدفع في IN. dequeue بيعمل pop من OUT، ولو OUT فاضي بينقل **كل** IN لـ OUT (فبيتعكس الترتيب). كل عنصر بينتقل مرة واحدة بس في عمره، فـ n عملية تكلّف O(n): **amortised O(1)** للعملية (رغم إن dequeue الواحدة ممكن تكلّف O(n)). البرنامج بيعدّ النقلات ويتأكد إنها ≤ عدد العمليات.

::figure ds-q2s

**stack من queueين:** push بيدخل العنصر الجديد في queue فاضية، ويدخل وراه كل القديم، وبعدين نبدّل الدورين. فالجديد بقى في المقدمة: push بـ O(n) وpop بـ O(1). (أو العكس.)

::code src="src/19-stacks-queues/stack_apps.c" region="queue2s"

::output src="src/19-stacks-queues/stack_apps.c" lines="8"

## 9) المونوتونك stack: next greater وstock span

سؤال متكرر: لكل عنصر، أول عنصر أكبر منه على يمينه. الحل الساذج O(n²). بـ stack فيه **أرقام الخانات اللي لسه ما عرفناش إجابتها** (قيمها متناقصة): لما يجي عنصر أكبر منهم، هو الإجابة بتاعتهم كلهم فنشيلهم. كل خانة تتدفع مرة وتتشال مرة: O(n). نفس الفكرة بتحل stock span (كام يوم متتالي السعر ≤ النهاردة).

::code src="src/19-stacks-queues/stack_apps.c" region="mono"

ولعمليات الـ stack بس (copy، compare، reverse) بدون أي هيكل تاني:

::code src="src/19-stacks-queues/stack_apps.c" region="stackops"

## 10) الريكيرشن = stack: نحاكي الـ call stack بإيدينا

كل نداء recursive بيعمل **frame** (الـ arguments والمتغيرات المحلية و«أكمّل من فين») على الـ call stack، وكل return بيشيله. عشان كده الريكيرشن العميق جدًا بيعمل *stack overflow*. أي دالة recursive ممكن نحوّلها لحلقة + stack صريح. الأمثلة هنا: factorial (الأبسط: stack من «الضربات المستنية»)، Fibonacci (frame بيه حالة 0/1/2 ومسجّل `ret`)، Hanoi (stack من «مهام»، بنحط المهام بعكس ترتيب التنفيذ)، وAckermann (stack من الـ m المستنية).

::code src="src/19-stacks-queues/recursion_stack.c" region="fact"

**Tail recursion:** لو النداء الـ recursive هو *آخر* حاجة بتتعمل (مفيش ضرب مستني)، فمش محتاجين نفتكر frame القديم: compiler بيحوّلها لحلقة. `fact_tail` بتحمل المجموع التراكمي في `acc`، وgcd بتتحوّل مباشرة. لاحظ إن `fact_rec` مش tail لأن `n *` مستنية بعد الرجوع.

::figure ds-fibtree

::code src="src/19-stacks-queues/recursion_stack.c" region="fib"

### أبراج هانوي

انقل n قرص من A لـ C بقرص واحد كل مرة وبدون كبير فوق صغير: انقل n−1 لـ B، انقل الأكبر لـ C، انقل n−1 من B لـ C. عدد الحركات `T(n) = 2T(n−1) + 1 = 2ⁿ − 1`، ومفيش حل أسرع (إثبات بالاستقراء: الأكبر لازم يتحرك مرة على الأقل، وقبلها n−1 لازم يكونوا على القضيب الوسيط).

::figure ds-hanoi

::code src="src/19-stacks-queues/recursion_stack.c" region="hanoi"

### دالة Ackermann

بتنمو أسرع من أي دالة primitive recursive: A(2,3) = 9 وA(3,3) = 61 وA(4,2) رقم بـ 19729 خانة. مثال كلاسيكي على recursion متداخل (النداء جوه argument نداء).

::code src="src/19-stacks-queues/recursion_stack.c" region="ackermann"

::output src="src/19-stacks-queues/recursion_stack.c" lines="8"

## 11) الـ queue الدايرية: فاضية ولا ممتلئة؟

في queue بـ array عادية، بعد ما REAR يوصل آخر المصفوفة بنقول OVERFLOW حتى لو الخانات اللي قبل FRONT فاضية (بعد dequeues). الحل: الدوران `(i+1) % N`. المشكلة الجديدة: لما FRONT == REAR+1 الـ queue ممكن تكون فاضية **أو** ممتلئة. تلات حلول (كلها في الكود ومتقارنة بنماذج مرجعية على 20000 خطوة عشوائية):

::figure ds-ring

<table class="comp-table">
<tr><th>الحل</th><th>الفاضية</th><th>الممتلئة</th><th>السعة</th></tr>
<tr><td>(a) حقل count</td><td><code>n == 0</code></td><td><code>n == N</code></td><td>N</td></tr>
<tr><td>(b) خانة فاضية دايمًا</td><td><code>front == rear</code></td><td><code>(rear+1)%N == front</code></td><td>N−1</td></tr>
<tr><td>(c) الكتاب (Thareja): −1</td><td><code>front == -1</code></td><td><code>front==0 &amp;&amp; rear==N-1</code> أو <code>front == rear+1</code></td><td>N</td></tr>
</table>

::code src="src/19-stacks-queues/deque_pq.c" region="circ"

ملحوظة على أسئلة Thareja: مفتاح الإجابات في آخر الكتاب بيقول إن الـ circular queue بتبقى ممتلئة لما `FRONT = 0 and REAR = MAX−1` (اختيار (b))، وده فعلًا حالة من الحالتين في الجدول (c). والمفتاح بيقول كمان إن «front = MAX−1 وrear = 0 تعني ممتلئة» صح؛ ده صحيح حرفيًا بس لما MAX = 2، والقاعدة العامة هي `front == (rear+1) % MAX`. التفصيل في صفحة التمارين.

## 12) الـ Deque (double-ended queue)

إدخال وحذف من الطرفين. بـ circular array: `push_left` بيرجّع `left` خطوة لورا (بالـ mod) ويخزّن، `push_right` بيخزّن عند `(left+n)%N`. نوعين مقيّدين: **input-restricted** (إدخال من طرف واحد، حذف من الاتنين) و**output-restricted** (حذف من طرف واحد، إدخال من الاتنين). همّا نفس الهيكل مع منع عمليتين. الـ stack = deque بتستخدم طرف واحد، والـ queue = deque (إدخال يمين، حذف شمال). تطبيقات: undo/redo، work-stealing، sliding window maximum.

::code src="src/19-stacks-queues/deque_pq.c" region="deque"

## 13) الـ Priority Queue بطريقتين

كل عنصر ليه أولوية، والأعلى أولوية يتخدم الأول، ولو اتساووا فالـ FCFS. **(1) قايمة مرتبة:** الإدخال O(n) (نمشي لحد أول عقدة أولويتها أقل *بشدة*، فالجديد يدخل بعد المتساوين)، والحذف O(1) (الأول). **(2) مصفوفة طوابير:** صف لكل مستوى أولوية، الإدخال O(1)، والحذف بيدوّر على أول صف مش فاضي O(P) (P عدد المستويات). الاتنين «غير مثاليين»؛ الـ [heap](lesson:pq-advanced) بيدّي O(log n) للاتنين.

::code src="src/19-stacks-queues/deque_pq.c" region="pqlist"

::figure ds-pqarr

::code src="src/19-stacks-queues/deque_pq.c" region="pqarray"

::output src="src/19-stacks-queues/deque_pq.c" lines="8"

:::quiz
ليه `A ^ B ^ C` بتتحوّل لـ `A B C ^ ^` مش `A B ^ C ^`؟

1. [ ] عشان ^ ليها أولوية أقل
2. [x] لأن ^ بتتجمّع من اليمين: A^(B^C)، فالـ ^ الأولى لازم تستنى على الـ stack لحد ما التانية تجي
3. [ ] عشان مفيش أقواس

> لما تيجي ^ ويكون على القمة ^ كمان، مانفرّغش القمة (مساوية في الأولوية لكنها يمينية).
:::

:::quiz
في تقييم postfix، لما نقابل `-` ونعمل pop مرتين، الـ pop الأول هو…؟

1. [ ] المعامل الشمال
2. [x] المعامل اليمين
3. [ ] مفيش فرق

> المعامل الشمال اتدفع الأول فهو تحت. `7 5 -` = 7 − 5 = 2 مش −2.
:::

:::quiz
dequeue واحدة في queue من stackين ممكن تكلّف O(n)، فليه نقول amortised O(1)؟

1. [ ] لأن n صغيرة
2. [x] لأن كل عنصر بينتقل من IN لـ OUT مرة واحدة بس في حياته، فمجموع النقلات ≤ عدد الـ enqueues
3. [ ] لأن الـ stacks سريعة

> التكلفة الغالية بتتوزّع على العمليات اللي دفعت العناصر.
:::

:::quiz
ليه queue دايرية (b) سعتها N−1؟

1. [ ] ده bug
2. [x] لأن لو اتملت كل الخانات هيبقى front == rear وده نفس شكل الفاضية، فبنسيب خانة فاضية للتفرقة
3. [ ] لأن الـ % بتاكل خانة

> الحل (a) بحقل count والحل (c) بقيمة −1 بيستعملوا كل الـ N خانة.
:::

:::quiz
عمق الـ call stack في `fib(25)` الـ recursive؟

1. [ ] 242785
2. [x] حوالي 25، أما عدد النداءات الكلي فهو 242785
3. [ ] 2²⁵

> الـ stack بيتمدد على مسار واحد من الجذر للورقة؛ الشجرة بتتمشى depth-first.
:::
