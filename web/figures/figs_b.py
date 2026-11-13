"""Figures for chapters 6-11 (sorting). Author: Mohammed El Sisi."""
from web.lib.render import *
F = {}
S = list('ASORTINGEXAMPLE')

def selection_trace():
    a = S[:]; rows = [('', a[:], {})]
    for i in range(len(a) - 1):
        m = min(range(i, len(a)), key=lambda k: (a[k], k))
        a[i], a[m] = a[m], a[i]
        c = {k: 'dim' for k in range(i)}; c[i] = 'hi'; c[m] = 'hi2' if m != i else 'hi'
        rows.append((str(i), a[:], c))
    F['sel-trace'] = ('fig', fig(array_rows(rows, 26), 'Selection sort على <code>ASORTINGEXAMPLE</code>: في كل سطر، المظلّل هو الخانة i اللي اتحط فيها الأصغر، والمحدّد هو المكان اللي الأصغر كان فيه (اتبدّلوا). الباهت خلاص في مكانه النهائي. خلّي بالك: الباهت مابيتلمسش تاني أبدًا.'))

def insertion_trace():
    a = S[:]; rows = [('', a[:], {})]
    for i in range(1, len(a)):
        v = a[i]; j = i
        while j > 0 and v < a[j - 1]: a[j] = a[j - 1]; j -= 1
        a[j] = v
        c = {k: 'dim' for k in range(i + 1, len(a))}
        for k in range(j + 1, i + 1): c[k] = 'hi2'
        c[j] = 'hi'
        rows.append((str(i), a[:], c))
    F['ins-trace'] = ('fig', fig(array_rows(rows, 26), 'Insertion sort: في السطر i، العنصر المظلّل هو اللي «اتزق» شمال لحد مكانه، والمحدّد هم اللي اتزحزحوا خطوة يمين. الجزء الباهت على اليمين لسه ماتلمسش. عدد الخانات المحدّد في كل سطر = عدد الـ inversions اللي اتصلحت.'))

def bubble_trace():
    a = S[:]; rows = [('', a[:], {})]
    n = len(a)
    for i in range(n - 1):
        moved = set()
        for j in range(n - 1, i, -1):
            if a[j] < a[j - 1]: a[j], a[j - 1] = a[j - 1], a[j]; moved.add(j); moved.add(j - 1)
        c = {k: 'dim' for k in range(i)}; c[i] = 'hi'
        for k in moved:
            if k > i: c[k] = 'hi2'
        rows.append((str(i), a[:], c))
    F['bub-trace'] = ('fig', fig(array_rows(rows, 26), 'Bubble sort: كل لفّة من اليمين للشمال بتطلّع أصغر عنصر باقي للخانة i (المظلّل). المحدّد: عناصر اتحركت في اللفّة. لاحظ إن الآخر بيتحسّن ببطء — العناصر الكبيرة بتمشي خطوة واحدة يمين في كل لفّة.'))

def shell_trace():
    a = list('ASORTINGEXAMPLE'); rows = [('', a[:], {})]
    for h in (13, 4, 1):
        for i in range(h, len(a)):
            v = a[i]; j = i
            while j >= h and v < a[j - h]: a[j] = a[j - h]; j -= h
            a[j] = v
        rows.append((f'h={h}', a[:], {k: ('hi' if (k % h == 0 and h > 1) else 'cell') for k in range(len(a))}))
    F['shell-trace'] = ('fig', fig(array_rows(rows, 44), 'Shellsort بالزيادات 13، 4، 1: بعد 13-sort، بعد 4-sort (المظلّل متتابعة واحدة من الأربعة: الخانات 0، 4، 8، 12 — مترتبة جواها)، وبعد 1-sort. شوف قد إيه الملف «شبه مترتب» قبل اللفّة الأخيرة، فالـ insertion sort الأخير بيخلص بسرعة.'))

