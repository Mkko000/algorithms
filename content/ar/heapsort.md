أقسام الكتاب: 9.1 Elementary Implementations (Definition 9.1, Programs 9.1–9.2, Table 9.1) · 9.2 Heap Data Structure (Definitions 9.2–9.3, Property 9.1) · 9.3 Algorithms on Heaps (Programs 9.3–9.5, Properties 9.2–9.3) · 9.4 Heapsort (Programs 9.6–9.7, Properties 9.4–9.6)

طوارئ المستشفى مش طابور FIFO: اللي جاي بأزمة قلبية يدخل قبل اللي جاي بصداع حتى لو الصداع وصل الأول. ده **priority queue**: كل مرة عايز «الأهم» مش «الأقدم». والسؤال: إزاي تخلي «ادخل» و«هات الأهم» الاتنين سريعين؟ الإجابة هيكل ظريف جدًا اسمه الـ heap — ومنه بيطلع ترتيب في المكان مضمون N log N.

:::goal هدف الدرس:
تعرف الـ PQ كـ ADT وتنفيذاته البسيطة وتكاليفها، تفهم الـ heap وتمثيله في مصفوفة، تكتب fixUp وfixDown وتحلّلهم، وتكتب heapsort وتعرف ليه بناء الـ heap خطي.
:::

## 9.1 التنفيذات البسيطة

**Definition 9.1:** priority queue = هيكل عناصر بمفاتيح بيدعم عمليتين أساسيتين: **insert** عنصر جديد، و**delete the maximum** (شيل الأكبر). وفيه عمليات تانية مفيدة: construct من N عنصر، replace the maximum، change priority، delete عنصر معيّن، و**join** اتنين PQ.

```c listing="Program 9.1"
/* Program 9.1 */
void PQinit(int);  int PQempty(void);
void PQinsert(Item); Item PQdelmax(void);
```

أبسط تنفيذين:

* **«كسول» — مصفوفة مش مترتبة (Program 9.2):** insert = حط في الآخر O(1). delmax = دوّر على الأكبر O(N)، بدّله مع الآخر وشيله.
* **«مجتهد» — مصفوفة مترتبة:** insert = زي insertion sort O(N). delmax = آخر عنصر O(1).

<table class="comp-table">
<tr><th>Table 9.1</th><th>insert</th><th>delmax</th><th>delete</th><th>find max</th><th>change</th><th>join</th></tr>
<tr><td>ordered array</td><td>N</td><td>1</td><td>N</td><td>1</td><td>N</td><td>N</td></tr>
<tr><td>ordered list</td><td>N</td><td>1</td><td>1</td><td>1</td><td>N</td><td>N</td></tr>
<tr><td>unordered array</td><td>1</td><td>N</td><td>1</td><td>N</td><td>1</td><td>N</td></tr>
<tr><td>unordered list</td><td>1</td><td>N</td><td>1</td><td>N</td><td>1</td><td>1</td></tr>
<tr><td><b>heap</b></td><td>lg N</td><td>lg N</td><td>lg N</td><td>1</td><td>lg N</td><td>N</td></tr>
<tr><td>binomial queue</td><td>lg N</td><td>lg N</td><td>lg N</td><td>lg N</td><td>lg N</td><td>lg N</td></tr>
</table>

كل تنفيذ بسيط عنده عملية واحدة على الأقل O(N). الـ heap بيوازن: كل حاجة lg N. والـ binomial queue (الدرس 22) بيحل مشكلة الـ join.

* [src/09-heapsort/pq\_elementary.c](src:src/09-heapsort/pq_elementary.c)

## 9.2 الـ Heap

**Definition 9.2:** شجرة **heap-ordered** = مفتاح كل عقدة ≥ مفاتيح أبنائها. **Property 9.1:** في شجرة زي دي، مفيش عقدة مفتاحها أكبر من الجذر — الأكبر دايمًا فوق.

**Definition 9.3:** الـ **heap** = شجرة ثنائية **كاملة** heap-ordered، متخزّنة في مصفوفة بترتيب المستويات. «كاملة» يعني كل المستويات مليانة ما عدا الأخير، والأخير مليان من الشمال. وده بيدّي تمثيل من غير مؤشرات خالص:

```
a[1] = الجذر
أبو a[k]   = a[k/2]
أبناء a[k] = a[2k] و a[2k+1]
```

