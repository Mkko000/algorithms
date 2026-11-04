أقسام الكتاب: 15.4 Multiway Tries and TSTs (Definitions 15.2–15.3، Programs 15.7–15.10، Properties 15.6–15.8) · 15.5 Text-String-Index Algorithms

لحد دلوقتي بنتفرّع على بت واحد في كل مرة (شمال/يمين). بس لو المفاتيح نصوص، الطبيعي إننا نتفرّع على **حرف** كامل: 26 اتجاه (أو 256). ده الـ **multiway trie** — هو الـ radix sort MSD لما يتحوّل لهيكل بحث. وبعدين هنشوف إزاي الـ TST بياخد سرعة الـ multiway trie من غير ما يبعزق الذاكرة.

:::goal هدف الدرس:
تبني R-way trie وتحسب تكلفته في الوقت والذاكرة، تفهم الـ TST وإزاي بيوفّر الذاكرة، تعمل partial-match وprefix search، وتبني index لنص بالـ TST.
:::

## 15.4 Multiway Tries

:::problem
الـ existence trie لمجموعة مفاتيح: لمجموعة فاضية = لينك فاضي؛ لمجموعة مش فاضية = عقدة داخلية فيها لينك لكل digit ممكن، كل لينك بيشاور على trie المفاتيح اللي بتبدأ بالـ digit ده (من غير الـ digit).
:::

:::problem
الـ multiway trie: زي فوق، بس المفاتيح ممكن تتخزّن في أوراق (مفتاح واحد = ورقة)، والعقد الداخلية فيها R لينك.
:::

البحث في existence trie: لكل حرف في المفتاح، امشي في اللينك بتاعه. لو لقيت NULL = مش موجود. لو المفتاح خلص ولقيت علامة النهاية = موجود. ومفيش مقارنة مفاتيح خالص!

```c
typedef struct STnode* link;
struct STnode { link next[R]; };
static link head;
int searchR(link h, char *v, int w)
  { int i = digit(v, w);
    if (h == NULL) return 0;
    if (i == NULLdigit) return 1;
    return searchR(h->next[i], v, w+1); }
link insertR(link h, char *v, int w)
  { int i = digit(v, w);
    if (h == NULL) h = NEW();
    if (i == NULLdigit) return h;
    h->next[i] = insertR(h->next[i], v, w+1);
    return h; }
```

(Program 15.7، والـ NULLdigit = نهاية النص، ولينك النهاية بيمثّل «المفتاح موجود».)

```
keys in order: aid all come for good is now of party people the their tim time to
  time    hit  after 5 nodes
  tim     hit  after 4 nodes
  ti      miss after 3 nodes
  theirs  miss after 6 nodes
  party   hit  after 6 nodes
15 distinct keys -> 43 nodes x 27 links = 1161 links (mostly NULL!)
```

:::problem
البحث أو الإضافة في R-ary trie عادي مبني من N bytestring عشوائي بيحتاج حوالي **log<sub>R</sub> N** مقارنة byte في المتوسط. وعدد اللينكات حوالي **RN/ln R**. وعدد المقارنات عمره ما يزيد عن عدد الـ bytes في مفتاح البحث.
:::

الوقت ممتاز: لـ R = 256 ومليون مفتاح، log<sub>256</sub> 10⁶ ≈ 2.5 حرف بس للـ miss العشوائي! بس الذاكرة: 256N/ln 256 ≈ 46N لينك. والمثال فوق: 15 كلمة → 1161 لينك، معظمهم NULL. ده تمن السرعة.

* [src/15-tries/multiway\_trie.c](src:src/15-tries/multiway_trie.c)

## Ternary Search Tries (TSTs)

الحل اللي Bentley وسادجوك نشروه سنة 1997: بدل R لينك في كل عقدة، خلّي كل عقدة فيها **حرف واحد وتلات لينكات**: أصغر، مساوي، أكبر. يعني كل عقدة trie اتحوّلت لـ BST صغيرة على الحروف. لو الحرف بتاعك أصغر من حرف العقدة روح شمال، أكبر روح يمين، ومساوي انزل في النص وانتقل للحرف اللي بعده.

::figure tst

