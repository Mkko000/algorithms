أقسام الكتاب: 12.1 Symbol-Table Abstract Data Type (Definition 12.1، Programs 12.1–12.2) · 12.2 Key-Indexed Search (Program 12.3، Property 12.1) · 12.3 Sequential Search (Programs 12.4–12.5، Properties 12.2–12.4) · 12.4 Binary Search (Program 12.6، Property 12.5)

القاموس، دليل التليفون، جدول الـ DNS، الـ variables جوه الـ compiler، الـ index بتاع أي داتابيز — كلهم نفس المسألة: عندي حاجات ليها **مفتاح**، وعايز أحط حاجة جديدة، وألاقي حاجة بمفتاحها. الجزء الخامس كله من الكتاب (الفصول 12–16) عن المسألة دي. وفي الدرس ده هنبدأ من الأساس: إيه بالظبط اللي محتاجينه، وإيه أبسط الحلول وتكلفة كل واحد.

:::goal هدف الدرس:
تعرّف الـ symbol table كـ ADT بكل عملياته، وتنفّذه بأربع طرق ابتدائية (key-indexed، مصفوفة مترتبة، قايمة مترتبة، قايمة مش مترتبة)، وتحسب تكلفة كل عملية، وتفهم الـ binary search والـ interpolation search بعمق.
:::

## 12.1 Symbol-Table Abstract Data Type

:::problem
الـ symbol table هي هيكل بيانات لعناصر ليها مفاتيح، بيدعم عمليتين أساسيتين: **insert** عنصر جديد، و**search** (رجّع العنصر اللي ليه مفتاح معيّن).
:::

بس في الحياة الحقيقية محتاجين أكتر من كده. الـ interface بتاع الكتاب (Program 12.1):

```
void STinit(int);          /* جهّز جدول فاضي */
int  STcount(void);         /* عدد العناصر */
void STinsert(Item);        /* ضيف */
Item STsearch(Key);         /* دوّر بالمفتاح */
void STdelete(Item);        /* امسح */
Item STselect(int);         /* رجّع العنصر رقم k في الترتيب */
void STsort(void (*visit)(Item));  /* لف على الكل بالترتيب */
```

العمليات دي مش كلها سهلة على كل الهياكل: الـ **select** والـ **sort** محتاجين إن الجدول يعرف «الترتيب»، والـ **delete** بيكسر حاجات كتير. وفي عمليات تانية زي **join** (ادمج جدولين). في كل تنفيذ هنسأل: العملية دي بتكلّف كام؟

### المفاتيح المتكررة

لو جالي عنصر بنفس مفتاح عنصر موجود، أعمل إيه؟ الكتاب بيعرض اختيارات: (1) ارفض التكرار وخلّي الأول (أو استبدله)، (2) خلّي كل عنصر يشيل قايمة بكل اللي ليهم نفس المفتاح، (3) اسمح بالتكرار وخلّي search يرجّع أي واحد. الاختيار بيعتمد على التطبيق: قاموس كلمات مختلف عن جدول معاملات بنكية.

### Client مثالي (Program 12.2)

البرنامج اللي بيقرا أرقام ويطبع الـ distinct بس: لكل رقم يعمل search، ولو مش موجود يعمل insert. بسيط بس بيبيّن نسبة العمليات: لو المفاتيح كلها مختلفة، كل insert قبله search miss. فلو عايز تحسّن، اعرف الأول تطبيقك فيه hits كتير ولا misses ولا inserts.

## 12.2 Key-Indexed Search

لو المفاتيح أرقام صغيرة (أقل من M)، ومختلفة، يبقى أسهل حاجة في الدنيا: مصفوفة `st[M]`، والعنصر اللي مفتاحه k يتحط في `st[k]`. خلاص.

```c
static Item *st; static int M = maxKey;
void STinit(int maxN)
  { int i; st = malloc((M+1)*sizeof(Item));
    for (i = 0; i <= M; i++) st[i] = NULLitem; }
void STinsert(Item item) { st[key(item)] = item; }
Item STsearch(Key v)     { return st[v]; }
void STdelete(Item item) { st[key(item)] = NULLitem; }
Item STselect(int k)
  { int i;
    for (i = 0; i < M; i++)
      if (!null(i)) if (k-- == 0) return st[i]; }
```

