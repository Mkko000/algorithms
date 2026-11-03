أقسام الكتاب: 14.3 Linear Probing (Programs 14.4–14.5، Property 14.3) · 14.4 Double Hashing (Program 14.6، Properties 14.4–14.5) · 14.5 Dynamic Hash Tables (Program 14.7، Property 14.6) · 14.6 Perspective (Table 14.1)

الـ separate chaining بيحتاج لينكات وmalloc لكل عنصر. لو عارف عدد المفاتيح تقريبًا، فيه طريقة أوفر: حط كل المفاتيح **جوّه الجدول نفسه** (M \> N)، ولو الخانة مشغولة، دوّر على خانة تانية فاضية بطريقة ثابتة. دي اسمها **open addressing**، ونسبة الامتلاء α = N/M هي اللي بتتحكم في كل حاجة.

:::goal هدف الدرس:
تكتب linear probing وتفهم الـ clustering، تمسح منه صح، تكتب double hashing وتفهم ليه بيحل المشكلة، تحسب عدد الـ probes من α، تعمل جدول بيكبر لوحده، وتعرف تختار بين الطرق التلاتة.
:::

## 14.3 Linear Probing

أبسط طريقة: لو الخانة i مشغولة، جرّب i+1، i+2، … (ولما توصل للآخر ارجع للأول). الـ search بيمشي نفس السكة لحد ما يلاقي المفتاح (hit) أو خانة فاضية (miss).

```c
void STinsert(Item item)
  { Key v = key(item);
    int i = hash(v, M);
    while (!null(i)) i = (i+1) % M;
    st[i] = item; N++; }
Item STsearch(Key v)
  { int i = hash(v, M);
    while (!null(i))
      if eq(v, key(st[i])) return st[i];
      else i = (i+1) % M;
    return NULLitem; }
```

تابع الـ stepper ده: نفس مثال الكتاب (Figure 14.7) — المفاتيح A S E R C H I N G X M P L بقيم الـ hash بتاعة الكتاب في جدول M = 13. الخانات الحمرا هي الـ probes اللي اصطدمت:

::figure linear-probe

شوف P: الـ hash بتاعه 8، بس 8 و9 و10 و11 و12 و0 … كلهم مشغولين، فلفّ لحد ما لقى 5. ده الـ **clustering**: المفاتيح بتتكتّل في بلوكات متصلة، وأي مفتاح يقع في أي حتة من البلوك لازم يمشي لآخره — ويكبّر البلوك أكتر. البلوكات بتكبر وبتلزق في بعض. البرنامج بيطبع نفس الجدول:

```
P h= 8: slot  5 after 11 probes
    *G *X *M *S *H *P  .  A *C *E *R *I *N
```

(النجمة = خانة اتفحصت. شغّل البرنامج تشوف كل الخطوات للمفاتيح كلها.)

* [src/14-hashing/hash\_linear\_probing.c](src:src/14-hashing/hash_linear_probing.c)

:::problem
مع linear probing، متوسط عدد الـ probes في جدول حجمه M فيه N = αM مفتاح حوالي **½ (1 + 1/(1−α))** للـ hit، و**½ (1 + 1/(1−α)²)** للـ miss.
:::

التحليل ده عمله Knuth سنة 1962، وكان علامة فارقة في تحليل الخوارزميات. الأرقام بتقول كل حاجة: α = ½ → 1.5 و2.5 probe. α = 0.9 → 5.5 و50.5! الـ miss بينفجر لما الجدول يقرب يتملي.

### الـ delete (Program 14.5)

مش هينفع تفضّي الخانة وخلاص: لو مفتاح بعدها في نفس الـ cluster كان «عدّى» من فوقها، البحث عنه هيقف عند الخانة الفاضية ويقول miss غلط. الحل: فضّيها، وبعدين **أعد إدخال** كل المفاتيح اللي بعدها في نفس الـ cluster لحد أول خانة فاضية.

