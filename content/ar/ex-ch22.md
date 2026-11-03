تمارين محلولة · الفصل 22 (M-way وB وB+ و2-3 وtrie) · Thareja ف11: Review Questions 1–13 + أسئلة الاختيار وصح/غلط والفراغات (بمفتاح Appendix G) · B&amp;D ف12.7 (Fig 12.18–12.19، Exercise 12) · P&amp;H (Fig 8.50–8.54، Exercises 16، 17، 21(b)، MCQ 23(viii)) · Weiss (Fig 4.63–4.67، Exercises 4.41–4.42)

الحلول في ملفين: `exercises/ex22_01_multiway.c` (Thareja) و`exercises/ex22_02_bd_ph_wb_btrees.c` (B&amp;D وP&amp;H وWeiss). الاتنين بيعملوا `#include` لبرامج الدرس (`mway.c` وbtree.c وbplus.c وtwothree.c وtrie.c وwbtree.c) فمفيش كود متكرر، وكل ناتج متأكد منه بـ `assert`.

:::goal هدف الصفحة:
تحل أي سؤال شجرة متعددة الفروع: ارسم النتيجة بعد كل إدخال أو حذف، واحسب الارتفاع، وقارن B بـ B+ وبالـ trie.
:::

* [src/22-multiway-trees/exercises/ex22\_01\_multiway.c](src:src/22-multiway-trees/exercises/ex22_01_multiway.c)
* [src/22-multiway-trees/exercises/ex22\_02\_bd\_ph\_wb\_btrees.c](src:src/22-multiway-trees/exercises/ex22_02_bd_ph_wb_btrees.c)
* [src/22-multiway-trees/btree.c](src:src/22-multiway-trees/btree.c)
* [src/22-multiway-trees/bplus.c](src:src/22-multiway-trees/bplus.c)
* [src/22-multiway-trees/mway.c](src:src/22-multiway-trees/mway.c)
* [src/22-multiway-trees/twothree.c](src:src/22-multiway-trees/twothree.c)
* [src/22-multiway-trees/trie.c](src:src/22-multiway-trees/trie.c)
* [src/22-multiway-trees/wbtree.c](src:src/22-multiway-trees/wbtree.c)
* [src/22-multiway-trees/mnode.h](src:src/22-multiway-trees/mnode.h)

### خريطة التمارين

<table class="comp-table">
<tr><th>التمرين</th><th>الموضوع</th><th>الحل في</th></tr>
<tr><td>Thareja RQ 1</td><td>ليه m كبير؟</td><td>ex22_01: <code>RQ1</code> (+ نص تحت)</td></tr>
<tr><td>RQ 2، 3</td><td>B مقابل B+ ومتى B+</td><td>ex22_01: <code>RQ2</code> · نص تحت</td></tr>
<tr><td>RQ 4</td><td>trie: ملاحظة قصيرة</td><td>نص تحت · trie.c</td></tr>
<tr><td>RQ 5، 6</td><td>BST مقابل trie، hash مقابل trie</td><td>ex22_01: <code>RQ5</code>, <code>RQ6</code></td></tr>
<tr><td>RQ 7</td><td>ملخص M-way search trees</td><td>نص تحت · mway.c</td></tr>
<tr><td>RQ 8</td><td>B tree (Fig 11.4): إدخال 1, 5, 7, 11, 13, 15, 17, 19 وحذف 30, 59, 67</td><td>ex22_01: <code>RQ8</code> · btree: <code>q8</code></td></tr>
<tr><td>RQ 9</td><td>مقال عن B+</td><td>نص تحت · bplus.c</td></tr>
<tr><td>RQ 10</td><td>B+ order 5: 90, 27, 7, 9, 18, 21, 3, 4, 16, 11, 21, 72</td><td>ex22_01: <code>RQ10</code> · bplus: <code>q10</code></td></tr>
<tr><td>RQ 11</td><td>تطبيقات B</td><td>نص تحت</td></tr>
<tr><td>RQ 12</td><td>B order 2 = شجرة ثنائية كاملة</td><td>ex22_01: <code>RQ12</code> · btree: <code>order2</code></td></tr>
<tr><td>RQ 13</td><td>3-way tree (Q.13): إدخال 23, 45, 67, 87, 54, 32, 11 ثم حذف 9, 36, 54</td><td>ex22_01: <code>RQ13</code> · mway: <code>q13</code></td></tr>
<tr><td>MCQ 1–5، T/F 1–9، FIB 1–5</td><td>الأسئلة الموضوعية</td><td>الجدول تحت + <code>objective</code></td></tr>
<tr><td>B&amp;D Fig 12.18 / 12.19</td><td>توليد B-tree order 5 (20 مفتاح) وحذف 337, 448, 436, 299</td><td>ex22_02: <code>bd_fig1218</code></td></tr>
<tr><td>B&amp;D Exercise 12</td><td>اكتب حذف B-tree</td><td>btree.c: <code>bt_delete</code>, <code>bt_fix</code></td></tr>
<tr><td>B&amp;D Example 12.11–12.15</td><td>lookup وnode_search وinsert وmovedown وputkey وsplit</td><td>btree.c: <code>bt_search</code>, <code>bt_ins</code> (الشرح في الدرس)</td></tr>
<tr><td>P&amp;H Fig 8.51، Exercise 16</td><td>B order 4 من 14 مفتاح (19 مكرر)</td><td>ex22_02: <code>ph_fig851</code></td></tr>
<tr><td>P&amp;H Fig 8.52، Exercise 17</td><td>B order 3 بالحروف</td><td>ex22_02: <code>ph_fig852</code></td></tr>
<tr><td>P&amp;H Fig 8.53</td><td>حذف 10, 87, 39, 60, 72, 35</td><td>ex22_02: <code>ph_fig853</code></td></tr>
<tr><td>P&amp;H Fig 8.50، 8.54</td><td>M-way، 2-3، 2-3-4</td><td>ex22_02: <code>ph_fig854</code></td></tr>
<tr><td>P&amp;H Exercise 21(b)، MCQ 23(viii)</td><td>ملاحظة عن B-tree؛ «A B-tree is…» → d</td><td>نص تحت · ex22_02: <code>ph_mcq</code></td></tr>
<tr><td>Weiss Fig 4.63–4.67</td><td>B (M=5, L=5): إدخال 57, 55, 40 وحذف 99</td><td>wbtree: <code>fig463</code></td></tr>
<tr><td>Weiss Exercise 4.41 (a)(b)(c)، 4.42</td><td>إدخال وحذف وadoption وB*</td><td>wbtree: <code>wb</code>, <code>adopt</code> · الدرس قسم 6</td></tr>
</table>

