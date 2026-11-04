تمارين محلولة · الفصل 23 (heaps وpriority queues) · Thareja ف12: Review Questions 1–30 + الأسئلة الموضوعية (MCQ وصح/غلط والفراغات، بمفتاح Appendix G) · Weiss ف6: تمارين 6.1–6.40 · P&amp;H: Algorithms Insert/Remove (فصل 6) وFig 8.23 وExercise 2 (فصل 8) · B&amp;D: Fig 10.7. (heap sort عند B&amp;D وP&amp;H في فصل الترتيب.)

الحلول في تلات ملفات كل واحد بيعمل `#include` لبرامج الدرس: `exercises/ex23_01_thareja_heaps.c` و`ex23_02_weiss_heaps.c` و`ex23_03_pph_bd_heaps.c`. كلها بتتأكد بـ `assert` وبتطبع الإجابة.

:::goal هدف الصفحة:
تحل أي سؤال heap: ارسم النتيجة، واحسب التكلفة، واعرف أي حاجة اتحلّت بالكود وأي حاجة نظرية فقط (مكتوبة بوضوح).
:::

* [src/23-heaps/exercises/ex23\_01\_thareja\_heaps.c](src:src/23-heaps/exercises/ex23_01_thareja_heaps.c)
* [src/23-heaps/exercises/ex23\_02\_weiss\_heaps.c](src:src/23-heaps/exercises/ex23_02_weiss_heaps.c)
* [src/23-heaps/exercises/ex23\_03\_pph\_bd\_heaps.c](src:src/23-heaps/exercises/ex23_03_pph_bd_heaps.c)
* [src/23-heaps/binheap.c](src:src/23-heaps/binheap.c)
* [src/23-heaps/leftist.c](src:src/23-heaps/leftist.c)
* [src/23-heaps/binomial.c](src:src/23-heaps/binomial.c)
* [src/23-heaps/fibheap.c](src:src/23-heaps/fibheap.c)
* [src/23-heaps/minmax.c](src:src/23-heaps/minmax.c)

## 1) Thareja ف12 — Review Questions

