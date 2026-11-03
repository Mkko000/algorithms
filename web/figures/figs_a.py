"""Figures for chapters 1-5. Author: Mohammed El Sisi."""
from web.lib.render import *
import math
F = {}   # name -> ('fig'|'steps', html)

PAIRS = [(3,4),(4,9),(8,0),(2,3),(5,6),(2,9),(5,9),(7,3),(4,8),(5,6),(0,2),(6,1)]

def uf_quickfind():
    idv = list(range(10)); rows = [('', list(range(10)), {i: 'dim' for i in range(10)})]
    for p, q in PAIRS:
        if idv[p] == idv[q]:
            rows.append((f'{p}-{q}', idv[:], {i: 'dim' for i in range(10)})); continue
        t = idv[p]; ch = {}
        for i in range(10):
            if idv[i] == t: idv[i] = idv[q]; ch[i] = 'hi'
        ch[q] = ch.get(q, 'hi2'); rows.append((f'{p}-{q}', idv[:], ch))
    F['uf-quickfind'] = ('fig', fig(array_rows(rows, 26, 44, 'quick-find id[] trace'),
        'تتبّع <b>quick-find</b> على أزواج الكتاب: السطر الأول أرقام الخانات، وكل سطر بعده هو <code>id[]</code> بعد الزوج المكتوب على الشمال. الخانات الملوّنة هي اللي اتغيرت — لاحظ إن union واحد ممكن يغيّر خانات كتير (ده سبب الـ N في كل union). السطور الباهتة: الزوج كان متوصّل أصلًا.'))

def forest(parent, hi=()):
    kids = {i: [] for i in range(len(parent))}
    roots = []
    for i, p in enumerate(parent):
        (roots if p == i else kids[p]).append(i)
    def mk(i): return M([i], [mk(c) for c in sorted(kids[i])], 'mnode hi' if i in hi else 'mnode')
    return mtree(M([], [mk(r) for r in roots], 'virtual'), kw=24, gap=10, dy=46)

def uf_weighted_steps():
    par = list(range(10)); sz = [1] * 10; frames = [frame(forest(par), 'البداية: كل عنصر شجرة لوحده.')]
    def root(i):
        while par[i] != i: i = par[i]
        return i
    for p, q in PAIRS:
        i, j = root(p), root(q)
        if i == j:
            frames.append(frame(forest(par, (p, q)), f'{p}-{q}: نفس الجذر ({i}) — متوصّلين أصلًا، مفيش تغيير.')); continue
        if sz[i] < sz[j]: par[i] = j; sz[j] += sz[i]; cap = f'{p}-{q}: شجرة {i} (حجمها أصغر) بقت تحت {j}.'
        else: par[j] = i; sz[i] += sz[j]; cap = f'{p}-{q}: شجرة {j} (الأصغر أو مساوية) بقت تحت {i}.'
        frames.append(frame(forest(par, (p, q)), cap + ' الأصغر دايمًا تحت الأكبر، فالارتفاع مايزيدش عن lg N.'))
    F['uf-weighted'] = ('steps', ''.join(frames))

def uf_quickunion_bad():
    par = list(range(8))
    for k in range(1, 8): par[k - 1] = k
    F['uf-qu-bad'] = ('fig', fig(forest(par), 'أسوأ حالة لـ quick-union من غير وزن: الأزواج 0-1، 1-2، 2-3… بتعمل «سلسلة» ارتفاعها N−1، وكل find على 0 بيمشي المسار كله.'))

