أقسام الكتاب: 4.5 Creation of a New ADT (Programs 4.6–4.8) · 4.6 FIFO Queues and Generalized Queues (Programs 4.9–4.11) · 4.7 Duplicate and Index Items (Program 4.12)

في الدرس اللي فات استخدمنا ADT جاهز. هنا هنعمل حاجتين: **نخترع** ADT جديد من مشكلة حقيقية (رجعة لـ union-find)، وبعدين نشوف عيلة الطوابير كلها — مش بس الطابور العادي.

:::goal هدف الدرس:
تحوّل خوارزمية union-find لـ ADT نضيف، تنفّذ الـ FIFO queue بمصفوفة دائرية وبقايمة، تعرف الطوابير المعمّمة (random queue، deque)، وتقرر تتعامل إزاي مع العناصر المكررة (ignore-new ضد forget-old).
:::

## 4.5 بناء ADT جديد: علاقات التكافؤ

في الدرس 02 كتبنا union-find كبرنامج واحد كتلة. سادجوك هنا بيسأل: **إيه العمليات اللي العميل محتاجها فعلًا؟** الإجابة تلات دوال (Program 4.7):

```c listing="Program 4.7"
void UFinit(int);
int  UFfind(int, int);    /* are p and q connected?  */
void UFunion(int, int);   /* connect them            */
```

العميل (Program 4.6) بقى سطرين: لو مش متوصّلين، وصّلهم واطبع الزوج. والتنفيذ (Program 4.8) هو weighted quick-union بالظبط بتاع الدرس 02، مستخبّي جوه ملف.

```c
if (!UFfind(p, q)) { UFunion(p, q); printf(" %d %d\n", p, q); }
```

المكسب؟ لو بكره لقيت تنفيذ أسرع (path compression مثلًا)، بتبدّل ملف `UF.c` بس. والعيب اللي الكتاب بيعترف بيه: `UFfind` و `UFunion` كل واحدة بتدوّر على الجذور لوحدها، فبنمشي المسار مرتين. التصميم النضيف أحيانًا بيكلّف شوية — وده قرار هندسي واعي، مش غلطة.

* [src/04-adt/equivalence\_adt.c](src:src/04-adt/equivalence_adt.c)

## 4.6 طوابير FIFO

أول داخل أول خارج: `QUEUEput` في الآخر، `QUEUEget` من الأول (Program 4.9).

### بالقايمة (Program 4.10)

مؤشر `head` للي هيخرج، ومؤشر `tail` للي دخل آخر واحد. الإدراج بعد الـ tail، والحذف من الـ head. الاتنين O(1).

### بالمصفوفة الدائرية (Program 4.11)

لو استخدمت مصفوفة عادية، الـ head بيتقدّم والخانات اللي وراه بتضيع. الحل: لما توصل للآخر، ارجع للأول (`% N`). والسؤال الصعب: إمتى الطابور فاضي وإمتى مليان؟ الاتنين لما head == tail! الكتاب بيحلّها بإنه يحجز خانة زيادة (N+1) ومايملاهاش أبدًا. ملفنا بيحلها بعدّاد عناصر — الاتنين صح.

```c listing="Program 4.11"
void QUEUEput(Item item)
  { q[tail++] = item; tail = tail % N; }
Item QUEUEget(void)
  { head = head % N; return q[head++]; }
```

::figure circ-queue

* [src/04-adt/queue.c](src:src/04-adt/queue.c)

### الطوابير المعمّمة

نفس الواجهة (insert / delete)، قواعد مختلفة لمين يخرج:

<table class="comp-table">
<tr><th>النوع</th><th>بيشيل مين؟</th><th>استخدام</th></tr>
<tr><td>Stack</td><td>الأحدث</td><td>ريكيرشن، DFS، تقييم تعابير</td></tr>
<tr><td>FIFO queue</td><td>الأقدم</td><td>BFS، طوابير الطباعة والشبكات</td></tr>
<tr><td>Random queue</td><td>عنصر عشوائي</td><td>محاكاة، توزيع شغل بعدالة</td></tr>
<tr><td>Deque</td><td>من أي طرف</td><td>بيعمم الـ stack والـ queue مع بعض</td></tr>
<tr><td>Priority queue</td><td>الأكبر</td><td>جدولة، heapsort (الدرس 21)</td></tr>
<tr><td>Symbol table</td><td>صاحب مفتاح معيّن</td><td>قواميس، فهارس (جزء البحث كله)</td></tr>
</table>

