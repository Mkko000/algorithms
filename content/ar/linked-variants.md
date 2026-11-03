المراجع: Thareja ف6 (6.1–6.9) · Lipschutz ف5 (5.9 Header lists، 5.10 Circular، 5.11 Two-way، 5.12 Josephus، polynomials) · Weiss ف3 (3.2: header + cursor implementation، polynomial ADT) · Bandyopadhyay &amp; Dey ف4–5 · Pal &amp; Halder ف5

[درس 06](lesson:linked-lists) شرح القايمة المتصلة العادية (singly linked): insert/delete/traverse وإزاي تتحكم في الـ pointers. الدرس ده بيكمّل الصورة بالـ **أنواع التانية** اللي كل كتاب مقرّر بيقولك عليها وماكانتش مغطّاة: القايمة اللي ليها header، الدايرية (circular)، المزدوجة (doubly)، القايمة من غير pointers أصلًا (cursor)، والتطبيق الكلاسيكي: كثيرات الحدود. كمان خوارزميات الـ interview الشهيرة على القوايم (قلب، دمج، منتصف، cycle، ...).

:::goal هدف الدرس:
تختار نوع القايمة المناسب، تكتب كل عملية بدون حالات خاصة (header/sentinel)، وتحوّل أي خوارزمية على قايمة لـ O(1) مساحة بالـ relinking. كل الكود في `src/18-linked-list-variants/` بيتشغّل بـ AddressSanitizer من غير تسريب.
:::

:::tip بدون تكرار
الـ Josephus بـ circular list اتحلّت قبل كده في [تمارين الفصل 3](lesson:ex-ch3) (تمرين 3.30). هنا بنربطها بالصيغة المغلقة O(n) وبالـ header list.
:::

## 1) ليه أنواع كتير؟ الجدول الأول

<table class="comp-table">
<tr><th>النوع</th><th>الإضافة</th><th>بيحلّ إيه</th><th>التكلفة</th></tr>
<tr><td>Singly</td><td>next بس</td><td>أبسط حاجة</td><td>حذف عقدة معروفة = O(n) (محتاجين السابق)</td></tr>
<tr><td>Header (sentinel)</td><td>عقدة وهمية في الأول</td><td>مفيش حالة خاصة للأول/الفاضية</td><td>عقدة زيادة</td></tr>
<tr><td>Circular</td><td>الآخر يشاور على الأول</td><td>الدوران (round-robin، Josephus، CPU scheduling)، الوصول للآخر O(1) لو شاورنا على الآخر</td><td>خطر لوب لا نهائي</td></tr>
<tr><td>Doubly</td><td>prev + next</td><td>حذف عقدة معروفة O(1)، مشي للخلف، undo/redo</td><td>pointer زيادة لكل عقدة + كتابات أكتر</td></tr>
<tr><td>Cursor</td><td>array بدل malloc</td><td>لغات من غير pointers، ذاكرة ثابتة، تخزين على ملف</td><td>حجم ثابت</td></tr>
</table>

## 2) الـ Header node: امسح الحالات الخاصة

مع القايمة العادية، إدخال أو حذف **أول** عقدة حالة خاصة لأن `head` نفسه بيتغيّر. لو في الأول عقدة وهمية ما بتتحذفش أبدًا، كل عقدة بيانات ليها «سابق» وكل العمليات بتبقى نفس الكود. وفي الـ header بنقدر نخزّن **عدد العقد** (ملحوظة Thareja وLipschutz) فالـ length بقى O(1).

::figure circ-header

::code src="src/18-linked-list-variants/circular_header.c" region="def"

لاحظ إن `ch_delete_after` هي **الأداة الوحيدة** للحذف: كل حذف تاني (أول، آخر، k-th، بالقيمة) هو إيجاد السابق وبعدين استدعاءها.

::code src="src/18-linked-list-variants/circular_header.c" region="ops"