والارتفاع بالظبط ⌊lg N⌋ — الشجرة الكاملة مستحيل تبقى غير متوازنة. ده سر كل حاجة جاية.

::trace heap

## 9.3 الخوارزميات على الـ heap

أي عملية بتبوّظ الخاصية في نقطة واحدة، وبنصلّحها بطريقة من اتنين:

### fixUp — لو عقدة كبرت (Program 9.3)

زي موظف اترقّى: يفضل يتبادل مع مديره لحد ما يبقى مديره أكبر منه.

```c listing="Program 9.3"
fixUp(Item a[], int k)
  { while (k > 1 && less(a[k/2], a[k]))
      { exch(a[k], a[k/2]); k = k/2; } }
```

::figure heap-insert

### fixDown — لو عقدة صغرت (Program 9.4)

بالعكس: مدير اتنزّل. يتبادل مع **الأكبر** من ابنيه لحد ما يبقى أكبر من الاتنين.

```c listing="Program 9.4"
fixDown(Item a[], int k, int N)
  { int j;
    while (2*k <= N)
      { j = 2*k;
        if (j < N && less(a[j], a[j+1])) j++;
        if (!less(a[k], a[j])) break;
        exch(a[k], a[j]); k = j; } }
```

### الـ PQ (Program 9.5)

```c
void PQinsert(Item v) { pq[++N] = v; fixUp(pq, N); }
Item PQdelmax()
  { exch(pq[1], pq[N]);
    fixDown(pq, 1, N-1);
    return pq[N--]; }
```

* **Property 9.2:** insert ≤ lg N مقارنة (مقارنة لكل مستوى طالع)، delmax ≤ 2 lg N (مقارنتين لكل مستوى نازل: بين الابنين، وبين الأكبر والعقدة).
* **Property 9.3:** change priority وdelete وreplace the maximum ≤ 2 lg N برضه — بس محتاجين «handle» للعنصر، فدول في الدرس 22.

```
N=100000, lg N=16.6
worst insert 16 compares (<= lg N), worst delmax 32 (<= 2 lg N)
```

* [src/09-heapsort/heap.c](src:src/09-heapsort/heap.c)

## 9.4 Heapsort

### أول فكرة: رتّب بـ PQ (Program 9.6)

حط الكل في PQ وطلّعهم بالترتيب. شغال، بس محتاج مصفوفة تانية، والبناء بـ N insert ممكن ياخد N lg N.

### Heapsort الحقيقي (Program 9.7)

ملاحظتين بيحوّلوا الفكرة لخوارزمية حقيقية: (1) المصفوفة نفسها ممكن تبقى الـ heap، (2) البناء أسرع من تحت لفوق: كل عقدة من النص للأول، اعمل fixDown. الأوراق (نص المصفوفة) heaps جاهزة أصلًا.

```c listing="Program 9.7"
void heapsort(Item a[], int l, int r)
  { int k, N = r-l+1;
    Item *pq = a+l-1;
    for (k = N/2; k >= 1; k--)
      fixDown(pq, k, N);
    while (N > 1)
      { exch(pq[1], pq[N]);
        fixDown(pq, 1, --N); } }
```

المرحلة التانية: بدّل الجذر (الأكبر) مع آخر عنصر، صغّر الـ heap، fixDown. الأكبر بيتراكم في آخر المصفوفة بالترتيب. لاحظ الحيلة `pq = a+l-1` عشان `pq[1]` يبقى `a[l]` ونحتفظ بالفهرسة من 1.

:::tip ليه الريبو مش بيستخدم a+l-1؟
الكتاب بيكتب `pq = a+l-1` عشان `pq[1]` يبقى `a[l]`. المشكلة إن لما `l` تبقى 0، المؤشر ده بيشاور على خانة قبل أول المصفوفة، ولغة C مش بتسمح بكده حتى لو عمرنا ما قرينا الخانة دي. فالـ `heap.c` اللي في الريبو بيحتفظ بنفس الفهرسة من 1، بس بيحسبها كـ offset: `H(k)` معناها `a[l + k - 1]`. نفس الخوارزمية بالظبط، من غير المؤشر ده.
:::

::code src="src/09-heapsort/heap.c" fn="H"

::code src="src/09-heapsort/heap.c" fn="heapsort"

::figure heapsort-steps

