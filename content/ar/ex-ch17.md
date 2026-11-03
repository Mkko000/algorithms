تمارين محلولة · الفصل 17 (Arrays and Matrices) · Thareja ف3: Review Questions 1–15 + Programming Exercises 1–25 + الأسئلة المتعددة والصح/الغلط والفراغات

كل برنامج هنا اتكتب واتجرّب بـ `assert` على بيانات حقيقية في `src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c`. الشرح بيركّز على **ليه** الحل شغّال، والأخطاء الشائعة، وبعد الحل بتلاقي أسئلة الاختيار والصح/الغلط كلها بإجاباتها.

:::goal هدف الدرس:
تحل أي سؤال عناوين من غير ما تحفظ — بس تفتكر إن row-major بيضرب الصف في عدد الأعمدة — وتكتب عمليات المصفوفات بالـ pointers والـ indices بثقة.
:::

* [src/17-arrays-matrices/exercises/ex17\_01\_arrays\_matrices.c](src:src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c)

### أسئلة المراجعة 1–7 و 10–12 (نظرية)

:::solution
**1)** الـ array مجموعة عناصر من نفس النوع تحت اسم واحد بتتوصّل لها بـ index. محتاجينها عشان نتجنّب 1000 متغير منفصل ونقدر نلف بحلقة. **2)** عناصر ورا بعض في الذاكرة، عنوان العنصر i = Base + w·i. **3)** C بتعتبر المصفوفة 2-D «array من arrays»، وبتخزّنها row-major: صف ورا صف. **4)** تمثّل جداول (علامات طلبة × مواد) وصور (بكسلات) ومصفوفات رياضية وبيانات 3-D. **5-7)** الـ sparse matrix أغلبها أصفار، فتخزينها كمصفوفة عادية مضيّع ذاكرة ووقت؛ بنخزّن غير الصفري بس (triplets أو قوايم مترابطة) أو بمعادلة لو ليها شكل ثابت — شوف الدرس.

**6)** `a[i][j]` ≡ `*(*(a+i)+j)`. وهنا `a+i` بيتخطّى **صف كامل** (`int (*p)[C]` = pointer لـ array من C عناصر). **10)** اسم الـ array بيتحوّل لـ pointer لأول عنصر، و `a[i]` ≡ `*(a+i)`، والفرق الوحيد إن `sizeof(a)` بيدّي حجم الـ array كلها مش حجم pointer، وإن الاسم مش بيتغيّر (`a++` غلط). **11)** array of pointers: `char *names[3]` — كل خانة pointer، فتقدر تشاور على سلاسل بأطوال مختلفة (ragged). **12)** بنمرّر الاسم (يعني العنوان) + الحجم، فالدالة بتعدّل الـ array الأصلية.
:::

### سؤال 15: initializers أقل أو أكتر من الحجم

:::solution
(a) أقل: باقي العناصر بتتصفّر (`int a[5] = {1,2};` → 1 2 0 0 0) — وده كمان حل لتصفير المصفوفة كلها بـ `= {0}`. (b) أكتر: في C ده **constraint violation** — gcc بيطلّع `warning: excess elements in array initializer` ويتجاهل الزيادة (وفي C++ ده error).
:::

### أسئلة 8 و9 و13: حساب العناوين

:::think
اكتب المعادلة الأول وبعدين عوّض: row-major `Base + w·(i·C + j)` و column-major `Base + w·(j·R + i)`. في كل سؤال بنعرف R وC من تعريف المصفوفة.
:::

:::solution
**سؤال 8:** 1000 + 2·35 = **1070**.<br>**سؤال 9:** `Marks[10][5]` → C=5: 2000 + 2·(8·5 + 5) = **2090**. *تنبيه:* العمود 5 برّه الحدود (0..4)، فالعنوان ده هو في الحقيقة `Marks[9][0]`. المعادلة بتحسب، بس القراءة من هناك مش آمنة.<br>**سؤال 13:** `arr[10][10]` column-major: R=10، فـ 1000 + 2·(5·10 + 8) = **1116**.
:::

### سؤال 14: الإزاحة في إدخال وحذف أسماء

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="RQ14"

