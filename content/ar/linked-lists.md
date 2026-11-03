أقسام الكتاب: 3.3 Linked Lists (Definitions 3.2–3.3, Program 3.9) · 3.4 Elementary List Processing (Programs 3.10–3.14, Table 3.1) · 3.5 Memory Allocation for Lists

المصفوفة بتديك a\[i\] في خطوة، بس لو عايز تزق عنصر في النص لازم تزحزح الباقي. القائمة المترابطة بتقلب الصفقة: **الإدراج والحذف بيبقوا تعديل مؤشرين**، بس عشان توصل للعنصر رقم 500 لازم تمشي 500 خطوة. اختيار بينهم = اختيار أنهي عملية هتتكرر أكتر.

:::goal هدف الدرس:
تكتب قوائم بإيدك من غير ما تكسرها: الإدراج والحذف، القايمة الدائرية (Josephus)، العكس، الـ insertion sort على قايمة، القوائم المزدوجة، اصطلاحات الـ head والـ tail، وإزاي تدير الذاكرة بنفسك بـ free list.
:::

## 3.3 القائمة المترابطة

**Definition 3.2:** مجموعة عناصر، كل عنصر جوّه «عقدة» فيها كمان مؤشر لعقدة. وتعريف أضيق (3.3): القائمة إما فاضية (null) أو مؤشر لعقدة فيها عنصر ومؤشر لقائمة. التعريف التاني ريكيرسف — وهيخلّي الدوال الريكيرسف على القوائم طبيعية جدًا (الدرس 11).

```c
typedef struct node *link;
struct node { Item item; link next; };

/* insert t after x */          /* delete the node after x */
t->next = x->next;               t = x->next;
x->next = t;                     x->next = t->next;  free(t);
```

لاحظ ترتيب السطرين في الإدراج: لو عكستهم، `x->next` هيضيع قبل ما تنسخه، وباقي القائمة يتقطع ويتسرّب.

::figure list-insert

### القايمة الدائرية: Josephus (Program 3.9)

N واحد في دايرة، كل M-th بيخرج لحد ما يفضل واحد. آخر عقدة بتشاور على الأولى، فالعدّ بيلف لوحده، والحذف = `x->next = x->next->next`. لـ N=9 و M=5 الناجي هو 8. ولـ 41 و 3 (القصة الأصلية) الناجي 31.

```c listing="Program 3.9"
while (x != x->next)
  { for (i = 1; i < M; i++) x = x->next;
    x->next = x->next->next; N--; }
printf("%d\n", x->item);
```

* [src/03-data-structures/josephus.c](src:src/03-data-structures/josephus.c)

::figure josephus

## 3.4 معالجة القوائم الأساسية

### عكس قايمة (Program 3.10)

تلات مؤشرات: `r` (اللي اتعكس لحد دلوقتي)، `y` (اللي جاي)، و`x` (الحالي). في كل خطوة بنقلب سهم واحد.

```c listing="Program 3.10"
link reverse(link x)
  { link t, y = x, r = NULL;
    while (y != NULL)
      { t = y->next; y->next = r; r = y; y = t; }
    return r; }
```

::figure list-reverse

### Insertion sort على قايمة (Program 3.11)

اسحب العقد من القايمة الأصلية واحدة واحدة، وادخل كل واحدة مكانها في قايمة ناتج مترتّبة (ليها head وهمي). مفيش عنصر بيتنقل في الذاكرة — بس مؤشرات بتتغيّر.

* [src/03-data-structures/linked\_list.c](src:src/03-data-structures/linked_list.c)

### اصطلاحات الـ head والـ tail (Table 3.1)

<table class="comp-table">
<tr><th>الاصطلاح</th><th>القايمة الفاضية</th><th>الميزة / العيب</th></tr>
<tr><td>دائرية، مش فاضية أبدًا</td><td>—</td><td>Josephus؛ لازم أول عقدة تتعامل لوحدها</td></tr>
<tr><td>مؤشر head، والآخر NULL</td><td><code>head = NULL</code></td><td>الأبسط، بس الإدراج في الأول حالة خاصة</td></tr>
<tr><td>head وهمي (dummy)، الآخر NULL</td><td><code>head-&gt;next = NULL</code></td><td>كل إدراج «بعد عقدة» — مفيش حالات خاصة</td></tr>
<tr><td>head وهمي و tail وهمي</td><td><code>head-&gt;next = z</code></td><td>الحذف والإدراج في الطرفين موحّدين</td></tr>
</table>

مافيش اصطلاح «صح»؛ اختار واحد والتزم بيه في البرنامج كله. أغلب الـ bugs بتيجي من خلط اتنين.

### القوائم المزدوجة (doubly linked)

