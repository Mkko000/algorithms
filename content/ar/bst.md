أقسام الكتاب: 12.5 Binary Search Trees (Definition 12.2، Programs 12.7–12.9) · 12.6 Performance Characteristics of BSTs (Properties 12.6–12.8) · 12.7 Index Implementations with Symbol Tables (Program 12.10)

الدرس اللي فات خلص بمشكلة: المصفوفة المترتبة بتدوّر بسرعة بس بتضيف ببطء، والقايمة المتصلة بتضيف بسرعة بس بتدوّر ببطء. الـ BST هو الجواب البسيط العبقري: **خد مرونة الـ links من القايمة، وخد فكرة «قسّم نصّين» من الـ binary search.**

:::goal هدف الدرس:
تكتب BST كامل (search، insert ريكيرسف وغير ريكيرسف، sort)، تفهم ليه متوسط البحث 1.39 lg N وإمتى بيبقى N، وتبني index لنص كبير بالـ BST.
:::

## 12.5 Binary Search Trees

:::problem
الـ BST هو binary tree كل عقدة داخلية فيه ليها مفتاح، والمفتاح ده أكبر من (أو يساوي) كل المفاتيح في الشجرة الشمال، وأصغر من (أو يساوي) كل المفاتيح في الشجرة اليمين.
:::

البحث نفسه هو الـ binary search بالظبط: قارن بالجذر، لو أصغر روح شمال، لو أكبر روح يمين. والفرق إن «النص» هنا مش محسوب، ده متخزّن كـ link.

::figure bst-search

```c listing="Program 12.7"
/* Program 12.7: BST-based symbol table */
typedef struct STnode* link;
struct STnode { Item item; link l, r; int N; };
static link head, z;
Item searchR(link h, Key v)
  { Key t = key(h->item);
    if (h == z) return NULLitem;
    if eq(v, t) return h->item;
    if less(v, t) return searchR(h->l, v);
             else return searchR(h->r, v); }
link insertR(link h, Item item)
  { Key v = key(item), t = key(h->item);
    if (h == z) return NEW(item, z, z, 1);
    if less(v, t)
         h->l = insertR(h->l, item);
    else h->r = insertR(h->r, item);
    (h->N)++; return h; }
```

الـ insert = search miss + تعليق عقدة جديدة في الـ link الفاضي اللي البحث وقف عنده. والـ `z` عقدة dummy بتمثّل كل الـ links الفاضية (زي الـ sentinel)، والـ `N` في كل عقدة = حجم الشجرة اللي تحتها (هنحتاجه في select).

::figure bst-insert

دوس «▶ شغّل» وشوف الشجرة بتتبني من A S E R C H I N G X M P L. لاحظ إن شكل الشجرة متحدد بالكامل من **ترتيب الإضافة**: A أول واحد فبقى الجذر، وبما إنه أصغر حرف، الشجرة كلها مايلة يمين.

::trace bst

### Sort ببلاش (Program 12.8)

الـ inorder traversal (شمال، العقدة، يمين) بيزور المفاتيح **بالترتيب**. فـ STsort مجرد:

::code src="src/12-bst/bst.c" fn="sortR"

ولو فكّرت فيها: بناء BST من N مفتاح وبعدين inorder = خوارزمية ترتيب. وهي في الحقيقة **quicksort** متنكّر: الجذر هو أول pivot، وكل المفاتيح اللي بعده بتتقسم شمال ويمين… نفس المقارنات بالظبط (لو الـ pivot هو أول عنصر).

### Insert من غير ريكيرشن (Program 12.9)

```c listing="Program 12.9"
void STinsert(Item item)
  { Key v = key(item); link p = head, x = p;
    if (head == NULL) { head = NEW(item, NULL, NULL, 1); return; }
    while (x != NULL)
      { p = x; x->N++;
        x = less(v, key(x->item)) ? x->l : x->r; }
    x = NEW(item, NULL, NULL, 1);
    if (less(v, key(p->item))) p->l = x; else p->r = x; }
```

نمشي لتحت ونفتكر الأب `p`، ولما نوصل لـ NULL نعلّق تحت p. أسرع شوية من الريكيرسف ومابيستهلكش stack.

* [src/12-bst/bst.c](src:src/12-bst/bst.c)

## 12.6 Performance Characteristics of BSTs

وقت البحث = عمق المفتاح + 1. فالسؤال كله: الشجرة شكلها إيه؟ لو اتبنت من مفاتيح بترتيب عشوائي:

:::problem
الـ search hit بيحتاج حوالي **2 ln N ≈ 1.39 lg N** مقارنة في المتوسط، في BST مبنية من N مفتاح عشوائي.
:::

:::problem
الـ insert والـ search miss بيحتاجوا حوالي **2 ln N ≈ 1.39 lg N** مقارنة في المتوسط، في BST مبنية من N مفتاح عشوائي.
:::

**ليه 2 ln N؟** مجموع أعماق كل العقد = *internal path length* C<sub>N</sub>. الجذر بيتختار عشوائي، فلو كان رقم k، الشمال فيه k−1 والـيمين N−k، وكل عقدة فيهم عمقها زاد 1:

<p style="text-align:center;direction:ltr">C<sub>N</sub> = N − 1 + (1/N) Σ<sub>k</sub> (C<sub>k−1</sub> + C<sub>N−k</sub>)</p>

ودي **نفس** الـ recurrence بتاعة quicksort في الدرس 17! وحلها C<sub>N</sub> ≈ 2N ln N. قسّم على N يطلع متوسط العمق ≈ 2 ln N. يعني BST بتدوّر أبطأ من الـ binary search بحوالي 39% بس، وفي المقابل الـ insert بقى بنفس التكلفة بدل N/2. صفقة ممتازة.

