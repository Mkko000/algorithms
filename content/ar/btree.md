أقسام الكتاب: مقدمة الفصل 16 · 16.1 Rules of the Game (Definition 16.1) · 16.2 Indexed Sequential Access (Property 16.1) · 16.3 B Trees (Definition 16.2، Programs 16.1–16.4، Properties 16.2–16.3)

تخيّل داتابيز فيها مليار سجل على الهارد. كل «قراية» من الديسك أبطأ من آلاف العمليات في الذاكرة. الـ red-black tree هيحتاج حوالي 30 مستوى، يعني 30 قراية ديسك لكل بحث — كارثة. الفصل الأخير في الكتاب عن مسألة واحدة: **إزاي ندوّر في جدول ضخم بأقل عدد قرايات ممكن؟** والإجابة الكلاسيكية (اللي تحت كل داتابيز وكل file system تقريبًا) هي الـ B tree.

:::goal هدف الدرس:
تفهم نموذج التكلفة (pages وprobes)، تبني ISAM وتعرف عيبه، تبني B tree بالـ splits من تحت لفوق، وتحسب عدد الـ probes من M وN بالظبط.
:::

## 16.1 Rules of the Game

:::problem
الـ **page** بلوك بيانات متصل. الـ **probe** هو أول وصول لـ page. وإحنا مهتمين بتنفيذات للـ symbol table بتستخدم عدد probes صغير.
:::

النموذج ده مجرّد عن قصد: الـ page ممكن تكون block على الهارد، أو صفحة في الـ virtual memory، أو cache line. المهم: الوصول لأول مرة لـ page غالي جدًا، وبعد كده أي حاجة جوّاها ببلاش تقريبًا. فكل همّنا: **اقلّل عدد الـ pages اللي بنلمسها**، وخلّي كل page مليانة معلومات مفيدة.

والفكرة الأساسية: لو كل page بتشيل M مفتاح ولينك، يبقى بدل ما نتفرّع على اتنين (BST) نتفرّع على M. الارتفاع يبقى log<sub>M</sub> N بدل lg N. لـ M = 1000 ومليار مفتاح: **3 مستويات** بدل 30.

## 16.2 Indexed Sequential Access (ISAM)

أبسط حل: رتّب الملف مرة، وقسّمه pages كل واحدة فيها M سجل. واعمل **index**: page فيها أول مفتاح في كل data page. وفوقها index للـ index لو محتاج. البحث: probe للـ index يقولك أنهي data page، وprobe للـ page دي.

::figure isam

:::problem
البحث في indexed sequential file بيحتاج عدد **ثابت** من الـ probes، بس الـ insert ممكن يتطلّب إعادة بناء الـ index كله.
:::

(«ثابت» هنا معناها log<sub>M</sub> N، وده عمليًا 2–4.) المشكلة كلها في الإضافة: لو الـ page مليانة، فين نحط السجل الجديد؟ الحل التقليدي: صفحات **overflow** متعلّقة بالـ page. بس مع كل إضافة السلاسل بتطول، والبحث بيتقل:

```
fresh ISAM file: 1000 data pages, 2.00 page reads per (miss) search
after  50% more inserts: 3.00 page reads per (miss) search: overflow chains growing
after 100% more inserts: 3.46 page reads per (miss) search: overflow chains growing
after 150% more inserts: 4.02 page reads per (miss) search: overflow chains growing
after 200% more inserts: 4.51 page reads per (miss) search: overflow chains growing
```

من 2 لـ 4.5 probes. ومفيش حل غير إعادة تنظيم الملف كله كل فترة (offline). ISAM كان الأساس في أنظمة IBM القديمة، وكويس جدًا لملف قليل التغيير.

* [src/16-external-search/isam.c](src:src/16-external-search/isam.c)

## 16.3 B Trees

