أقسام الكتاب: 12.8 Insertion at the Root in BSTs (Programs 12.11–12.12) · 12.9 BST Implementations of Other ADT Functions (Programs 12.13–12.16)

الـ BST بتاعنا بيعمل search وinsert وsort. بس الـ symbol-table ADT فيه select وdelete وjoin كمان. والأداة اللي هتفتح لنا كل ده هي حركة صغيرة جدًا اسمها **rotation**. لو فهمتها كويس، هتفهم كل الشجر المتوازن في الفصل الجاي.

:::goal هدف الدرس:
تعمل rotation بإيدك وتفهم ليه مابتبوّظش الترتيب، تعمل root insertion، تكتب select وpartition بالـ counts، تمسح من BST صح، وتدمج شجرتين.
:::

## 12.8 Insertion at the Root in BSTs

### الـ Rotation (Program 12.11)

عندك عقدة h وولدها الشمال x. الـ right rotation بتخلّي x هو الأب وh ولده اليمين. الحاجة الوحيدة اللي بتتنقل هي الشجرة اللي كانت يمين x: بتروح شمال h.

```c listing="Program 12.11"
link rotR(link h)
  { link x = h->l; h->l = x->r; x->r = h; return x; }
link rotL(link h)
  { link x = h->r; h->r = x->l; x->l = h; return x; }
```

::figure rotation

ليه آمن؟ لأن الـ inorder ماتغيّرش: C E R S X قبل وبعد. الـ rotation بتغيّر **الشكل** بس مش الترتيب. وده اللي بيخليها الأداة الأساسية للموازنة: بتطلّع عقدة مستوى وتنزّل التانية مستوى، بتلات assignments.

### Root insertion (Program 12.12)

عايزين العنصر الجديد يبقى **الجذر**. ليه؟ لأن في تطبيقات كتير، اللي اتضاف جديد هو اللي هيتدوّر عليه قريب (زي الـ cache). الطريقة: ضيفه تحت عادي، وبعدين اعمل rotations لحد ما يطلع فوق. بالريكيرشن أسهل بكتير:

```c listing="Program 12.12"
link insertT(link h, Item item)
  { Key v = key(item);
    if (h == z) return NEW(item, z, z, 1);
    if (less(v, key(h->item)))
      { h->l = insertT(h->l, item); h = rotR(h); }
    else
      { h->r = insertT(h->r, item); h = rotL(h); }
    return h; }
```

بعد ما الريكيرشن يحط العنصر كجذر للشجرة الفرعية (شمال مثلًا)، rotation واحدة بتطلّعه فوق h. وهكذا لفوق.

::figure root-insert

```
root = G
inorder:  A A C E G H I N R S
preorder: G C A A E N I H R S
```

البرنامج بيبني نفس المفاتيح بالـ root insertion: آخر واحد اتضاف (G) هو الجذر، والـ inorder لسه مترتب. والجميل: لو المفاتيح عشوائية، شجرة الـ root insertion ليها **نفس التوزيع** بتاع الشجرة العادية، فنفس الأداء في المتوسط.

* [src/12-bst/bst\_rotations.c](src:src/12-bst/bst_rotations.c)

## 12.9 BST Implementations of Other ADT Functions

### Select (Program 12.13)

عايز العنصر رقم k (من 0) في الترتيب. هنا بيظهر فايدة الـ `N` اللي في كل عقدة: لو الشجرة الشمال فيها t عنصر:

* لو t \> k: اللي بندوّر عليه في الشمال.
* لو t \< k: في اليمين، ونبقى بندوّر على رقم k − t − 1 هناك.
* لو t == k: الجذر هو الإجابة.

```c
Item selectR(link h, int k)
  { int t = h->l->N;
    if (h == z) return NULLitem;
    if (t > k) return selectR(h->l, k);
    if (t < k) return selectR(h->r, k-t-1);
    return h->item; }
```

تكلفته = ارتفاع الشجرة، مش N. وده نفس روح الـ quickselect (الدرس 18) بس على شجرة متبنية.

### Partition (Program 12.14)

نفس الـ select، بس بدل ما نرجّع العنصر، **نطلّعه للجذر** بالـ rotations (زي root insertion بالظبط):

::code src="src/12-bst/bst_ops.c" fn="partR"

```
inorder: 20 30 35 40 45 50 60 65 70 80   (N=10, height=3)
select(0)=20 select(4)=45 select(9)=80
after partition(4) root=45 (the 5th smallest)
```

:::tip الـ N لازم يتحدّث
في أي rotation، الـ N بتاع العقدتين بيتغيّر. التنفيذ الكامل بيعمل `fixN` بعد كل rotation: N = 1 + N(شمال) + N(يمين). لو نسيته، select هيرجّع حاجات غلط من غير ما يعمل crash، وده أسوأ نوع bug.
:::

### Delete (Program 12.15)

المسح من BST هو أصعب عملية. لو العقدة ورقة: سهل، شيلها. لو ليها ولد واحد: علّق الولد مكان العقدة. بس لو ليها ولدين؟ مين ياخد مكانها؟ الإجابة: **أصغر عنصر في الشجرة اليمين** (الـ successor). والكتاب بيعملها بشكل أنيق بالـ partition:

```c listing="Program 12.15"
link joinLR(link a, link b)
  { if (b == z) return a;
    b = partR(b, 0); b->l = a;
    return b; }
link deleteR(link h, Key v)
  { link x; Key t = key(h->item);
    if (h == z) return z;
    if (less(v, t)) h->l = deleteR(h->l, v);
    if (less(t, v)) h->r = deleteR(h->r, v);
    if (eq(v, t))
      { x = h; h = joinLR(h->l, h->r); free(x); }
    return h; }
```

`partR(b, 0)` بيطلّع أصغر عنصر في b للجذر، فأكيد مالوش ولد شمال. نعلّق a (الشجرة الشمال كلها) هناك، وخلاص.

::figure bst-delete

```
after delete 30, 50: 20 35 40 45 60 65 70 80   N=8
```

:::oops المسح بيبوّظ التوازن
لو عملت inserts وdeletes كتير ورا بعض، وكل delete بياخد من اليمين دايمًا، الشجرة بتميل شمال مع الوقت، ومتوسط البحث بيبقى أسوأ من 2 ln N (تحليل ده صعب ولسه فيه أسئلة مفتوحة). حلول: بدّل عشوائي بين successor وpredecessor، أو استخدم randomized BST (الدرس الجاي) اللي الـ delete فيه بيحافظ على العشوائية.
:::

وفيه طريقة كسولة: **lazy deletion** — علّم العقدة إنها ممسوحة وماتشيلهاش، وبعدين ابني الشجرة من جديد لما الممسوح يبقى كتير. سهلة جدًا وكويسة لو الـ deletes قليلة.

### Join (Program 12.16)

عندك شجرتين a وb (مفيش شرط إن كل مفاتيح a أصغر). ادمجهم: خد جذر a، اعمله root insertion في b (فـ b اتقسمت لشمال ويمين حواليه)، وبعدين ادمج ريكيرسف شمال a مع شمال b ويمين a مع يمين b:

```c listing="Program 12.16"
link STjoin(link a, link b)
  { if (b == z) return a;
    if (a == z) return b;
    b = STinsertT(b, a->item);
    b->l = STjoin(a->l, b->l);
    b->r = STjoin(a->r, b->r);
    free(a);
    return b; }
```

```
after join with {33 55 99 1}: 1 20 33 35 40 45 55 60 65 70 80 99   N=12
```

تكلفته في أسوأ حالة ممكن تبقى كبيرة، بس عملية join مش موجودة أصلًا في معظم هياكل البحث التانية (جرّب تدمج two hash tables!). ودي من نقط قوة الـ BST.

* [src/12-bst/bst\_ops.c](src:src/12-bst/bst_ops.c)

### الصورة الكاملة

<table class="comp-table">
<tr><th>العملية</th><th>Program</th><th>التكلفة</th></tr>
<tr><td>search / insert</td><td>12.7، 12.9</td><td>العمق</td></tr>
<tr><td>sort</td><td>12.8</td><td>N</td></tr>
<tr><td>root insertion</td><td>12.12</td><td>العمق</td></tr>
<tr><td>select / partition</td><td>12.13، 12.14</td><td>العمق (بالـ counts)</td></tr>
<tr><td>delete</td><td>12.15</td><td>العمق</td></tr>
<tr><td>join</td><td>12.16</td><td>ممكن أكتر، حسب الشجرتين</td></tr>
</table>

كل حاجة «العمق». يعني لو ضمنّا إن العمق lg N، كل العمليات هتبقى ممتازة. وده بالظبط هدف الفصل 13.

:::quiz
بعد rotR(h)، إيه اللي اتغيّر في الـ inorder traversal؟

1. [ ] العقدتين اتبدّلوا
2. [x] ولا حاجة
3. [ ] اتعكس

> الـ rotation بتغيّر الأب والابن بس مع الحفاظ على ترتيب BST. عشان كده ينفع نستخدمها في أي وقت من غير ما نبوّظ البحث.
:::

:::quiz
في select(k)، لو الشجرة الشمال للجذر فيها 7 عناصر وk = 10، نروح فين؟

1. [ ] شمال، ودوّر على 10
2. [ ] يمين، ودوّر على 10
3. [x] يمين، ودوّر على 2
4. [ ] الجذر هو الإجابة

> الـ 7 اللي شمال + الجذر = 8 عناصر قبل اليمين. فاللي رقمه 10 هو رقم 10 − 7 − 1 = 2 في اليمين.
:::

:::quiz
بنمسح عقدة ليها ولدين. مين بياخد مكانها في Program 12.15؟

1. [ ] الولد الشمال
2. [x] أصغر عنصر في الشجرة اليمين
3. [ ] أي ورقة
4. [ ] الجذر

> joinLR بيعمل partR(right, 0) فيطلّع الأصغر في اليمين للجذر بتاعها، ويعلّق الشمال تحته. ده المفتاح الوحيد اللي يقدر يقعد مكان المحذوف من غير ما يبوّظ الترتيب (هو أو الأكبر في الشمال).
:::

:::quiz
ليه الـ root insertion مفيدة عمليًا؟

1. [ ] بتخلّي الشجرة متوازنة دايمًا
2. [x] العناصر الجديدة قريبة من الجذر، فلو هيتدوّر عليها قريب يبقى البحث سريع
3. [ ] بتوفّر ذاكرة

> مش بتضمن توازن (المفاتيح المترتبة لسه بتعمل قايمة)، بس بتستغل الـ locality: اللي اتضاف حالًا غالبًا هيتطلب قريب. وهي كمان أداة أساسية في join والـ randomized BST.
:::
