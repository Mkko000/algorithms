أقسام الكتاب: 6.1 Rules of the Game (Program 6.1) · 6.2 Selection Sort (Program 6.2) · 6.3 Insertion Sort (Program 6.3) · 6.4 Bubble Sort (Program 6.4) · 6.5 Performance Characteristics of Elementary Sorts (Properties 6.1–6.6)

ليه ندرس خوارزميات N² وإحنا عارفين إن فيه N log N؟ سادجوك عنده تلات أسباب مقنعة: (1) هي **أساس** الخوارزميات الأحسن — shellsort هو insertion sort متطوّر، وquicksort بيرجع لـ insertion sort في الملفات الصغيرة. (2) على ملفات صغيرة أو شبه مترتبة هي **فعلًا أسرع**. (3) بتعلّمك «قواعد اللعبة» اللي هنقيس بيها كل حاجة.

:::goal هدف الدرس:
تعرف قواعد اللعبة (less / exch / compexch، الثبات، الترتيب في المكان)، تكتب selection وinsertion (بالتحسينات) وbubble، وتعرف بالأرقام إمتى كل واحدة أحسن.
:::

## 6.1 قواعد اللعبة

كل خوارزميات الفصول 6–11 بتتكتب بنفس الشكل: `sort(Item a[], int l, int r)` ترتّب `a[l..r]`، وبتستخدم تلات عمليات بس على العناصر:

```c
typedef int Item;
#define key(A) (A)
#define less(A, B) (key(A) < key(B))
#define exch(A, B) { Item t = A; A = B; B = t; }
#define compexch(A, B) if (less(B, A)) exch(A, B)
```

Program 6.1 أبسط ترتيب ممكن: driver بيقرا N رقم (أو بيولّدهم عشوائي)، ويرتّب بـ compexch في loop مزدوج. ليه الماكروز؟ عشان لما نغيّر نوع العناصر (أرقام، نصوص، سجلات) نغيّر سطر واحد. وعشان نقدر **نعدّ** المقارنات والتبديلات بسهولة.

مصطلحات الفصل:

* **Internal / external:** البيانات في الذاكرة ولا على ديسك (الدرس 26).
* **In place:** مساحة إضافية صغيرة (ثابتة أو log N) — ضد اللي محتاج مصفوفة تانية بحجم N.
* **Stable:** العناصر اللي مفاتيحها متساوية بتفضل بنفس ترتيبها الأصلي. مهم لو بترتّب بالاسم وبعدين بالمدينة وعايز الأسماء تفضل مترتبة جوه كل مدينة.
* **Adaptive:** الوقت بيقل لو المدخل شبه مترتب.

والتكلفة بنقيسها بـ: عدد **المقارنات**، وعدد **التبديلات** (أو النقلات)، والمساحة الإضافية.

::trace selection

::trace insertion

## 6.2 Selection Sort (Program 6.2)

دوّر على الأصغر وحطه في الأول. بعدين الأصغر في الباقي وحطه تاني. وهكذا.

```c listing="Program 6.2"
void selection(Item a[], int l, int r)
  { int i, j;
    for (i = l; i < r; i++)
      { int min = i;
        for (j = i+1; j <= r; j++)
          if (less(a[j], a[min])) min = j;
        exch(a[i], a[min]); } }
```

ميزته الكبيرة: **كل عنصر بيتحرك مرة واحدة بالظبط** — N−1 تبديل بس. عيبه: **مش بيحس بالمدخل**؛ مصفوفة مترتبة أصلًا بتاخد نفس الوقت بالظبط.

::figure sel-trace

* [src/06-sorting/selection\_sort.c](src:src/06-sorting/selection_sort.c)

## 6.3 Insertion Sort (Program 6.3)

زي ما بترتّب كوتشينة في إيدك: كل ورقة جديدة تزقّها شمال لحد ما توصل مكانها. Program 6.1 البسيط كان بيعمل compexch على طول. سادجوك بيحسّنه **تلات تحسينات**:

1. **يقف بدري:** أول ما يلاقي عنصر أصغر، يوقف — مش يكمّل لحد الأول.
2. **نقلات بدل تبديلات:** خد العنصر في `v`، زحزح الأكبر منه خطوة لليمين، وحطه في المكان الفاضي. نقلة واحدة بدل 3 عمليات في كل exch.
3. **Sentinel:** حط أصغر عنصر في `a[l]` الأول، فالـ loop الداخلي مش محتاج يتشيك على `j > l`.

```c listing="Program 6.1"
void insertion(Item a[], int l, int r)
  { int i;
    for (i = r; i > l; i--) compexch(a[i-1], a[i]);   /* sentinel */
    for (i = l+2; i <= r; i++)
      { int j = i; Item v = a[i];
        while (less(v, a[j-1]))
          { a[j] = a[j-1]; j--; }
        a[j] = v; } }
```

::figure ins-trace

* [src/06-sorting/insertion\_sort.c](src:src/06-sorting/insertion_sort.c)

## 6.4 Bubble Sort (Program 6.4)

امشي من اليمين للشمال، وبدّل أي جارين مقلوبين. بعد اللفة الأولى، الأصغر «طلع» لأول المصفوفة زي فقاعة. كرّر.

```c listing="Program 6.4"
void bubble(Item a[], int l, int r)
  { int i, j;
    for (i = l; i < r; i++)
      for (j = r; j > i; j--)
        compexch(a[j-1], a[j]); }
```

