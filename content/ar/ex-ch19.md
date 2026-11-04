تمارين محلولة · الفصل 19 (Stacks وQueues: التطبيقات) · Thareja ف7: Review Questions 1–16 + Programming Exercises 1–10 · ف8: Review Questions 1–6 + Programming Exercises 1–12 · أسئلة الاختيار وصح/غلط والفراغات للفصلين (بمفتاح الكتاب Appendix G)

الحلول في ملفين `exercises/ex19_01_stacks.c` و`exercises/ex19_02_queues.c`. الملفين بيعملوا `#include` لبرامج الدرس (`expr_convert.c` وstack\_apps.c وdeque\_pq.c) بدل ما ينسخوا الكود، وكل نتيجة **assert** مش بس طباعة.

:::goal هدف الصفحة:
تحل أي تمرين stack أو queue بنفس الطريقة: ارسم الحالة بعد كل عملية، حدّد حالات الحافة (فاضي، ممتلئ، لفّة الدايرة)، واختبر.
:::

* [src/19-stacks-queues/exercises/ex19\_01\_stacks.c](src:src/19-stacks-queues/exercises/ex19_01_stacks.c)
* [src/19-stacks-queues/exercises/ex19\_02\_queues.c](src:src/19-stacks-queues/exercises/ex19_02_queues.c)
* [src/19-stacks-queues/expr\_convert.c](src:src/19-stacks-queues/expr_convert.c)
* [src/19-stacks-queues/stack\_apps.c](src:src/19-stacks-queues/stack_apps.c)
* [src/19-stacks-queues/recursion\_stack.c](src:src/19-stacks-queues/recursion_stack.c)
* [src/19-stacks-queues/deque\_pq.c](src:src/19-stacks-queues/deque_pq.c)

### خريطة التمارين

<table class="comp-table">
<tr><th>تمرين</th><th>الموضوع</th><th>الحل في</th></tr>
<tr><td>ف7 RQ 1–7، 15، 16</td><td>نظري</td><td>الصفحة دي (تحت)</td></tr>
<tr><td>ف7 RQ 8، 9</td><td>infix → postfix / prefix (7 تعبيرات)</td><td>ex19_01: <code>rq8_rq9</code></td></tr>
<tr><td>ف7 RQ 10، 11</td><td>postfix / prefix → infix</td><td>ex19_01: <code>rq10_rq11</code></td></tr>
<tr><td>ف7 RQ 12</td><td>حوّل وقيّم + برنامج التقييم</td><td>ex19_01: <code>rq12</code>, expr_convert: <code>eval_postfix</code></td></tr>
<tr><td>ف7 RQ 13، PE 5</td><td>نسخ stack بالترتيب / مقارنة stackين</td><td>stack_apps: <code>is_copy</code>, <code>is_equal</code></td></tr>
<tr><td>ف7 RQ 14</td><td>رسم الـ stack بعد كل عملية</td><td>ex19_01: <code>rq14</code></td></tr>
<tr><td>ف7 PE 1</td><td>stack بـ linked list</td><td>ex19_01: <code>ls_push</code>, <code>ls_pop</code></td></tr>
<tr><td>ف7 PE 2، 3</td><td>a+b → ab+ و+ab</td><td>ex19_01: <code>pe2_pe3</code></td></tr>
<tr><td>ف7 PE 4</td><td>stack من أسماء الطلبة</td><td>ex19_01: <code>name_stack</code></td></tr>
<tr><td>ف7 PE 6–9</td><td>F(x,y) وF(n,r) وLambda(n) وF(M,N)</td><td>ex19_01: <code>F6</code> <code>F7</code> <code>Lambda</code> <code>F9</code></td></tr>
<tr><td>ف7 PE 10</td><td>عكس string بالـ recursion</td><td>ex19_01: <code>rev</code></td></tr>
<tr><td>ف8 RQ 1–3</td><td>نظري</td><td>الصفحة دي (تحت)</td></tr>
<tr><td>ف8 RQ 4، 5</td><td>رسم الـ queue بعد العمليات (خطية + دايرية)</td><td>ex19_02: <code>rq4</code>, <code>rq5</code></td></tr>
<tr><td>ف8 RQ 6</td><td>رسم الـ deque (7 عمليات)</td><td>ex19_02: <code>rq6</code></td></tr>
<tr><td>ف8 PE 1</td><td>عدد العناصر في queue</td><td>ex19_02: <code>lq_count</code>, <code>circ_count</code></td></tr>
<tr><td>ف8 PE 2</td><td>linear queue من 10 قيم</td><td>ex19_02: main</td></tr>
<tr><td>ف8 PE 3، 6، 7</td><td>queue بإدخال من الطرفين / حذف من الطرفين / الاتنين</td><td>deque_pq: <code>dq_*</code> + ex19_02 main</td></tr>
<tr><td>ف8 PE 4</td><td>deque بـ linked list</td><td>ex19_02: <code>ldeque</code></td></tr>
<tr><td>ف8 PE 5</td><td>إدخال في أي خانة فاضية عند الـ rear</td><td>deque_pq: <code>cc_put</code> + ex19_02 main</td></tr>
<tr><td>ف8 PE 8</td><td>priority queue</td><td>deque_pq: <code>pq_insert</code>, <code>pq_delete</code></td></tr>
<tr><td>ف8 PE 9، 10</td><td>queue من stack / stack من queue</td><td>stack_apps: <code>queue2s</code>, <code>stack2q</code></td></tr>
<tr><td>ف8 PE 11</td><td>عكس queue</td><td>ex19_02: <code>queue_reverse</code></td></tr>
<tr><td>ف8 PE 12</td><td>مقارنة queueين</td><td>ex19_02: <code>queue_equal</code></td></tr>
</table>