## Thareja ف11: الحل المفصل

### RQ 1 — ليه محتاجين m كبير؟

الارتفاع = عدد قراءات الـ disk، وارتفاع شجرة order m لـ N مفتاح بين log<sub>m</sub>(N+1) (كل العقد ممتلية) و1 + log<sub>⌈m/2⌉</sub>((N+1)/2) (كلها نصف ممتلية). ولـ 10<sup>9</sup> مفتاح: order 2 → 30 قراءة، order 101 → 5 إلى 6، order 1001 → 3 إلى 4. وm بتتحدد من حجم الصفحة: صفحة 4096 byte، مفتاح 4 byte ومؤشر 4 byte → m = 512.

::figure ds22-q8

::code src="src/22-multiway-trees/exercises/ex22_01_multiway.c" region="RQ1"

### RQ 2، 3، 9 — B مقابل B+ (والمقال)

**الفرق:** B فيه البيانات في كل العقد، والمفتاح بيظهر مرة واحدة. B+ فيه البيانات في الأوراق فقط، المفاتيح في الفهرس نسخ للتوجيه، والأوراق موصولة بسلسلة. **القياس:** 20000 مفتاح عشوائي order 9: الاتنين ارتفاع 6؛ range بـ 1000 مفتاح بيقرا 188 عقدة في B ومعاها 184 في B+ (B بيعمل traversal للشجرة، B+ بيمشي على الأوراق)؛ B+ فيه 4151 عقدة منها 3502 ورقة، وB فيه 3619.

::code src="src/22-multiway-trees/exercises/ex22_01_multiway.c" region="RQ2"

**متى B+ (RQ 3):** (1) range queries وscan متسلسل (السلسلة)، (2) عقدة الفهرس أصغر فبتشيل مفاتيح أكتر ويقل الارتفاع، (3) كل البحث بياخد نفس عدد القراءات (لحد الورقة) فالأداء متوقع، (4) بيتبنى منه index على disk بسهولة (الداتا بترتبها الأوراق). **مقال B+ (RQ 9):** التعريف (البيانات بالأوراق، فهرس، سلسلة)، الإدخال (copy عند تقسيم ورقة، move عند تقسيم عقدة)، الحذف، التكلفة O(log<sub>m</sub> n)، والتطبيقات (indexes في قواعد البيانات وfile systems). كل نقطة فيها مثال محلول في `bplus.c` والدرس قسم 4.

### RQ 4، 5، 6 — الـ trie