```c listing="Program 14.5"
void STdelete(Item item)
  { int j, i = hash(key(item), M); Item v;
    while (!null(i))
      if eq(key(item), key(st[i])) break;
      else i = (i+1) % M;
    if (null(i)) return;
    st[i] = NULLitem; N--;
    for (j = i+1; !null(j); j = (j+1) % M, N--)
      { v = st[j]; st[j] = NULLitem; STinsert(v); } }
```

```
linear probing M=13 after 5 18 31 6 44: -1 -1 -1 -1 -1 5 18 31 6 44 -1 -1 -1
after delete 18 (cluster re-inserted):     -1 -1 -1 -1 -1 5 31 6 44 -1 -1 -1 -1
search 31 -> hit, 44 -> hit (naive emptying would lose them)
```

## 14.4 Double Hashing

المشكلة في linear probing إن كل المفاتيح اللي وقعت في نفس المنطقة بتمشي **نفس الخطوات**. الحل: خلّي كل مفتاح ليه «خطوة» مختلفة، محسوبة بـ hash function تانية: i, i+k, i+2k, … حيث k = hash2(v).

```c
void STinsert(Item item)
  { Key v = key(item);
    int i = hash(v, M);
    int k = hashtwo(v, M);
    while (!null(i)) i = (i+k) % M;
    st[i] = item; N++; }
```

شروط hash2: عمرها ما ترجّع 0 (وإلا هتلف في نفس الخانة للأبد)، ولازم k وM يكونوا **أوليين نسبيًا** عشان الـ probe sequence تزور كل الخانات. أسهل طريقة: M عدد أولي. مثال شائع: `hashtwo(v) = (v % 97) + 1` مع M أولي أكبر من 97.

::figure double-hash

نفس المفاتيح (Figure 14.9 في الكتاب): R بقى بيروح لخانة 1 بخطوة 5 بدل ما يزق في 10. وP بيتنطط 8 → 12 → 3 → 7 → 11 → 2 → 6. المفاتيح اللي ليها نفس الـ hash الأول بتتفرّق بسرعة.

```
average probes for a search hit: linear 2.77, double 2.23 (load factor 13/13 = 1.00)
```

:::problem
مع double hashing، متوسط عدد الـ probes حوالي **(1/α) ln(1/(1−α))** للـ hit، و**1/(1−α)** للـ miss.
:::

الصيغة دي هي اللي بتطلع لو كل probe كان عشوائي تمامًا (uniform hashing) — وdouble hashing قريب جدًا منها عمليًا. البرنامج بيقيس ويقارن بالصيغ:

```
alpha | linear: hit (pred) miss (pred) | double: hit (pred) miss (pred)
 0.25 |  1.17 ( 1.17)   1.38 (  1.39) |  1.16 ( 1.15)  1.33 ( 1.33)
 0.50 |  1.49 ( 1.50)   2.51 (  2.50) |  1.39 ( 1.39)  2.01 ( 2.00)
 0.75 |  2.48 ( 2.50)   8.32 (  8.50) |  1.87 ( 1.85)  4.06 ( 4.00)
 0.90 |  5.51 ( 5.50)  51.61 ( 50.50) |  2.60 ( 2.56) 10.65 (10.00)
```

التجربة مطابقة للنظرية بشكل مذهل. وعند α = 0.9: الـ miss بالـ linear 51 probe، وبالـ double 10.6 بس.

::trace probing

:::problem
نقدر نضمن إن متوسط تكلفة كل البحوث أقل من t probe لو خلّينا α أقل من **1 − 1/√t** للـ linear probing، وأقل من **1 − 1/t** للـ double hashing.
:::

يعني لو عايز ≤ 4 probes للـ miss: الـ linear يتحمّل لحد α = ½، والـ double لحد α = ¾. بنفس الذاكرة، الـ double بيشيل 50% مفاتيح زيادة.