### ف7: أسئلة المراجعة النظرية 1–7، 15، 16

:::solution
**1)** *Overflow*: push على stack ممتلئ (`TOP == MAX−1`). *Underflow*: pop (أو peek) من stack فاضي (`TOP == −1`). في الـ linked stack مفيش overflow غير لو malloc فشل.

**2)** الـ array: وصول لأي عنصر بالـ index وإدخال/حذف في أي مكان. الـ stack: واجهة مقيّدة (LIFO) تتعامل بس مع القمة؛ ممكن تتنفّذ فوق array أو list.

**3)** Array stack: حجم ثابت (overflow)، ذاكرة متصلة، من غير pointers. Linked stack: حجم ديناميكي، كل عقدة فيها pointer زيادة، وpush/pop = إدخال/حذف عند head (O(1)).

**4)** `peek` بترجّع القمة **من غير ما تشيلها**، و`pop` بترجّعها وتشيلها.

**5)** لأن ترتيب المعاملات والعمليات في postfix/prefix بيحدد ترتيب التنفيذ بالكامل: كل عملية بتشتغل على آخر معاملين جاهزين (postfix) أو التاليين ليها (prefix)، فمفيش لبس يحتاج أقواس أو أولويات.

**6)** أي algorithm recursive ممكن نكتبه من غير recursion بـ stack صريح بنخزّن فيه اللي الـ call stack كان هيخزّنه (arguments، حالة، عنوان الاستكمال). أمثلة محلولة بالكود: factorial وfib وHanoi وAckermann في `recursion_stack.c`، والـ traversals غير الـ recursive للأشجار في الدرس الجاي.

**7)** Multiple stack = أكتر من stack في نفس الـ array (مثلًا واحد من كل طرف). مفيد لأنه بيستغل المساحة كلها: الـ overflow بس لما المصفوفة كلها تتملي، مش لما نصّها يتملي (راجع `two_stacks`).

**15)** *iterative* = حلقة (O(1) مساحة stack، أسرع عادةً). *recursive* = دالة بتنادي نفسها (كود أقصر وأقرب للتعريف، بس بتكلّف frame لكل نداء وممكن stack overflow). نفضّل الـ recursive لو المشكلة تعريفها recursive طبيعيًا والعمق صغير (أشجار، divide and conquer، backtracking)، والـ iterative لو فيه حلقة واضحة (factorial، gcd، fib)، وخصوصًا لو العمق ممكن يوصل آلاف. مثال: fib recursive أُسّي مقابل O(n) بحلقة.

**16)** أبراج هانوي: 3 قضبان وn أقراص بأحجام مختلفة على الأول؛ انقلهم للتالت بقرص واحد في المرة، من غير كبير فوق صغير. الحل recursive: انقل n−1 لـ B، الأكبر لـ C، n−1 من B لـ C. الأدنى 2ⁿ − 1 حركة. (الشرح والصورة والكود في الدرس، والبرنامج بيتأكد إن الـ explicit-stack والـ recursive بيطلّعوا نفس الحركات لكل n حتى 10.)
:::

### ف7 RQ 8 و9: infix → postfix و prefix

نص السؤالين في النسخة اللي عندي بيقولوا الاتنين «postfix»، فحليت **الشكلين لكل تعبير** وأي واحد المقصود تلاقيه.

::code src="src/19-stacks-queues/exercises/ex19_01_stacks.c" region="RQ8"

