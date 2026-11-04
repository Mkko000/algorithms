أقسام الكتاب: 16.4 Extendible Hashing (Programs 16.5–16.8، Definition 16.3، Properties 16.4–16.5) · 16.5 Perspective

الـ B tree بيدّي log<sub>M</sub> N probe — 3 أو 4. طب ينفع نعمل أحسن؟ الـ hashing في الذاكرة بيدّي probe واحد تقريبًا. المشكلة إن الـ hash table لما بيكبر لازم يتعاد بناؤه كله (rehash)، وده على الديسك مستحيل لملف ضخم. الـ **extendible hashing** (Fagin وNievergelt وPippenger وStrong، 1979) بيجمع الاتنين: **2 probe** لأي بحث، ومفيش rehash كامل أبدًا.

:::goal هدف الدرس:
تفهم الـ directory والـ pages والعلاقة بالـ tries، تتبّع الـ split ومضاعفة الـ directory خطوة بخطوة، تحسب حجم الـ directory والصفحات، وتطلع بخريطة كاملة لكل طرق البحث في الكتاب.
:::

## 16.4 Extendible Hashing

### الفكرة

خد أول d بت من الـ hash بتاع المفتاح، واستخدمهم index في **directory** فيه 2<sup>d</sup> خانة. كل خانة بتشاور على page فيها لحد M item. البحث: probe للـ directory (أو هو في الذاكرة أصلًا) + probe للـ page. خلاص.

:::problem
الـ extendible hash table من رتبة d هو directory فيه 2<sup>d</sup> reference لـ pages فيها لحد M item بمفاتيح. الـ items في كل page متطابقين في أول k بت، والـ directory فيه 2<sup>d−k</sup> pointer للـ page دي، بيبدأوا من الخانة اللي أول k بت فيها هم البتات دول.
:::

يعني page ممكن «يشاور عليها» أكتر من خانة. الـ k ده اسمه **local depth** للـ page، وd هو الـ global depth. لو k \< d، الـ page بتغطّي 2<sup>d−k</sup> خانة متتالية.

### الـ insert: split أو مضاعفة (Programs 16.6–16.8)

1. احسب أول d بت، روح للـ page، حط الـ item لو فيه مكان.
2. لو الـ page مليانة وk \< d: **قسمها** على البت رقم k+1. اعمل page جديدة، وزّع الـ items على حسب البت ده، وخلّي نص الخانات اللي كانت بتشاور على القديمة تشاور على الجديدة. باقي الـ directory ماتغيّرش!
3. لو الـ page مليانة وk = d: لازم الأول **نضاعف الـ directory** (d يزيد واحد، كل خانة تتنسخ مرتين)، وبعدين نقسم.

::figure extendible

اتفرّج على الـ stepper: نفس المفاتيح الأوكتال بتاعة الكتاب بـ M = 4. لاحظ إن معظم الـ inserts بتدخل على طول، وإن الـ split بيلمس page واحدة بس، وإن مضاعفة الـ directory بتحصل نادرًا وبتنسخ pointers بس (مش بيانات).

:::problem
الـ extendible hash table المبني من مجموعة مفاتيح بيعتمد على قيم المفاتيح بس، مش على ترتيب إدخالها.
:::

ليه؟ لأنه في الحقيقة **trie** (الدرس 35)! كل page = شجرة فرعية في الـ trie المبني على البتات، متقطوعة عند المستوى اللي فيه ≤ M مفتاح. والـ directory هو الـ trie ده «متسطّح» لمصفوفة بعمق d. والـ trie مستقل عن الترتيب (Property 15.2)، فالـ hash table كمان.

:::problem
بصفحات بتشيل M item، الـ extendible hashing بيحتاج حوالي **1.44 (N/M)** صفحة لملف فيه N item في المتوسط. والعدد المتوقع للخانات في الـ directory حوالي **3.92 (N<sup>1/M</sup>)(N/M)**.
:::

```
N=200000, M=32: directory depth d=14 (16384 entries), 8845 pages, 70.7% average fill
Property 16.5 predicts ~1.44 N/M = 9000 pages
10000 hits: 10000 found; 10000 misses: 10000 not found; page reads per search: 1.00
```

8845 صفحة مقابل 9000 متوقعة، ونفس الـ 69–71% امتلاء زي الـ B tree (مش صدفة: الاتنين بيعملوا split نصّين). وكل بحث = قراية page واحدة (والـ directory في الذاكرة). والـ directory؟ الصيغة بتدّي 3.92 × 200000<sup>1/32</sup> × 6250. لاحظ إن N<sup>1/M</sup> هنا ≈ 1.46 (لأن M = 32 صغيرة نسبيًا)، والكتاب بيقول إن في المدى العملي N<sup>1/M</sup> قريبة من 1 فالـ directory عمليًا حوالي **4(N/M)** خانة. وكمان الحجم لازم يبقى قوة لـ 2، فالقيمة الحقيقية بتتذبذب حوالين المتوسط ده (الكتاب بيقول كده صراحة). إحنا طلع معانا 2<sup>14</sup> = 16384 — يعني الـ directory أكبر من عدد الصفحات بحوالي الضعف، وكل pointer بس 8 بايت، فده رخيص.

* [src/16-external-search/extendible\_hash.c](src:src/16-external-search/extendible_hash.c)

:::oops نقطة الضعف
لو المفاتيح (أو الـ hash بتاعها) مش عشوائية — مثلًا M+1 مفتاح بيشتركوا في أول 20 بت — الـ directory هيتضاعف 20 مرة عشان يفصلهم! حجم الـ directory ممكن ينفجر. عشان كده لازم hash function كويسة، وفيه نسخ بتحط حد أقصى لـ d وتستخدم overflow pages لو حصل كده.
:::