كل عقدة فيها `prev` و `next`. التمن: مؤشر زيادة وتحديث 4 مؤشرات في كل إدراج. المكسب: تقدر تحذف عقدة وإنت ماسك مؤشر ليها **هي نفسها** (مش اللي قبلها)، وتمشي في الاتجاهين. ده بالظبط اللي هنحتاجه في الـ priority queue بالـ handles (الدرس 22).

```c
/* delete t, O(1), no search */
t->prev->next = t->next;
t->next->prev = t->prev;  free(t);
```

* [src/03-data-structures/doubly\_linked.c](src:src/03-data-structures/doubly_linked.c)

### واجهة صناديق سودا (Program 3.12)

سادجوك بيعمل خطوة مهمة: يحط عمليات القايمة ورا واجهة — `newNode`، `freeNode`، `insertNext`، `deleteNext`، `Next`، `Item` — والعميل (Josephus في Program 3.13) مايلمسش المؤشرات بنفسه. ده تمهيد مباشر للـ ADTs.

## 3.5 إدارة الذاكرة بنفسك: الـ free list

`malloc` و `free` عموميين وبطيئين نسبيًا. لو برنامجك بيخلق ويمسح عقد من نفس الحجم طول الوقت، ممكن تعمل أحسن: **احجز كل العقد مرة واحدة**، وحط اللي مش مستخدم في قايمة «فاضية» (free list). `newNode` = شيل من أول الـ free list، `freeNode` = رجّعها في أولها. الاتنين O(1) ومن غير أي نداء للنظام (Program 3.14).

```c listing="Program 3.14"
void initNodes(int N)
  { freelist = malloc((N+1)*(sizeof *freelist));
    for (i = 0; i < N+1; i++) freelist[i].next = &freelist[i+1];
    freelist[N].next = NULL; }
link newNode(int i)  { link x = deleteNext(freelist); x->item = i; x->next = x; return x; }
void freeNode(link x) { insertNext(freelist, x); }
```

نفس الفكرة تقدر تعملها بـ **فهارس في مصفوفة** بدل مؤشرات (cursor implementation): `next[i]` رقم الخانة الجاية. ملفنا عامل الاتنين وبيتأكد إن Josephus بيدّي نفس الناجي.

* [src/03-data-structures/list\_memory.c](src:src/03-data-structures/list_memory.c)

:::problem
الكتاب بيلمّح إن المخصّص الحقيقي (`malloc`) لازم يتعامل مع أحجام مختلفة وتفتّت، وإن لغات تانية عندها garbage collection بيلم العقد اللي محدش بيشاور عليها. الـ free list هو أبسط نسخة من الشغل ده — وبتفهّمك ليه الـ memory leaks بتحصل: عقدة مش في القايمة ومش في الـ free list = ضاعت.
:::

:::oops أشهر 3 bugs في القوائم
(1) ترتيب سطرين الإدراج معكوس → القايمة تتقطع. (2) تستخدم `t->next` بعد `free(t)`. (3) تنسى حالة القايمة الفاضية أو العقدة الأخيرة — عشان كده الـ dummy nodes موجودة.
:::

:::quiz
عايز تضيف عناصر كتير في نص قايمة طويلة. أنهي هيكل أريح؟

1. [ ] مصفوفة
2. [x] قائمة مترابطة
3. [ ] الاتنين نفس التكلفة

> الإدراج = تعديل مؤشرين O(1) (لو معاك مكانه). في المصفوفة إزاحة O(N).
:::

:::quiz
الكود ده بيعمل إيه غلط؟ `x->next = t; t->next = x->next;`

1. [ ] مفيش غلط
2. [x] بعد السطر الأول x-\>next بقى t، فـ t بيشاور على نفسه وباقي القايمة ضاع
3. [ ] بيعمل memory leak لـ t بس

> لازم تنسخ x-\>next في t-\>next الأول، وبعدين تخلّي x يشاور على t.
:::

:::quiz
ميزة القايمة المزدوجة الأساسية؟

1. [ ] بتستهلك ذاكرة أقل
2. [x] تحذف عقدة في O(1) وإنت معاك مؤشر ليها هي، وتمشي في الاتجاهين
3. [ ] الترتيب فيها أسرع

> في القايمة المفردة محتاج العقدة اللي قبلها، ودي ممكن تاخد N خطوة عشان تلاقيها.
:::

:::quiz
ليه نعمل free list بدل malloc/free لكل عقدة؟

1. [ ] عشان malloc مش موجود في C99
2. [x] عشان newNode/freeNode يبقوا O(1) من غير نداءات للنظام، لما العقد كلها نفس الحجم
3. [ ] عشان نمنع الـ recursion

> حجز مرة واحدة وإعادة استخدام. أسرع ومتوقّع — بس لازم تعرف الحد الأقصى من الأول.
:::