def kic_trace():
    a = list('31202130102'); R = 4
    cnt = [0] * (R + 1)
    for x in a: cnt[int(x) + 1] += 1
    c1 = cnt[:]
    for r in range(R): cnt[r + 1] += cnt[r]
    c2 = cnt[:]
    b = [''] * len(a); c = cnt[:]
    for x in a: b[c[int(x)]] = x; c[int(x)] += 1
    rows = [('a', a, {}), ('cnt', c1, {i: 'hi2' for i in range(R + 1)}), ('start', c2, {i: 'hi' for i in range(R + 1)}), ('b', b, {i: 'ok' for i in range(len(b))})]
    F['kic-trace'] = ('fig', fig(array_rows(rows, 50), 'Key-indexed counting على مفاتيح من 0 لـ 3: (1) <code>cnt[k+1]</code> = عدد المفتاح k. (2) الجمع التراكمي يدّي <b>مكان بداية</b> كل مفتاح: الأصفار تبدأ من 0، الواحدات من 3، الاتنينات من 7… (3) التوزيع بالترتيب يدّي b مترتب ومستقر.'))

def partition_steps():
    a = S[:]; l, r = 0, len(a) - 1; v = a[r]; i, j = l - 1, r; fr = []
    def snap(cap, done=False):
        c = {r: 'piv'}
        if 0 <= i < len(a): c[i] = 'hi'
        if 0 <= j < len(a) and j != i: c[j] = 'hi2'
        if done: c = {k: ('dim' if k != i else 'ok') for k in range(len(a))}
        idx = [''] * len(a)
        if 0 <= i < len(a): idx[i] = 'i'
        if 0 <= j < len(a): idx[j] = 'j' if j != i else 'ij'
        fr.append(frame(array_rows([('', a[:], c), ('', idx, {k: 'dim' for k in range(len(a))})], 0), cap))
    snap(f'الـ pivot هو آخر عنصر <b>{v}</b> (مظلّل). i هيمشي من الشمال وj من اليمين.')
    while True:
        i += 1
        while a[i] < v: i += 1
        j -= 1
        while v < a[j] and j > l: j -= 1
        if i >= j:
            snap(f'i ({i}) عدّى j ({j}): خلاص. الشمال كله ≤ {v} واليمين ≥ {v}.')
            a[i], a[r] = a[r], a[i]
            snap(f'بدّلنا a[i] مع الـ pivot: <b>{v}</b> في مكانه النهائي (خانة {i}). نرتّب الشمال واليمين ريكيرسف.', True); break
        snap(f'i وقف عند {a[i]} (مش أصغر من {v})، وj وقف عند {a[j]} (مش أكبر). الاتنين في الجهة الغلط — هنبدّلهم.')
        a[i], a[j] = a[j], a[i]
        snap(f'بعد التبديل: {a[i]} شمال و{a[j]} يمين. نكمّل المسح.')
    F['partition'] = ('steps', ''.join(fr))

def quick_trace():
    a = S[:]; rows = [('', a[:], {})]
    def part(l, r):
        v = a[r]; i, j = l - 1, r
        while True:
            i += 1
            while a[i] < v: i += 1
            j -= 1
            while v < a[j] and j > l: j -= 1
            if i >= j: break
            a[i], a[j] = a[j], a[i]
        a[i], a[r] = a[r], a[i]; return i
    def qs(l, r):
        if r <= l:
            if r == l: rows.append(('', a[:], {**{k: 'dim' for k in range(len(a)) if k != l}, l: 'ok'}))
            return
        i = part(l, r)
        c = {k: 'dim' for k in range(len(a)) if k < l or k > r}; c[i] = 'piv'
        rows.append(('', a[:], c)); qs(l, i - 1); qs(i + 1, r)
    qs(0, len(a) - 1)
    F['quick-trace'] = ('fig', fig(array_rows(rows, 0), 'quicksort كامل على <code>ASORTINGEXAMPLE</code>: كل سطر partition واحد. المظلّل هو الـ pivot اللي استقر في مكانه، والجزء غير الباهت هو الملف الفرعي اللي اتقسم. السطور بعنصر رمادي واحد: ملفات حجمها 1 (خلصانة). اقرا العمود الأوسط من فوق لتحت: كل عنصر بيستقر مرة واحدة.'))