:::solution
الـ array فيها 7 أسماء (Adam, Charles, Dicken, Esha, Georgia, Hillary, Mishael). **Andrew** ترتيبه أبجديًا بعد Adam (pos=1)، فكل اللي من Charles لآخر الأسماء بيتحرّك: 7−1 = **6** → الاختيار (iv). حذف **Esha** (index 3): بيتحرّك اللي بعده: 7−3−1 = **3** → (i).
:::

### تمرين 1: إحصائيات علامات 20 طالب في 5 مواد

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE1"

:::think
متوسط المادة = مجموع **عمود** ÷ عدد الطلبة، ومتوسط الطالب = مجموع **صف** ÷ 5. الفخ: القسمة الصحيحة. لازم `double` قبل القسمة.
:::

:::solution
الدالة فوق بتحسب الأربعة (c) بيتحسب بنفس لفّة المتوسط (متوسط الطالب \< 50). (d) عرض الدرجات حلقتين متداخلتين. على بيانات الاختبار الناتج: `PE1: subject avgs 52.5 45.0 51.2 52.5 55.0; below-50 students = 2` (الطالبين صفوفهم 2 و4).
:::

### تمرين 2: كل الأزواج اللي مجموعها 50

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE2"

:::think
الطريقة الساذجة حلقتين = O(n²) لـ n=100. لو رتّبنا الأول، نمشي بمؤشرين من الطرفين: لو المجموع أصغر نحرّك الشمال، لو أكبر نحرّك اليمين. O(n log n) للترتيب + O(n) للمرور.
:::

:::solution
على المصفوفة `{10,40,25,30,20,15,35,0,50}`: الأزواج (0,50) (10,40) (15,35) (20,30) — أربعة بس، و25 معندهوش شريك (لأن ما فيش 25 تانية). الحل ده بيفترض قيم مش متكررة؛ لو متكررة تحتاج عدّاد لكل قيمة.
:::

### تمارين 3–10

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE3"

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE5"

:::solution
**3:** تبديل a\[1\] مع a\[n−2\] (لازم n ≥ 4 وإلا هما نفس العنصر). **4:** مجموع المربعات بـ `long` لأن `int` بيطفح بسرعة. **5:** mean = sum ÷ (r·c) بالـ double. **6:** قراءة وعرض مصفوفة بدوال: بنمرّر `int a[][N]` — لازم نكتب البُعد التاني عشان المترجم يعرف يحسب `i·N + j`. **7:** `*(*(a+i)+i)` بالـ pointers. **8:** جمع 3×3 بـ `int (*a)[3]`. **9:** حاصل ضرب الفوق-قطر `j > i` (ابدأ بـ 1 مش 0). **10:** عدّاد `a[i][j] != 0`.
:::

### تمرين 11: فصل الزوجي والفردي

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE11"