:::oops الـ delete في double hashing
إعادة إدخال الـ cluster مش هتنفع هنا (مفيش cluster متصل). الحل: **علّم** الخانة إنها «ممسوحة» (tombstone): البحث يعدّي من فوقها، والـ insert يقدر يستخدمها. بس الخانات الممسوحة بتتراكم وبتبطّأ البحث، فلازم إعادة بناء كل فترة.
:::

* [src/14-hashing/open\_addressing.c](src:src/14-hashing/open_addressing.c)

## 14.5 Dynamic Hash Tables

كل اللي فات افترض إننا عارفين N تقريبًا. لو مش عارفين؟ ابدأ بجدول صغير، ولما يوصل لنص الامتلاء، اعمل جدول **ضعف الحجم** وأعد إدخال كل المفاتيح فيه (Program 14.7):

```c listing="Program 14.7"
void expand()
  { int i; Item *t = st;
    init(M+M);
    for (i = 0; i < M/2; i++)
      if (key(t[i]) != key(NULLitem))
        STinsert(t[i]);
    free(t); }
void STinsert(Item item)
  { /* linear probing زي ما هو */ ...
    if (N++ >= M/2) expand(); }
```

```
dynamic table: 100000 inserts -> M=393215, total rehash moves 196589 (1.97 per insert)
```

ليه ده رخيص؟ لما الجدول يتضاعف من M لـ 2M، بننقل M/2 مفتاح. وقبلها كان فيه M/4 مفتاح جديد اتضافوا من آخر مضاعفة. فكل insert «بيدفع» حوالي 2 نقلة. ده تحليل **amortized** زي اللي في الدرس 4 بالظبط، والرقم الحقيقي 1.97.

:::problem
أي سلسلة من t عملية search وinsert وdelete ممكن تتنفّذ في وقت متناسب مع t، والذاكرة دايمًا في حدود معامل ثابت من عدد المفاتيح.
:::

الشرط عشان ده يشتغل مع الـ delete كمان: **كبّر** لما الجدول يوصل نص، و**صغّر للنص** لما يوصل ثُمن (مش ربع!). لو الحدين قريبين من بعض، insert وdelete ورا بعض عند الحد هيعملوا expand وshrink كل مرة. بالحدين ½ و⅛، بعد أي إعادة بناء الجدول بيبقى ربعه مليان، ومحتاج N/4 عملية على الأقل قبل إعادة البناء الجاية.

## 14.6 Perspective

ملخّص سادجوك للطرق التلاتة في جملة: **linear probing الأسرع** (لو فيه ذاكرة كفاية إن الجدول يفضل فاضي)، **double hashing الأوفر في الذاكرة** (بس محتاج hash تاني)، و**separate chaining الأسهل** في الكتابة والاستخدام.

### مقارنة عادلة بنفس الذاكرة

مثال الكتاب: separate chaining بقوايم متوسط طولها 4 وكل item ولينك كلمة واحدة = 9M كلمة (4M items + 5M لينك) ومتوسط بحث حوالي 2 probe (بحسبة الكتاب). linear probing لنفس الـ 4M مفتاح في جدول 9M: α = 4/9، والـ hit = ½(1 + 1/(1 − 4/9)) = **1.4** probe — أسرع بـ 30%. ولو عايز نفس السرعة (2 probe)، جدول 6M يكفي — **ذاكرة أقل بـ 33%**.

يعني الأداء نادرًا ما يكون سبب نختار separate chaining. بس بنختاره عمليًا كتير لأسباب تانية: الـ delete سهل، ومفيش «امتلاء» مفاجئ، والأداء بيقل بالراحة لو N زاد عن المتوقع.

### طرق متقدمة (الكتاب بيذكرها بس)