**RQ 4 (ملاحظة قصيرة):** شجرة مرتبة بتخزّن مجموعة نصوص بحيث كل مستوى حرف؛ المسار من الجذر = prefix؛ عقدة بـ 26 فرع؛ البحث/الإدخال/الحذف O(L). **RQ 5:** BST بتقارن كلمات كاملة (كل مقارنة بتبص على أحرف كتير): 5000 كلمة، متوسط 51.6 حرف مفحوص للبحث مقابل 7.0 في الـ trie. المزايا: سرعة مستقلة عن عدد الكلمات، prefix search، ترتيب أبجدي مجاني. العيوب: مساحة (26 مؤشر لكل عقدة)، سوء الأداء مع cache. **RQ 6:** hash مقابل trie: الـ hash O(1) للكلمة الكاملة بس مفيش prefix ولا ترتيب ولا أطول prefix، وفيه collisions؛ الـ trie لا collisions ويدعم prefix: لـ «car» بينزل 3 خطوات ويعد car وcard وcare وcarp، والـ hash لازم يفحص كل الـ 15 إدخال.

::code src="src/22-multiway-trees/exercises/ex22_01_multiway.c" region="RQ5"

::code src="src/22-multiway-trees/exercises/ex22_01_multiway.c" region="RQ6"

### RQ 7 — ملخص M-way search trees

شجرة بحث، كل عقدة حتى M−1 مفتاح و M فرع، المفاتيح مرتبة والفروع بين المفاتيح، كل الشجرة الفرعية نفس الخاصية. **مش متوازنة** فالأداء O(n) في الأسوأ (قسم 2 في الدرس: نفس 12 مفتاح ارتفاع 6 ولا 4 حسب الترتيب). B-tree هو M-way + توازن.

### RQ 8 — إدخال وحذف في شجرة Fig 11.4

(a) الإدخال: 1, 5, 7, 11, 13, 15, 17, 19. (b) الحذف: 30, 59, 67. الخطوات في الشكل فوق (`ds22-q8`). النتيجتين المطبوعتين:

::code src="src/22-multiway-trees/exercises/ex22_01_multiway.c" region="RQ8"

:::oops ملاحظة
الكتاب بيرسم order 4 والـ median لـ 4 مفاتيح مؤقتة مش واضح؛ بنستعمل الـ median الأدنى. لو اخترت الأعلى بتطلع شجرة مختلفة صحيحة، فالـ checker بتاع الإجابة لازم يقبل الاتنين.
:::

### RQ 10 — B+ tree order 5

90, 27, 7, 9, 18, 21, 3, 4, 16, 11, **21**, 72 (الـ 21 التاني تكرار فبيتجاهل). النتيجة `[7 18 27]{[3 4] [7 9 11 16] [18 21] [27 72 90]}` وخطوات الشكل `ds22-q10` في الدرس.

::code src="src/22-multiway-trees/exercises/ex22_01_multiway.c" region="RQ10"

### RQ 11 — تطبيقات B-trees

file systems (NTFS وHFS+ وext4 directories)، فهارس قواعد البيانات (MySQL InnoDB، PostgreSQL)، فهرسة النصوص، تخزين المفاتيح في ذاكرة خارجية، caching في الأنظمة الموزعة. كلها نفس المبرر: قلة قراءات الـ disk.

### RQ 12 — B order 2 شجرة ثنائية كاملة

order 2: كل عقدة 1 مفتاح و2 ابن (أو صفر)، وأقل عدد أبناء ⌈2/2⌉ = 2 فمفيش عقدة بابن واحد، والأوراق في نفس المستوى؛ فكل مستوى ممتلي: ارتفاع h يعني 2<sup>h</sup>−1 مفتاح، وده تعريف الشجرة الثنائية الكاملة (full/perfect). البرنامج بيبني ويتأكد لـ 1..6 مستويات:

::code src="src/22-multiway-trees/exercises/ex22_01_multiway.c" region="RQ12"

### RQ 13 — شجرة 3-way (إدخال 23, 45, 67, 87, 54, 32, 11 ثم حذف 9, 36, 54)

::figure ds22-mway-q13

::code src="src/22-multiway-trees/exercises/ex22_01_multiway.c" region="RQ13"

### الأسئلة الموضوعية (مفتاح الكتاب Appendix G)