def three_way_steps():
    a = list('ABRACADABRACADABRA'); v = a[-1]
    lt = [x for x in a if x < v]; eq = [x for x in a if x == v]; gt = [x for x in a if x > v]
    res = lt + eq + gt
    rows = [('in', a, {len(a) - 1: 'piv'}), ('out', res, {**{k: 'hi' for k in range(len(lt))}, **{k: 'piv' for k in range(len(lt), len(lt) + len(eq))}, **{k: 'hi2' for k in range(len(lt) + len(eq), len(a))}})]
    F['three-way'] = ('fig', fig(array_rows(rows, 34), f'3-way partitioning بالـ pivot <b>{v}</b>: أصغر (مظلّل) | <b>مساوي</b> (مظلّل) | أكبر (محدّد). كل الـ {len(eq)} نسخ من {v} استقروا مرة واحدة وماحدش منهم هيدخل ريكيرشن تاني. ملف فيه مفاتيح متكررة كتير بيخلص بسرعة جدًا.'))

def merge_trace():
    a = list('ASORTINGEXAMPLE'); rows = [('', a[:], {})]
    def ms(l, r):
        if r <= l: return
        m = (l + r) // 2; ms(l, m); ms(m + 1, r)
        a[l:r + 1] = sorted(a[l:r + 1])
        rows.append((f'{l}-{r}', a[:], {**{k: 'dim' for k in range(len(a)) if k < l or k > r}, **{k: 'hi' for k in range(l, r + 1)}}))
    ms(0, len(a) - 1)
    F['merge-td'] = ('fig', fig(array_rows(rows, 44), 'Top-down mergesort: كل سطر merge واحد للجزء المظلّل (الحدود على الشمال). لاحظ الترتيب: الشمال كله بيخلص قبل ما اليمين يبدأ — بالظبط ترتيب الـ postorder في شجرة الريكيرشن.'))
    a = list('ASORTINGEXAMPLE'); rows = [('', a[:], {})]; m = 1
    while m < len(a):
        for i in range(0, len(a) - m, 2 * m):
            a[i:min(i + 2 * m, len(a))] = sorted(a[i:min(i + 2 * m, len(a))])
        rows.append((f'm={m}', a[:], {k: ('hi' if (k // (2 * m)) % 2 == 0 else 'hi2') for k in range(len(a))})); m *= 2
    F['merge-bu'] = ('fig', fig(array_rows(rows, 44), 'Bottom-up mergesort: كل سطر لفّة كاملة. الألوان المتبادلة بتوضح الملفات المترتبة بعد اللفّة (حجمها 2، 4، 8، 16). 4 لفّات = ⌈lg 15⌉.'))

def merge_tree():
    def mk(l, r):
        n = r - l + 1
        if n == 1: return N('1')
        m = (l + r) // 2; return N(str(n), mk(l, m), mk(m + 1, r))
    F['merge-tree'] = ('fig', fig(tree(mk(0, 10), dx=26, dy=44), 'شجرة الـ divide-and-conquer لـ mergesort على N = 11: كل عقدة = حجم merge. مجموع الأرقام في كل مستوى ≤ N، وفيه ⌈lg N⌉ مستوى. وشكلها بيعتمد على N بس — مش على البيانات (Property 8.4).'))

def heap_tree(a, hi=(), hi2=()):
    n = len(a) - 1
    def mk(k):
        if k > n: return None
        return N(a[k], mk(2 * k), mk(2 * k + 1), 'node hi' if k in hi else 'node hi2' if k in hi2 else 'node')
    return tree(mk(1), dx=30, dy=46)

def heap_steps():
    a = [None]; fr = []
    for ch in 'ASORTINGEXAMPLE':
        a.append(ch); k = len(a) - 1; path = [k]
        while k > 1 and a[k // 2] < a[k]:
            a[k], a[k // 2] = a[k // 2], a[k]; k //= 2; path.append(k)
        fr.append(frame(heap_tree(a, hi=(k,), hi2=tuple(path[:-1])), f'insert <b>{ch}</b>: اتحط في الآخر (خانة {len(a)-1}) وطلع بـ fixUp {len(path)-1} مستوى. المظلّل مكانه النهائي، والمحدّد اللي نزل مكانه.'))
    F['heap-insert'] = ('steps', ''.join(fr))
    a = [None] + list('ASORTINGEXAMPLE'); n = 15; fr = [frame(heap_tree(a), 'heapsort — مرحلة البناء: المصفوفة زي ما هي، كشجرة كاملة. هنعمل fixDown من الخانة 7 لحد 1.')]
    def fixdown(k, n):
        path = [k]
        while 2 * k <= n:
            j = 2 * k
            if j < n and a[j] < a[j + 1]: j += 1
            if not a[k] < a[j]: break
            a[k], a[j] = a[j], a[k]; k = j; path.append(k)
        return path
    for k in range(n // 2, 0, -1):
        p = fixdown(k, n)
        fr.append(frame(heap_tree(a, hi=(p[-1],), hi2=tuple(p[:-1])), f'fixDown من الخانة {k}: نزل {len(p)-1} مستوى. كل الشجرة اللي تحت الخانة {k} بقت heap.'))
    while n > 1:
        a[1], a[n] = a[n], a[1]; n -= 1; p = fixdown(1, n)
        if n in (14, 13, 12, 8, 4, 1):
            b = a[:n + 1]
            fr.append(frame(heap_tree(b, hi2=tuple(p)) + array_rows([('', a[1:], {k: ('ok' if k >= n else 'cell') for k in range(15)})], 0),
                            f'شيلنا الأكبر وحطيناه في الآخر (الرمادي = مترتب نهائي). باقي في الـ heap {n} عنصر.'))
    F['heapsort-steps'] = ('steps', ''.join(fr))

def binomial_fig():
    def p2(keys):
        # power-of-2 heap from sorted-desc keys, simple build by pairing
        hs = [N(k) for k in keys]
        while len(hs) > 1:
            nh = []
            for x, y in zip(hs[0::2], hs[1::2]):
                big, small = (x, y) if x.key >= y.key else (y, x)
                small.r = big.l; big.l = small; nh.append(big)
            hs = nh
        return hs[0]
    t8 = p2(list('XTSRPONM')); t4 = p2(list('LIGE')); t1 = p2(['A'])
    F['binomial'] = ('fig', fig(f'<div style="display:flex;gap:16px;justify-content:safe center;align-items:flex-start;flex-wrap:wrap;direction:ltr"><div><small>bq[3] (8)</small><br>{tree(t8, dx=26, dy=40)}</div><div><small>bq[2] (4)</small><br>{tree(t4, dx=26, dy=40)}</div><div><small>bq[0] (1)</small><br>{tree(t1, dx=26, dy=40)}</div></div>',
        'binomial queue فيه 13 عنصر = 1101 بالثنائي: power-of-2 heaps بأحجام 8 و4 و1، ومفيش heap بحجم 2 (الخانة bq[1] فاضية). كل جذر أكبر من كل اللي في شجرته الشمال، واليمين بتاع الجذر فاضي.'))

def lsd_trace():
    w = 'now for tip ilk dim tag jot sob nob sky hut ace bet men egg few'.split()
    cols = [w[:]]
    for d in (2, 1, 0):
        w = sorted(w, key=lambda s: s[d]); cols.append(w[:])
    W = 4 * 90 + 20; H = len(w) * 20 + 40; out = [DEFS]
    heads = ['input', 'by 3rd', 'by 2nd', 'by 1st']
    for c, (col, hd) in enumerate(zip(cols, heads)):
        x = 10 + c * 90; out.append(text(x + 40, 16, hd, 'elab', 'middle', 12))
        d = [None, 2, 1, 0][c]
        for r_, s in enumerate(col):
            y = 34 + r_ * 20
            for k, ch in enumerate(s):
                out.append(text(x + 28 + k * 12, y, ch, 'elab' if k == d else 'tc', 'middle', 13))
    F['lsd-trace'] = ('fig', fig(svg(W, H, ''.join(out)), 'LSD radix sort على كلمات 3 حروف: كل عمود هو نتيجة ترتيب <b>مستقر</b> على الحرف الملوّن، من اليمين للشمال. في العمود الأخير، الكلمات اللي بتبدأ بنفس الحرف (زي nob وnow، أو sky وsob) محتفظة بترتيبها من العمود اللي قبله — وده بالظبط اللي بيخلّي النتيجة صح.'))

def msd_fig():
    words = 'she sells seashells by the sea shore the shells she sells are surely seashells'.split()
    def mk(ws, d):
        groups = {}
        for x in ws:
            groups.setdefault(x[d] if d < len(x) else '$', []).append(x)
        kids = []
        for ch in sorted(groups):
            g = groups[ch]
            if ch == '$' or len(set(g)) == 1 and len(g[0]) <= d + 1:
                node = M([g[0] + (f'×{len(g)}' if len(g) > 1 else '')], [], 'mnode hi'); node.bw = 9 * len(node.keys[0]) + 10; node.label = ch; kids.append(node)
            elif len(set(g)) == 1:
                node = M([g[0] + (f'×{len(g)}' if len(g) > 1 else '')], [], 'mnode hi'); node.bw = 9 * len(node.keys[0]) + 10; node.label = ch; kids.append(node)
            else:
                node = mk(g, d + 1); node.label = ch; kids.append(node)
        return M([], kids, 'mnode')
    t = mk(words, 0)
    F['msd-trie'] = ('fig', fig(mtree(t, gap=10, dy=52, edge_labels=True), 'شجرة الريكيرشن بتاعة MSD radix sort على كلمات الكتاب: كل نقطة = نداء على ملف فرعي، والحرف على الخط = الصندوق. الأوراق الملوّنة = مفتاح وصل لوحده فوقف (و«×2» يعني نسختين متطابقتين). دي نفسها شكل الـ trie (الدرس 35).'))

def batcher_fig(Nn=8):
    # exact comparator list printed by src/11-special-sorts/batcher_network.c for N = 8
    comps = [tuple(map(int, c.split(':'))) for c in '0:1 2:3 0:2 1:3 1:2 4:5 6:7 4:6 5:7 5:6 0:4 2:6 2:4 1:5 3:7 3:5 1:2 3:4 5:6'.split()]
    W = len(comps) * 22 + 40; H = Nn * 26 + 20; out = [DEFS]
    for k in range(Nn):
        out.append(line(20, 15 + k * 26, W - 10, 15 + k * 26, 'edge')); out.append(text(10, 19 + k * 26, k, 'lab', 'middle', 11))
    for c, (i, j) in enumerate(comps):
        x = 32 + c * 22
        out.append(line(x, 15 + i * 26, x, 15 + j * 26, 'edge hi'))
        out.append(f'<circle cx="{x}" cy="{15+i*26}" r="5" class="dot"/><circle cx="{x}" cy="{15+j*26}" r="5" class="dot"/>')
    F['batcher-net'] = ('fig', fig(svg(W, H, ''.join(out)), f'شبكة Batcher لـ N = {Nn}: كل خط أفقي «سلك»، وكل خط رأسي comparator بيبدّل لو العلوي أكبر. {len(comps)} comparator. الترتيب ثابت مهما كانت البيانات (nonadaptive)، فالـ comparators اللي مابيلمسوش نفس الأسلاك ينفع يشتغلوا في نفس اللحظة.'))

def runs_fig():
    rows = [('runs', ['1000'] * 8, {k: 'hi' for k in range(8)}), ('pass1', ['4000', '', '', '', '4000', '', '', ''], {0: 'hi2', 4: 'hi2'}), ('pass2', ['8000', '', '', '', '', '', '', ''], {0: 'ok'})]
    F['ext-runs'] = ('fig', fig(array_rows(rows, 50), 'Sort-merge خارجي بذاكرة M = 1000 ودمج 4-way: لفّة توزيع بتعمل runs مترتبة طولها M، وبعدين كل لفّة دمج بتضرب الطول في 4. عدد اللفّات 1 + ⌈log₄(N/M)⌉ (Property 11.4).'))

for f in (selection_trace, insertion_trace, bubble_trace, shell_trace, kic_trace, partition_steps, quick_trace, three_way_steps,
          merge_trace, merge_tree, heap_steps, binomial_fig, lsd_trace, msd_fig, batcher_fig, runs_fig):
    f()

def split_interleave():
    rows_seq = [['AEGGIMNR', 'ABEELMPX']]
    cur = [list('AEGGIMNR'), list('ABEELMPX')]
    def ce(rows):
        for i in range(1, len(rows) - 1, 2) if len(rows) > 2 else [0]:
            top, bot = rows[i], rows[i + 1]
            for c in range(len(top)):
                if top[c] > bot[c]: top[c], bot[c] = bot[c], top[c]
    fr = []
    def snap(cap):
        fr.append(frame(array_rows([('', r, {}) for r in cur], 0), cap))
    snap('الملفّين المترتبين فوق بعض (كل صف مترتب).')
    ce(cur); snap('قارن كل عنصرين فوق بعض، والأصغر يطلع فوق. دلوقتي الصفوف والأعمدة مترتبين.')
    while len(cur[0]) > 1:
        h = len(cur[0]) // 2; new = []
        for r in cur: new += [r[:h], r[h:]]
        # interleave: halves of row0 and row1 alternate
        inter = []
        for k in range(0, len(new), 4):
            a1, a2, b1, b2 = new[k:k + 4] if k + 3 < len(new) else (new[k:] + [[]] * 4)[:4]
            inter += [a1, b1, a2, b2]
        cur = [r for r in inter if r]
        snap(f'قسّم كل صف نصّين ودخّل الأنصاص في بعض (split-interleave): بقى عندنا {len(cur)} صف بطول {len(cur[0])}.')
        if len(cur) > 2:
            for i in range(1, len(cur) - 1, 2):
                t, b = cur[i], cur[i + 1]
                for c in range(len(t)):
                    if t[c] > b[c]: t[c], b[c] = b[c], t[c]
            snap('قارن بس الصفوف (2 مع 3)، (4 مع 5)… — باقي الأزواج مضمون إنها مترتبة من الخطوة اللي فاتت.')
    F['split-interleave'] = ('steps', ''.join(fr))

def replacement_sel(M=5):
    data = list('ASORTINGEXAMPLEWITHMANYKEYS')
    import heapq
    h = []; it = iter(data); run = 0; last = None; runs = [[]]
    for _ in range(M): heapq.heappush(h, (0, next(it)))
    for x in it:
        r, k = heapq.heappop(h)
        if r != run: run = r; runs.append([])
        runs[-1].append(k); last = k
        heapq.heappush(h, (run + 1 if x < last else run, x))
    while h:
        r, k = heapq.heappop(h)
        if r != run: run = r; runs.append([])
        runs[-1].append(k)
    rows = [('in', data, {})] + [(f'run{i}', r, {k: 'hi' if i % 2 == 0 else 'hi2' for k in range(len(r))}) for i, r in enumerate(runs)]
    F['repl-sel'] = ('fig', fig(array_rows(rows, 44), f'Replacement selection بـ heap حجمه {M} بس على {len(data)} مفتاح: طلّع {len(runs)} runs أطوالها {", ".join(str(len(r)) for r in runs)} — متوسط ≈ {len(data)/len(runs):.1f}، يعني حوالي ضعف حجم الذاكرة (Property 11.5). لو كنا رتّبنا 5 بـ 5 كان هيطلع {-(-len(data)//M)} runs.'))

def polyphase_table():
    rows = [('start', ['13', '8', '0'], {0: 'hi', 1: 'hi'}), ('1', ['5', '0', '8'], {2: 'hi2'}), ('2', ['0', '5', '3'], {1: 'hi2'}),
            ('3', ['3', '2', '0'], {0: 'hi2'}), ('4', ['1', '0', '2'], {2: 'hi2'}), ('5', ['0', '1', '1'], {1: 'hi2'}), ('6', ['1', '0', '0'], {0: 'ok'})]
    F['polyphase'] = ('fig', fig(array_rows(rows, 50), 'Polyphase على 3 شرايط و21 run (= 13 + 8، أرقام فيبوناتشي) — ده الخرج الحقيقي بتاع <code>polyphase.c</code>. في كل phase ندمج من الشريطين المليانين للفاضي لحد ما واحد منهم يخلص، والمحدّد هو شريط الخرج. لاحظ: الأعمدة كلها أرقام فيبوناتشي، ومفيش أي نسخ زيادة.'))

for f in (split_interleave, replacement_sel, polyphase_table): f()
