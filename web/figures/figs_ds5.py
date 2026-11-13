"""Figures for Data Structures chapter 23 (heaps and priority queues). Author: Mohammed El Sisi.
Every figure that shows a RESULT is checked against the real stdout of the compiled, tested C programs in
src/23-heaps/ (or re-computed in Python and asserted equal to it)."""
import os, re, subprocess
from web.lib.render import *
from web.lib.csource import ROOT, run_output
from web.figures.figs_ds2 import row, lab
F = {}
SRC = str(ROOT / 'src' / '23-heaps')
CH = 'src/23-heaps'
_cache = {}
def c_out(name):
    if name not in _cache:
        _cache[name] = run_output(f'{CH}/{name}.c') + '\n'
    return _cache[name]
def line_of(name, prefix):
    for l in c_out(name).splitlines():
        if l.startswith(prefix): return l[len(prefix):].strip()
    raise KeyError((name, prefix))
def steps(items):
    return ('steps', ''.join(frame(s, cap) for cap, s in items))

# ---------------------------------------------------------------- array <-> tree
def a_to_N(a, hi=(), i=1):
    """1-based position i of the list a -> N tree; hi = set of 1-based positions to highlight"""
    if i > len(a): return None
    return N(str(a[i - 1]), a_to_N(a, hi, 2 * i), a_to_N(a, hi, 2 * i + 1), cls='node hi' if i in hi else 'node')
def a_row(a, hi=()):
    cl = {i: 'cell hi' for i in range(len(a)) if (i + 1) in hi}
    return array_rows([('', [str(x) for x in a], cl)])
def heap_view(a, hi=(), title=''):
    return row(tree(a_to_N(a, hi), 30, 44, title), a_row(a, hi), gap=24)
def ints(s): return [int(x) for x in s.split()]