## 16.5 Perspective

أهم تطبيق لطرق الفصل ده: indexes لداتابيز ضخمة على الديسك. بس سادجوك بيحذّر: تحويل الكود ده لـ file system حقيقي شغل كبير — لازم تقرا وتكتب من الديسك فعلًا، تظبط البارامترات (حجم الـ page والـ directory) على الهاردوير بالظبط، وتهتم بالـ reliability (لو الكهربا قطعت في نص split؟). كل ده برّه نطاق الكتاب.

بس لو عندك **virtual memory**، الكود زي ما هو بيشتغل: الـ page miss في الـ cache تقريبًا = probe. ومع الـ B tree، الجذر دايمًا في الـ cache، فالبحث العادي بيعمل 1–2 miss. ومع الـ extendible hashing، الـ directory غالبًا مش كله في الـ cache، فأسوأ حالة 2 miss (directory + page).

<table class="comp-table">
<tr><th></th><th>ISAM</th><th>B tree</th><th>Extendible hashing</th></tr>
<tr><td>probes للبحث</td><td>ثابت (بيزيد مع الـ overflow)</td><td>log<sub>M</sub> N (3–4)، 1–2 عمليًا</td><td>2 (أو 1 لو الـ directory في الذاكرة)</td></tr>
<tr><td>insert</td><td>overflow chains</td><td>split نادر</td><td>split نادر + مضاعفة نادرة</td></tr>
<tr><td>امتلاء الصفحات</td><td>100% في الأول</td><td>~69%</td><td>~69%</td></tr>
<tr><td>ترتيب / range search</td><td>أيوه</td><td>أيوه</td><td>لأ</td></tr>
<tr><td>حساسية للمفاتيح</td><td>لأ</td><td>لأ</td><td>محتاج hash كويس</td></tr>
</table>

### الخريطة الكاملة للبحث (الفصول 12–16)

<table class="comp-table">
<tr><th>الطريقة</th><th>worst case</th><th>متوسط search hit</th><th>ترتيب؟</th><th>أنسب لـ</th></tr>
<tr><td>key-indexed array</td><td>1</td><td>1</td><td>أيوه</td><td>مفاتيح أرقام صغيرة</td></tr>
<tr><td>binary search</td><td>lg N</td><td>lg N</td><td>أيوه</td><td>جدول ثابت</td></tr>
<tr><td>BST</td><td>N</td><td>1.39 lg N</td><td>أيوه</td><td>مفاتيح عشوائية، كود بسيط</td></tr>
<tr><td>randomized / skip list</td><td>N (احتمال ضئيل)</td><td>~1.39 lg N</td><td>أيوه</td><td>ضمان احتمالي، select/join</td></tr>
<tr><td>red-black</td><td>2 lg N</td><td>~lg N</td><td>أيوه</td><td>الاختيار العام في الذاكرة</td></tr>
<tr><td>hashing</td><td>N (حظ وحش)</td><td>1–2 probe</td><td>لأ</td><td>أسرع بحث لو مش محتاج ترتيب</td></tr>
<tr><td>patricia / TST</td><td>طول المفتاح</td><td>~lg N بت / ~ln N حرف</td><td>أيوه</td><td>نصوص، prefixes، مفاتيح طويلة</td></tr>
<tr><td>B tree</td><td>log<sub>M/2</sub> N probe</td><td>~log<sub>M</sub> N probe</td><td>أيوه</td><td>بيانات على الديسك</td></tr>
<tr><td>extendible hashing</td><td>2 probe</td><td>2 probe</td><td>لأ</td><td>ديسك، بحث بالمساواة بس</td></tr>
</table>

والجملة اللي سادجوك بيقفل بيها الجزء ده: عشان تستخدم الطرق دي صح، لازم تفهم الأساسيات كلها — binary search، الـ BSTs، الشجر المتوازن، الـ hashing، والـ tries. مع بعض، دول حل لمسألة الـ symbol table في كل موقف تقريبًا، ومثال عظيم على قوة «التكنولوجيا الخوارزمية».

:::quiz
extendible hash بـ d = 10 وpage عمقها k = 7. كام خانة في الـ directory بتشاور عليها؟

1. [ ] 1
2. [ ] 7
3. [x] 8
4. [ ] 1024

> 2<sup>d−k</sup> = 2³ = 8 خانات متتالية (Definition 16.3). كلهم بيشتركوا في أول 7 بت.
:::

:::quiz
page مليانة وعمقها k = d. إيه اللي لازم يحصل قبل الـ split؟

1. [ ] rehash لكل الملف
2. [x] مضاعفة الـ directory (d يزيد واحد)
3. [ ] overflow page

> لو k = d، الـ page بتشاور عليها خانة واحدة بس، فمفيش «نص» نديه للـ page الجديدة. نضاعف الـ directory (نسخ pointers بس)، وبعدين نقسم.
:::

:::quiz
إيه العلاقة بين extendible hashing والـ tries؟

1. [ ] مفيش علاقة
2. [x] الـ directory هو trie على البتات متسطّح لمصفوفة، وكل page = شجرة فرعية فيها ≤ M مفتاح
3. [ ] الاتنين بيستخدموا hash functions

> وده سبب Property 16.4: الشكل مستقل عن ترتيب الإدخال، زي الـ trie بالظبط (Property 15.2).
:::

:::quiz
داتابيز على الديسك، ومحتاج «كل العملاء اللي اسمهم بين أحمد ومحمد». تختار؟

1. [ ] extendible hashing
2. [x] B tree
3. [ ] ISAM مع overflow كتير

> الـ range search محتاج ترتيب، والـ hashing مابيحافظش عليه. الـ B tree (خصوصًا لو الأوراق مربوطة ببعض) بيعمله بكفاءة.
:::