```c
typedef struct STnode* link;
struct STnode { int d; link l, m, r; };
static link head;
int searchR(link h, char *v)
  { int i = *v;
    if (h == z) return 0;
    if (i == '\0' && h->d == '\0') return 1;
    if (i < h->d) return searchR(h->l, v);
    if (i > h->d) return searchR(h->r, v);
    return searchR(h->m, v+1); }
link insertR(link h, char *v)
  { int i = *v;
    if (h == z) { h = NEW(i); }
    if ((i == '\0') && (h->d == '\0')) return h;
    if (i < h->d) h->l = insertR(h->l, v);
    if (i == h->d) h->m = insertR(h->m, v+1);
    if (i > h->d) h->r = insertR(h->r, v);
    return h; }
```

(Program 15.8: existence TST.) لاحظ الشبه بالـ 3-way radix quicksort في الدرس 24: نفس التقسيم لتلاتة على حرف واحد. الـ TST هو الـ 3-way radix quicksort «متجمّد» كهيكل بحث، بالظبط زي ما الـ BST هو quicksort متجمّد.

:::problem
البحث أو الإضافة في TST كامل بياخد وقت متناسب مع طول المفتاح. وعدد اللينكات على الأكتر **3 أضعاف** عدد الحروف في كل المفاتيح.
:::

```
she      -> 0
sells    -> 1
...
shell    -> -1 (miss expected -1)
TST nodes: 27 (3 links each) for 12 keys
```

27 عقدة × 3 = 81 لينك، مقابل مئات في الـ multiway trie. والبحث؟ كل حرف بيعدّي على BST صغيرة من الحروف اللي موجودة فعلًا في المستوى ده — بدل log<sub>R</sub> N بقى حوالي ln N مقارنة حرف، بس كل مقارنة رخيصة جدًا (حرف واحد).

### تحسينات (Property 15.8)

* **الأوراق**: لما فرع مايبقاش فيه غير مفتاح واحد، خزّن المفتاح نفسه في ورقة بدل سلسلة عقد (Program 15.10 بيعمل كده للـ symbol table الكامل).
* **جذر R<sup>t</sup>-way**: خلّي أول t حروف multiway حقيقي (مثلًا 256 أو 256² لينك في الجذر)، والباقي TST. الجذر بيقطع معظم الشغل والذاكرة بتفضل معقولة.

:::problem
البحث أو الإضافة في TST بالأوراق وجذر R<sup>t</sup>-way بيحتاج حوالي **ln N − t ln R** وصول byte لـ N مفتاح عشوائي. واللينكات = R<sup>t</sup> (للجذر) + ثابت صغير × N.
:::

### Partial match وprefix (Program 15.9)

هنا الـ TST بيلمع: عايز كل الكلمات اللي شكلها «s.a» (النقطة = أي حرف)؟ في الـ hash مستحيل، في الـ BST صعب. في الـ TST: لو الحرف نقطة، استكشف التلات اتجاهات؛ لو حرف عادي، امشي عادي.

```c listing="Program 15.9"
void matchR(link h, char *v, int i)
  {
    if (h == z) return;
    if ((*v == '\0') && (h->d == '\0'))
      { word[i] = h->d; visit(word); }
    if ((*v == '.') || (*v < h->d))
      matchR(h->l, v, i);
    if ((*v == '.') || (*v == h->d))
      { word[i] = h->d; matchR(h->m, v+1, i+1); }
    if ((*v == '.') || (*v > h->d))
      matchR(h->r, v, i);
  }
```

```
partial match "s.a": sea
partial match "s..l.": sells shell
partial match "...": sat sea she sun the
prefix "shel": shell shells shelter
```

والـ prefix search (كل الكلمات اللي بتبدأ بـ «shel») هو بالظبط الـ autocomplete اللي في الموبايل: امشي لآخر الـ prefix، واجمع كل الشجرة اللي تحت.

* [src/15-tries/tst.c](src:src/15-tries/tst.c)
* [src/15-tries/tst\_advanced.c](src:src/15-tries/tst_advanced.c)

