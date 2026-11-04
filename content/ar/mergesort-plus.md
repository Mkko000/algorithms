أقسام الكتاب: 8.5 Bottom-Up Mergesort (Program 8.5, Properties 8.5–8.6) · 8.6 Performance Characteristics of Mergesort (Table 8.1) · 8.7 Linked-List Implementations of Mergesort (Programs 8.6–8.8) · 8.8 Recursion Revisited

في الدرس 11 رسمنا المسطرة بطريقتين: من فوق لتحت بالريكيرشن، ومن تحت لفوق بـ loops. mergesort عنده نفس الثنائية بالظبط. والنسخة اللي من تحت لفوق مش مجرد «ريكيرشن متشال» — هي الشكل الطبيعي للترتيب على قوائم وعلى ديسكات.

:::goal هدف الدرس:
تكتب mergesort من تحت لفوق وتعرف خواصه، تقارن أداء نسخ mergesort ببعض وبالـ quicksort بالأرقام، ترتّب قوائم مترابطة بالدمج (من فوق ومن تحت)، وتفهم الفرق الفلسفي بين quicksort وmergesort.
:::

## 8.5 Mergesort من تحت لفوق (Program 8.5)

انسى الريكيرشن. اعتبر كل عنصر ملف مترتب حجمه 1. ادمج كل اتنين جنب بعض ← ملفات حجمها 2. ادمج كل اتنين ← 4. وهكذا لحد ما يفضل ملف واحد.

```c listing="Program 8.5"
#define min(A, B) (A < B) ? A : B
void mergesortBU(Item a[], int l, int r)
  { int i, m;
    for (m = 1; m <= r-l; m = m+m)
      for (i = l; i <= r-m; i += m+m)
        merge(a, i, i+m-1, min(i+m+m-1, r)); }
```

على `ASORTINGEXAMPLE` (N = 15):

```
  m=1   merges: 1+1 1+1 1+1 1+1 1+1 1+1 1+1
  m=2   merges: 2+2 2+2 2+2 2+1
  m=4   merges: 4+4 4+3
  m=8   merges: 8+7
sorted: AAEEGILMNOPRSTX
```

::figure merge-bu

* **Property 8.5:** في كل لفة، كل الدمجات على ملفات حجمها قوة لـ 2، ما عدا يمكن آخر واحد (شوف `2+1` و`4+3` و`8+7`).
* **Property 8.6:** عدد اللفّات = عدد البتات في N بالثنائي.

```
N=1000   passes=10 bits(N)=10 bits(N-1)=10
N=1024   passes=10 bits(N)=11 bits(N-1)=10     ← استثناء
N=1025   passes=11 bits(N)=11 bits(N-1)=11
N=65536  passes=16 bits(N)=17 bits(N-1)=16     ← استثناء
```

:::oops قياس صادق
الأرقام بتقول إن عدد اللفّات بالظبط **⌈lg N⌉** = عدد بتات N−1. ده بيتفق مع صياغة الكتاب في كل الأحجام ما عدا لما N قوة لـ 2 بالظبط (1024 فيها 11 بت بس محتاجة 10 لفّات). الفكرة صح، والصياغة محتاجة تعديل صغير — وده بالظبط ليه بنقيس.
:::

الفرق بين النسختين: top-down بيقسم **بالنص** فالأحجام غير المتوازنة بتتوزع في كل مكان، وbottom-up بيقسم بقوى 2 فالفضلة كلها في الآخر. والاتنين نفس عدد المقارنات تقريبًا لما N قوة لـ 2. والـ bottom-up بيعدّي على **الملف كله** في كل لفة — وده اللي بيخليه مناسب للترتيب الخارجي على شرايط وديسكات (الدرس 26).

* [src/08-mergesort/mergesort\_bu.c](src:src/08-mergesort/mergesort_bu.c)

## 8.6 الأداء بالأرقام (Table 8.1)

الكتاب بيقيس كل التحسينات ويوصل لإن التركيز على الـ loop الداخلي ممكن يقص الوقت للنص. وبيقارن بالـ quicksort: الـ loop الداخلي بتاع mergesort أطول شوية، بس بيتنفّذ N lg N مرة بس، وquicksort بيتنفّذ 39% أكتر (29% مع median-of-3). والنتيجة النهائية عنده: mergesort المحسّن لسه أبطأ من quicksort بحوالي 25%. قسنا على مليون رقم:

```
time, N=1000000 random ints:
  top-down, Program 8.3    0.128s
  improved, Program 8.4    0.113s
  bottom-up, Program 8.5   0.115s
  quicksort, Program 7.4   0.085s
```

quicksort أسرع بحوالي 25% من أحسن mergesort — نفس رقم الكتاب تقريبًا بعد 25 سنة من الأجهزة. بس افتكر: mergesort **مستقر** و**مضمون**، وquicksort لأ. الوقت مش المعيار الوحيد.

* [src/08-mergesort/mergesort\_nocopy.c](src:src/08-mergesort/mergesort_nocopy.c)

## 8.7 Mergesort على القوائم المترابطة

mergesort محتاج مساحة إضافية على أي حال — طب نستخدمها **روابط** بدل مصفوفة. والقايمة مناسبة جدًا: الدمج بيغيّر مؤشرات بس، ومفيش ولا عنصر بيتنسخ.

### دمج قايمتين (Program 8.6)