```
random keys N=16384: avg depth 17.91 (about 2 ln N = 1.39 lg N = 19.46, minus a small constant), height 32
```

(الصيغة المظبوطة 2 ln N − 3 + …، فالـ 17.9 منطقي.) وخلّي بالك: **الارتفاع** (أطول مسار) 32، أكبر من المتوسط بكتير بس لسه حوالي 3 lg N.

:::problem
في أسوأ حالة، البحث في BST فيها N مفتاح ممكن يحتاج **N** مقارنة.
:::

```
sorted keys N=2000: height 1999  <- the worst case of Property 12.8
```

لو المفاتيح جت **مترتبة** (أو مترتبة بالعكس، أو متبادلة كبير-صغير-كبير-صغير)، الشجرة بتبقى قايمة متصلة. وده مش نادر: ملف بيانات بتضيفه بالترتيب، أو بيانات مترتبة جزئيًا — حاجات بتحصل كل يوم. الفصل 13 كله عن إزاي نتجنّب الحالة دي.

:::oops BST تحت ضغط
بتبني BST من مفاتيح بتيجي من ملف logs مترتب بالوقت؟ هيبقى عندك قايمة طولها مليون، وكل بحث مليون مقارنة، والريكيرشن هيعمل stack overflow. لو مش ضامن العشوائية، استخدم شجرة متوازنة (الدروس 30–32).
:::

* [src/12-bst/bst\_ops.c](src:src/12-bst/bst_ops.c)

## 12.7 Index Implementations with Symbol Tables

في كتير من التطبيقات، مش عايز تنقل البيانات نفسها — عايز **index** بيشاور عليها. فكرة الـ index items من الدرس 9: العنصر في الـ BST هو pointer (أو رقم) والمفتاح نفسه عايش في مكان تاني.

### Index لنص (Program 12.10)

خد نص طويل (رواية مثلًا). لكل مكان i في النص، اعتبر «النص اللي بيبدأ من i» مفتاح، وحط **الرقم i** في BST. المقارنة بين مفتاحين = strcmp بين النصين من المكانين دول. دلوقتي «هل الكلمة X موجودة في الرواية؟» = بحث في BST بيقارن X بأول strlen(X) حرف بس.

```
money  -> BST:   81   suffix array:   81
purse  -> BST:   93   suffix array:   93
ishm   -> BST:    8   suffix array:    8
whale  -> BST:   -1   suffix array:   -1
```

(لاحظ «ishm» لقاها في نص كلمة Ishmael — البحث عن أي substring مش بس كلمة كاملة.) لو النص عشوائي، البناء بياخد حوالي 2N ln N مقارنة نصوص، وأي بحث حوالي 2 ln N — سادجوك بيقول إن رواية زي Moby Dick (مليون حرف) تقدر تلاقي فيها أي عبارة في حوالي **30** مقارنة نصوص.

ونفس الـ index ممكن يتبني كمصفوفة مترتبة من المواقع + binary search، ودي اسمها **suffix array** (البرنامج بيعمل الاتنين ويطلّع نفس النتايج). المصفوفة أوفر في الذاكرة لو النص ثابت، والـ BST أحسن لو النص بيتغيّر.

* [src/12-bst/text\_index.c](src:src/12-bst/text_index.c)

:::tip النص المتكرر
لو النص فيه تكرار كتير (زي «ababab…»)، المقارنات بقت طويلة لأن البادئات المشتركة طويلة، وكمان الـ BST هتبقى مش متوازنة. ده اللي بيخلّي الـ tries والـ TSTs (الفصل 15) والـ suffix trees أحسن للنصوص.
:::

### Index لملف خارجي

نفس الفكرة لو البيانات على الهارد: الـ BST فيها مفاتيح + مكان السجل على الديسك. وده أول خطوة ناحية الـ B trees في الفصل 16، بس مع BST عادي كل مستوى ممكن يبقى قراية ديسك — كتير قوي. عشان كده محتاجين شجر «عريض» مش «طويل».

:::quiz
BST مبنية من 1,000,000 مفتاح عشوائي. متوسط عدد المقارنات في search hit تقريبًا؟

1. [ ] 20
2. [x] حوالي 28
3. [ ] 500,000
4. [ ] 1000

> 1\.39 × lg(10⁶) ≈ 1.39 × 20 ≈ 27.7. أكتر من الـ binary search (20) بحوالي 39%.
:::

:::quiz
ضفت المفاتيح 1، 2، 3، …، 1000 بالترتيب في BST عادي. ارتفاع الشجرة؟

1. [ ] 10
2. [ ] 500
3. [x] 999 (قايمة)

> كل مفتاح أكبر من كل اللي قبله، فبيتعلّق يمين آخر عقدة. ده Property 12.8 بالظبط.
:::

:::quiz
إيه العلاقة بين بناء BST وquicksort؟

1. [ ] مفيش علاقة
2. [x] نفس المقارنات: الجذر هو أول pivot، والمفاتيح بتتقسم شمال ويمين بنفس الطريقة
3. [ ] BST أسرع دايمًا

> كل مفتاح بيتقارن بكل «أجداده» — بالظبط زي ما كل عنصر في quicksort بيتقارن بالـ pivots اللي فوقه. عشان كده الـ recurrence واحدة والحل 2N ln N.
:::

:::quiz
في text index بالـ BST، بنخزّن إيه في كل عقدة؟

1. [ ] نسخة من النص من المكان ده لحد الآخر
2. [x] رقم المكان بس، والمقارنة بتبص في النص الأصلي
3. [ ] أول حرف بس

> ده الـ index item: مش بنكرّر البيانات. لو خزّنا نسخة لكل مكان، الذاكرة هتبقى N² تقريبًا.
:::