<table class="comp-table">
<tr><th>Property</th><th>النص</th></tr>
<tr><td>9.4</td><td>بناء الـ heap من تحت لفوق <b>خطي</b>. معظم الـ heaps صغيرة: N/4 عقدة ارتفاعها 1، N/8 ارتفاعها 2… المجموع &lt; 2N مقارنة.</td></tr>
<tr><td>9.5</td><td>heapsort بيستخدم أقل من <b>2N lg N</b> مقارنة. وفي المكان. وده ضمان، مش متوسط.</td></tr>
<tr><td>9.6</td><td>أكبر k من N: خطي لو k صغير أو قريب من N (ابني الـ heap في 2N، شيل k في 2k lg N)، وN log N غير كده.</td></tr>
</table>

```
build by 100000 inserts :   228790 compares (random keys)
build by 100000 inserts :  1468946 compares (increasing keys: ~N lg N)
build bottom-up    :   188340 compares (< 2N = 200000)
PQsort (9.6)       :  3060761 compares
heapsort (9.7)     :  3019894 compares (< 2N lg N = 3321928)
heapsort, Floyd    :  1723081 compares (about N lg N = 1660964)
10 largest: ...  using 188658 compares (2N + 2k lg N = 200332)
```

السطر «Floyd» تحسين بيذكره الكتاب ومنسوب لـ Floyd: في الـ delmax، العنصر اللي جه من الآخر صغير وغالبًا هينزل لتحت خالص. فبدل مقارنتين في كل مستوى، انزل بالـ «حفرة» لآخر الشجرة بمقارنة واحدة لكل مستوى (أكبر ابن يطلع)، وبعدين حط العنصر في الآخر واعمله fixUp — اللي غالبًا بيطلع خطوة أو اتنين. المقارنات بتقل للنص تقريبًا.

:::tip Heapsort ضد quicksort ضد mergesort
heapsort هو الوحيد اللي **في المكان** و**مضمون** N log N مع بعض. بس عمليًا أبطأ من quicksort: الـ loop الداخلي أطول، والوصول لـ a\[2k\] بيقفز في الذاكرة فبيضرب الكاش. ومش مستقر. الكتاب بيقول: استخدمه لما الضمان والمساحة مهمين مع بعض.
:::

:::oops الفهرسة من 0
لو بدأت من 0، الأبناء بقوا 2k+1 و2k+2 والأب (k−1)/2. ناس كتير بتخلط الصيغتين. سادجوك بيستخدم الفهرسة من 1 وحيلة `a+l-1` (والريبو بيعمل نفس الفهرسة بـ offset بدل المؤشر ده)؛ اختار واحدة والتزم.
:::

:::quiz
heap فيه مليون عنصر. أقصى عدد مقارنات لـ delmax تقريبًا؟

1. [ ] 20
2. [x] 40
3. [ ] مليون

> Property 9.2: 2 lg N ≈ 2 × 20.
:::

:::quiz
ليه بناء الـ heap من تحت لفوق خطي، رغم إن كل fixDown ممكن ياخد lg N؟

1. [ ] لأن fixDown خطي
2. [x] لأن أغلب العقد قريبة من الأوراق فالـ fixDown بتاعها قصير جدًا
3. [ ] مش خطي، هو N log N

> نص العقد مش بتعمل حاجة، ربعها مستوى واحد، تمنها اتنين… المجموع \< 2N.
:::

:::quiz
عايز أكبر 5 عناصر من 10 مليون. heapsort الكامل ولا حاجة أذكى؟

1. [x] ابني heap (2N) وطلّع 5 بس
2. [ ] لازم heapsort كامل
3. [ ] insertion sort

> Property 9.6: خطي لما k صغير.
:::

:::quiz
أنهي تنفيذ PQ بسيط ممتاز لو الـ inserts كتير جدًا والـ delmax نادر؟

1. [x] مصفوفة مش مترتبة
2. [ ] مصفوفة مترتبة
3. [ ] heap دايمًا

> insert O(1) والتمن بيتدفع بس في delmax النادر. الاختيار حسب نسبة العمليات.
:::

:::quiz
heapsort مستقر؟

1. [ ] أيوه
2. [x] لأ
3. [ ] بس لو المفاتيح مختلفة

> التبديل بين الجذر والآخر بيحرك العناصر مسافات بعيدة، فالمتساويين ممكن يتقلبوا.
:::