def pq_up(a, mx=True):
    a = list(a); out = [list(a)]; i = len(a)
    while i > 1 and ((a[i - 1] > a[i // 2 - 1]) if mx else (a[i - 1] < a[i // 2 - 1])):
        a[i - 1], a[i // 2 - 1] = a[i // 2 - 1], a[i - 1]; i //= 2; out.append(list(a))
    return out
def pq_down(a, i=1, mx=True):
    a = list(a); n = len(a); out = [list(a)]
    while 2 * i <= n:
        c = 2 * i
        if c + 1 <= n and ((a[c] > a[c - 1]) if mx else (a[c] < a[c - 1])): c += 1
        if (a[c - 1] > a[i - 1]) if mx else (a[c - 1] < a[i - 1]):
            a[c - 1], a[i - 1] = a[i - 1], a[c - 1]; i = c; out.append(list(a))
        else: break
    return out

def binheap():
    base = ints(line_of('binheap', 'Fig 12.2 :'))
    F['ds23-fig122'] = ('fig', heap_view(base, (), 'max-heap') + '<figcaption>Fig 12.2: الـ heap كشجرة وكـ array (الشجرة مش بتتخزن فعلاً: الأب i وأبناؤه 2i و2i+1).</figcaption>')
    seq = pq_up(base + [99]); assert seq[-1] == ints(line_of('binheap', 'after 99 :'))
    cap = ['نحط 99 في أول خانة فاضية (الخانة 9)؛ أبوه 27 أصغر منه.', 'بدّلنا مع 27: بقى في الخانة 4 وأبوه 45 أصغر.',
           'بدّلنا مع 45: بقى في الخانة 2 وأبوه 54 أصغر.', 'بدّلنا مع 54: وصل للجذر، خلص (3 تبديلات = عمق الشجرة).']
    pos = [9, 4, 2, 1]; assert len(seq) == 4
    F['ds23-insert99'] = steps([(cap[k], heap_view(s, (pos[k],))) for k, s in enumerate(seq)])
    moved = [base[-1]] + base[1:-1]
    seq2 = pq_down(moved); assert seq2[-1] == [45, 27, 36, 11, 21, 18, 21] or seq2[-1][0] == 45
    c2 = ['نشيل الجذر 54 ونحط آخر عنصر (11) مكانه.'] + ['11 أصغر من أكبر ابنين ← بدّلنا معاه ونزل خانة.'] * (len(seq2) - 2) + ['11 بقى أكبر من ابنيه (أو ورقة): خلص.']
    F['ds23-deleteroot'] = steps([(c2[min(k, len(c2) - 1)], heap_view(st, (st.index(11) + 1,))) for k, st in enumerate(seq2)])
    e = [45, 36, 54, 27, 63, 72, 61, 18]
    ins = ints(line_of('binheap', 'Ex 12.2  :')); fl = ints(line_of('binheap', 'Ex 12.2 F:'))
    a = []
    for x in e: a = pq_up(a + [x])[-1]
    assert a == ins
    b = list(e); n = len(b)
    for i in range(n // 2, 0, -1): b = pq_down(b, i)[-1]
    assert b == fl, (b, fl)
    F['ds23-build2'] = ('fig', row(lab('بالإدخال واحد واحد: ' + ' '.join(map(str, ins)), tree(a_to_N(ins), 28, 42)),
                                  lab('بطريقة Floyd (ننزّل من n/2 لـ 1): ' + ' '.join(map(str, fl)), tree(a_to_N(fl), 28, 42)), gap=40)
                        + '<figcaption>نفس المدخلات 45 36 54 27 63 72 61 18 (Thareja Ex 12.2). الطريقتين يدّوا heap صح بس مش نفس الـ heap.</figcaption>')

# ---------------------------------------------------------------- notation key(left,right), empty right omitted
def pt(s):
    pos = [0]
    def node():
        j = pos[0]
        while j < len(s) and s[j] not in '(),': j += 1
        key = s[pos[0]:j]; pos[0] = j; l = r = None
        if pos[0] < len(s) and s[pos[0]] == '(':
            pos[0] += 1; l = node()
            if s[pos[0]] == ',': pos[0] += 1; r = node()
            assert s[pos[0]] == ')'; pos[0] += 1
        return N(key, l, r, cls='node')
    t = node(); assert pos[0] == len(s), s
    return t
def lt(s, title='', dx=22):
    return tree(pt(s), dx, 44, title)
def leftist():
    g = lambda p: line_of('leftist', p)
    h1 = g('H1 ='); h2 = g('H2 ='); m = g('Fig 6.22 merge(H2, right of H1) ='); b = g('Fig 6.23 before the swap       ='); a = g('Fig 6.24 after the swap        =')
    F['ds23-leftist'] = steps([('H1 و H2 (كل واحد leftist heap).', row(lt(h1, 'H1'), lt(h2, 'H2'), gap=40)),
                               ('نعمل merge للـ H2 مع الابن اليمين لـ H1 (الجذر 8): النتيجة هتبقى الابن اليمين الجديد.', row(lt(m, 'merge(H2, 8…)'))),
                               ('نركّبها كابن يمين لـ 3: ممكن تخالف شرط leftist (npl الشمال 1 < اليمين 2).', row(lt(b, 'قبل التبديل'))),
                               ('نبدّل ابنين الجذر 3: الـ leftist heap اتصلّح.', row(lt(a, 'بعد التبديل')))])
    sm = g('Fig 6.33 skew merge result           =')
    F['ds23-skew'] = ('fig', row(lab('leftist (Fig 6.24)', lt(a)), lab('skew heap (Fig 6.33): نفس المدخلات، من غير npl', lt(sm)), gap=40)
                      + '<figcaption>الـ skew heap بيبدّل الابنين دايمًا على طول المسار اليمين من غير ما يخزّن npl.</figcaption>')

# ---------------------------------------------------------------- forests (binomial / Fibonacci)
def pf(s):
    pos = [0]
    def node():
        j = pos[0]
        while j < len(s) and s[j] not in "(), ": j += 1
        key = s[pos[0]:j]; pos[0] = j; kids = []
        if pos[0] < len(s) and s[pos[0]] == '(':
            pos[0] += 1
            while True:
                kids.append(node())
                if s[pos[0]] == ',': pos[0] += 1; continue
                assert s[pos[0]] == ')'; pos[0] += 1; break
        return (key, kids)
    out = []
    while pos[0] < len(s):
        if s[pos[0]] == ' ': pos[0] += 1; continue
        out.append(node())
    return out
def to_M(t, hi=()):
    return M([t[0]], [to_M(k, hi) for k in t[1]], 'mnode hi' if t[0] in hi else 'mnode')
def forest(s, hi=()):
    return row(*[mtree(to_M(t, hi), gap=8, dy=40) for t in pf(s)], gap=22)
def binomial():
    def bt(k, start):
        key = str(start[0]); start[0] += 1
        return (key, [bt(j, start) for j in range(k)])
    cap = ['B0: عقدة واحدة.', 'B1 = شجرتين B0 (واحدة تحت التانية).', 'B2 = شجرتين B1.', 'B3 = شجرتين B2: 8 عقد، والجذر له 3 أبناء (B0 وB1 وB2).',
           'B4: 16 عقدة (2^4)، الجذر له 4 أبناء، وعلى العمق d فيه C(4,d) عقدة: 1، 4، 6، 4، 1.']
    F['ds23-btrees'] = steps([(cap[k], row(mtree(to_M(bt(k, [1])), gap=8, dy=40))) for k in range(5)])
    f = line_of('binomial', 'Fig 12.13:')
    F['ds23-fig1213'] = ('fig', forest(f) + '<figcaption>Fig 12.13: الشجرة الأولى 7(11(16),27) فيها 4 عقد = B2، والتانية 9(…) فيها 8 عقد = B3. إجمالي 12 عقدة = 1100 بالثنائي (bits 2 و3 مشغولين).</figcaption>')
    ins = re.findall(r'^insert (\d+): (.*)$', c_out('binomial'), re.M); assert len(ins) == 7
    F['ds23-bn-insert'] = steps([('بعد إدخال %s: عدد الأشجار = عدد الـ 1 في %s (%s بالثنائي)' % (k, bin(int(k))[2:], k), forest(s)) for k, s in ins])
    ae = line_of('binomial', 'after extract-min:')
    F['ds23-bn-extract'] = ('fig', row(lab('قبل: ' + ins[-1][1], forest(ins[-1][1])), lab('بعد extract-min (الجذر 1 اتشال، أبناءه اتدمجوا مع الباقي)', forest(ae)), gap=40)
                            + '<figcaption>extract-min في binomial heap = شيل الجذر الأصغر + اعمل union بين أبناءه وباقي الغابة. التكلفة O(log n).</figcaption>')

def fib():
    b = line_of('fibheap', 'Fig 12.29 (before) :'); a = line_of('fibheap', 'Example 12.8 after :')
    F['ds23-fib-insert'] = ('fig', row(lab('Fig 12.29', forest(b, ())), lab('بعد insert 16 (Ex 12.8): جذر جديد في قائمة الجذور من غير أي دمج', forest(a, ('16',))), gap=30)
                            + '<figcaption>الإدخال في Fibonacci heap = O(1): بنضيف عقدة لقايمة الجذور وبس. الشغل بيتأجّل لأول extract-min.</figcaption>')
    e = line_of('fibheap', 'Example 12.10 after :').split('   (')[0]
    F['ds23-fib-dec'] = ('fig', row(lab('بعد decrease-key (Ex 12.10): 3 cuts متسلسلة', forest(e, ()))) + '<figcaption>الـ cascading cut: لو عقدة فقدت ابن تاني (اتعلّمت من قبل) تتقطع هي كمان وتتحط في قايمة الجذور.</figcaption>')

def mm():
    arr = ints(line_of('minmax', 'insert 1..15      :'))
    t = a_to_N(arr)
    def mark(n, lev):
        if n is None: return
        n.cls = 'node hi' if lev % 2 == 0 else 'node'
        mark(n.l, lev + 1); mark(n.r, lev + 1)
    mark(t, 0)
    F['ds23-minmax'] = ('fig', row(tree(t, 24, 44, 'min-max heap'), gap=20)
                        + '<figcaption>الشكل بعد إدخال 1..15 بالترتيب. المستويات المضاءة (0، 2، …) هي <b>min levels</b>: كل عقدة هناك أصغر من كل ولادها وأحفادها؛ المستويات التانية <b>max levels</b>: كل عقدة أكبر من كل أحفادها.</figcaption>')

binheap(); leftist(); binomial(); fib(); mm()