:::think
في (f) و(g) القسمة والضرب نفس الأولوية وشماليين: `2*(B+C)/D*E` = `((2*(B+C))/D)*E` فالـ postfix `2 B C + * D / E *`. وفي (g): `14/7*3` = `(14/7)*3` = 6، فالـ postfix `14 7 / 3 * ...`. وقيمة (g) = 6 − 4 + 4.5 = 6.5.
:::

### ف7 RQ 10 و11: postfix/prefix → infix

::code src="src/19-stacks-queues/exercises/ex19_01_stacks.c" region="RQ10"

:::solution
(10-a) `(A + B) * C − D` · (10-b) `A + B * C − D` · (11-a) `(A + B − C) * D` · (11-b) `a − B * C + D`. البرنامج بيطبعها مقوّسة كاملًا لأن ده ما بيحتاجش فهم أولويات.
:::

### ف7 RQ 12: حوّل وقيّم 10 + ((7 − 5) + 10) / 2

::code src="src/19-stacks-queues/exercises/ex19_01_stacks.c" region="RQ12"

:::solution
postfix: `10 7 5 − 10 + 2 / +` والقيمة **16** (7−5=2، 2+10=12، 12/2=6، 10+6=16). برنامج التقييم المطلوب هو `eval_postfix` في الدرس (وأعلاه نسخة بتطبع الـ stack بعد كل توكن).
:::

### ف7 RQ 13 و PE 5: نسخ ومقارنة stacks

::code src="src/19-stacks-queues/stack_apps.c" region="stackops"

:::think
الـ pop بيعكس. لو نسخنا stack لـ temp بالـ pop هنحصل على **معكوس**؛ فنحتاج مرور تاني من temp لـ (الأصل + النسخة) فيرجع الترتيب. نفس الفكرة في المقارنة: نشيل الاتنين مع بعض ونقارن، ونحفظ اللي شلناه عشان نرجّعه.
:::

### ف7 RQ 14: الـ stack بعد كل عملية

::code src="src/19-stacks-queues/exercises/ex19_01_stacks.c" region="RQ14"

### ف7 PE 1: stack بـ linked list، PE 2 و3 و4

::code src="src/19-stacks-queues/exercises/ex19_01_stacks.c" region="PE1"

::code src="src/19-stacks-queues/exercises/ex19_01_stacks.c" region="PE2"

::code src="src/19-stacks-queues/exercises/ex19_01_stacks.c" region="PE4"

### ف7 PE 6–10: التعريفات الـ recursive

::code src="src/19-stacks-queues/exercises/ex19_01_stacks.c" region="PE6"

:::solution
**PE 6** F(x,y) = ⌊x / y⌋ (قسمة صحيحة بالطرح المتكرر). y = 0 لانهائية، فبنحرسها بـ `assert`.

**PE 7** نص السؤال ما حددش الحالات الأساسية. اخترت F(n,0) = F(n,n) = 1، وساعتها F(n,r) = C(n,r) (قاعدة باسكال). البرنامج بيقارنها بالصيغة المباشرة لـ n ≤ 12 (مثلًا F(10,5) = 252).

**PE 8** Lambda(n) = ⌊log₂ n⌋ (عدد مرات القسمة على 2 لحد ما نوصل 1). اتأكد لـ n = 1…1000.

**PE 9** F(M,N): الحالة الأساسية «M = 0 أو M ≥ N ≥ 1» = 1. لما N ≤ 0 و M \> 0 مفيش حالة أساسية بتتحقق غير M = 0، فكل الـ 2<sup>M</sup> فرع بينتهي بـ 1: **F(M,0) = 2<sup>M</sup>**. ولما N \> M ≥ 1 القيم زي باسكال (F(2,3) = 4، F(3,4) = 8). الجدول المطبوع (M صفوف 0–6، N أعمدة 0–6) بيوضح الشكل: مثلث الوحايد، وأعمدة 2<sup>M</sup> على الأطراف. (الدالة بتنتهي دايمًا لأن M بتقلّ في كل نداء.)

**PE 10** عكس string بالـ recursion: بدّل الطرفين وانزل للوسط. حالة الـ string الفاضية والحرف الواحد بتقف على `lo >= hi`.
:::

**ناتج تشغيل ex19\_01\_stacks كامل** (كل سطر اتأكّد بـ assert قبل ما يتطبع):

::output src="src/19-stacks-queues/exercises/ex19_01_stacks.c" lines="100"

### ف7: اختيار / صح-غلط / فراغات (بمفتاح الكتاب)

