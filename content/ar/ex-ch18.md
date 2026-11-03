تمارين محلولة · الفصل 18 (Linked list variants) · Thareja ف6: Review Questions 1–11 + Programming Exercises 1–39 + أسئلة الاختيار/صح-غلط/الفراغات

الحلول متقسّمة على 4 ملفات كلها بتتشغّل بـ `assert`: التمارين المنفردة في `exercises/ex18_01_linked_lists.c`، وتمارين الـ circular/doubly/polynomial محلولة جوه برامج الدرس نفسها (`circular_header.c` و`doubly_list.c` و`poly_list.c`) لأنها ببساطة **هي** الدوال دي. الجدول الجاي بيقولك كل تمرين فين.

:::goal هدف الدرس:
تحل أي تمرين قايمة بنفس القالب: ارسم، حدّد الحالات الصغيرة (فاضية، عقدة، عقدتين)، اكتب بالـ pointer-to-pointer أو بالـ header، وبعدين اختبر.
:::

:::tip قالب pointer-to-pointer
معظم حلول الصفحة بتستخدم `node **pp = &head`: `pp` بيشاور على *الحقل* اللي بيشاور على العقدة (head نفسه أو next لعقدة). الحذف والإدخال بقوا سطر واحد من غير حالة «أول عقدة».
:::

* [src/18-linked-list-variants/exercises/ex18\_01\_linked\_lists.c](src:src/18-linked-list-variants/exercises/ex18_01_linked_lists.c)
* [src/18-linked-list-variants/doubly\_list.c](src:src/18-linked-list-variants/doubly_list.c)
* [src/18-linked-list-variants/circular\_header.c](src:src/18-linked-list-variants/circular_header.c)
* [src/18-linked-list-variants/poly\_list.c](src:src/18-linked-list-variants/poly_list.c)

### خريطة التمارين

<table class="comp-table">
<tr><th>تمرين</th><th>الموضوع</th><th>الحل في</th></tr>
<tr><td>1</td><td>حذف كل المكرر</td><td>ex18: <code>dedup</code></td></tr>
<tr><td>2، 21</td><td>عدد مرات ظهور قيمة</td><td>ex18: <code>count_val</code></td></tr>
<tr><td>3</td><td>اضرب كل عنصر في 10</td><td>ex18: <code>scale</code></td></tr>
<tr><td>4، 27</td><td>عدد غير الصفري (عادية ودايرية)</td><td>ex18: <code>count_nonzero</code> · circular: <code>ch_count_nonzero</code></td></tr>
<tr><td>5، 35</td><td>هل القايمة مرتبة؟</td><td>ex18: <code>isSorted</code></td></tr>
<tr><td>6</td><td>نسخ قايمة دايرية</td><td>circular: <code>ch_copy</code></td></tr>
<tr><td>7، 9</td><td>دمج قايمتين / مرتبتين</td><td>ex18: <code>concat</code>, <code>merge_sorted</code></td></tr>
<tr><td>8</td><td>ترتيب doubly circular</td><td>doubly: <code>dl_sort</code></td></tr>
<tr><td>10، 37–39، 25</td><td>header lists: حذف أول/آخر/وسط، حذف السالب، كل حالات الحذف</td><td>ex18: <code>header_delete_first_last_middle</code> · circular: <code>ch_delete_*</code></td></tr>
<tr><td>11</td><td>قايمة بكل عنصر تاني</td><td>ex18: <code>alternate_copy</code></td></tr>
<tr><td>12</td><td>دمج doubly lists</td><td>ex18: <code>d_concat</code> · doubly: <code>dl_concat</code></td></tr>
<tr><td>13</td><td>الأول يتحوّل آخر (doubly)</td><td>ex18: <code>d_first_to_last</code> · doubly: <code>dl_rotate_left</code></td></tr>
<tr><td>14</td><td>حذف أول/آخر/كل ظهور لحرف</td><td>ex18: <code>del_*_occ</code></td></tr>
<tr><td>15، 33</td><td>قلب بالـ recursion وبقلب الروابط</td><td>ex18: <code>rev_rec</code>, <code>rev_iter</code></td></tr>
<tr><td>16</td><td>أرقام عدد في عقد</td><td>ex18: <code>digits_list</code></td></tr>
<tr><td>17، 23</td><td>المجموع والمتوسط</td><td>ex18: <code>sum_mean</code></td></tr>
<tr><td>18</td><td>الأصغر والأكبر</td><td>ex18: <code>min_max</code></td></tr>
<tr><td>19</td><td>تبديل أول↔آخر، تاني↔قبل الآخر (doubly)</td><td>ex18: <code>d_mirror_values</code></td></tr>
<tr><td>20</td><td>الأول يبقى الآخر (singly)</td><td>ex18: <code>first_to_last</code></td></tr>
<tr><td>22</td><td>زوّد 10 في doubly</td><td>ex18: <code>d_add</code></td></tr>
<tr><td>24</td><td>حذف الـ k-th</td><td>ex18: <code>del_kth</code> · circular: <code>ch_delete_kth</code></td></tr>
<tr><td>26</td><td>ضرب polynomial في رقم</td><td>poly: <code>poly_scale</code></td></tr>
<tr><td>28–31، RQ 7–9</td><td>قايمة سجلات موظفين/طلبة: عرض، بحث، إدخال، حذف</td><td>ex18: <code>rec_*</code></td></tr>
<tr><td>32</td><td>نقل العقدة الوسطى للأول (doubly)</td><td>ex18: <code>d_middle_to_front</code></td></tr>
<tr><td>34</td><td>n-th من الآخر بمرور واحد</td><td>ex18: <code>nth_from_end</code></td></tr>
<tr><td>36</td><td>تبديل k و k+1 في doubly circular</td><td>doubly: <code>dl_swap_with_next</code></td></tr>
<tr><td>RQ 10</td><td>قايمة حروف → حروف كبيرة وصغيرة</td><td>ex18: <code>split_case</code></td></tr>
<tr><td>RQ 11</td><td>ترتيب أسماء موظفين</td><td>ex18: <code>names_sort</code></td></tr>
</table>