:::tip TST ولا hash للنصوص؟
الـ TST بيكسب في: الـ miss (بيقف بعد كام حرف، الـ hash لازم يقرا المفتاح كله)، الترتيب (sort وrange)، والـ partial/prefix matching. الـ hash بيكسب في الـ hit البسيط لمفاتيح قصيرة. وسادجوك في Table 15.2 بيقيس ده: الـ TSTs بتنافس الـ hashing بقوة على النصوص.
:::

## 15.5 Text-String-Index Algorithms

رجعنا لمسألة الدرس 28: عندك نص ضخم، وعايز تعرف «هل الكلمة X موجودة فيه؟ وفين؟» بسرعة. الفكرة: كل مكان في النص بداية مفتاح طويل جدًا (لحد آخر النص)، ونبني symbol table من الـ pointers دي. الشجرة اللي بتتبني كده اسمها **suffix tree**.

البحث = هل X **prefix** لأي مفتاح؟ يعني لازم نعرّف «المساواة» إنها «واحد prefix للتاني». ولأن المفاتيح طويلة جدًا ومش عشوائية (كلها حتت من نفس النص)، الطرق المعتمدة على الـ digits (patricia وTSTs) مناسبة جدًا: وقتها بيعتمد على عدد الحروف اللي **بتميّز** المفاتيح، مش على طولها. والـ hashing مش بيشتغل هنا أصلًا لأن وقته متناسب مع طول المفتاح.

* **BST** (Program 12.10): بسيط وكويس لمعظم النصوص.
* **Patricia**: اتصممت أصلًا للتطبيق ده. محتاج بس دالة bit(pointer, i) بترجّع البت رقم i من النص اللي بيبدأ عند الـ pointer.
* **TST**: نشيل كود نهاية المفتاح (مفيش مفتاح prefix لتاني)، وكل عقدة تخزّن pointer للنص.

```
index "times": @19 @45
index "worst": @36
index "was": @3 @28 @54
index "love": not found
```

(البرنامج بيبني TST من كل بدايات الكلمات في «It was the best of times, it was the worst of times…» ويرجّع كل الأماكن.) الـ index ده بيتبني من بدايات الكلمات بس؛ لو عملته من كل حرف هيبقى أكمل (أي substring) بس ذاكرته أكبر بكتير.

:::quiz
multiway trie بـ R = 26، والكلمة «algorithm». كام عقدة بيزور البحث؟

1. [ ] lg N
2. [x] على الأكتر 10 (حرف لكل عقدة + النهاية)
3. [ ] 26

> كل حرف عقدة واحدة، مهما كان عدد الكلمات في الـ trie. وده بالظبط Property 15.6: مش أكتر من طول المفتاح.
:::

:::quiz
ليه الـ TST بيوفّر ذاكرة جامد مقارنة بالـ multiway trie؟

1. [ ] لأنه بيضغط الحروف
2. [x] كل عقدة فيها 3 لينكات بس، والحروف اللي مش موجودة مالهاش لينكات أصلًا
3. [ ] لأنه مابيخزّنش المفاتيح

> الـ multiway trie بيحجز R لينك في كل عقدة حتى لو حرفين بس موجودين. الـ TST بيحط BST صغيرة من الحروف الموجودة بس (Property 15.7: ≤ 3 لينكات لكل حرف مخزّن).
:::

:::quiz
عايز تعمل autocomplete لقاموس 500 ألف كلمة. أنسب هيكل؟

1. [ ] hash table
2. [x] TST
3. [ ] مصفوفة مش مترتبة

> الـ prefix search طبيعي جدًا في الـ TST: امشي لآخر الـ prefix واجمع الشجرة. الـ hash مالوش أي فكرة عن الـ prefixes.
:::

:::quiz
ليه الـ hashing مش مناسب لـ text-string index (suffix tree)؟

1. [ ] لأنه بطيء في الـ hit
2. [x] المفاتيح طويلة جدًا (لحد آخر النص)، والـ hash لازم يقرا المفتاح كله، والبحث المطلوب prefix مش مساواة
3. [ ] لأنه مابيدعمش الحروف العربي

> الطرق الـ digit-based بتقرا بس الحروف اللي بتميّز المفاتيح، وده كام حرف. والـ hash مابيعرفش يجاوب «هل X prefix لحاجة موجودة؟».
:::
