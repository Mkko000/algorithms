"""Figures for the solved-exercise (تمارين محلولة) lessons. Author: Mohammed El Sisi."""
from svglib import *
F = {}

# ---------- chapter 5 ----------
def fibtree(h):
    if h == 0: return None
    if h == 1: return N('1')
    return N(str(h), fibtree(h - 1), fibtree(h - 2))

def dctree(n):
    if n == 1: return N('1', cls='node hi2')
    return N(str(n), dctree(n // 2), dctree((n + 1) // 2))

F['ex-fibtree'] = ('fig', fig(tree(fibtree(5), 30, 46, 13, 'Fibonacci tree of height 5', show_null=True),
    'شجرة Fibonacci ارتفاعها 5 (تمرين 5.73): تحت الجذر شجرة Fibonacci ارتفاعها 4 على الشمال و3 على اليمين، وهكذا لحد تحت. فيها 12 node و13 external node (الخطوط المتقطعة). يعني N + 1 = F<sub>7</sub> = 13. وفي كل node، الفرق بين ارتفاع الشمال واليمين بالظبط 1، وده أسوأ توازن ممكن لشجرة AVL.'))
F['ex-dctree'] = ('fig', fig(tree(dctree(11), 30, 46, 13, 'divide-and-conquer tree for N = 11'),
    'شجرة divide-and-conquer لـ N = 11 (تمرين 5.74): كل node مكتوب فيها حجم المسألة، وبتتقسم ⌊N/2⌋ و⌈N/2⌉. الأوراق البنفسجي هي الـ 11 مسألة اللي حجمها 1. مجموع أعماقهم = 39، وده بين N lg N = 38.1 وN lg N + N = 49.1 (تمرين 5.75).'))

def hanoi_rows(n=4):
    rows, moves = [], []
    def h(k, d):
        if k == 0: return
        h(k - 1, -d); moves.append(k * d); h(k - 1, -d)
    h(n, 1)
    rows.append(('i', [str(i) for i in range(1, len(moves) + 1)], {}))
    rows.append(('disk', [str(abs(m)) for m in moves], {i: ('hi' if abs(m) % 2 else 'hi2') for i, m in enumerate(moves)}))
    rows.append(('dir', ['→' if m > 0 else '←' for m in moves], {i: ('hi' if abs(m) % 2 else 'hi2') for i, m in enumerate(moves)}))
    return array_rows(rows, 26, 44, 'towers of Hanoi moves N=4')
F['ex-hanoi'] = ('fig', fig(hanoi_rows(),
    'الـ 15 حركة بتوع Hanoi لـ 4 أقراص (تمرين 5.22–5.23): صف «disk» هو نفسه علامات المسطرة (1 2 1 3 1 2 1 4 …)، والقرص اللي بيتحرك في الخطوة i = 1 + عدد الأصفار في آخر i بالـ binary. والأقراص الفردية (أزرق) دايمًا بتمشي في اتجاه واحد (←)، والزوجية (بنفسجي) في الاتجاه التاني (→).'))

# ---------- chapter 6 ----------
EQ = list('EASYQUESTION')
def ex6_traces():
    # selection (Program 6.2)
    a = EQ[:]; rows = [('', a[:], {})]
    for i in range(len(a) - 1):
        m = i
        for j in range(i + 1, len(a)):
            if a[j] < a[m]: m = j
        a[i], a[m] = a[m], a[i]
        c = {k: 'dim' for k in range(i)}; c[i] = 'hi'
        if m != i: c[m] = 'hi2'
        rows.append((str(i), a[:], c))
    sel = array_rows(rows, 24, 26, 'selection sort EASYQUESTION')
    # insertion (Program 6.3: sentinel pass first)
    a = EQ[:]; rows = [('', a[:], {})]
    for i in range(len(a) - 1, 0, -1):
        if a[i] < a[i - 1]: a[i], a[i - 1] = a[i - 1], a[i]
    rows.append(('min', a[:], {0: 'hi'}))
    for i in range(2, len(a)):
        v = a[i]; j = i
        while v < a[j - 1]: a[j] = a[j - 1]; j -= 1
        a[j] = v
        c = {k: 'dim' for k in range(i + 1, len(a))}
        for k in range(j + 1, i + 1): c[k] = 'hi2'
        c[j] = 'hi'
        rows.append((str(i), a[:], c))
    ins = array_rows(rows, 24, 30, 'insertion sort EASYQUESTION')
    # bubble (Program 6.4)
    a = EQ[:]; rows = [('', a[:], {})]; n = len(a)
    for i in range(n - 1):
        mv = set()
        for j in range(n - 1, i, -1):
            if a[j] < a[j - 1]: a[j], a[j - 1] = a[j - 1], a[j]; mv |= {j, j - 1}
        c = {k: 'dim' for k in range(i)}; c[i] = 'hi'
        for k in mv:
            if k > i: c[k] = 'hi2'
        rows.append((str(i), a[:], c))
    bub = array_rows(rows, 24, 26, 'bubble sort EASYQUESTION')
    F['ex6-sel'] = ('fig', fig(sel, 'تمرين 6.11: selection sort على EASYQUESTION. الأزرق هو الخانة i اللي استقبلت الأصغر، والبنفسجي هو المكان اللي الأصغر جه منه. لاحظ السطر 2: الـ E التانية (من الخانة 6) نطّت فوق S وY وQ وU. عشان كده selection sort مش stable.'))
    F['ex6-ins'] = ('fig', fig(ins, 'تمرين 6.15: insertion sort (Program 6.3). أول لفّة بتجيب الأصغر (A) للخانة 0 كـ sentinel، وبعد كده كل سطر بيدخّل عنصر واحد (أزرق) ويزق اللي أكبر منه خطوة يمين (بنفسجي). الـ 44 مقارنة دول أقل من نص الـ 66 بتوع selection.'))
    F['ex6-bub'] = ('fig', fig(bub, 'تمرين 6.20: bubble sort (Program 6.4). كل لفّة من اليمين بتطلّع أصغر عنصر باقي للخانة i. من بعد السطر 7 الملف خلص، بس Program 6.4 بيكمّل اللفّات للآخر (66 مقارنة). وده اللي تمرين 6.24 بيصلّحه.'))
ex6_traces()

def ex6_index():
    a = EQ; idx = sorted(range(len(a)), key=lambda i: (a[i], i))
    rows = [('i', [str(i) for i in range(len(a))], {}),
            ('key', a, {}),
            ('a[i]', [str(x) for x in idx], {i: 'hi' for i in range(len(a))}),
            ('key[a[i]]', [a[x] for x in idx], {i: 'ok' for i in range(len(a))})]
    F['ex6-index'] = ('fig', fig(array_rows(rows, 28, 70, 'index sort EASYQUESTION'),
        'تمرين 6.58: index sort. المفاتيح نفسها ماتحرّكتش. اللي اترتّب هو مصفوفة <code>a</code>: a[0] = 1 يعني «أصغر مفتاح في الخانة 1» (A). ولو قريت <code>key[a[i]]</code> بالترتيب، هتلاقيها مترتبة. وبعد كده Program 6.14 بيحرّك المفاتيح فعلًا بأقل عدد نقلات.'))
ex6_index()

def ex6_kic():
    s = list('ABRACADABRA'); keys = 'ABCDR'
    cnt = [0] * 6
    for x in s: cnt[keys.index(x) + 1] += 1
    c1 = cnt[:]
    for j in range(1, 6): cnt[j] += cnt[j - 1]
    c2 = cnt[:]
    b = [''] * len(s); c = cnt[:]
    for x in s: b[c[keys.index(x)]] = x; c[keys.index(x)] += 1
    rows = [('a', s, {}), ('cnt', [str(x) for x in c1], {i: 'hi2' for i in range(6)}),
            ('start', [str(x) for x in c2], {i: 'hi' for i in range(6)}), ('b', b, {i: 'ok' for i in range(len(b))})]
    F['ex6-kic'] = ('fig', fig(array_rows(rows, 28, 50, 'key-indexed counting ABRACADABRA'),
        'تمرين 6.74: key-indexed counting على ABRACADABRA (المفاتيح A B C D R = 0..4). صف cnt: الخانة j+1 فيها عدد المفتاح j (5 A، 2 B، 1 C، 1 D، 2 R). صف start بعد الجمع التراكمي: الـ A تبدأ من 0، والـ B من 5، والـ C من 7، والـ D من 8، والـ R من 9. وبعد كده بنوزّع من الشمال لليمين، فالترتيب مستقر.'))
ex6_kic()

# ---------- chapter 7 ----------
def ex7_trace():
    a = EQ[:]; rows = [('', a[:], {})]
    def part(l, r):
        i, j, v = l - 1, r, a[r]
        while True:
            i += 1
            while a[i] < v: i += 1
            j -= 1
            while v < a[j]:
                if j == l: break
                j -= 1
            if i >= j: break
            a[i], a[j] = a[j], a[i]
        a[i], a[r] = a[r], a[i]
        return i
    def qs(l, r):
        if r <= l: return
        i = part(l, r)
        cls = {k: 'dim' for k in range(len(a)) if k < l or k > r}
        cls[i] = 'piv'
        rows.append((f'[{l},{r}]', a[:], cls))
        qs(l, i - 1); qs(i + 1, r)
    qs(0, len(a) - 1)
    rows.append(('', a[:], {k: 'ok' for k in range(len(a))}))
    return array_rows(rows, 26, 54, 'quicksort (Program 7.1) on EASYQUESTION')
F['ex7-trace'] = ('fig', fig(ex7_trace(),
    'تتبّع تمرين 7.1: كل صف هو partition واحد على الـ subfile المكتوب على الشمال. العنصر الملوّن هو الـ partitioning element وقد استقر في مكانه النهائي، والرمادي برّه الـ subfile. لاحظ إن الـ subfiles اللي حجمها 1 مابيتعملّهاش صف، لأن <code>if (r &lt;= l) return</code>.'))

def ex7_best():
    def best(keys):
        n = len(keys)
        if n == 0: return []
        nl = (n - 1) // 2
        L = best(keys[:nl]); R = best(keys[nl + 1:])
        if len(R) > 1: R = [R[-1]] + R[:-1]
        return L + R + [keys[nl]]
    f = best(list(range(15)))
    hexs = '0123456789ABCDE'
    return array_rows([('file', [hexs[x] for x in f], {14: 'piv'}),
                       ('sorted', list(hexs), {7: 'piv'})], 26, 54, 'best-case file for N = 15 (keys 0..E)')
F['ex7-best'] = ('fig', fig(ex7_best(),
    'ملف best-case لـ N = 15 (تمرين 7.10): آخر عنصر (7) هو الـ median بالظبط، والنص الشمال كله أصغر منه واليمين كله أكبر، فالـ partition مابيعملش ولا exchange غير الأخير. والجزء اليمين متلفّ خطوة لليمين عشان بعد التبديل الأخير يرجع هو كمان best-case.'))

# ---------- chapter 8 ----------
def ex8_merges(bottom_up=False):
    a = EQ[:]; rows = [('', a[:], {})]
    def merge(l, m, r):
        a[l:r + 1] = sorted(a[l:r + 1])   # stable merge of two sorted halves
        cls = {k: 'dim' for k in range(len(a)) if k < l or k > r}
        for k in range(l, r + 1): cls[k] = 'hi' if k <= m else 'hi2'
        rows.append((f'{m - l + 1}-by-{r - m}', a[:], cls))
    def td(l, r):
        if r <= l: return
        m = (l + r) // 2; td(l, m); td(m + 1, r); merge(l, m, r)
    if bottom_up:
        m = 1; n = len(a)
        while m < n:
            for i in range(0, n - m, 2 * m): merge(i, i + m - 1, min(i + 2 * m - 1, n - 1))
            m *= 2
    else:
        td(0, len(a) - 1)
    return array_rows(rows, 24, 54, ('bottom-up' if bottom_up else 'top-down') + ' mergesort on EASYQUESTION')
F['ex8-td'] = ('fig', fig(ex8_merges(False),
    'تمرين 8.9: الـ merges اللي بيعملها Program 8.3 بالترتيب. الأزرق هو النص الشمال المترتب، والبنفسجي هو اليمين. لاحظ إن الشجرة بتخلّص الشمال كله (6 عناصر) قبل ما تلمس اليمين، يعني ده postorder بالظبط.'))
F['ex8-bu'] = ('fig', fig(ex8_merges(True),
    'تمرين 8.24: bottom-up بيعمل نفس الشغل بس «مستوى مستوى»: كل الـ 1-by-1 الأول، وبعدين الـ 2-by-2، وهكذا. والـ merge الأخير 8-by-4 مش متوازن، لأن 12 مش قوة لـ 2. وده الفرق الوحيد في الشكل عن top-down.'))

# ---------- chapter 9 ----------
def heap_tree(keys, hi=()):
    def mk(i):
        if i > len(keys): return None
        return N(keys[i - 1], mk(2 * i), mk(2 * i + 1), 'node hi' if i in hi else 'node')
    return mk(1)
F['ex9-heap'] = ('fig', fig(tree(heap_tree('YTUSQNEASIOE'), 30, 48, 14, 'heap after inserting EASYQUESTION'),
    'تمرين 9.21: الـ heap بعد ما ندخّل E A S Y Q U E S T I O N واحد واحد بـ fixUp. المصفوفة YTUSQNEASIOE هي نفس الشجرة دي متقرية مستوى مستوى. وكل أب أكبر من أو يساوي ولاده، ومافيش ترتيب بين الإخوات.'))
F['ex9-heapmin'] = ('fig', fig(tree(heap_tree([str(x) for x in [15, 14, 13, 4, 7, 11, 12, 2, 3, 5, 1, 6, 8, 9, 10]], hi=(15, 1, 2)), 30, 48, 14, 'delete-max with only 2 exchanges'),
    'تمرين 9.26: heap فيه 15 مفتاح، والـ delete-max بيعمل فيه exchange-ين بس. الـ 10 اللي في الآخر بتطلع للجذر، وتنزل خطوة واحدة مكان الـ 14، وتقف هناك لأن ولادها 4 و7 أصغر منها. والأقل من كده مستحيل لو المفاتيح مختلفة: الـ 10 جاية من تحت الـ 13، فلازم تبقى أصغر من الـ 13، فلازم تنزل خطوة على الأقل.'))

def bq_m(s):
    m = re.match(r'(\w)(?:\((.*)\))?$', s)
    key, rest = m.group(1), m.group(2)
    kids, depth, cur = [], 0, ''
    if rest:
        for ch in rest:
            if ch == ' ' and depth == 0: kids.append(cur); cur = ''; continue
            depth += (ch == '(') - (ch == ')'); cur += ch
        kids.append(cur)
    return M([key], [bq_m(k) for k in kids])
import re
F['ex9-binom'] = ('fig', fig(mtree(M([], [bq_m('Y(U(S(E) Q) E(A) S)'), bq_m('T(O(N) I)')], 'virtual'), kw=22, gap=14, dy=48, title='binomial queue for EASYQUESTION'),
    'تمرين 9.56: 12 = 1100 بالـ binary، فالـ binomial queue فيه شجرتين: B<sub>3</sub> (8 nodes، جذرها Y) وB<sub>2</sub> (4 nodes، جذرها T). كل شجرة heap-ordered، والجذر بتاع B<sub>k</sub> ليه k ولد أحجامهم B<sub>k−1</sub>، …، B<sub>0</sub>. والـ max لازم يبقى في واحد من الجذور، فبنشوف lg N جذر بس.'))

# ---------- chapter 10 ----------
def ex10_binq():
    f0 = [1, 3, 5, 6, 0, 1, 2, 7, 6, 2]
    def qb(a, l, r, w, lim, marks):
        if r <= l or w >= lim: return
        i, j = l, r
        while j != i:
            while ((a[i] >> (2 - w)) & 1) == 0 and i < j: i += 1
            while ((a[j] >> (2 - w)) & 1) == 1 and j > i: j -= 1
            a[i], a[j] = a[j], a[i]
        if ((a[r] >> (2 - w)) & 1) == 0: j += 1
        if w == lim - 1: marks.append(j)
        qb(a, l, j - 1, w + 1, lim, marks); qb(a, j, r, w + 1, lim, marks)
    rows = [('file', [format(x, '03b') for x in f0], {})]
    for lim in (1, 2, 3):
        a = f0[:]; marks = []; qb(a, 0, 9, 0, lim, marks)
        cls = {k: ('hi' if (a[k] >> (3 - lim)) & 1 else 'hi2') for k in range(10)}
        rows.append((f'bit {lim - 1}', [format(x, '03b') for x in a], cls))
    return array_rows(rows, 36, 50, 'binary quicksort (Program 10.1), level by level')
F['ex10-binq'] = ('fig', fig(ex10_binq(),
    'تمرين 10.9: binary quicksort على الملف بتاع التمرين، مستوى ورا مستوى. في كل صف الـ bit اللي اتقسم عليه ملوّن: البنفسجي 0 والأزرق 1. بعد أول partition كل اللي بيبدأ بـ 0 بقى على الشمال، ومش هيعدّي الخط ده تاني أبدًا. وبعد 3 مستويات (عدد الـ bits) الملف خلص.'))

# ---------- chapter 11 ----------
def ex11_batcher():
    steps = [('input', 'AEQSUY~~EINOST~~'), ('unshuffle', 'AQU~ENS~ESY~IOT~'), ('merge', 'AENQSU~~EIOSTY~~'),
             ('shuffle', 'AEEINOQSSTUY~~~~'), ('compexch', 'AEEINOQSSTUY~~~~')]
    rows = []
    for lab, s in steps:
        cls = {i: 'dim' for i, c in enumerate(s) if c == '~'}
        if lab == 'merge':
            for i in range(16):
                if s[i] != '~': cls[i] = 'hi' if i < 8 else 'hi2'
        rows.append((lab, [('∞' if c == '~' else c) for c in s], cls))
    return array_rows(rows, 24, 70, "Batcher's odd-even merge, padded to 8 + 8")
F['ex11-batcher'] = ('fig', fig(ex11_batcher(),
    'تمرين 11.8: Program 11.2 محتاج نصين حجمهم قوة لـ 2. فبنكمّل كل نص بـ ∞ (رمادي) لحد 8. الـ unshuffle بيفصل المواقع الزوجية عن الفردية، وكل نص بيتدمج لوحده recursively (أزرق وبنفسجي)، والـ shuffle بيرجّعهم متداخلين. وفي الآخر compare-exchange بين الجيران (1,2)، (3,4)، … . هنا الخطوة الأخيرة ماغيّرتش حاجة، بس في 11.9 بدّلت U وT.'))

# ---------- chapter 12 ----------
def ptree(s, hi=()):
    """parse 'E(A,S(-,Y))' into svglib N nodes"""
    pos = [0]
    def node():
        c = s[pos[0]]; pos[0] += 1
        if c == '-': return None
        l = r = None
        if pos[0] < len(s) and s[pos[0]] == '(':
            pos[0] += 1; l = node(); pos[0] += 1; r = node(); pos[0] += 1
        return N(c, l, r, 'node hi' if c in hi else 'node')
    return node()
F['ex12-bst'] = ('fig', fig(tree(ptree('E(A,S(Q(E(-,I(-,O(N,-))),-),Y(U(S(-,T),-),-)))'), 30, 44, 14, 'BST of EASYQUESTION (standard insertion)'),
    'تمرين 12.45: الـ BST بعد ما ندخّل E A S Y Q U E S T I O N بالترتيب. المفاتيح المتساوية بتروح يمين (Program 12.7)، فالـ E التانية نزلت تحت الـ Q، والـ S التانية تحت الـ U. الارتفاع 7، ومتوسط تكلفة الـ search hit = 48/12 = 4 مقارنات.'))
F['ex12-root'] = ('fig', fig(tree(ptree('N(I(E(A(-,E),-),-),O(-,T(S(Q(-,S),-),U(-,Y))))'), 30, 44, 14, 'same keys, root insertion'),
    'تمرين 12.70: نفس المفاتيح بـ root insertion. آخر مفتاح دخل (N) هو الجذر، وقبله O وI قريبين منه. يعني الـ root insertion بيخلّي «الجديد قريب من فوق»، وده بالظبط اللي بيقيسه تمرين 12.74.'))

# ---------- chapter 13 ----------
def ptree_rb(s):
    pos = [0]
    def node():
        c = s[pos[0]]; pos[0] += 1
        if c == '-': return None
        red = False
        if pos[0] < len(s) and s[pos[0]] == '*': red = True; pos[0] += 1
        l = r = None
        if pos[0] < len(s) and s[pos[0]] == '(':
            pos[0] += 1; l = node(); pos[0] += 1; r = node(); pos[0] += 1
        return N(c, l, r, 'node', red)
    return node()
F['ex13-234'] = ('fig', fig(mtree(M(list('EOS'), [M(['A']), M(list('IN')), M(['Q']), M(list('TUY'))]), kw=22, gap=16, dy=56, title='2-3-4 tree for EASYQUTION'),
    'تمرين 13.39/13.40: شجرة 2-3-4 بعد ما ندخّل E A S Y Q U T I O N. هنا top-down وbottom-up طلّعوا نفس الشجرة. كل الأوراق على نفس العمق، وفيه 4-node واحدة في الجذر (E O S) و4-node في الأوراق (T U Y). يعني أول insert جاي هيعمل split.'))
F['ex13-rb'] = ('fig', fig(tree(ptree_rb('O(E*(A,I(-,N*)),S*(Q,U(T*,Y*)))'), 32, 46, 14, 'red-black BST for EASYQUTION'),
    'تمرين 13.48: نفس الشجرة كـ red-black BST (الـ links الحمرا = nodes جوه نفس الـ 2-3-4 node). O مع E وS الحمر هم الجذر [E O S]، وT وY الحمر مع U هم [T U Y]، وI مع N الحمرا هما [I N]. وكل مسار من الجذر لـ null فيه نفس عدد الـ nodes السودا (2).'))

# ---------- chapter 14 ----------
def ex14_tables():
    lin = list('.S.YIO.EU.NAQT..'); dbl = list('.S.YIO.EU.NAT.Q.')
    moved_lin = {4: 'hi2', 8: 'hi2', 12: 'hi2', 13: 'hi2'}  # I, U, Q and T pushed by collisions
    moved_dbl = {4: 'hi2', 8: 'hi2', 14: 'hi2'}            # I, U, Q (Q jumps by 3)
    rows = [('i', [str(i) for i in range(16)], {}),
            ('linear', [' ' if c == '.' else c for c in lin], moved_lin),
            ('double', [' ' if c == '.' else c for c in dbl], moved_dbl)]
    return array_rows(rows, 26, 60, 'EASYQUTION, M = 16, h = 11k mod 16')
F['ex14-probe'] = ('fig', fig(ex14_tables(),
    'تمرين 14.25 و14.31: نفس المفاتيح في جدول حجمه 16. البنفسجي هي المفاتيح اللي ماقعدتش في مكانها الأصلي. في الـ linear probing الـ Q (h = 11) لقت A فنزلت خطوة لـ 12، فلما جت الـ T (h = 12) اتزقّت هي كمان لـ 13. وده الـ clustering. وفي الـ double hashing الـ Q نطّت بـ 3 لـ 14، فسابت الخانة 12 فاضية، وقعدت فيها الـ T من غير ولا collision. والـ I والـ U نفس الحكاية في الاتنين، لأن خطوتهم 1.'))