:::problem
لو المفاتيح أعداد موجبة أقل من M ومختلفة، الـ symbol table ينفع يتنفّذ بمصفوفة key-indexed: insert وsearch وdelete في **وقت ثابت**، وinit وselect وsort في وقت متناسب مع M.
:::

وده نفس الـ key-indexed counting بتاع الدرس 16، ونفس روح الـ radix. القيد كبير (المفاتيح لازم أرقام صغيرة)، بس الفكرة دي هي اللي هتتطوّر لـ **hashing** في الفصل 14: «حوّل المفتاح لرقم صغير، واستخدمه index».

ولو المفاتيح أرقام صغيرة بس من غير بيانات (يعني بس «موجود ولا لأ»)، تقدر تستخدم **bit** لكل مفتاح بدل Item — ده الـ *existence table*، 32 مفتاح في كل كلمة.

## 12.3 Sequential Search

لو المفاتيح مش أرقام صغيرة، أبسط حل: حطهم في مصفوفة أو قايمة، ودوّر واحد واحد. السؤال: نخليهم مترتبين ولا لأ؟

* **مترتب** (Program 12.4 مصفوفة، أو قايمة مترتبة): الـ insert لازم يلاقي مكانه ويزحزح الباقي (زي insertion sort). ميزته إن search miss بيقف بدري لما يعدّي المفتاح، وإن select وsort بقوا سهلين.
* **مش مترتب** (Program 12.5 قايمة متصلة): الـ insert في الأول = وقت ثابت. بس search miss لازم يلف على الكل.

```c listing="Program 12.5"
/* Program 12.5: unordered linked list */
Item searchR(link t, Key v)
  { if (t == z) return NULLitem;
    if (eq(key(t->item), v)) return t->item;
    return searchR(t->next, v); }
void STinsert(Item item)
  { head = NEW(item, head); N++; }
```

الأرقام بالظبط:

* **Property 12.2:** sequential search في جدول فيه N عنصر بيستخدم حوالي **N/2** مقارنة للـ search hit في المتوسط (مترتب أو لأ، مصفوفة أو قايمة).
* **Property 12.3:** في جدول **مش مترتب**: الـ insert بعدد خطوات ثابت، والـ search miss بياخد **N** مقارنة (دايمًا).
* **Property 12.4:** في جدول **مترتب**: insert وsearch hit وsearch miss كلهم حوالي **N/2** مقارنة في المتوسط.

```
unordered list : hit  avg 250.5 compares (N/2 = 250)
unordered list : miss avg 500.0 compares (N = 500)
```

* [src/12-bst/st\_elementary.c](src:src/12-bst/st_elementary.c)

:::tip Self-organizing search
لو فيه مفاتيح بيتدوّر عليها أكتر من غيرها (وده الطبيعي في الحياة)، حيلة بسيطة: كل ما تلاقي عنصر، **انقله لأول القايمة** (move-to-front). العناصر الشعبية هتتجمّع فوق، والمتوسط ممكن ينزل جدًا. ده مثال على إن «الافتراضات عن التوزيع» بتفرق أكتر من الخوارزمية نفسها.
:::

## 12.4 Binary Search

لو المصفوفة مترتبة، ليه نمشي واحد واحد؟ بص في النص: لو المفتاح أصغر روح للنص الشمال، لو أكبر روح لليمين. كل مقارنة بتشيل نص الاحتمالات (Program 12.6):

```c listing="Program 12.6"
Item search(int l, int r, Key v)
  { int m = (l+r)/2;
    if (l > r) return NULLitem;
    if eq(v, key(st[m])) return st[m];
    if (l == r) return NULLitem;
    if less(v, key(st[m]))
         return search(l, m-1, v);
    else return search(m+1, r, v); }
Item STsearch(Key v)
  { return search(0, N-1, v); }
```

:::problem
الـ binary search عمره ما بيستخدم أكتر من **⌊lg N⌋ + 1** مقارنة (hit أو miss).
:::

ودي نفس الـ recurrence C<sub>N</sub> = C<sub>⌊N/2⌋</sub> + 1 اللي حليناها في الدرس 4. وملحوظة لطيفة من الكتاب: أقصى عدد مقارنات لـ N هو بالظبط عدد البتات في التمثيل الثنائي لـ N. مليون عنصر = 20 مقارنة. مليار = 30.

بس خلّي بالك من التمن: الـ **insert** في مصفوفة مترتبة لسه بياخد N/2 حركة في المتوسط. يعني binary search ممتاز لو الجدول ثابت (اتبنى مرة واتدوّر فيه كتير) — زي قاموس مطبوع. لو فيه inserts كتير مع searches كتير، محتاجين حاجة أحسن… وده الـ BST في الدرس الجاي.