:::oops فخ حذف داخل لفّة
في `ch_delete_negatives` مش بنتقدّم بـ `p = p->next` بعد الحذف، لأن العقدة اللي بعد p اتغيّرت وممكن تكون هي كمان سالبة (مثال: `-1 -2 -3` ورا بعض). لو تقدّمت هتفوّت واحدة. اتقدّم **بس** لما ما تحذفش.
:::

## 3) الـ Josephus على قايمة دايرية

n ناس في دايرة، كل m-ـة بتتشال لحد ما يفضل واحد. بالقايمة الدايرية: ندور m−1 خطوة ونحذف التالي. التكلفة O(n·m). فيه كمان علاقة تكرارية `J(i) = (J(i−1) + m) mod i` بتدّي الإجابة في O(n) من غير ما نبني قايمة أصلًا. البرنامج بيتأكد إن الاتنين بيدّوا نفس الرقم لكل n ≤ 40 وm ≤ 9 (والحالة التاريخية J(41,3) = 31).

::code src="src/18-linked-list-variants/circular_header.c" region="josephus"

::output src="src/18-linked-list-variants/circular_header.c" lines="10"

## 4) القايمة المزدوجة (doubly) الدايرية بـ header

أقوى نسخة: كل عقدة ليها `prev` و`next`، والـ header بيقفل الدايرة. النتيجة: **مفيش NULL في أي مكان** وبالتالي مفيش `if (p == NULL)` في الكود.

::figure dlist-insert

::code src="src/18-linked-list-variants/doubly_list.c" region="def"

العمليات المهمة: القلب بتبديل `prev`/`next` في كل عقدة (بما فيها الـ header)، والدمج O(1) لأن بنوصّل آخر a بأول b بس، ونقل عقدة للأول من غير allocation.

::code src="src/18-linked-list-variants/doubly_list.c" region="ops"

### تبديل عقدتين بالـ relinking والترتيب

لما العقدة كبيرة (record)، تبديل **البيانات** غالي. تبديل **الروابط** بيكلّف ثابت. وقاعدة ترتيب الإدراج (insertion sort) على القايمة المزدوجة بتمشي لليسار من العقدة لحد مكانها — اللي ممكن لأن عندنا `prev`.

::code src="src/18-linked-list-variants/doubly_list.c" region="swap"

::output src="src/18-linked-list-variants/doubly_list.c" lines="12"

## 5) خوارزميات القوايم الكلاسيكية

خمس حاجات بتظهر في كل امتحان ومقابلة:

### القلب (reverse)

::figure ds-list-reverse

::code src="src/18-linked-list-variants/list_algos.c" region="reverse"

### الدمج والتقسيم

::code src="src/18-linked-list-variants/list_algos.c" region="merge"

::code src="src/18-linked-list-variants/list_algos.c" region="split"

عقدة `dummy` على الـ stack بتلغي حالة «أول عقدة في النتيجة». ده نفس فكرة الـ header بس مؤقتة.

### مؤشرين بسرعتين

::code src="src/18-linked-list-variants/list_algos.c" region="twoptr"

ليه `has_cycle` بتشتغل؟ لو فيه دايرة، الاتنين هيدخلوها، والمسافة بينهم بتقلّ واحد كل خطوة (الأرنب بيسبق بخطوة زيادة)، فلازم يتقابلوا. ولو مفيش، الأرنب بيوصل NULL.

### التكرار والترتيب (merge sort)

::code src="src/18-linked-list-variants/list_algos.c" region="dups"

::code src="src/18-linked-list-variants/list_algos.c" region="msort"

Merge sort هو الترتيب الأنسب للقوايم: الدمج بيعيد ربط العقد بس، فمفيش مصفوفة مساعدة (O(1) مساحة زيادة، مقابل O(n) في المصفوفات)، والوصول العشوائي اللي quicksort/heapsort محتاجينه مش موجود. التحليل الكامل في [درس mergesort](lesson:mergesort).

