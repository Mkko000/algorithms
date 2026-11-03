أقسام الكتاب: 9.5 Priority-Queue ADT (Programs 9.8–9.10) · 9.6 Priority Queues for Index Items (Programs 9.11–9.12) · 9.7 Binomial Queues (Definitions 9.4–9.6, Programs 9.13–9.16, Properties 9.7–9.8)

الـ heap بتاع الدرس اللي فات عنده عيبين بيظهروا أول ما تستخدمه في برنامج حقيقي. الأول: لو عايز تغيّر أولوية مهمة معيّنة («العميل ده بقى VIP»)، إزاي تلاقيها جوه الـ heap؟ التاني: لو عندك اتنين PQ (طوابير سيرفرين) وعايز تدمجهم، الـ heap محتاج يتبني من الأول — O(N). الدرس ده بيحل الاتنين.

:::goal هدف الدرس:
تصمّم PQ «first-class» بـ handles عشان delete وchange، تبني index heap يسيب البيانات مكانها، وتفهم الـ binomial queue اللي كل عملياته — حتى الـ join — lg N، وتشوف العلاقة الحلوة بينه وبين العدّ الثنائي.
:::

## 9.5 الـ PQ كـ ADT كامل

في التطبيقات الحقيقية، العميل مش عايز «القيمة الأكبر» — عايز يعرف **أنهي سجل** هو الأهم (المهمة، العميل، الحدث). والعمليات زي delete وchange priority محتاجة طريقة تحدد بيها العنصر. الحل (فكرة الدرس 10): **handles**.

```c listing="Program 9.8"
/* Program 9.8 */
typedef struct pq* PQ;
typedef struct PQnode* PQlink;
PQ     PQinit();
int    PQempty(PQ);
PQlink PQinsert(PQ, Item);      /* returns a handle */
Item   PQdelmax(PQ);
void   PQchange(PQ, PQlink, Item);
void   PQdelete(PQ, PQlink);
void   PQjoin(PQ, PQ);
```

فيه نوعين handles: واحد للـ **PQ نفسه** (عشان يبقى عندك كذا PQ)، وواحد لكل **عنصر** (اللي insert بترجّعه). والكتاب صريح في القيود: العميل مسؤول يستخدم الـ handles صح (مفيش طريقة نضيفة تكتشف handle لعنصر اتمسح)، والتنفيذ **مش حر يحرّك البيانات** لأن العميل ماسك مؤشرات ليها.

### تنفيذ بقايمة مزدوجة مش مترتبة (Programs 9.9–9.10)

insert = حط عقدة في الأول O(1). delmax = لف على القايمة O(N). بس **delete وchange وjoin كلهم O(1)**: معاك المؤشر للعقدة نفسها، والقايمة المزدوجة (الدرس 06) بتسمح تشيلها من غير ما تدوّر. والـ join = وصّل آخر قايمة بأول التانية.

```c
void PQdelete(PQ pq, PQlink x)
  { x->next->prev = x->prev;
    x->prev->next = x->next;
    free(x); }
```

```
first-class PQ after change/delete/join: 12 10 8 6
```

* [src/09-heapsort/pq\_elementary.c](src:src/09-heapsort/pq_elementary.c)

## 9.6 PQ لعناصر فهارس (index items)

حالة شائعة جدًا: البيانات في مصفوفة عند العميل (`data[k]` = أولوية المهمة رقم k)، والـ PQ يشتغل على **أرقام** 0..N−1 بس. ده بالظبط index sort بتاع الدرس 16، مطبّق على الـ heap.

```c listing="Program 9.11"
/* Program 9.11 */
void PQinit(); int PQempty();
void PQinsert(int);  int PQdelmax();
void PQchange(int);  void PQdelete(int);
```

التنفيذ (Program 9.12) بيحتفظ بمصفوفتين:

* `pq[i]` = رقم العنصر اللي في خانة i في الـ heap.
* `qp[k]` = خانة العنصر k في الـ heap — **المعكوس**.

كل exch بيحدّث الاتنين. فلما العميل يقول `PQchange(k)` («أولوية k اتغيرت»)، نلاقي مكانه في O(1) من `qp[k]`، ونعمل fixUp وfixDown — O(lg N).

```c
#define less(i, j) (data[pq[i]] < data[pq[j]])
static void exch(int i, int j)
  { int t = pq[i]; pq[i] = pq[j]; pq[j] = t;
    qp[pq[i]] = i; qp[pq[j]] = j; }
void PQchange(int k)
  { fixUp(qp[k]); fixDown(qp[k], N); }
```

```
order of service: lunch(10) tests(6) build(5) review(4) email(2) deploy(0)
```

ده تحديدًا اللي خوارزميات Dijkstra وPrim في جزء الجرافات محتاجينه: «المسافة للرأس k قلّت — حدّث مكانه».

* [src/09-heapsort/index\_heap.c](src:src/09-heapsort/index_heap.c)

## 9.7 Binomial Queues

عايزين join في lg N. الـ heap مش هينفع لأنه مصفوفة شكلها ثابت. الكتاب بيبني الحل خطوة خطوة:

* **Definition 9.4:** شجرة ثنائية **left-heap-ordered** = مفتاح كل عقدة ≥ كل المفاتيح في الشجرة الشمال بتاعتها (اليمين مالوش شرط).
* **Definition 9.5:** **power-of-2 heap** = شجرة left-heap-ordered فيها جذر الشمال بتاعه شجرة كاملة واليمين فاضي. عدد عقدها قوة لـ 2 بالظبط.
* **Definition 9.6:** **binomial queue** = مجموعة power-of-2 heaps، **مفيش اتنين بنفس الحجم**.