<table class="comp-table">
<tr><th>السؤال</th><th>الإجابة / مكانها</th></tr>
<tr><td>1</td><td>الـ binary heap = شجرة ثنائية كاملة + كل عقدة ≤ (أو ≥) ولادها. الدرس قسم 2.</td></tr>
<tr><td>2</td><td>min-heap: الجذر أصغر عنصر وكل أب ≤ ابنه. max-heap: الجذر أكبر عنصر وكل أب ≥ ابنه. (Fig 8.23 في P&amp;H: ex23_03.)</td></tr>
<tr><td>3</td><td>الـ binary tree العادي ممكن يكون أي شكل ومفيش ترتيب بين العقد (والـ BST: شمال &lt; جذر &lt; يمين)؛ الـ heap شكله كامل (يتخزن في array) وترتيبه أب/ابن فقط، والأخوات مش مرتبين. Q28 تحت بيبيّن إنهم نادرًا يتقابلوا.</td></tr>
<tr><td>4، 5</td><td>إدخال وحذف بمثال: الدرس قسم 3 و4 (Example 12.1 وExample 12.3 بالرسم).</td></tr>
<tr><td>6</td><td>تطبيقات: priority queue، heap sort، جدولة العمليات، الـ selection problem (Weiss 6.39)، Huffman، Dijkstra/Prim.</td></tr>
<tr><td>7</td><td><code>50 40 35 25 20 27 33</code>: max-heap بالإدخال واحد واحد = <code>50 40 35 25 20 27 33</code> (هو أصلاً max-heap فمفيش تبديل). min-heap بالإدخال = <code>20 25 27 50 35 40 33</code>، وبـ Floyd = <code>20 25 27 50 40 35 33</code> (الاتنين صح). ex23_01 <code>Q7</code>.</td></tr>
<tr><td>8</td><td>لأن findMin O(1) وinsert وdeleteMin O(log n) من غير مؤشرات (الجدول في الدرس قسم 1).</td></tr>
<tr><td>9</td><td>شجرة binomial B<sub>k</sub> (شجرتين B<sub>k−1</sub>)، وـ binomial heap = مجموعة أشجار binomial heap-ordered، أقصى واحدة من كل رتبة. الدرس قسم 11 والرسم Fig 12.13 وB0…B4.</td></tr>
<tr><td>10</td><td>جدول المقارنة في الدرس قسم 13: binary بـ O(n) للـ union، وbinomial بـ O(log n)، وFibonacci بـ O(1) والـ decrease-key بـ O(1) amortized.</td></tr>
<tr><td>11، 18–20</td><td>عمليات Fibonacci: insert (O(1): نضيف لقايمة الجذور)، union (نربط القايمتين O(1))، extract-min (نشيل الـ min، أبناؤه يبقوا جذور، ثم consolidate بربط الجذور المتساوية الرتبة؛ O(log n) amortized)، decrease-key (cut وcascading cut)، delete. الدرس قسم 12 + <code>fibheap.c</code>.</td></tr>
<tr><td>12</td><td>لأن decrease-key وunion وinsert أرخص (O(1) amortized)، فتفيد Dijkstra وPrim. (والتحفّظ العملي في الدرس.)</td></tr>
<tr><td>13</td><td>union بتدمج قايمتي الجذور (O(log n) لأن كل قايمة ≤ log n + 1 جذر) ثم تمر بلفة واحدة تربط المتساوي (O(log n))؛ المجموع O(log n). وفي T/F 10 الكتاب بيقول إن Merge_Binomial-Heap بيشتغل في O(m log m) (m = عدد الجذور) ومفتاحه False لأنه خطي في m.</td></tr>
<tr><td>14</td><td>الـ min في أحد الجذور، وفيه على الأكتر ⌊log n⌋+1 جذر، فبنلف عليهم كلهم: O(log n) (<code>bn_min</code>).</td></tr>
<tr><td>15</td><td>إدخال في binomial heap: نعمل heap من عقدة واحدة ونعمل union؛ مثال 7 إدخالات 1…7 في الدرس (fig ds23-bn-insert): N−popcount(N) links.</td></tr>
<tr><td>16</td><td>extract-min: find-min O(log n) + عكس قايمة الأبناء O(log n) + union O(log n) = O(log n).</td></tr>
<tr><td>17</td><td>delete: decrease القيمة لـ −∞ (تطلع للجذر) ثم extract-min. <code>bn_delete</code> وثبوتها في <code>binomial.c</code> (على نمط Example 12.7: بعد حذف 5 من الـ queue اللي في الدرس بتبقى <code>4 2(3(6),7)</code>).</td></tr>
<tr><td>21</td><td>الشجرة <code>4 | 6 12 | 7 9 21 39 | 13 19 10</code> هي min-heap (مش max-heap). <code>Q21</code>.</td></tr>
<tr><td>22</td><td>الـ percolate up لـ 54: <code>54 45 36 27 18 29 21 16</code>. <code>Q22</code>.</td></tr>
<tr><td>23</td><td>التمثيل: <code>4 6 12 7 9 21 39 13 19 10</code> (فهرس 1…10).</td></tr>
<tr><td>24</td><td>أب 10 = 18، أب 21 = 36، أب 23 = 36؛ ابنين 23 عند 14 و15 (بره الـ heap الحجم 8).</td></tr>
<tr><td>25</td><td>(a) max-heap، (b) لا، (c) نعم (min وmax).</td></tr>
<tr><td>26، 27</td><td>يتشال 52 ثم 42 ثم 37، وبعد الإضافات: <code>52 35 42 22 32 27 37 12 7 12 24 10</code>.</td></tr>
<tr><td>28</td><td>heap وBST في نفس الوقت: <b>بالقوة الغاشمة</b>: n=1: موجود، n=2: ترتيب واحد (max-heap [2,1])، وn=3…7: <b>مفيش ولا واحد</b>. السبب: عقدة بابنين في BST لازم يكون الشمال أصغر واليمين أكبر، وفي الـ heap لازم الاتنين ≤ الجذر أو ≥ الجذر. فالمثال الوحيد الممكن: عقدة أو عقدتين (جذر بابن شمال). (الكتاب نفسه بيطلب «ارسم» فالجواب: شجرة <code>2(1)</code> أو عقدة واحدة.)</td></tr>
<tr><td>29</td><td>heapify: عقدة بارتفاع h تنزل ≤ h مستوى (مقارنتين لكل مستوى)؛ مجموع الارتفاعات ≤ n−1 فالمقارنات &lt; 2n؛ O(n). اتأكد لكل n من 1 لـ 995. الدرس قسم 5.</td></tr>
<tr><td>30</td><td>Fibonacci heap: decrease 33→9، insert 5، delete 19. النتيجة <code>5(9,36(41)) 16(36,17(24(27(39),44),18),18)</code> (الشكل الأصلي مقروء جزئيًا؛ قراءتنا متحقق منها بالـ invariants).</td></tr>
</table>

### الأسئلة الموضوعية (Appendix G)

<table class="comp-table">
<tr><th>النوع</th><th>الإجابات</th></tr>
<tr><td>MCQ 1–7</td><td>1 (b) O(log n) · 2 (a) 2i · 3 (c) O(n log n) · 4 (d) i · 5 (b) 1 · 6 (d) O(1) · 7 (d) O(1)</td></tr>
<tr><td>True/False 1–11</td><td>T، F، T، T، F، F، T، F، T، F، F</td></tr>
<tr><td>Fill 1–11</td><td>2i+1 · priority queues · partially ordered trees · min heap · root · i · set of binomial trees (حسب تعريف الكتاب) · 2<sup>i</sup> · O(1) · collection of heap-ordered trees · whether x lost a child</td></tr>
</table>