### أسئلة المراجعة 1–6 و RQ5 (نظرية)

:::solution
**1)** الـ array: وصول عشوائي O(1)، الحجم ثابت، إدخال/حذف في النص O(n) إزاحة، أحسن cache locality. القايمة: وصول O(n)، حجمها بيتغيّر، إدخال/حذف O(1) لو عندك مكانه، وكل عقدة ليها overhead pointer. استخدم array لو الحجم معروف والوصول بالـ index كتير؛ قايمة لو الإدخال/الحذف كتير والحجم مش معروف.

**2)** الـ doubly بتحذف عقدة معروفة O(1) من غير ما تدوّر على السابق، وتمشي في الاتجاهين (undo/redo، browser history، LRU cache). **3)** الـ circular مناسبة للدوران (round-robin scheduling، Josephus، media playlist loop)، وبتخلّي الدخول للآخر قريب. **4)** الـ header بتلغي الحالات الخاصة وتخزّن metadata (العدد). **6)** في الـ circular آخر عقدة بتشاور على الأول (مفيش NULL) فلازم شرط وقف مختلف.

**5)** 7x³y² − 8x²y + 3xy + 11x − 4: 5 عقد، كل واحدة (c, ex, ey): `(7,3,2) → (−8,2,1) → (3,1,1) → (11,1,0) → (−4,0,0) → NULL`. ده بالظبط اللي `poly_list.c` بيبنيه ويطبعه.
:::

### التمارين 1–5 و 21 و 35

::code src="src/18-linked-list-variants/exercises/ex18_01_linked_lists.c" region="PE1"

::code src="src/18-linked-list-variants/exercises/ex18_01_linked_lists.c" region="PE2"

:::think
تمرين 1 فيه فخّين: (أ) لما نحذف `q->next` مانتقدّمش بـ q (العقدة الجديدة لسه ممكن تكون مكررة)، (ب) كل عقدة p بتحذف المكرر **بعدها** بس، فأول ظهور هو اللي بيفضل. O(n²) وبدون مساحة زيادة. لو مسموح بمساحة: hash set بتخلّيها O(n).
:::

### التمارين 7 و 9: الدمج

::code src="src/18-linked-list-variants/exercises/ex18_01_linked_lists.c" region="PE7"

:::solution
الدمج العادي (7) = ربط آخر الأولى بأول التانية. الدمج المرتب (9): مؤشرين وعقدة dummy، وبناخد من b **بس لو** `b < a` (يعني التساوي بيجيب من a: stable). مفيش allocation.
:::

### التمرين 10: حذف الأول والآخر والوسط في header list

::code src="src/18-linked-list-variants/exercises/ex18_01_linked_lists.c" region="PE10"

:::oops اختار ترتيب الحذف بعناية
الكود بيحذف **الوسط الأول** (قبل ما العدد يتغيّر)، وبعدين الآخر، وبعدين الأول. لو بدأت بحذف الأول، «الوسط» هيتحسب على قايمة متغيّرة. ولو القايمة عقدة واحدة، الوسط والآخر والأول هما نفس العقدة — الكود بيتعامل (الحذف التاني والتالت بيلاقوا قايمة فاضية فبيتجاهلوا).
:::

### التمرين 11 و 14

::code src="src/18-linked-list-variants/exercises/ex18_01_linked_lists.c" region="PE11"

::code src="src/18-linked-list-variants/exercises/ex18_01_linked_lists.c" region="PE14"