* **Brent's method**: بيحرّك المفاتيح وقت الـ insert في double hashing عشان الـ hit يبقى ثابت حتى في جدول مليان. مفيد لو الـ hits هي الغالبة.
* **Ordered hashing**: الـ linear probing بس البحث بيقف لما يلاقي مفتاح ≥ المطلوب، فالـ miss بقى زي الـ hit. مفيد لو الـ misses هي الغالبة — زي **exception dictionary** (قاموس الكلمات الشاذة في برنامج hyphenation: كل البحوث تقريبًا misses).
* وطرق تانية كتير (perfect hashing، cuckoo hashing بعد الكتاب…) — والنصيحة المعتادة: ماتستخدمش طريقة متقدمة غير لما الاحتياج حقيقي.

<table class="comp-table">
<tr><th></th><th>separate chaining</th><th>linear probing</th><th>double hashing</th><th>BST متوازن</th></tr>
<tr><td>search/insert</td><td>1 + α/2</td><td>سريع جدًا لو α صغير</td><td>ثابت حتى لو α كبير</td><td>~lg N</td></tr>
<tr><td>delete</td><td>سهل</td><td>إعادة إدخال الـ cluster</td><td>tombstones</td><td>ممكن</td></tr>
<tr><td>ذاكرة</td><td>لينك لكل عنصر</td><td>جدول فاضي كبير</td><td>الأوفر</td><td>2 لينك لكل عنصر</td></tr>
<tr><td>ترتيب (select, range)</td><td>لأ</td><td>لأ</td><td>لأ</td><td>أيوه</td></tr>
<tr><td>worst case</td><td>N (حظ وحش)</td><td>N</td><td>N</td><td>lg N (red-black)</td></tr>
</table>

:::quiz
linear probing بـ α = 0.8. متوسط probes الـ search miss؟

1. [ ] 5
2. [x] 13
3. [ ] 3
4. [ ] 1\.8

> ½(1 + 1/(1−0.8)²) = ½(1 + 25) = 13. عشان كده الـ linear probing لازم يفضل أقل من نص أو تلت أرباع.
:::

:::quiz
ليه مش بنفضّي الخانة وخلاص في الـ delete بتاع linear probing؟

1. [ ] عشان الذاكرة
2. [x] لأن مفاتيح بعدها في نفس الـ cluster هتبقى «مش قابلة للوصول» — البحث هيقف عند الخانة الفاضية
3. [ ] لأن الـ hash هيتغيّر

> البحث بيقف عند أول خانة فاضية. أي مفتاح اتحط بعد الخانة دي بسبب collision لازم يتعاد إدخاله.
:::

:::quiz
hash2 رجّع k = 6 وM = 12. إيه المشكلة؟

1. [ ] مفيش مشكلة
2. [x] الـ probes هتزور خانتين بس (i وi+6) وهتلف فيهم للأبد لو الاتنين مليانين
3. [ ] البحث هيبقى أبطأ شوية

> gcd(6, 12) = 6، فالـ sequence بتزور 12/6 = 2 خانة بس. عشان كده M لازم يبقى أولي (أو k وM أوليين نسبيًا).
:::

:::quiz
جدول بيكبر بالمضاعفة لما يوصل نص. إيه التكلفة الكلية لـ N insert؟

1. [ ] N²
2. [ ] N lg N
3. [x] متناسبة مع N (حوالي 2 نقلة لكل insert)

> مجموع النقلات M/2 + M/4 + … \< M ≈ 2N. التجربة: 1.97 نقلة لكل insert. ده amortized O(1).
:::

:::quiz
عايز تضمن ≤ 3 probes في المتوسط وذاكرتك ضيقة. تختار إيه؟

1. [ ] linear probing بـ α = 0.6
2. [x] double hashing بـ α = 0.66
3. [ ] linear probing بـ α = 0.9

> Property 14.5: double يتحمّل α \< 1 − 1/3 = 0.67، والـ linear لازم α \< 1 − 1/√3 ≈ 0.42. الـ double بيشيل مفاتيح أكتر في نفس الذاكرة.
:::