::output src="src/18-linked-list-variants/list_algos.c" lines="12"

## 6) كثيرات الحدود (polynomials) كقايمة

كثيرة حدود زي 7x³y² − 8x²y + 3xy + 11x − 4 مصفوفة معاملات هتكون مضيّعة (مصفوفة 2D فيها أغلبها أصفار — راجع sparse في [الدرس السابق](lesson:arrays-matrices)). الحل: قايمة **حدود** (معامل، أس x، أس y) مرتبة تنازليًا. كده الجمع = دمج (merge) وبيحذف الحد لو المعاملين اتلغوا.

::code src="src/18-linked-list-variants/poly_list.c" region="term"

::code src="src/18-linked-list-variants/poly_list.c" region="add"

الضرب: ضرب كل حد من q في كل p، وتجميع النتائج. لـ m وn حد: O(m·n) ضربة. الطريقة اللي فوق بتجمّع الجزئيات واحد واحد (أبطأ شوية من heap-merge) بس أبسط وأسهل في الفهم. (قارن بـ [درس 10](lesson:first-class) اللي عمل نفس الـ ADT بمصفوفات.)

::code src="src/18-linked-list-variants/poly_list.c" region="mul"

::output src="src/18-linked-list-variants/poly_list.c" lines="10"

الاختبار بيتحقق من الضرب بخاصية التقييم: `(pq)(a,b) = p(a,b)·q(a,b)` على 25 نقطة، وده أقوى من ملاحظة الناتج بالعين.

## 7) الـ Cursor: قايمة من غير pointers

في FORTRAN القديمة وأنظمة الـ embedded من غير `malloc`، وحتى في قواعد البيانات (السجل بيشاور على سجل برقمه)، مفيش pointers. الحل: array ضخمة من العقد، و«pointer» = رقم خانة، والصفر = NULL. الخانة 0 هي header الـ **free list**.

::figure cursor-pool

::code src="src/18-linked-list-variants/cursor_list.c" region="pool"

::code src="src/18-linked-list-variants/cursor_list.c" region="list"

::output src="src/18-linked-list-variants/cursor_list.c" lines="10"

الميزة التانية: قايمتين أو أكتر بيشاركوا نفس الـ pool، والـ OVERFLOW حاجة معروفة (`alloc` بترجّع 0) بدل ما برنامج يقع.

:::quiz
ليه إدخال في قايمة doubly circular بـ header مفيهوش if؟

1. [ ] لأنها أسرع
2. [x] لأن كل عقدة، حتى الأول والآخر، ليها prev وnext صالحين، والفاضية هي header بيشاور على نفسه
3. [ ] لأن malloc مبيفشلش

> الـ header بيلغي حالة NULL. نفس الأربع كتابات تشتغل في كل مكان.
:::

:::quiz
في حذف كل القيم السالبة، إمتى مانتقدّمش بـ p = p-\>next؟

1. [ ] أبدًا
2. [x] بعد ما نحذف العقدة اللي بعد p
3. [ ] لما تكون القيمة صفر

> العقدة الجديدة اللي بعد p ممكن تكون سالبة هي كمان.
:::

:::quiz
مساحة merge sort على linked list؟

1. [ ] O(n) مصفوفة مساعدة
2. [x] O(log n) stack للريكيرشن، والدمج نفسه O(1)
3. [ ] O(n²)

> الدمج بيعيد ربط العقد بس.
:::

:::quiz
إيه اللي بيضمن إن slow وfast هيتقابلوا لو فيه cycle؟

1. [ ] إنهم بيبدأوا من نفس العقدة بس
2. [x] الفجوة بينهم بتقلّ خطوة كل تكرار جوه الدايرة فلازم توصل صفر
3. [ ] عشان الدايرة بتبقى زوجية

> الفجوة (mod طول الدايرة) بتنقص 1 في كل دورة.
:::