عايزين نفس سرعة الـ ISAM بس مع inserts رخيصة. الفكرة: **عمّم الـ 2-3-4 tree** (الدرس 31) لعقد كبيرة. بدل عقد فيها 1 لـ 3 مفاتيح، عقد (pages) فيها من M/2 لـ M−1. والتوازن الكامل جاي من نفس الحيلة: الـ split، والشجرة بتطول من فوق.

:::problem
الـ B tree من رتبة M يا إما فاضي، يا إما مكوّن من k-nodes (k−1 مفتاح وk لينك للفترات اللي المفاتيح بتحددها)، بشروط: k بين 2 وM في الجذر، وبين M/2 وM في كل عقدة تانية، وكل اللينكات للشجر الفاضي على نفس المسافة من الجذر.
:::

(الاسم «B tree» جاي من Bayer وMcCreight سنة 1970، ومحدش متأكد الـ B معناها إيه بالظبط: Bayer؟ Boeing اللي كانوا شغالين فيها؟ balanced؟)

### نسخة سادجوك: البيانات في الأوراق (Programs 16.1–16.3)

الكتاب بيبني نوع عملي جدًا: كل الـ **items** في الصفحات الخارجية (الأوراق)، والصفحات الداخلية فيها **نسخ** من المفاتيح + لينكات بس. كل entry في صفحة داخلية = (أول مفتاح في الشجرة اللي تحت، لينك ليها). كده الصفحات الداخلية بتشيل entries أكتر (مفيش بيانات تقيلة)، وكل البيانات على نفس العمق.

```c
typedef struct STnode* link;
typedef struct
  { Key key; union { link next; Item item; } ref; }
entry;
struct STnode { entry b[M]; int m; };
static link head;
static int H, N;

Item searchR(link h, Key v, int H)
  { int j;
    if (H == 0)
      for (j = 0; j < h->m; j++)
        if (eq(v, h->b[j].key))
          return h->b[j].ref.item;
    if (H != 0)
      for (j = 0; j < h->m; j++)
        if ((j+1 == h->m) || less(v, h->b[j+1].key))
          return searchR(h->b[j].ref.next, v, H-1);
    return NULLitem; }
```

(Program 16.2: في الصفحة الداخلية، روح للـ entry الأخير اللي مفتاحه ≤ المطلوب. وفي الورقة، دوّر sequential أو binary.)

### الـ insert والـ split (Program 16.3)

ابحث عن الورقة وحط فيها الـ item. لو الصفحة وصلت **M** entry: قسمها نصّين (كل نص M/2)، ورجّع للأب entry جديد (أول مفتاح في النص التاني + لينك). الأب يحطه عنده؛ ولو اتملى، يتقسم هو كمان… ولو الجذر اتقسم، اعمل جذر جديد فيه entryين. ده **bottom-up** (عكس الـ 2-3-4 بتاع الدرس 31 اللي كان top-down)، وده مناسب هنا لأن الـ splits نادرة جدًا مع M كبير.

::figure btree-split

```c
link split(link h)
  { int j; link t = NEW();
    for (j = 0; j < M/2; j++)
      t->b[j] = h->b[M/2+j];
    h->m = M/2; t->m = M/2;
    return t; }
void STinsert(Item item)
  { link t, u = insertR(head, item, H);
    if (u == NULL) return;
    t = NEW(); t->m = 2;
    t->b[0].key = head->b[0].key;
    t->b[0].ref.next = head;
    t->b[1].key = u->b[0].key;
    t->b[1].ref.next = u;
    head = t; H++; }
```

اتفرّج على B tree بـ M = 4 بيتبني من المفاتيح الأوكتال بتاعة الكتاب (706 176 601 153 513 …). الصفحة الزرقا هي اللي دخل فيها المفتاح الجديد، وكل ما صفحة توصل 4 بتتقسم:

::figure btree-insert

* [src/16-external-search/btree\_pages.c](src:src/16-external-search/btree_pages.c)
* [src/16-external-search/btree.c](src:src/16-external-search/btree.c)