:::oops اختلافات مع مفتاح الكتاب
**MCQ 3:** المفتاح (c) O(n log n) لبناء heap من n عنصر في أسوأ حالة، لكن بناء Floyd خطي O(n) (اللي أثبتناه وقسناه)؛ الإجابة (c) صحيحة بس لو الكتاب يقصد «الإدخال واحد واحد». **T/F 6:** «Bi فيها 2i عقدة» نص مستخرج من PDF؛ العدد الصح 2<sup>i</sup>، ومفتاح الكتاب False. **T/F 9:** Min\_Binomial-Heap O(log n) = True (بالمفتاح). **جدول 12.1:** بيكتب find-min وcreate بـ O(n) لـ Fibonacci، والصح O(1).
:::

::output src="src/23-heaps/exercises/ex23_01_thareja_heaps.c" lines="56"

## 2) Weiss ف6 — التمارين 6.1–6.40

<table class="comp-table">
<tr><th>التمرين</th><th>الإجابة</th><th>الحل في</th></tr>
<tr><td>6.1</td><td>نعم لو مفيش deleteMin (نحتفظ بأصغر واحد). وفي الـ binary heap المتوسط ثابت تقريبًا (2.24 مقارنة قيس).</td><td>ex23_02 <code>6.1</code></td></tr>
<tr><td>6.2، 6.3</td><td>(a) <code>1 3 2 6 7 5 4 15 14 12 9 10 11 13 8</code> · (b) <code>1 3 2 12 6 4 8 15 14 9 7 5 11 13 10</code> · بعد 3 deleteMin: <code>4 6 5 13 7 10 8 15 14 12 9 11</code></td><td><code>6.2</code> <code>6.3</code></td></tr>
<tr><td>6.4</td><td>الحجم المطلوب للـ array لشجرة غير كاملة: (a) 8N، (b) 2N² (c) 2N<sup>4.1</sup> (d) 2<sup>N</sup>−1.</td><td><code>6.4</code></td></tr>
<tr><td>6.5</td><td>الحارس في الخانة 0: نفس النتيجة (2000 heap).</td><td><code>bh_insert_sentinel</code></td></tr>
<tr><td>6.6</td><td>الشكل Fig 6.13 رسم بدون مفاتيح أو أعداد: مش بالإمكان الإجابة من النص. بس: شجرة كاملة بارتفاع h فيها بين 2<sup>h</sup> و2<sup>h+1</sup>−1 عقدة.</td><td><code>6.6</code></td></tr>
<tr><td>6.7</td><td>(a) ≤ 2N−2 (N ≤ 600). (b) 8 مقارنات بالظبط لكل الـ 8! ترتيب. (c) غير متنفّذ.</td><td><code>6.7</code></td></tr>
<tr><td>6.8</td><td>أكبر عنصر في min-heap لازم يكون في ورقة (كل N≤15)، وعدد الأوراق ⌈N/2⌉.</td><td><code>6.8</code></td></tr>
<tr><td>6.9</td><td><b>غير محلول</b> (إثبات احتمالي).</td><td>—</td></tr>
<tr><td>6.10</td><td>(a) خوارزمية بتزور عقدة وتقطع عند مفتاح ≥ X: بتزور ≤ 2K+1 عقدة (500 heap). (b) تنفع على أي heap-ordered tree (leftist/skew/binomial/Fibonacci). (c) غير متنفّذ.</td><td><code>6.10</code></td></tr>
<tr><td>6.11</td><td><b>غير محلول</b>.</td><td>—</td></tr>
<tr><td>6.12</td><td>N = 200000: مرتب 199,999 مقارنة للاثنين؛ معكوس: إدخال 3,137,875 وبناء 399,976؛ عشوائي 456,453 و376,556.</td><td><code>6.12</code></td></tr>
<tr><td>6.13</td><td>(a) مسار «أحسن ابن» + binary search: 18.4 مقارنة بدل 29.3 (65536 مفتاح). (b)–(d) غير متنفّذة.</td><td><code>6.13</code></td></tr>
<tr><td>6.14، 6.15</td><td><code>parent(i) = ⌊(i−2)/d⌋+1</code>، <code>child k = d(i−1)+2+k</code>. أنسب d: 4 (M=N)، 9 (M=10N)، 38 (M=100N) عند N=10<sup>6</sup>. (b) d=2: O((M+N) log N). (c) d=Θ(N): O(M + N²).</td><td><code>6.14</code> <code>6.15</code></td></tr>
<tr><td>6.16</td><td>نمشي على bits العدد i بعد أول 1 (0 شمال، 1 يمين): O(log N).</td><td><code>6.16</code></td></tr>
<tr><td>6.17</td><td><b>غير محلول</b> (خوارزميات دمج heaps كاملة بمؤشرات).</td><td>—</td></tr>
<tr><td>6.18</td><td>(a) min=a[1]، max=الأكبر من a[2] وa[3]. (b) الإدخال و(c) الحذفين بـ O(log N). (d) البناء الخطي نعم (<code>mm_build</code>). (e) <b>غير محلول</b>.</td><td><code>minmax.c</code></td></tr>
<tr><td>6.19–6.22</td><td>انظر الدرس قسم 9 و10.</td><td><code>6.19</code>…<code>6.22</code></td></tr>
<tr><td>6.23</td><td>سؤال نظري؛ <b>غير متنفّذ</b> (انظر الدرس قسم 9).</td><td>—</td></tr>
<tr><td>6.24، 6.25، 6.26–6.29</td><td>lazy deletion، buildHeap بالـ queue، وskew heap: الدرس قسم 9 و10.</td><td><code>6.24</code>…<code>6.29</code></td></tr>
<tr><td>6.30، 6.31</td><td>B<sub>0</sub>…B<sub>12</sub>: الجذر له k ابن، وعلى العمق d عدد C(k,d) عقدة.</td><td><code>6.30</code></td></tr>
<tr><td>6.32</td><td>انظر الدرس قسم 11.</td><td><code>6.32</code></td></tr>
<tr><td>6.33–6.36</td><td>N − popcount(N) links؛ إدخال مباشر مطابق؛ early stop: 32 → 2.13 دورة؛ 6.35(b) «الصغير في الكبير» (5.9 مقابل 11.4)؛ 6.36 نظري فقط.</td><td><code>6.33</code>…<code>6.36</code></td></tr>
<tr><td>6.37</td><td>bin packing C=5، أوزان 2 2 3 3: first fit = 3 صناديق، أفضل = 2. الترتيب التنازلي أولاً: أحسن في 145 من 300 حالة، ومساوي في الباقي (155)، فالإجابة (d): «غالبًا لكن مش دايمًا (اتحسب)».</td><td><code>6.37</code></td></tr>
<tr><td>6.38</td><td><code>decreaseAllKeys</code> بـ O(1): كل مفتاح مخزّن = الحقيقي + offset؛ العملية بتغيّر الـ offset (جمع واحد).</td><td><code>6.38</code> (<code>OffHeap</code>)</td></tr>
<tr><td>6.39</td><td>selection: الخوارزمية A (buildHeap + k deleteMin) مقابل B (heap بحجم k)؛ الأرقام في الدرس قسم 13.</td><td><code>6.39</code></td></tr>
<tr><td>6.40</td><td>نسخ/تحرير leftist heap بدون recursion عميق (العمق 2 بدل 20000).</td><td><code>6.40</code></td></tr>
</table>