<table class="comp-table">
<tr><th>#</th><th>إجابة الكتاب</th><th>ليه</th></tr>
<tr><td colspan="3"><b>اختيار من متعدد</b></td></tr>
<tr><td>1</td><td>(a) LIFO</td><td>آخر داخل أول خارج.</td></tr>
<tr><td>2</td><td>(b) push()</td><td>pop بتشيل، peek بتقرأ، isEmpty بتسأل.</td></tr>
<tr><td>3</td><td>(a) Stack</td><td>أقراص فوق بعض.</td></tr>
<tr><td>4</td><td>(c) Postfix</td><td>Reverse Polish = postfix.</td></tr>
<tr><td colspan="3"><b>صح/غلط</b></td></tr>
<tr><td>1</td><td>False</td><td>pop بتشيل؛ push هي اللي بتضيف.</td></tr>
<tr><td>2</td><td>True</td><td>postfix مفيهوش أولويات: ترتيب القراءة بس.</td></tr>
<tr><td>3</td><td>True</td><td>divide-and-conquer.</td></tr>
<tr><td>4</td><td>True</td><td>frames + نداءات (fib(25) = 242785 نداء).</td></tr>
<tr><td>5</td><td>False</td><td>recursion بتنزل top-down (للمشكلة الأصغر)، مش bottom-up.</td></tr>
<tr><td>6</td><td>True</td><td>indirect recursion: f تنادي g وg تنادي f.</td></tr>
<tr><td>7</td><td>False</td><td>peek بتعرض ومبتحذفش.</td></tr>
<tr><td>8</td><td>True</td><td>LIFO.</td></tr>
<tr><td>9</td><td>False</td><td>TOP = MAX−1 هو overflow، والـ underflow عند TOP = −1.</td></tr>
<tr><td>10</td><td>True</td><td>O(n) مساحة، O(1) للعمليات.</td></tr>
<tr><td>11</td><td>False</td><td>push على linked stack = إدخال عند head = O(1).</td></tr>
<tr><td>12</td><td>False</td><td>الـ multiple stacks بتقلّل الـ overflow بس مبتمنعوش لما المصفوفة كلها تتملي.</td></tr>
<tr><td colspan="3"><b>الفراغات</b></td></tr>
<tr><td>1</td><td>stack</td><td>تحويل infix → postfix.</td></tr>
<tr><td>2</td><td>stack</td><td>تنفيذ recursive بشكل non-recursive.</td></tr>
<tr><td>3</td><td>O(n)</td><td>مساحة linked stack.</td></tr>
<tr><td>4</td><td>بنحاول نحذف من stack فاضي</td><td>underflow.</td></tr>
<tr><td>5</td><td>اليسار لليمين</td><td>تقييم postfix.</td></tr>
<tr><td>6</td><td>Non-tail</td><td>لما فيه عملية مستنية بعد النداء.</td></tr>
<tr><td>7</td><td>Directly</td><td>بتنادي نفسها صراحة.</td></tr>
</table>

### ف8: أسئلة المراجعة النظرية 1–3

:::solution
**1)** الـ priority queue: كل عنصر ليه أولوية والأعلى يتخدم الأول (والمتساويين FCFS). استخدامات: جدولة العمليات في الـ OS، Dijkstra وPrim، ضغط Huffman، محاكاة الأحداث، الـ routers (QoS).

**2)** الـ circular queue بتعتبر آخر خانة متصلة بأولها (`(i+1) % N`). أحسن من الخطية لأنها **بتستعمل الخانات اللي اتفضّت** بعد الـ dequeues؛ الخطية بتقول OVERFLOW لما REAR يوصل النهاية حتى لو نص المصفوفة فاضي (مثال محلول في RQ 5).

**3)** Multiple queues: أكتر من queue في نفس الـ array لنفس سبب الـ multiple stacks: تجنّب الـ overflow المتكرر وتجنّب حجز مساحة كبيرة لكل queue. (للـ queues مش كفاية «طرفين»، فبنستخدم circular أو نعيد التوزيع.)
:::

### ف8 RQ 4: queue فاضية والعمليات (a)–(f)

::code src="src/19-stacks-queues/exercises/ex19_02_queues.c" region="RQ4"

### ف8 RQ 5: أول FRONT=1 وREAR=5 (A..E)

السؤال مابيذكرش حجم المصفوفة، فحليته لحجمين: MAX = 10 (الـ linear يكفي)، وMAX = 6 (الخانات 0..5) فالـ linear بيعمل OVERFLOW من أول عملية والـ circular بيكمّل: كل العمليات بتنجح، والمصفوفة بتتملي بالظبط (6 عناصر) مرتين.

::code src="src/19-stacks-queues/exercises/ex19_02_queues.c" region="RQ5"