### الأداء

:::problem
البحث أو الإضافة في B tree من رتبة M فيه N item بيحتاج بين **log<sub>M</sub> N** و**log<sub>M/2</sub> N** probe — يعني عدد ثابت عمليًا.
:::

:::problem
الـ B tree من رتبة M المبني من N item عشوائي متوقع يبقى فيه حوالي **1.44 N/M** صفحة.
:::

يعني الصفحات في المتوسط مليانة حوالي 1/1.44 ≈ **69%** (ln 2). وتجربتنا:

```
N=1000000 random keys, M=64 entries/page: height H=3, 23325 pages (67% full on average)
log_M N = 3.32, log_{M/2} N = 3.99  -> every search reads H+1 = 4 pages
1000 searches for inserted keys: 1000 hits, 4.0 page reads each
```

مليون مفتاح بـ M = 64 بس: 4 قرايات. ولو M = 1000 (page 16KB مثلًا): مليار مفتاح في 3–4 قرايات. والأحسن: الجذر (وغالبًا المستوى التاني) هيبقوا في الذاكرة طول الوقت لأنهم بيتلمسوا في كل بحث، فعمليًا **1–2 قراية ديسك** لأي بحث.

```
B-tree:
[ 9 ]
  [ 3 ]
    [ 1 ]
    [ 5 7 8 ]
  [ 18 22 ]
    [ 12 14 15 ]
    [ 19 ]
    [ 25 30 ]
```

(ده من `btree.c`: النسخة الكلاسيكية اللي المفاتيح فيها في كل العقد، للمقارنة.)

:::tip تحسينات من الكتاب
(1) بدل الـ split على طول لما صفحة تتملى، جرّب تنقل entries لأختها لو فيها مكان — ده بيخلّي الصفحات مليانة أكتر (B\* trees). (2) استخدم binary search جوه الصفحة لو M كبير. (3) الـ delete: الطريقة الكسولة (سيب الصفحة تفضى شوية) عملية جدًا. (4) ربط الأوراق ببعض بلينكات بيخلّي الـ range search سهل (وده اللي الداتابيز بتسميه B+ tree).
:::

:::quiz
B tree بـ M = 500 فيه 100 مليون مفتاح. أقصى عدد probes للبحث تقريبًا؟

1. [ ] 27
2. [x] 4
3. [ ] 500
4. [ ] 1

> log<sub>250</sub> 10⁸ ≈ 3.3، يعني 4 على الأكتر. وlog<sub>500</sub> 10⁸ ≈ 3 هو أحسن حالة.
:::

:::quiz
إيه العيب الأساسي في ISAM اللي الـ B tree بيحلّه؟

1. [ ] البحث بطيء
2. [x] الإضافات بتعمل overflow chains بتطول، ومفيش حل غير إعادة البناء
3. [ ] بيستهلك ذاكرة كتير

> ISAM index ثابت. الـ B tree بيعدّل نفسه بالـ splits مع كل insert، فالبحث يفضل log<sub>M</sub> N دايمًا.
:::

:::quiz
إمتى الـ B tree بيطول مستوى؟

1. [ ] لما ورقة تتقسم
2. [x] لما الجذر يتقسم
3. [ ] كل M insert

> زي الـ 2-3-4 بالظبط: الشجرة بتكبر من فوق، فكل الأوراق دايمًا على نفس العمق.
:::

:::quiz
في B tree مبني من مفاتيح عشوائية، الصفحات مليانة كام في المتوسط؟

1. [ ] 50%
2. [x] حوالي 69%
3. [ ] 100%

> Property 16.3: حوالي 1.44 N/M صفحة، يعني كل صفحة فيها M/1.44 ≈ 0.69M. بعد الـ split الصفحة 50%، وبتتملى لحد 100%، والمتوسط ln 2.
:::