### Interpolation search

لما بتدوّر في القاموس على «ياسمين»، مابتفتحش في النص — بتفتح قرب الآخر. ده بالظبط الـ interpolation search: بدل m = (l+r)/2، خمّن مكان المفتاح من **قيمته**:

```c
m = l + (v - key(a[l])) * (r - l) / (key(a[r]) - key(a[l]));
```

لو المفاتيح موزّعة بانتظام، ده بيحتاج حوالي **lg lg N** probe بس — يعني لمليار عنصر حوالي 5! بس فيه شروط: المفاتيح أرقام (أو حاجة تتحسب عليها نسبة)، التوزيع منتظم تقريبًا، والحساب نفسه أغلى من مقارنة. وفي أسوأ حالة (توزيع غريب) ممكن يبقى N.

```
binary search  : hit  avg 15.0 compares (lg N = 9.0; counts 2 per probe here)
interpolation  : hit  avg 4.8 compares (lg lg N = 3.2)
key-indexed    : search 597 -> val 7 in ONE array access
```

(البرنامج بيعدّ مقارنتين لكل probe في الـ binary search: واحدة للمساواة وواحدة للأصغر، فـ 15 ≈ 2 × 7.5 probe.)

### ملخّص التكاليف (Table 12.1)

<table class="comp-table">
<tr><th>التنفيذ</th><th>insert</th><th>search hit</th><th>search miss</th><th>select</th><th>sort</th></tr>
<tr><td>key-indexed array</td><td>1</td><td>1</td><td>1</td><td>M</td><td>M</td></tr>
<tr><td>مصفوفة مترتبة</td><td>N/2</td><td>lg N</td><td>lg N</td><td>1</td><td>N</td></tr>
<tr><td>قايمة مترتبة</td><td>N/2</td><td>N/2</td><td>N/2</td><td>N</td><td>N</td></tr>
<tr><td>قايمة مش مترتبة</td><td>1</td><td>N/2</td><td>N</td><td>—</td><td>N lg N</td></tr>
<tr><td>BST (الدرس الجاي)</td><td>1.39 lg N</td><td>1.39 lg N</td><td>1.39 lg N</td><td>~lg N</td><td>N</td></tr>
</table>

مفيش ولا واحد من الأربعة الأولانيين بيدّينا **insert وsearch الاتنين سريعين**. وده بالظبط السؤال اللي الفصول 12–16 كلها بتجاوب عليه.

:::quiz
جدول فيه 10,000 مفتاح في قايمة مش مترتبة. search miss بياخد كام مقارنة؟

1. [ ] حوالي 5000
2. [x] 10,000 بالظبط
3. [ ] حوالي 14

> Property 12.3: عشان تتأكد إن المفتاح مش موجود، لازم تبص على كل العناصر. مفيش ترتيب يخلّيك توقف بدري.
:::

:::quiz
بتبني قاموس فيه 100 ألف كلمة مرة واحدة، وبعدين ملايين عمليات بحث ومفيش إضافات. أنسب حل ابتدائي؟

1. [ ] قايمة مش مترتبة
2. [x] مصفوفة مترتبة + binary search
3. [ ] قايمة مترتبة

> التمن الكبير للمصفوفة المترتبة هو الـ insert، وهنا مفيش inserts بعد البناء (والبناء نفسه = sort مرة واحدة). كل بحث ≤ 17 مقارنة.
:::

:::quiz
أقصى عدد مقارنات للـ binary search في 1000 عنصر؟

1. [ ] 9
2. [x] 10
3. [ ] 500
4. [ ] 32

> ⌊lg 1000⌋ + 1 = 9 + 1 = 10. و1000 بالثنائي 1111101000 = 10 بت — نفس الرقم.
:::

:::quiz
إمتى الـ interpolation search يبقى فكرة وحشة؟

1. [ ] لما المفاتيح أرقام موزّعة بانتظام
2. [x] لما المفاتيح متكتّلة بشكل غير منتظم أو مش أرقام أصلًا
3. [ ] لما الجدول كبير

> التخمين معتمد على إن القيمة بتتناسب مع المكان. لو التوزيع مكتّل، التخمينات بتغلط جامد وممكن يوصل لـ N probe.
:::