الـ random queue بالمصفوفة سهل جدًا: عشان تشيل عنصر عشوائي، بدّل خانة عشوائية مع آخر خانة وشيل الآخر. O(1).

```c
Item RQget(void)
  { int i = rand() % N; Item t = q[i];
    q[i] = q[N-1]; N--; return t; }
```

```
random queue gets: 33 66 11 77 22 44 55 88
deque (-1 0 1 2): popFront=-1 popBack=2 popFront=0 popBack=1
```

## 4.7 العناصر المكررة وعناصر الفهارس

سؤال بنتجاهله عادة: لو حطّيت حاجة موجودة أصلًا في الـ ADT، يحصل إيه؟ في تطبيقات كتير مش عايز تكرار. سياستين:

* **ignore-new:** العنصر موجود؟ تجاهل الجديد.
* **forget-old:** العنصر موجود؟ شيل القديم وحط الجديد (مكانه الجديد في الترتيب).

لاحظ إن السياسة بتغيّر **الترتيب** اللي العناصر هتطلع بيه. على نفس المدخل `3 5 3 7 5 1` في stack:

```
ignore-new  bottom->top: 3 5 7 1
forget-old  bottom->top: 3 7 5 1
```

طب نعرف منين بسرعة إن العنصر موجود؟ لو العناصر أرقام صغيرة (0 لـ N−1، «index items»)، نعمل مصفوفة `t[i]` فيها 1 لو i موجود. ده Program 4.12: كل عملية لسه O(1). ولو العناصر مش أرقام صغيرة؟ محتاجين symbol table — وده بالظبط موضوع الجزء الرابع.

```c listing="Program 4.12"
void STACKpush(int item)
  { if (t[item] == 1) return;          /* ignore-new */
    s[N++] = item; t[item] = 1; }
int STACKpop(void)
  { N--; t[s[N]] = 0; return s[N]; }
```

* [src/04-adt/generalized\_queues.c](src:src/04-adt/generalized_queues.c)

:::oops الفخ في forget-old
عشان تشيل القديم من نص الـ stack، محتاج تعرف مكانه وتزحزح اللي فوقه — يعني O(N) لو مش واخد بالك. ignore-new أرخص بكتير، وعشان كده هو الشائع.
:::

:::quiz
ليه الكتاب حوّل union-find لـ ADT؟

1. [ ] عشان يبقى أسرع
2. [x] عشان العميل ينفصل عن التنفيذ ونقدر نبدّل الخوارزمية من غير ما نلمسه
3. [ ] عشان C بتشترط كده

> التنفيذ ممكن يبقى حتى أبطأ شوية (بنلاقي الجذور مرتين)، بس التصميم أنضف.
:::

:::quiz
في المصفوفة الدائرية، head == tail معناها إيه؟

1. [ ] مليان أكيد
2. [ ] فاضي أكيد
3. [x] ملتبسة — عشان كده بنحجز خانة زيادة أو نعدّ العناصر

> الحالتين بيدوّوا نفس الشرط لو ملّيت المصفوفة كلها.
:::

:::quiz
إزاي تشيل عنصر عشوائي من random queue في O(1)؟

1. [ ] تزحزح كل العناصر بعده
2. [x] تبدّله مع آخر عنصر وتقلّل N
3. [ ] مستحيل في O(1)

> الترتيب جوه random queue مش مهم أصلًا، فالتبديل مايضرّش.
:::

:::quiz
stack بسياسة ignore-new، دخّلنا `1 2 1 3`. أول pop بيرجّع إيه وتاني pop؟

1. [ ] 1 ثم 3
2. [x] 3 ثم 2
3. [ ] 3 ثم 1

> الـ 1 التانية اتجاهلت، فالـ stack: 1 2 3. يطلع 3 ثم 2.
:::
