أقسام الكتاب: مقدمة الفصل 13 (Program 13.1) · 13.1 Randomized BSTs (Programs 13.2–13.4، Properties 13.1–13.3) · 13.2 Splay BSTs (Program 13.5، Properties 13.4–13.5)

الـ BST العادي ممتاز في المتوسط وكارثي في أسوأ حالة (مفاتيح مترتبة = قايمة). الفصل 13 كله عن سؤال واحد: **إزاي نضمن إن الشجرة تفضل قصيرة مهما كان ترتيب الإدخال؟** وسادجوك بيعرض تلات استراتيجيات مختلفة تمامًا: **العشوائية** (randomized BST، skip lists)، **الـ amortization** (splay trees)، و**الضمان** (2-3-4 / red-black). الدرس ده عن أول اتنين.

:::goal هدف الدرس:
تفهم ليه إعادة البناء الكاملة مش حل، تكتب randomized BST (insert وjoin وdelete) وتفهم ليه شكلها مابيعتمدش على ترتيب الإدخال، وتكتب splay tree وتفهم الفرق بين الـ splay والـ rotate-to-root، ومعنى «amortized guarantee».
:::

## البداية: ليه مش نوازن كل شوية؟ (Program 13.1)

أول فكرة: كل فترة، خد الشجرة واعمل منها شجرة متوازنة تمامًا. والطريقة سهلة بالـ partition من الدرس اللي فات: طلّع الـ **median** للجذر، ووازن الشمال واليمين ريكيرسف.

::code src="src/13-balanced-trees/randomized_bst.c" fn="balanceR"

```
keys 0..2999 inserted in ORDER
  plain BST     : height 2999, avg depth  1499.5
  plain after balanceR (Program 13.1): height 11 (lg N = 11.6)
```

شغّال — من 2999 لـ 11. بس التكلفة؟ تقريبًا N lg N لكل إعادة بناء (أو N لو الشجرة شبه متوازنة). لو عملناها بعد كل insert يبقى N² lg N في الآخر. ولو كل فترة طويلة، الشجرة ممكن تبوظ في النص. محتاجين طريقة **تحافظ** على التوازن مع كل عملية بتكلفة صغيرة.

## 13.1 Randomized BSTs

الملاحظة الذكية: الـ BST العادي بيبقى ممتاز لو المفاتيح جت بترتيب عشوائي. طب ليه ما نخلّيش الشجرة «تتصرّف» كأن الترتيب عشوائي مهما كان؟

في BST مبنية من ترتيب عشوائي، أي مفتاح احتمال إنه يبقى الجذر = 1/N. فلو جه مفتاح جديد على شجرة فيها N عنصر، المفروض احتمال 1/(N+1) إنه يبقى الجذر. فـ Program 13.2: بالاحتمال ده اعمله **root insertion**، وإلا انزل ريكيرسف (شمال أو يمين) وكرّر نفس القرار على الشجرة الفرعية:

```c listing="Program 13.2"
link insertR(link h, Item item)
  { Key v = key(item), t = key(h->item);
    if (h == z) return NEW(item, z, z, 1);
    if (rand() < RAND_MAX/(h->N+1))
      return insertT(h, item);
    if less(v, t) h->l = insertR(h->l, item);
             else h->r = insertR(h->r, item);
    (h->N)++; return h; }
```

:::problem
بناء randomized BST مكافئ لبناء BST عادي من ترتيب عشوائي للمفاتيح. بنستخدم حوالي 2N ln N مقارنة لبناء randomized BST فيها N عنصر (**مهما كان** ترتيب الإدخال)، وحوالي 2 ln N مقارنة للبحث.
:::

```
keys 0..2999 inserted in ORDER
  plain BST     : height 2999, avg depth  1499.5
  randomized BST: height   25, avg depth    12.0   (2 ln N = 16.0)
```

نفس المفاتيح المترتبة بالظبط. العادي قايمة، والعشوائي متوسط عمقه 12. والفرق الجوهري عن quicksort العشوائي: هنا العشوائية **جوّه الخوارزمية**، فمفيش input «سيّئ» — السيء الوحيد هو حظ وحش في الأرقام العشوائية، واحتماله ضئيل جدًا:

:::problem
احتمال إن تكلفة بناء randomized BST تبقى أكتر من α ضعف المتوسط أقل من e<sup>−α</sup>.
:::

يعني احتمال إن البناء ياخد 10 أضعاف المتوسط أقل من 0.00005. ولـ 20 ضعف؟ أقل من واحد على 485 مليون.

### Join وDelete العشوائيين (Programs 13.3–13.4)

عشان الضمان يفضل موجود بعد عمليات تانية، كل عملية لازم تحافظ على «العشوائية». الـ join بتاع Program 12.16 كان بياخد جذر a دايمًا؛ النسخة العشوائية بتختار جذر a باحتمال N<sub>a</sub>/(N<sub>a</sub>+N<sub>b</sub>) وجذر b بالباقي:

```c listing="Program 12.16"
link STjoin(link a, link b)
  { if (rand()/(RAND_MAX/(a->N+b->N)+1) < a->N)
         joinR(a, b);
    else joinR(b, a); }
```

والـ delete: نفس Program 12.15، بس الـ joinLR بيختار عشوائي بين جذر الشمال وجذر اليمين بنفس النسبة.

:::problem
عمل شجرة بأي سلسلة من randomized insert وdelete وjoin مكافئ لبناء BST عادي من ترتيب عشوائي للمفاتيح اللي فيها.
:::

```
  randomized BST after deleting the 1500 even keys: N=1500, height 22, still a BST: yes
  STjoin of trees with 1000 and 2000 interleaved keys: N=3000, height 29, still a BST: yes
```

* [src/13-balanced-trees/randomized\_bst.c](src:src/13-balanced-trees/randomized_bst.c)

:::tip التمن
رقم عشوائي في كل عقدة في كل insert (وده مش رخيص لو الـ generator قوي)، وfield للعدد N في كل عقدة. الكتاب بيقول تقدر تستخدم أرقام «شبه عشوائية» رخيصة لأن الهدف مش التشفير، الهدف بس إن الـ inputs الطبيعية (المترتبة وشبهها) ماتعملش worst case. والـ N field غالبًا محتاجه أصلًا عشان select.
:::

## 13.2 Splay BSTs

فكرة تانية خالص: مافيش عشوائية، ومافيش أي معلومات توازن في العقد. بس كل ما تلمس مفتاح (insert أو search)، **طلّعه للجذر**. الفكرة إن العناصر المطلوبة كتير هتفضل فوق. بس الـ root insertion العادي مابيصلّحش الشجرة… والـ splay بيصلّحها بتعديل صغير جدًا:

* لو المسار من الجد للحفيد بيلف (شمال وبعدين يمين، أو العكس — **zig-zag**): نفس الـ root insertion: rotation تحت وبعدين فوق.
* لو المسار في نفس الاتجاه مرتين (**zig-zig**): اعمل rotation عند **الجد الأول**، وبعدين عند الأب. ده الفرق كله!

```c
link splay(link h, Item item)
  { Key v = key(item);
    if (h == z) return NEW(item, z, z, 1);
    if (less(v, key(h->item)))
      {
        if (hl == z) return NEW(item, z, h, h->N+1);
        if (less(v, key(hl->item)))
          { hll = splay(hll, item); h = rotR(h); }
        else
          { hlr = splay(hlr, item); hl = rotL(hl); }
        return rotR(h);
      }
    else
      { /* نفس الكلام بالعكس */ } }
```

::figure splay

في الرسمة: سلسلة 8 عقد ودوّرنا على 1 (أعمق واحدة). الـ rotate-to-root طلّع 1 فوق بس الباقي فضل سلسلة طولها 7. الـ splay طلّعه فوق **وكمان** نص طول المسار. لو دوّرت بعدها على 2، المسار بقى قصير. كل بحث «مكلّف» بيصلّح الشجرة للي بعده.