::output src="src/23-heaps/exercises/ex23_02_weiss_heaps.c" lines="30"

## 3) P&amp;H وB&amp;D

<table class="comp-table">
<tr><th>التمرين</th><th>الإجابة</th></tr>
<tr><td>P&amp;H Algorithm Insert / Remove (فصل 6)</td><td>اتنفّذت بالحرف (<code>ph_insert</code>، <code>ph_heapify</code>، <code>ph_remove</code>) واتقاست تكلفة Table 6.7.</td></tr>
<tr><td>P&amp;H Fig 8.23</td><td>(a) max-heap، (b) min-heap: اتأكد إن كل واحد مش من النوع التاني.</td></tr>
<tr><td>P&amp;H Ex 8.2: min-heap من <code>12, 11, 7, 3, 10, −5, 0, 9, 2</code></td><td>بالإدخال: <code>−5 2 0 7 10 11 3 12 9</code>. وبـ Floyd (shift-down): <code>−5 2 0 3 10 7 12 9 11</code>. الاتنين صح.</td></tr>
<tr><td>B&amp;D Fig 10.7</td><td>التلات arrays فيهم نفس الـ 9 أرقام وكلها min-heap. من بين 362,880 ترتيب، <b>896</b> بس heap (= 9!/405، حيث 405 = حاصل ضرب أحجام الأشجار الفرعية).</td></tr>
</table>

::output src="src/23-heaps/exercises/ex23_03_pph_bd_heaps.c" lines="14"

:::tip ملحوظة على التغطية
التمارين المكتوبة «غير محلول» أو «غير متنفّذ» فوق اتسجّلت في `COVERAGE.md` كمتبقيات، ومش بنادّعي إنها اتحلت.
:::