def recur_tree():
    def mk(n, d):
        if n <= 1 or d > 3: return N(str(n))
        return N(str(n), mk(n // 2, d + 1), mk(n - n // 2, d + 1))
    F['recur-tree'] = ('fig', fig(tree(mk(16, 0), dx=30, dy=46, r=14), 'شجرة الـ recurrence <code>C(N) = 2C(N/2) + N</code> لـ N = 16: كل مستوى مجموع أرقامه 16 (= N)، وفيه lg N = 4 مستويات تحت الجذر — فالمجموع N lg N. ده البرهان «بالرسم» لـ Formula 2.4.'))

def list_insert_steps():
    fr = []
    fr.append(frame(llist(['A', 'B', 'D'], {1: 'hi'}), 'عايزين ندخّل <b>C</b> بعد العقدة x (اللي فيها B).'))
    ex = rect(110, 62, 44, 28, 'hi2', 4) + text(128, 81, 'C', 'tc') + text(102, 81, 't', 'lab', 'end', 12)
    fr.append(frame(llist(['A', 'B', 'D'], {1: 'hi'}, extra=ex, extra_h=30), 'عملنا عقدة جديدة t فيها C (لسه مش متوصّلة بحاجة).'))
    fr.append(frame(llist(['A', 'B', 'C', 'D'], {1: 'hi', 2: 'hi2'}), 'السطر الأول: <code>t->next = x->next</code> (C بقت بتشاور على D)، والتاني: <code>x->next = t</code>. لو عكست الترتيب، D والباقي يضيعوا.'))
    fr.append(frame(llist(['A', 'B', 'D'], {1: 'hi'}), 'والحذف بعد x: <code>t = x->next; x->next = t->next; free(t);</code> — رجعنا للأول.'))
    F['list-insert'] = ('steps', ''.join(fr))

def josephus_steps(N_=9, M_=5):
    alive = list(range(1, N_ + 1)); order = []
    pos = {k: (130 + 95 * math.cos(2 * math.pi * (k - 1) / N_ - math.pi / 2), 120 + 95 * math.sin(2 * math.pi * (k - 1) / N_ - math.pi / 2)) for k in range(1, N_ + 1)}
    def draw(dead, hi=None):
        al = [k for k in range(1, N_ + 1) if k not in dead]
        edges = [(al[i], al[(i + 1) % len(al)]) for i in range(len(al))] if len(al) > 1 else []
        cls = {k: 'node dim' for k in dead}
        if hi: cls[hi] = 'node hi'
        return graph({str(k): v for k, v in pos.items()}, [(str(a), str(b)) for a, b in edges], cls={str(k): v for k, v in cls.items()}, directed=True, W=260, H=245)
    fr = [frame(draw(set()), f'Josephus بـ N={N_} وM={M_}: قايمة دائرية، كل عقدة بتشاور على اللي بعدها.')]
    i = N_ - 1; dead = set()
    while len(alive) > 1:
        i = (i + M_) % len(alive); k = alive.pop(i); i -= 1; dead.add(k); order.append(k)
        fr.append(frame(draw(dead, k), f'عدّينا {M_} — خرج <b>{k}</b> (<code>x->next = x->next->next</code>). اللي خرجوا لحد دلوقتي: {" ".join(map(str, order))}'))
    fr.append(frame(draw(dead, alive[0]), f'فضل <b>{alive[0]}</b> لوحده — هو الناجي، وبيشاور على نفسه (<code>x == x->next</code>).'))
    F['josephus'] = ('steps', ''.join(fr))

def reverse_steps():
    items = ['1', '2', '3', '4']; fr = []
    for k in range(len(items) + 1):
        r = items[:k][::-1]; y = items[k:]
        a = llist(r, {0: 'hi'} if r else {}, null=True) if r else svg(80, 40, text(40, 25, 'r = NULL', 'lab'))
        b = llist(y, {0: 'hi2'} if y else {}, null=True) if y else svg(80, 40, text(40, 25, 'y = NULL', 'lab'))
        fr.append(frame(f'<div style="display:flex;gap:18px;justify-content:center;flex-wrap:wrap;direction:ltr"><div><small>r (اللي اتعكس)</small><br>{a}</div><div><small>y (الباقي)</small><br>{b}</div></div>',
                        'البداية: r فاضية وy هي القايمة كلها.' if k == 0 else (f'خطوة {k}: خدنا أول عقدة من y ({items[k-1]}) وخلّيناها تشاور على r. سهم واحد اتقلب.' if k < len(items) else 'y خلصت: r هي القايمة معكوسة. كل عقدة اتلمست مرة واحدة → O(N).')))
    F['list-reverse'] = ('steps', ''.join(fr))

def postfix_trace():
    toks = '5 9 8 + 4 6 * * 7 + *'.split(); st = []; rows = []
    for t in toks:
        if t.isdigit(): st.append(int(t)); c = {len(st) - 1: 'hi'}
        else:
            b, a = st.pop(), st.pop(); st.append(a + b if t == '+' else a * b); c = {len(st) - 1: 'hi2'}
        rows.append((t, [str(x) for x in st], c))
    F['postfix'] = ('fig', fig(array_rows(rows, 40, 30), 'الـ stack بعد كل رمز في <code>5 9 8 + 4 6 * * 7 + *</code> (قاعدة الـ stack على الشمال). الأزرق: رقم اتعمله push. البنفسجي: ناتج عملية (pop اتنين وpush واحد). آخر سطر: 2075.'))

def circ_queue_steps():
    Nq = 8; q = [None] * Nq; head = tail = 0; fr = []
    ops = [('put', 'A'), ('put', 'B'), ('put', 'C'), ('get', None), ('put', 'D'), ('put', 'E'), ('get', None), ('get', None), ('put', 'F'), ('put', 'G'), ('put', 'H'), ('put', 'I')]
    for op, v in ops:
        if op == 'put': q[tail] = v; tail = (tail + 1) % Nq; cap = f'put {v}: اتحط في خانة {(tail - 1) % Nq} وtail بقى {tail}.'
        else: x = q[head]; q[head] = None; head = (head + 1) % Nq; cap = f'get → {x}: head اتقدّم لـ {head}. الخانة اللي فضيت هتتستخدم تاني لما tail يلف.'
        cls = {i: 'hi' for i in range(Nq) if q[i] is not None}
        cls[head] = cls.get(head, 'cell')
        rows = [('', list(range(Nq)), {i: 'dim' for i in range(Nq)}), ('q', [x or '' for x in q], cls)]
        lab = svg(Nq * 30 + 50, 30, text(30 + head * 30 + 15, 18, 'head', 'elab', 'middle', 11) + text(30 + tail * 30 + 15, 28, 'tail', 'note', 'middle', 11))
        fr.append(frame(array_rows(rows, 30, 30) + '<br>' + lab, cap))
    F['circ-queue'] = ('steps', ''.join(fr))

def fib_tree():
    def mk(n):
        if n < 2: return N(f'F{n}')
        return N(f'F{n}', mk(n - 1), mk(n - 2))
    t = mk(5)
    def mark(t, seen):
        if t is None: return
        if t.key in seen: t.cls = 'node dim'
        seen.add(t.key); mark(t.l, seen); mark(t.r, seen)
    mark(t, set())
    F['fib-tree'] = ('fig', fig(tree(t, dx=34, dy=44, r=15), 'شجرة نداءات <code>F(5)</code> الساذجة: 15 نداء عشان 6 قيم مختلفة. العقد الباهتة كلها حسابات <b>متكررة</b> — اللي الـ memoization بيشيلها، فتفضل سلسلة واحدة من F5 لـ F0.'))

def knap_table():
    size = [3, 4, 7, 8, 9]; val = [4, 5, 10, 11, 13]; best = [0] * 18; item = [''] * 18
    for c in range(1, 18):
        for i in range(5):
            if size[i] <= c and best[c - size[i]] + val[i] > best[c]:
                best[c] = best[c - size[i]] + val[i]; item[c] = 'ABCDE'[i]
    rows = [('cap', list(range(18)), {i: 'dim' for i in range(18)}), ('best', best, {17: 'hi'}), ('item', item, {17: 'hi2', 10: 'hi2', 3: 'hi2'})]
    F['knap-table'] = ('fig', fig(array_rows(rows, 28, 40), 'جدول الـ DP للشنطة: لكل سعة من 0 لـ 17، أحسن قيمة وآخر حاجة اتحطت. عشان تبني الحل من 17: خد C (حجم 7) ← سعة 10، خد C ← سعة 3، خد A ← سعة 0. الحل A+C+C = 24.'))

def traversal_fig():
    t = N('E', N('D', N('B', N('A'), N('C'))), N('H', N('F', None, N('G')), N('I')))
    pre = 'EDBACHFGI'; ino = 'ABCDEFGHI'; post = 'ACBDGFIHE'
    def note(t):
        if t is None: return
        t.note = f'{pre.index(t.key)+1}/{ino.index(t.key)+1}/{post.index(t.key)+1}'; note(t.l); note(t.r)
    note(t)
    F['traversal'] = ('fig', fig(tree(t, dx=44, dy=56, r=15), 'الرقم فوق كل عقدة = ترتيب زيارتها في <b>pre/in/post</b>. مثلًا E هي 1 في preorder (أول حاجة)، 5 في inorder (في النص)، و9 في postorder (آخر حاجة).'))

def heights_fig():
    t = build_bst([8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15])
    F['complete-tree'] = ('fig', fig(tree(t, dx=28, dy=44, r=12, show_null=True), 'شجرة كاملة فيها N = 15 عقدة داخلية و16 عقدة خارجية (الخطوط المتقطعة) — Property 5.5. الارتفاع 3 = ⌊lg 15⌋، أقل ارتفاع ممكن (Property 5.8).'))

GPOS = {'0': (40, 40), '1': (300, 40), '2': (40, 150), '3': (230, 220), '4': (300, 150), '5': (130, 220), '6': (130, 110), '7': (200, 40)}
GE = [('0','2'),('0','5'),('0','7'),('1','7'),('2','6'),('3','4'),('3','5'),('4','5'),('4','6'),('4','7')]
def dfs_steps():
    adj = {k: [] for k in GPOS}
    for a, b in GE: adj[a].append(b); adj[b].append(a)
    for k in adj: adj[k].sort()
    order = []; tree_e = []; fr = []; seen = set()
    def dfs(v, par=None):
        seen.add(v); order.append(v)
        if par: tree_e.append((par, v))
        fr.append(frame(graph(GPOS, GE, cls={k: ('node hi' if k == v else 'node hi2' if k in seen else 'node') for k in GPOS}, ecls={e: 'edge hi' for e in tree_e}, W=340, H=250),
                        f'زرنا <b>{v}</b>. ترتيب الزيارة: {" ".join(order)}. الخطوط العريضة = أضلاع الـ spanning tree.'))
        for w in adj[v]:
            if w not in seen: dfs(w, v)
    dfs('0')
    F['dfs'] = ('steps', ''.join(fr))
    q = ['0']; seen = {'0'}; order = []; fr = []; dist = {'0': 0}
    while q:
        v = q.pop(0); order.append(v)
        for w in adj[v]:
            if w not in seen: seen.add(w); q.append(w); dist[w] = dist[v] + 1
        fr.append(frame(graph(GPOS, GE, cls={k: ('node hi' if k == v else 'node hi2' if k in order else 'node') for k in GPOS}, W=340, H=250),
                        f'طلّعنا <b>{v}</b> (مسافته {dist[v]}) وحطينا جيرانه الجداد. الطابور دلوقتي: [{" ".join(q)}]'))
    F['bfs'] = ('steps', ''.join(fr))

for f in (uf_quickfind, uf_weighted_steps, uf_quickunion_bad, recur_tree, list_insert_steps, josephus_steps, reverse_steps,
          postfix_trace, circ_queue_steps, fib_tree, knap_table, traversal_fig, heights_fig, dfs_steps):
    f()