```
after 1024 sorted splay insertions: height 1023 (a path! but the cost was only ~2N)
depth of key 0 = 1023
after ONE splay search for 0: root=0, height 512 (halved)
after 5 more random searches: height 157, BST ok: yes, N=1024
```

الملاحظة الغريبة الأولى: بعد inserts مترتبة الشجرة **سلسلة**! بس التكلفة الكلية كانت ~2N بس (كل insert جديد كان جنب الجذر). وأول بحث للعنصر العميق كلّف 1023، بس قسم الارتفاع نصّين. يعني الشجرة بتـ«تدفع» تمن العمليات الرخيصة اللي فاتت.

:::problem
عدد المقارنات لبناء splay BST من N insert في شجرة فاضية هو O(N lg N).
:::

:::problem
عدد المقارنات لأي سلسلة من M عملية insert أو search في splay BST فيها N عقدة هو O((N + M) lg(N + M)).
:::

ده **amortized guarantee** (زي اللي شفناه في الدرس 4): مش بنضمن إن كل عملية لوحدها سريعة — عملية واحدة ممكن تاخد N — بس بنضمن إن **المجموع** عمره ما يزيد. ومش متوسط احتمالي؛ ده ضمان لأي سلسلة عمليات، حتى لو حد شرير اختارها. البرهان (Sleator وTarjan سنة 1985) بيستخدم «potential function»: كل عملية غالية بتقلّل الـ potential (تصلّح الشجرة) بقد ما كلّفت.

* [src/13-balanced-trees/splay.c](src:src/13-balanced-trees/splay.c)

:::tip إمتى الـ splay بيكسب؟
لما الـ access مش منتظم: شوية مفاتيح بتتطلب كتير (زي cache أو قاموس كلمات حقيقي). العناصر الشعبية بتعيش قرب الجذر. العيب: كل search بقى بيكتب في الشجرة (rotations)، فمش thread-safe بسهولة ومش صديق للكاش.
:::

:::quiz
ضفت 1، 2، …، 10000 بالترتيب في randomized BST. الارتفاع المتوقع تقريبًا؟

1. [ ] 9999
2. [x] في حدود 30–40 (زي BST من ترتيب عشوائي)
3. [ ] بالظبط 14

> Property 13.1: الشجرة متوزعة كأن المفاتيح جت بترتيب عشوائي، مهما كان ترتيبها الحقيقي. فالعمق المتوسط ~2 ln N ≈ 18 والارتفاع حوالي 2–3 أضعاف lg N.
:::

:::quiz
في randomized insert، شجرة فيها 99 عنصر. احتمال إن العنصر الجديد يبقى جذرها؟

1. [ ] 1/2
2. [ ] 1/99
3. [x] 1/100

> بعد الإضافة هيبقى فيه 100 عنصر، وفي ترتيب عشوائي كل واحد احتمال إنه الجذر 1/100. عشان كده `RAND_MAX/(h->N+1)`.
:::

:::quiz
إيه الفرق بين الـ splay والـ root insertion العادي؟

1. [ ] الـ splay مابيستخدمش rotations
2. [x] في حالة zig-zig، الـ splay بيعمل rotation عند الجد الأول، وده بيقصّر المسار للنص تقريبًا
3. [ ] الـ splay بيستخدم أرقام عشوائية

> التعديل الصغير ده في ترتيب الـ rotations هو اللي بيدّي الضمان الـ amortized. الـ rotate-to-root العادي ممكن يسيب السلسلة زي ما هي.
:::

:::quiz
«Splay tree بتضمن O(lg N) amortized». معناها إيه؟

1. [ ] كل عملية O(lg N)
2. [ ] في المتوسط على inputs عشوائية
3. [x] أي M عملية مع بعض تكلفتهم O((N+M) lg(N+M))، حتى لو عملية لوحدها كانت غالية

> ده ضمان على المجموع لأي سلسلة. عملية واحدة ممكن تاخد N (زي البحث عن 0 بعد inserts مترتبة)، بس دي «اتدفعت» بالعمليات الرخيصة اللي قبلها.
:::