:::solution
`x & 1` أسرع طريقة تعرف بيها الفردي (وبتشتغل صح للسالب في تمثيل two's complement، عكس `x % 2 == 1` اللي بترجّع −1 للسالب الفردي).
:::

### تمرين 12: دمج مصفوفتين float وعرضهم بالعكس

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE12"

### تمرين 13: تبديل ثاني أكبر وثاني أصغر بالـ pointers

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE13"

:::think
مسح واحد بنحتفظ فيه بـ **pointer** لأكبر اتنين وأصغر اتنين (مش قيم) — فبعدها نبدّل القيم بـ `*max2` و`*min2` من غير بحث تاني. الفخ: لما يظهر عنصر جديد أكبر، الأكبر القديم بيبقى هو التاني.
:::

### تمرين 14: مصفوفات p×q×r

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE14"

:::solution
الجمع: عنصر بعنصر. الـ transpose لمصفوفة 3-D بنبدّل آخر index‑ين (`t[i][k][j] = a[i][j][k]`) لكل شريحة. «الضرب» في الكتاب غير معرّف لـ 3-D، فاخترنا التفسير المعتاد: ضرب شريحة شريحة (batch) لـ A (q×r) في B<sup>T</sup> (r×q) فيطلع q×q لكل شريحة.
:::

### تمارين 15–20: القطر والمثلثات والتماثل

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE15"

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE18"

:::oops تصحيح تلميحة في الكتاب
تلميح Thareja بيقول «upper triangular لو A<sub>ij</sub> = 0 و i \> j» — ده معناه «**لكل** i\>j يبقى A<sub>ij</sub> = 0». يعني كل اللي **تحت** القطر أصفار. والكود بيختبر `j < i` بالظبط كده.
:::

### تمرين 21–25

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE21"

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE22"

:::solution
`int (*p)[3] = a;` الأقواس ضرورية: بدونها `int *p[3]` = array من 3 pointers. `p+1` بيقفز 3·sizeof(int) بايت = صف كامل، والـ assert في الكود بيثبّت ده.
:::

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE23"

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE24"

:::think
الحذف «بالإزاحة» لكل تكرار O(n²) وبيعدّل n مرات. طريقة **write-index**: مؤشر قراءة r ومؤشر كتابة w؛ بنكتب العنصر بس لو ما ظهرش قبل كده في `a[0..w)`. مرور واحد على القراءة، والمقارنة جوه الجزء المكتوب بس.
:::

::code src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" region="PE25"

:::solution
نفس إدخال الدرس بـ `memmove` (مهم: `memmove` مش `memcpy` لأن المنطقتين متداخلتين). بنتأكد إن `n < cap` و`0 ≤ pos ≤ n`.
:::

### تشغيل ملف التمارين

::output src="src/17-arrays-matrices/exercises/ex17_01_arrays_matrices.c" lines="10"

### الأسئلة المتعددة (Multiple-choice)

:::solution
<table class="comp-table">
<tr><th>#</th><th>الإجابة</th><th>ليه</th></tr>
<tr><td>1</td><td>الكتاب: (b) 2؛ دلوقتي <b>4</b></td><td><code>sizeof(int)</code> = 2 على Turbo C القديم، و4 على أي نظام حديث. الإجابة الأدق إنها <code>sizeof(int)</code> وملهاش علاقة بالـ index.</td></tr>
<tr><td>2</td><td>(b) 7</td><td>arr[3] هو الرابع: 1,3,5,<b>7</b>.</td></tr>
<tr><td>3</td><td>(d) 400</td><td>50 × sizeof(double)=8.</td></tr>
<tr><td>4</td><td>(b) 50</td><td>index من 0 لـ 49.</td></tr>
<tr><td>5</td><td>(b) 25</td><td>5×5.</td></tr>
<tr><td>6</td><td>(d) الكل</td><td><code>*(arr+i)</code> ≡ <code>*(i+arr)</code> ≡ <code>arr[i]</code> (الجمع تبادلي، فحتى <code>i[arr]</code> شغّالة!).</td></tr>
</table>
:::

### صح أو غلط

:::solution
<table class="comp-table">
<tr><th>#</th><th>الإجابة</th><th>#</th><th>الإجابة</th></tr>
<tr><td>1</td><td>صح</td><td>9</td><td>غلط — عنوان <b>أول</b> عنصر</td></tr>
<tr><td>2</td><td>صح</td><td>10</td><td>صح</td></tr>
<tr><td>3</td><td>صح</td><td>11</td><td>صح</td></tr>
<tr><td>4</td><td>غلط — متتالية</td><td>12</td><td>غلط — بيتمرّر العنوان</td></tr>
<tr><td>5</td><td>غلط — دي <b>أول</b> index</td><td>13</td><td>غلط — نفس النوع</td></tr>
<tr><td>6</td><td>صح</td><td>14</td><td>غلط — مفيش حد أقصى (غير حدود الذاكرة)</td></tr>
<tr><td>7</td><td>صح</td><td>15</td><td>صح</td></tr>
<tr><td>8</td><td>صح</td><td></td><td></td></tr>
</table>
:::

### املا الفراغ

:::solution
1\) subscript (index) · 2) متتالية (consecutive) · 3) n · 4) pointer ثابت لأول عنصر · 5) النوع، الاسم، الحجم · 6) base address · 7) العناصر · 8) array of arrays · 9) integer · 10) الـ **رابع** (index 3).
:::