```c listing="Program 8.6"
link merge(link a, link b)
  { struct node head; link c = &head;
    while ((a != NULL) && (b != NULL))
      if (less(a->item, b->item))
           { c->next = a; c = a; a = a->next; }
      else { c->next = b; c = b; b = b->next; }
    c->next = (a == NULL) ? b : a;
    return head.next; }
```

الـ head الوهمي (الدرس 06) بيشيل حالة «أول عنصر» الخاصة.

### من فوق لتحت (Program 8.7)

المشكلة: إزاي تقسم القايمة في النص من غير ما تعرف طولها؟ الحيلة: مؤشرين، واحد بيمشي خطوة والتاني خطوتين. لما السريع يوصل للآخر، البطيء في النص.

```c listing="Program 8.7"
link mergesort(link c)
  { link a, b;
    if (c == NULL || c->next == NULL) return c;
    a = c; b = c->next;
    while ((b != NULL) && (b->next != NULL))
      { c = c->next; b = b->next->next; }
    b = c->next; c->next = NULL;
    return merge(mergesort(a), mergesort(b)); }
```

### من تحت لفوق بـ queue (Program 8.8)

حط كل عقدة في FIFO queue كقايمة طولها 1. وبعدين: شيل اتنين من الأول، ادمجهم، حط الناتج في الآخر. كرّر لحد ما يفضل واحد. أنيقة جدًا — والـ queue بتضمن إن القوائم المدموجة أحجامها متقاربة.

```
input:     55 47 96 81 47 13 77 21 82 93 11 49 42 44 96
top-down:  11 13 21 42 44 47 47 49 55 77 81 82 93 96 96
bottom-up: 11 13 21 42 44 47 47 49 55 77 81 82 93 96 96
```

* [src/08-mergesort/list\_mergesort.c](src:src/08-mergesort/list_mergesort.c)

:::problem
تمارين الكتاب بتقترح: بدل ما تبدأ بقوائم طولها 1، ابدأ بـ «الـ runs» المترتبة الموجودة أصلًا في المدخل. ملف شبه مترتب فيه runs طويلة قليلة، فيخلص في لفّات قليلة جدًا. نفس الفكرة دي هي قلب Timsort، الترتيب الافتراضي في Python وJava للكائنات.
:::

## 8.8 الريكيرشن تاني

الكتاب بيقارن quicksort وmergesort بتشبيه المديرين: في quicksort، كل مدير بيتعب في إنه يقسم الشغل **صح** (الـ partition)، فلما المرؤوسين يخلصوا، الشغل خلص. في mergesort، كل مدير بيقسم بسرعة وعشوائي في النص، وبعدين **يتعب في لم النتايج** (الدمج).

<table class="comp-table">
<tr><th></th><th>Quicksort</th><th>Mergesort</th></tr>
<tr><td>الشغل فين</td><td>قبل الريكيرشن (conquer-and-divide)</td><td>بعد الريكيرشن (divide-and-conquer)</td></tr>
<tr><td>بيبدأ بـ</td><td>أكبر ملف وبيخلص بالصغيرين</td><td>أصغر ملفات وبيخلص بالأكبر</td></tr>
<tr><td>شكل الشجرة</td><td>بيعتمد على البيانات</td><td>بيعتمد على N بس</td></tr>
<tr><td>غير ريكيرسف</td><td>محتاج stack (الأجزاء بتعتمد على البيانات)</td><td>loops بسيطة (القسمة معروفة مسبقًا)</td></tr>
<tr><td>الثبات</td><td>صعب</td><td>سهل لو الدمج مستقر</td></tr>
</table>

والكتاب بيضيف ملاحظة لطيفة: ينفع تتخيّل quicksort «من فوق لتحت بالمستويات» — لفّات على المصفوفة كل واحدة بتقسّم كل الأجزاء. للمصفوفات مكلّف تتبّع الأجزاء، بس للقوائم ده بالظبط نظير bottom-up mergesort.

:::quiz
bottom-up mergesort على N = 100. كام لفّة؟

1. [ ] 6
2. [x] 7
3. [ ] 100

> ⌈lg 100⌉ = 7 (64 \< 100 ≤ 128).
:::

:::quiz
إزاي Program 8.7 بيلاقي نص القايمة؟

1. [ ] بيعدّ الطول الأول
2. [x] مؤشر بخطوة ومؤشر بخطوتين؛ لما السريع يخلص، البطيء في النص
3. [ ] بيقسم عشوائي

> ده بيوفّر لفّة عدّ كاملة، ونفس الحيلة بتستخدم في كشف الدورات.
:::

:::quiz
ليه mergesort غير الريكيرسف أسهل بكتير من quicksort غير الريكيرسف؟

1. [ ] لأن mergesort أقصر
2. [x] لأن طريقة قسمة mergesort مش معتمدة على البيانات، فنقدر نرتّب الشغل بـ loops
3. [ ] لأن quicksort مالوش نسخة غير ريكيرسف

> quicksort محتاج يفتكر حدود أجزاء اتحددت وقت التشغيل، فلازم stack.
:::

:::quiz
عايز ترتّب قايمة مترابطة مليون عقدة وتحافظ على الثبات. أنسب حاجة؟

1. [ ] quicksort
2. [ ] selection sort على القايمة
3. [x] mergesort على القايمة

> N lg N، مستقر، ومفيش مساحة إضافية غير الروابط الموجودة أصلًا.
:::
