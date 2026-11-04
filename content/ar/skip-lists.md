أقسام الكتاب: 13.5 Skip Lists (Definition 13.5، Programs 13.7–13.10، Properties 13.10–13.11) · 13.6 Performance Characteristics (Table 13.1)

كل اللي فات في الفصل ده كان شجر. الـ skip list بتبدأ من حتة تانية خالص: **قايمة متصلة مترتبة**. المشكلة في القايمة إنك لازم تمشي واحد واحد. طب لو ضفنا «طرق سريعة» فوقها؟ لينكات بتنط عناصر كتير مرة واحدة، وفوقها لينكات بتنط أكتر… ده بالظبط زي نظام القطارات: قطر بيقف في كل محطة، وإكسبريس بيقف كل 4، وسوبر إكسبريس كل 16.

:::goal هدف الدرس:
تكتب skip list (search وinsert وdelete)، تفهم ليه الـ levels العشوائية بتدّي lg N في المتوسط وإيه دور البارامتر t، وتقارن كل طرق الفصل 13 بأرقام حقيقية وتعرف تختار.
:::

## 13.5 Skip Lists

:::problem
الـ skip list هي قايمة متصلة مترتبة كل عقدة فيها عدد متغيّر من اللينكات، واللينكات رقم i في العقد بتعمل قوايم متصلة بتنط فوق العقد اللي عندها أقل من i لينك.
:::

::figure skiplist

لو العقد اللي ليها 2 لينك كل تاني عقدة بالظبط، و3 لينكات كل رابع عقدة، وهكذا — البحث يبقى binary search بالظبط. بس الشكل المثالي ده صعب نحافظ عليه مع الـ inserts (زي المصفوفة المترتبة). الحل: **العشوائية**. كل عقدة جديدة بنختار لها عدد لينكات عشوائي: احتمال ½ يبقى 1، ¼ يبقى 2، ⅛ يبقى 3… وفي المتوسط الشكل هيبقى قريب من المثالي.

### البحث (Program 13.7)

```c listing="Program 13.7"
Item searchR(link t, Key v, int k)
  { if (t == z) return NULLitem;
    if (eq(v, key(t->item))) return t->item;
    if (less(v, key(t->next[k]->item)))
      {
        if (k == 0) return NULLitem;
        return searchR(t, v, k-1);
      }
    return searchR(t->next[k], v, k); }
Item STsearch(Key v)
  { return searchR(head, v, lgN); }
```

ابدأ من أعلى level في الـ head. لو اللي جاي أكبر من المفتاح، انزل level. لو أصغر، امشي قدام. كرّر لحد level 0.

### الـ init والـ insert (Programs 13.8–13.9)

```c
int randX()
  { int i, j, t = rand();
    for (i = 1, j = 2; i < lgNmax; i++, j += j)
      if (t > RAND_MAX/j) break;
    if (i > lgN) lgN = i;
    return i; }
void insertR(link t, link x, int k)
  { Key v = key(x->item);
    if (less(v, key(t->next[k]->item)))
      {
        if (k < x->sz)
          { x->next[k] = t->next[k];
            t->next[k] = x; }
        if (k == 0) return;
        insertR(t, x, k-1); return;
      }
    insertR(t->next[k], x, k); }
void STinsert(Item item)
  { insertR(head, NEW(item, randX()), lgN); N++; }
```

الـ randX بيدّي j باحتمال 1/2<sup>j</sup>، بحيلة واحدة: رقم عشوائي واحد، ونشوف هو أكبر من RAND\_MAX/2، /4، /8… والـ insert نفس البحث، بس في كل level العقدة الجديدة ليها لينك فيه، نربطها هناك.

### الـ delete (Program 13.10)

نفس السكة: في كل level العقدة موجودة فيه، فكّ اللينك بتاعها. وبعد ما تفكّها من level 0 امسحها من الذاكرة.

```
N=32768, levels used lgN=20; nodes with j links: 1:16390 2:8296 3:4022 4:2052 5:996 6:505 7:250 8:124 ...
search hits 32768/32768, avg 32.5 links examined incl. level drops (lg N = 15)
total links 65343 = 1.99 N  (Property 13.11: t/(t-1) = 2)
after deleting half: 16384 keys remain
bottom level still sorted: yes
```

شوف التوزيع: 16390 عقدة بلينك واحد (نص)، 8296 بلينكين (ربع)، 4022 بتلاتة (تُمن)… تقريبًا بالظبط اللي اتصمّم.

* [src/13-balanced-trees/skip\_list.c](src:src/13-balanced-trees/skip_list.c)

### الأداء والبارامتر t

بدل احتمال ½، ممكن نستخدم 1/t: كل عقدة احتمال 1/t إنها تطلع level كمان.

:::problem
البحث والـ insert في randomized skip list بالبارامتر t بيحتاجوا حوالي **(t log<sub>t</sub> N)/2 = (t/(2 lg t)) lg N** مقارنة في المتوسط.
:::

:::problem
الـ skip lists فيها حوالي **(t/(t−1)) N** لينك في المتوسط.
:::

يعني t = 2: حوالي lg N مقارنة و2N لينك. t = 4: نفس عدد المقارنات تقريبًا (4/(2×2) = 1) بس 1.33N لينك بس. t = e ≈ 2.72 بيدّي أقل مقارنات نظريًا. والبرنامج بيقيس 32.5 «لينك متفحّص» شاملًا النزول بين الـ levels، يعني حوالي 2 lg N خطوة — قريب من التوقع.