يعني شكل الـ binomial queue اللي فيه N عنصر = **التمثيل الثنائي لـ N**. لو N = 13 = 1101، يبقى فيه heap بـ 8 وheap بـ 4 وheap بـ 1.

::figure binomial

### دمج اتنين بنفس الحجم في O(1) (Program 9.13)

```c listing="Program 9.13"
PQlink pair(PQlink p, PQlink q)
  { if (less(p->key, q->key))
         { p->r = q->l; q->l = p; return q; }
    else { q->r = p->l; p->l = q; return p; } }
```

الجذر الأصغر بقى ابن شمال للأكبر، والشمال القديم بقى يمين الأصغر. مقارنة واحدة وتلات مؤشرات.

### العمليات = عمليات حسابية على أرقام ثنائية

* **insert (Program 9.14):** زي ما تزوّد 1 على عدّاد ثنائي: لو الخانة 0 فاضية حط فيها. لو مليانة، pair وشيل «الحمل» للخانة 1، وهكذا.
* **delmax (Program 9.15):** الأكبر في جذر واحد من الـ lg N شجرة — لف عليهم. شيله، وشجرته الشمال بتتفك لـ power-of-2 heaps أصغر (بالظبط حجم 2^(k−1)، …، 1) — اعملهم binomial queue وjoin مع الباقي.
* **join (Program 9.16):** **جمع ثنائي** بحمل: لكل خانة i، عندك لحد 3 heaps (من الاتنين ومن الحمل)، وجدول حالات بيقول تحط إيه وتشيل إيه.

```
a after 11 inserts (1011b) sizes present: 2^3 2^1 2^0
b after 5 inserts (101b)   sizes present: 2^2 2^0
a joined with b (16=10000b) sizes present: 2^4
delmax order: 104 103 102 101 100 89 55 34 21 13 8 7 5 3 2 1
```

11 + 5 = 16 = 10000 بالثنائي: كل الحمل اتجمع في heap واحد بـ 16.

* **Property 9.7:** كل عمليات الـ PQ — بما فيها join — **O(lg N)**.
* **Property 9.8:** بناء binomial queue بـ N insert من الفاضي = **O(N)** مقارنات في الأسوأ: نص الـ inserts من غير مقارنات، ربعها مقارنة واحدة… المجموع \< N. نفس تحليل العدّاد الثنائي المهلك (amortized) في الدرس 04.

* [src/09-heapsort/binomial\_queue.c](src:src/09-heapsort/binomial_queue.c)

:::oops تنبيه الكتاب نفسه
Program 9.16 (join) مابيحرّرش الـ struct بتاع الـ PQ التاني — يعني memory leak صغير. الكتاب بيعترف بده وبيرجّع للدرس 10: الـ first-class ADTs في C محتاجة سياسة واضحة لمين يحرر إيه.
:::

:::problem
الكتاب بيشاور في المراجع على هياكل أعقد (Fibonacci heaps وغيرها) بتوصل نظريًا لـ insert وjoin وchange في O(1) amortized وdelmax في lg N — الصف الأخير «الأحسن نظريًا» في Table 9.1. عمليًا الـ binary heap والـ binomial queue هما اللي بيتستخدموا؛ الباقي ثوابته كبيرة.
:::

:::quiz
ليه PQ بقايمة مزدوجة بيعمل delete في O(1)؟

1. [ ] لأن القايمة مترتبة
2. [x] لأن العميل معاه handle للعقدة نفسها، والقايمة المزدوجة بتسمح تشيلها من غير البحث عن اللي قبلها
3. [ ] لأنه بيعلّم العقدة ممسوحة بس

> التمن إن delmax بقى O(N). كل تنفيذ بيختار أنهي عملية يدفع فيها.
:::

:::quiz
في الـ index heap، دور `qp[]`؟

1. [ ] بيخزّن الأولويات
2. [x] بيقول مكان العنصر k في الـ heap، عشان change(k) يلاقيه في O(1)
3. [ ] بيخزّن نسخة احتياطية

> من غيره هتدوّر على k في الـ heap كله.
:::

:::quiz
binomial queue فيه 22 عنصر. فيه power-of-2 heaps بأنهي أحجام؟

1. [x] 16 و 4 و 2
2. [ ] 16 و 6
3. [ ] 8 و 8 و 4 و 2

> 22 = 10110 بالثنائي = 16 + 4 + 2. ومفيش حجمين متساويين أبدًا.
:::

:::quiz
join اتنين binomial queues بيشبه إيه؟

1. [ ] ضرب رقمين
2. [x] جمع رقمين ثنائيين بالحمل
3. [ ] ترتيب بالدمج

> كل خانة فيها لحد 3 heaps بنفس الحجم: اتنين منهم يعملوا pair ويتشالوا حمل.
:::

:::quiz
ليه الـ binary heap العادي مابيعملش join في lg N؟

1. [ ] لأنه مش مترتب
2. [x] لأنه مصفوفة شكلها ثابت (شجرة كاملة)، فدمج اتنين محتاج إعادة بناء O(N)
3. [ ] بيعمل عادي

> الـ binomial queue مبني على روابط وأشجار متعددة عشان يتفادى ده.
:::