رأي سادجوك صريح: bubble sort **أبطأ** من الاتنين في أغلب الحالات. الأشهر بس مش الأحسن. ممكن تحسّنه بإنك توقف لو لفّة عدّت من غير تبديلات، أو تمشي في الاتجاهين (shaker sort)، بس برضه مش هيكسب insertion.

::figure bub-trace

* [src/06-sorting/bubble\_sort.c](src:src/06-sorting/bubble_sort.c)

## 6.5 الأداء بالأرقام

<table class="comp-table">
<tr><th>Property</th><th>النص</th></tr>
<tr><td>6.1</td><td>Selection: حوالي <b>N²/2 مقارنة</b> و<b>N تبديل</b> — دايمًا.</td></tr>
<tr><td>6.2</td><td>Insertion: حوالي <b>N²/4</b> مقارنة وN²/4 نقلة في المتوسط، ضعفهم في الأسوأ.</td></tr>
<tr><td>6.3</td><td>Bubble: حوالي <b>N²/2</b> مقارنة وN²/2 تبديل في المتوسط والأسوأ.</td></tr>
<tr><td>6.4</td><td>Insertion وbubble <b>خطيين</b> لو كل عنصر عنده عدد ثابت من الـ inversions بس.</td></tr>
<tr><td>6.5</td><td>Insertion <b>خطي</b> لو عدد ثابت بس من العناصر عنده أكتر من عدد ثابت من الـ inversions.</td></tr>
<tr><td>6.6</td><td>Selection <b>خطي</b> لو العناصر كبيرة والمفاتيح صغيرة.</td></tr>
</table>

**Inversion** = زوج عناصر مقلوب. مصفوفة مترتبة: صفر. معكوسة: N(N−1)/2. والنقطة المحورية: كل نقلة في insertion sort بتصلّح inversion واحد بالظبط، فـ **عدد النقلات = عدد الـ inversions**.

قسناها على N = 1000:

```
sort       input       compares exch/moves
selection  random        499500        999
selection  sorted        499500        999
insertion  random        250121     249113     ← N²/4 = 250000
insertion  sorted          1997          0     ← خطي!
insertion  reverse       500498     499500     ← الأسوأ: N²/2
insertion  10 swaps        1997          8     ← Property 6.5
bubble     random        499500     247782
bubble     sorted        499500          0
```

* [src/06-sorting/elementary\_compare.c](src:src/06-sorting/elementary_compare.c)

### Property 6.6: لما التبديل غالي

لو كل عنصر سجل حجمه M ضعف المفتاح (مثلًا 1KB والمفتاح 4 بايت)، المقارنة تكلّف 1 والتبديل يكلّف M. Selection: N²/2 + N·M. لو M أكبر من N تقريبًا، تكلفة التبديلات N·M بتغلب — والـ selection هو الوحيد اللي عدد تبديلاته N. (والحل الأحسن: ماتحرّكش السجلات أصلًا — رتّب مؤشرات، الدرس 16.)

### الثبات

```
stability on keys 2a 1b 2c 1d 2e
  insertion: 1b 1d 2a 2c 2e   (stable)
  selection: 1b 1d 2c 2a 2e   (NOT stable)
```

Selection بيبدّل عناصر بعيدة، فممكن يقفز عنصر فوق عنصر مساوي ليه. Insertion وbubble بيبدّلوا جيران بس، وبيقفوا عند المساوي — فمستقرين.

:::oops غلطة في insertion sort
`while (j > l && less(v, a[j-1]))` — لو قلبت ترتيب الشرطين، `a[j-1]` ممكن تبقى `a[-1]`. ده بالظبط اللي الـ sentinel بيشيله.
:::

:::quiz
أنهي خوارزمية بتعمل نفس عدد المقارنات بالظبط لأي مدخل؟

1. [ ] Insertion
2. [x] Selection
3. [ ] الاتنين

> selection دايمًا N(N−1)/2 مقارنة لأن الـ loop الداخلي مالوش شرط توقف.
:::

:::quiz
مصفوفة مترتبة ما عدا 3 عناصر في أماكن غلط. أسرع خوارزمية بسيطة؟

1. [ ] Selection
2. [x] Insertion
3. [ ] Bubble (بدون تحسين)

> Property 6.5: عدد قليل من العناصر عنده inversions كتير → insertion خطي.
:::

:::quiz
عدد النقلات في insertion sort بيساوي إيه؟

1. [ ] N²/2 دايمًا
2. [x] عدد الـ inversions في المدخل
3. [ ] N − 1

> كل نقلة لجار أكبر بتصلّح زوج مقلوب واحد بالظبط.
:::

:::quiz
بترتّب سجلات حجم كل واحد 10KB بمفتاح 4 بايت، N = 100. أنسب خوارزمية بسيطة؟

1. [ ] Bubble
2. [ ] Insertion
3. [x] Selection

> Property 6.6: selection بيحرّك كل سجل مرة واحدة، والتحريك هنا هو الغالي.
:::

:::quiz
إيه فايدة الـ sentinel في Program 6.3؟

1. [ ] بيخلّي الترتيب مستقر
2. [x] بيشيل شرط حدود من الـ loop الداخلي
3. [ ] بيقلّل عدد التبديلات للنص

> أصغر عنصر في a\[l\] بيضمن إن less(v, a\[j-1\]) هتبقى false قبل ما نخرج برّه المصفوفة.
:::