ومعلومة سادجوك بيلفت النظر ليها: الـ skip list في الحقيقة **تمثيل تاني لشجرة متوازنة**. لو جمّعت العقد اللي بين كل لينكين في level أعلى، هيطلع لك multiway tree. وskip list كل لينك فيها بينط 1 أو 2 أو 3 من اللي تحته = 2-3-4 tree بالظبط. هنرجع للفكرة دي في الـ B-trees.

:::tip ليه الناس بتحب الـ skip lists؟
الكود أبسط من red-black بكتير (خصوصًا الـ delete)، والـ select وjoin سهلين يتضافوا (Exercises 13.77–13.78)، ولما يبقى فيه threads كتير بتكتب في نفس الوقت، الـ locking على قايمة أسهل من شجرة بتعمل rotations. عشان كده بتلاقيها في Redis (الـ sorted sets) وفي ConcurrentSkipListMap بتاع Java.
:::

## 13.6 Performance Characteristics

طب نختار إيه؟ الكتاب عمل Table 13.1: بنى جداول من N رقم عشوائي بكل طريقة وقاس. إحنا عملنا نفس التجربة على جهازنا:

```
         | construction time (s)              | N search misses: time (s) / avg compares
       N |      B      R      S      C      L | B R S C L
   50000 |  0.011  0.019  0.012  0.011  0.018 | 0.006 0.007 0.013 0.003 0.014  /  21.0 21.2 21.3 15.9 31.7
  200000 |  0.071  0.132  0.106  0.089  0.123 | 0.046 0.065 0.126 0.025 0.106  /  22.4 24.8 24.3 18.1 34.8
  800000 |  0.595  1.080  0.959  0.781  1.125 | 0.449 0.557 1.067 0.181 0.824  /  26.3 25.5 27.2 20.0 39.5
```

(B = BST عادي، R = randomized، S = splay، C = red-black، L = skip list.)

* **Red-black بيكسب البحث بفرق كبير**: مسارات أقصر بحوالي 25–35% (20 مقارنة مقابل 26 لـ 800 ألف)، ومفيش شغل زيادة في البحث. الكتاب لاقى نفس النتيجة بالظبط: «مسارات red-black أقصر بـ 35% من randomized أو splay».
* **BST عادي على مفاتيح عشوائية** سريع في البناء — بس ده لأن المفاتيح عشوائية فعلًا. جرّبه على مفاتيح مترتبة وهيموت.
* **Randomized وskip list** بيدفعوا تمن رقم عشوائي في كل insert.
* **Splay** بيعمل rotations في كل search كمان، فبحثه أبطأ على مفاتيح منتظمة الطلب — بس لو فيه مفاتيح بتتطلب أكتر بكتير، ممكن يكسب.

* [src/13-balanced-trees/balanced\_compare.c](src:src/13-balanced-trees/balanced_compare.c)

<table class="comp-table">
<tr><th>الطريقة</th><th>الضمان</th><th>ذاكرة زيادة لكل عقدة</th><th>تكسب إمتى</th></tr>
<tr><td>Randomized BST</td><td>احتمالي (Property 13.2)</td><td>عدد N</td><td>محتاج select/join وضمان قوي من غير تعقيد</td></tr>
<tr><td>Splay BST</td><td>amortized</td><td>ولا حاجة</td><td>طلبات مش منتظمة (cache-like)، ذاكرة ضيقة</td></tr>
<tr><td>Red-black BST</td><td>worst case 2 lg N</td><td>بت واحد</td><td>الاختيار العام: أسرع بحث، ضمان كامل</td></tr>
<tr><td>Skip list</td><td>احتمالي</td><td>t/(t−1) لينك</td><td>كود بسيط، concurrency</td></tr>
</table>

والخلاصة اللي سادجوك بيوصل لها: كلهم كويسين حتى للجداول الضخمة، ومفيش سبب تستخدم BST عادي في تطبيق مش ضامن فيه ترتيب الإدخال. وأي واحد فيهم أحسن بمراحل من «أمل إن البيانات عشوائية».

:::quiz
في skip list بـ t = 2 فيها 1024 عقدة، كام عقدة متوقع يكون عندها 3 لينكات أو أكتر؟

1. [ ] 512
2. [x] حوالي 256
3. [ ] 10

> احتمال ≥ 3 لينكات = ½ × ½ = ¼. يعني ربع العقد ≈ 256.
:::

:::quiz
ليه randX بيستخدم رقم عشوائي واحد بس مش رقم لكل level؟

1. [ ] عشان الكود يبقى أقصر
2. [x] عشان الأرقام العشوائية غالية، والمقارنة بـ RAND\_MAX/2، /4، /8 بتدّي نفس التوزيع
3. [ ] عشان الـ levels تبقى ثابتة

> احتمال إن t \> RAND\_MAX/2 = ½، و\> RAND\_MAX/4 بس مش /2… وهكذا. رقم واحد بيكفي لتوزيع هندسي كامل.
:::

:::quiz
عايز أقل ذاكرة لينكات مع نفس سرعة البحث تقريبًا. تغيّر t من 2 لإيه؟

1. [ ] 1
2. [x] 4
3. [ ] 100

> t = 4: اللينكات (4/3)N بدل 2N، والمقارنات (4/(2×2)) lg N = lg N زي t = 2. t = 100 هتقلّل اللينكات بس المقارنات هتبقى 100/(2×6.6) ≈ 7.5 lg N.
:::

:::quiz
تطبيق بيعمل 99% بحث و1% inserts، والمفاتيح ممكن تيجي مترتبة. أنسب اختيار؟

1. [ ] BST عادي
2. [ ] Splay
3. [x] Red-black

> البحث هو الغالب، والـ red-black أسرع بحث (مسارات ~lg N ومفيش overhead)، وضمان worst case ضد المفاتيح المترتبة. الـ splay هيعمل rotations في كل بحث.
:::