:::solution
في الحالتين المحتوى G H I. MAX=10: FRONT=7 وREAR=9. MAX=6 دايرية: FRONT=1 وREAR=3.
:::

### ف8 RQ 6: الـ deque (LEFT=1، RIGHT=5، المصفوفة 10 خانات دايرية)

::code src="src/19-stacks-queues/exercises/ex19_02_queues.c" region="RQ6"

:::solution
المحتوى من الشمال لليمين `J B C D E G` (LEFT=1، RIGHT=6). لاحظ إن (a) أدخل F في الخانة 0 (LEFT رجع خطوة من 1) وإن (d) شالت F ثم A.
:::

### ف8 PE 1، 4، 11، 12

::code src="src/19-stacks-queues/exercises/ex19_02_queues.c" region="PE1"

::code src="src/19-stacks-queues/exercises/ex19_02_queues.c" region="PE4"

::code src="src/19-stacks-queues/exercises/ex19_02_queues.c" region="PE11"

:::think
مقارنة queueين بدون ما نبوّظهم: نعمل dequeue من الاتنين ونقارن، وبعدين enqueue لكل واحد لعنصره. بعد n دورة بيرجعوا لنفس الترتيب بالظبط (دوران كامل).
:::

**ناتج تشغيل ex19\_02\_queues كامل** (RQ 4 → 6 بالرسم، وبعدها PE 1–12):

::output src="src/19-stacks-queues/exercises/ex19_02_queues.c" lines="70"

### ف8: اختيار / صح-غلط / فراغات (بمفتاح الكتاب)

<table class="comp-table">
<tr><th>#</th><th>إجابة الكتاب</th><th>ليه</th></tr>
<tr><td colspan="3"><b>اختيار من متعدد</b></td></tr>
<tr><td>1</td><td>(b) Queue</td><td>طابور البقالة.</td></tr>
<tr><td>2</td><td>(a) Rear</td><td>الإدخال من الـ rear.</td></tr>
<tr><td>3</td><td>(b) dequeue</td><td>بتشيل من الـ front.</td></tr>
<tr><td>4</td><td>(a) O(1)</td><td>كل العمليات الأساسية.</td></tr>
<tr><td>5</td><td>(b) FRONT = 0 and REAR = MAX−1</td><td>إحدى حالتَي الامتلاء (الحالة التانية <code>FRONT == REAR+1</code>).</td></tr>
<tr><td colspan="3"><b>صح/غلط</b></td></tr>
<tr><td>1</td><td>False</td><td>(مفتاح الكتاب) في الدايرية أول عنصر مش لازم يبقى في أول المصفوفة.</td></tr>
<tr><td>2</td><td>False</td><td>الـ priority queue بتخدم بالأولوية مش بالتتابع.</td></tr>
<tr><td>3</td><td>False</td><td>الـ linked queue مالهاش حد أقصى ثابت.</td></tr>
<tr><td>4</td><td>True</td><td>نفس FIFO.</td></tr>
<tr><td>5</td><td>False</td><td>بتكبر وتصغر وقت التشغيل.</td></tr>
<tr><td>6</td><td>True</td><td>FCFS عند تساوي الأولوية.</td></tr>
<tr><td>7</td><td>True</td><td>output-restricted: حذف من طرف واحد، إدخال من الاتنين.</td></tr>
<tr><td>8</td><td>True (بمفتاح الكتاب)</td><td>تنبيه: <code>front = MAX−1, rear = 0</code> معناها عنصرين بس، فهي ممتلئة حرفيًا بس لما MAX = 2. القاعدة العامة <code>front == (rear+1) % MAX</code> (مثلًا front=MAX−1 وrear=MAX−2).</td></tr>
<tr><td colspan="3"><b>الفراغات</b></td></tr>
<tr><td>1</td><td>Rear</td><td>العقد الجديدة بتتضاف في الـ rear.</td></tr>
<tr><td>2</td><td>Deque</td><td>إدخال من الطرفين مش الوسط.</td></tr>
<tr><td>3</td><td>O(1)</td><td>linked queue.</td></tr>
<tr><td>4</td><td>Input restricted deque</td><td>إدخال من طرف واحد وحذف من الاتنين.</td></tr>
<tr><td>5</td><td>Circular array أو circular doubly linked list</td><td>تنفيذ الـ deque.</td></tr>
<tr><td>6</td><td>Priority queues</td><td>(مفتاح الكتاب) معالجة batch programs.</td></tr>
<tr><td>7</td><td>Queues</td><td>(مفتاح الكتاب) قائمة موظفين بنظام الأقدمية. لو الأقدمية = أولوية فالـ priority queue هي الأدق.</td></tr>
</table>