<table class="comp-table">
<tr><th>سؤال</th><th>الإجابة</th><th>السبب</th></tr>
<tr><td>MCQ 1</td><td>(b) M−1</td><td>عقدة M-way فيها M فرع و M−1 مفتاح</td></tr>
<tr><td>MCQ 2</td><td>(a) M</td><td>B-tree: أقصى M ابن</td></tr>
<tr><td>MCQ 3</td><td>(c) Trie</td><td>القاموس: prefix وبحث O(L)</td></tr>
<tr><td>MCQ 4</td><td>(c) Degree of node</td><td>M = أقصى فروع</td></tr>
<tr><td>MCQ 5</td><td>(c) O(log n)</td><td>أحسن حالة بحث في BST بتبقى شجرة متوازنة</td></tr>
<tr><td>T/F 1</td><td>صح</td><td>كل الأوراق في مستوى واحد (<code>bt_valid</code>)</td></tr>
<tr><td>T/F 2</td><td>صح (مفتاح الكتاب)</td><td>نص السؤال «stores data only in the i-nodes». المفتاح بيقول True؛ والمقصود إن البيانات في الأوراق بس وليس في الفهرس، فصياغة «i-nodes» الكتاب غامضة</td></tr>
<tr><td>T/F 3</td><td>خطأ</td><td>in-order بيطلع مرتب</td></tr>
<tr><td>T/F 4</td><td>خطأ</td><td>m ابن و m−1 مفتاح، مش m−1 ابن</td></tr>
<tr><td>T/F 5</td><td>خطأ</td><td>«leaf nodes of a B tree are often linked»: السلسلة خاصية B+ مش B (مفتاح الكتاب False)</td></tr>
<tr><td>T/F 6</td><td>صح</td><td>«B+ tree stores redundant search key»: مفتاح الفهرس نسخة من مفتاح ورقة</td></tr>
<tr><td>T/F 7</td><td>صح</td><td>«A trie is an ordered tree»: الأبناء بترتيب الحروف ولسه الـ in-order أبجدي (<code>all words, alphabetical</code>)</td></tr>
<tr><td>T/F 8</td><td>خطأ (مفتاح الكتاب)</td><td>«A trie uses more space than a BST»: الكتاب بيقول False. تحفظ: بتعتمد على الداتا؛ لـ 17785 كلمة مختلفة الـ trie محتاج 48623 عقدة بس (44% من عدد الحروف المخزنة)، لكن كل عقدة ممكن تشيل 26 مؤشر فالمساحة بالـ byte مش دايمًا أقل — شوف <code>compare</code></td></tr>
<tr><td>T/F 9</td><td>خطأ</td><td>«External nodes are called index nodes»: الـ external = الأوراق، والـ index nodes هي الداخلية</td></tr>
<tr><td>FIB 1</td><td>M ؛ M−1</td><td>M-way: مؤشرات M فرع و M−1 مفتاح</td></tr>
<tr><td>FIB 2</td><td>m ؛ m−1</td><td>أقصى m−1 مفتاح و m مؤشر</td></tr>
<tr><td>FIB 3</td><td>m/2 (⌈m/2⌉ بالتدقيق)</td><td>على الأقل ⌈m/2⌉ ابن</td></tr>
<tr><td>FIB 4</td><td>B tree</td><td>(مفتاح الكتاب) البيانات في العقد الداخلية والأوراق</td></tr>
<tr><td>FIB 5</td><td>O(log N)</td><td>الشجرة المتوازنة تضمن ده للبحث والإدخال والحذف</td></tr>
</table>

::code src="src/22-multiway-trees/exercises/ex22_01_multiway.c" region="objective"

## B&amp;D وP&amp;H: الحلول

التفاصيل الكاملة بالأشكال في الدرس قسم 3. هنا الكود اللي بيتأكد منهم:

::code src="src/22-multiway-trees/exercises/ex22_02_bd_ph_wb_btrees.c" region="BD1218"

::code src="src/22-multiway-trees/exercises/ex22_02_bd_ph_wb_btrees.c" region="PH851"

::code src="src/22-multiway-trees/exercises/ex22_02_bd_ph_wb_btrees.c" region="PH852"

::code src="src/22-multiway-trees/exercises/ex22_02_bd_ph_wb_btrees.c" region="PH854"

::code src="src/22-multiway-trees/exercises/ex22_02_bd_ph_wb_btrees.c" region="objective"

**P&amp;H Exercise 21(b) «Write short notes on B-tree»:** شجرة بحث متعددة الفروع متوازنة (order m: ≤ m ابن، ≥ ⌈m/2⌉ ابن لغير الجذر، كل الأوراق في مستوى واحد)، مصممة للـ disk (عقدة = block)، عمليات البحث/الإدخال/الحذف O(log n)، الإدخال بالتقسيم والنمو من الجذر، الحذف بالاستعارة أو الدمج؛ ومعاها B+ لسلسلة الأوراق (P&amp;H 12.7 بيقول نفس الكلام).

## Weiss: تمارين 4.41 و4.42

4\.41 (a) و(b) و(c) متنفذين في `wbtree.c` (الإدخال، الحذف بالـ adopt/combine، والـ adoption قبل التقسيم)، وإجابة سؤال «هل لازم نحدّث المعلومات في العقد الداخلية؟» في الدرس قسم 6. 4.42 (B\*-tree): وصف الطريقة في الدرس قسم 6 (ما فيش تنفيذ لأن السؤال «describe»).

::code src="src/22-multiway-trees/wbtree.c" region="fig463"

::code src="src/22-multiway-trees/wbtree.c" region="adopt"