:::think
حذف «آخر ظهور» في تمرين 14: بنفتكر **pointer-to-pointer** لآخر عقدة طابقت أثناء المرور الوحيد، وبعدين نحذفها. من غير ما نمرّ مرتين أو نخزّن السابق.
:::

### التمارين 15 و 20 و 33: القلب ونقل الأول

::code src="src/18-linked-list-variants/exercises/ex18_01_linked_lists.c" region="PE15"

### التمارين 16 و 17 و 18 و 23

::code src="src/18-linked-list-variants/exercises/ex18_01_linked_lists.c" region="PE16"

:::solution
بنبني القايمة بـ push على الرأس (`h = mk(digit, h)`)، فأول رقم بيطلع (آخر خانة) بينتهي في آخر القايمة بالظبط الترتيب المطلوب: `12345 → 1,2,3,4,5`. والـ `do…while` بتضمن إن 0 يدّي عقدة واحدة.
:::

### التمارين 24 و 34

::code src="src/18-linked-list-variants/exercises/ex18_01_linked_lists.c" region="PE24"

:::think
مؤشران بينهم فجوة n: لما `lead` يوصل NULL يبقى `lag` هو الـ n-th من الآخر. مرور واحد، O(1) مساحة. الـ n أكبر من الطول يرجّع NULL.
:::

### التمارين 12 و 13 و 19 و 22 و 32: القايمة المزدوجة (النسخة اللي بتنتهي بـ NULL)

::code src="src/18-linked-list-variants/exercises/ex18_01_linked_lists.c" region="DL"

:::oops تمرين 19: اتجاه المؤشرين
شرط الوقف `i != j && i->prev != j`: لو العدد زوجي، المؤشرين بيعدّوا بعض (i بعد j) فلازم نوقف قبل ما نبدّل تاني (وإلا هيرجع كل حاجة زي ما كانت). لو فردي بيتقابلوا عند الوسط.
:::

### تمارين 28–31 و RQ 7–11: السجلات

::code src="src/18-linked-list-variants/exercises/ex18_01_linked_lists.c" region="REC"

:::solution
قايمة الموظفين بتتحافظ **مرتبة بالـ id** فالإدخال بيجيب مكانه من غير ترتيب بعدين (insertion in sorted list). الحذف بالـ id بيستخدم نفس `**pp`. ترتيب الأسماء (RQ 11) selection sort على الـ links: بنفصل أصغر عقدة ونضيفها آخر القايمة الناتجة، فمفيش نسخ strings. (تعقيد O(n²)؛ لو عايز أحسن استخدم merge sort من الدرس.)
:::

### تشغيل الملفات

::output src="src/18-linked-list-variants/exercises/ex18_01_linked_lists.c" lines="10"

::output src="src/18-linked-list-variants/doubly_list.c" lines="5"

### أسئلة الاختيار (Multiple-choice)

:::solution
<table class="comp-table">
<tr><th>#</th><th>الإجابة</th><th>#</th><th>الإجابة</th></tr>
<tr><td>1</td><td>(b) Sequential access</td><td>5</td><td>(b) Circular</td></tr>
<tr><td>2</td><td>(a) Random access (الـ index مباشر O(1))</td><td>6</td><td>(d) Circular header</td></tr>
<tr><td>3</td><td>(d) الكل</td><td>7</td><td>(b) Multi-linked (n pointers)</td></tr>
<tr><td>4</td><td>(c) Doubly (وكمان circular doubly)</td><td></td><td></td></tr>
</table>
:::

### صح أو غلط

:::solution
1\) صح · 2) صح · 3) صح · 4) غلط (ده doubly) · 5) غلط (أي نوع بيانات) · 6) غلط (sequential) · 7) صح · 8) غلط (ممكن أي حقول) · 9) صح · 10) صح.
:::

### املا الفراغ

:::solution
1\) AVAIL (بداية الـ free list) · 2) O(1) · 3) O(n) (محتاج السابق للآخر) · 4) **4** كتابات في singly-with-START شكل doubly: `new.next=START; new.prev=NULL; START.prev=new; START=new` · 5) **2** (`new.next=p.next; p.next=new`) · 6) **2** (`new.next=START; last.next=new`) · 7) **5** (`new.next, new.prev, last.next, START.prev, START`) · 8) **1** (`START=START.next`) · 9) **2** (`p.prev.next` و `p.next.prev`) · 10) **1** (`beforeLast.next=START`) · 11) node · 12) head/START · 13) INFO (data) · 14) مفيش خلية حرة متاحة (AVAIL = NULL) · 15) first (الأول).

*ملاحظة:* أرقام 4–10 بتعدّ كتابات الـ pointers في الكود المذكور؛ لو الكتاب قاس بطريقة تانية (مثلًا من غير ما يحسب START) هتختلف بواحد. المهم الفكرة: كل عملية = عدد ثابت من الكتابات.
:::
