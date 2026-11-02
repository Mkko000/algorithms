"""Figures for the Data Structures section (chapters 17-31). Author: Mohammed El Sisi."""
from svglib import *
import math
F = {}   # name -> ('fig'|'steps', html)

def grid(vals, cell=34, cls=None, title='', off=(0, 0)):
    """vals: 2-D list of strings; cls: dict (i,j)->css class."""
    cls = cls or {}
    R, C = len(vals), len(vals[0])
    out = [DEFS]
    for i in range(R):
        for j in range(C):
            c = cls.get((i, j), 'cell')
            out.append(rect(10 + j * cell, 8 + i * cell, cell - 2, cell - 2, c, 4))
            out.append(text(10 + j * cell + (cell - 2) / 2, 8 + i * cell + cell * 0.62, vals[i][j], 'tc', 'middle', 12))
    return svg(C * cell + 20, R * cell + 16, ''.join(out), title)

# ---------------- chapter 17: arrays ----------------
def arr_insert():
    base = [10, 20, 30, 40, 50]
    fr = []
    a = base + ['']
    fr.append(frame(array_rows([('index', list(range(6)), {i: 'dim' for i in range(6)}), ('a', a, {})], 40, 50, ''),
                    'عايزين نحط <b>25</b> في الخانة 2. فيه خانة فاضية في الآخر (السعة 6).'))
    cur = a[:]
    for k, src in enumerate((4, 3, 2)):
        cur[src + 1] = cur[src]
        cl = {src + 1: 'hi', src: 'dim'}
        fr.append(frame(array_rows([('index', list(range(6)), {i: 'dim' for i in range(6)}), ('a', cur[:], cl)], 40, 50, ''),
                        f'نسخنا a[{src}] لـ a[{src+1}]. <b>بنبدأ من الآخر</b> عشان ما نكتبش فوق قيمة لسه محتاجينها.'))
    cur[2] = 25
    fr.append(frame(array_rows([('index', list(range(6)), {i: 'dim' for i in range(6)}), ('a', cur[:], {2: 'hi2'})], 40, 50, ''),
                    'خانة 2 فضيت، حطّينا 25. اتحرّك 3 عناصر = n − pos. لو الإدخال في الأول: n حركة، وفي الآخر: صفر.'))
    F['arr-insert'] = ('steps', ''.join(fr))

def arr_rowcol():
    R, C = 3, 4
    rm = [[str(i * C + j) for j in range(C)] for i in range(R)]
    cm = [[str(j * R + i) for j in range(C)] for i in range(R)]
    left, right = grid(rm, 36, title='row-major'), grid(cm, 36, title='column-major')
    html = ('<div class="fside"><div class="fside-item"><b>Row-major (لغة C)</b>' + left + '</div><div class="fside-item"><b>Column-major (Fortran / MATLAB)</b>' + right + '</div></div>'
            '<figcaption>نفس المصفوفة 3×4، والرقم جوه الخلية هو <b>ترتيب تخزينها في الذاكرة</b>. في row-major بنخلّص الصف كله قبل ما ننزل للي بعده، وفي column-major بنخلّص العمود. العنوان: row-major = <code>Base + w·(i·C + j)</code> وcolumn-major = <code>Base + w·(j·R + i)</code>.</figcaption>')
    F['arr-rowcol'] = ('fig', html)

def sparse_triplet():
    D = [[15, 0, 0, 22, 0, -15], [0, 11, 3, 0, 0, 0], [0, 0, 0, -6, 0, 0], [0, 0, 0, 0, 0, 0], [91, 0, 0, 0, 0, 0]]
    cls = {(i, j): 'hi' for i in range(5) for j in range(6) if D[i][j]}
    g = grid([[str(v) if v else '·' for v in r] for r in D], 36, cls, 'sparse matrix')
    terms = [(i, j, D[i][j]) for i in range(5) for j in range(6) if D[i][j]]
    tb = '<table class="comp-table"><tr><th></th>' + ''.join(f'<th>{k}</th>' for k in range(len(terms) + 1)) + '</tr>'
    tb += '<tr><th>row</th><td>5</td>' + ''.join(f'<td>{t[0]}</td>' for t in terms) + '</tr>'
    tb += '<tr><th>col</th><td>6</td>' + ''.join(f'<td>{t[1]}</td>' for t in terms) + '</tr>'
    tb += '<tr><th>val</th><td>7</td>' + ''.join(f'<td>{t[2]}</td>' for t in terms) + '</tr></table>'
    F['sparse-triplet'] = ('fig', '<div class="fside"><div class="fside-item">' + g + '</div><div class="fside-item">' + tb + '</div></div>'
        '<figcaption>مصفوفة 5×6 فيها 7 قيم غير صفرية بس. التخزين الكثيف 30 خانة، والـ triplet 24 (العمود 0 هو الـ header: عدد الصفوف، الأعمدة، الحدود). الفايدة الحقيقية بتظهر لما المصفوفة 1000×1000 وفيها 3000 حد: 3000×3 بدل مليون.</figcaption>')

def tri_pack():
    n = 5
    vals = [[(str(i * (i + 1) // 2 + j) if j <= i else '0') for j in range(n)] for i in range(n)]
    cls = {(i, j): ('hi' if j <= i else 'dim') for i in range(n) for j in range(n)}
    g = grid(vals, 38, cls, 'lower triangular packing')
    F['tri-pack'] = ('fig', g + '<figcaption>مصفوفة lower-triangular 5×5. الأرقام هي <b>رقم الخانة في المصفوفة المضغوطة</b> (<code>i(i+1)/2 + j</code>): الصف 0 ليه خلية، الصف 1 اتنين، وهكذا لحد 15 خانة بدل 25. الأصفار الباهتة مش متخزّنة.</figcaption>')


# ---------------- chapter 18: linked list variants ----------------
def path_arrow(d, cls='arr'):
    return f'<path d="{d}" class="{cls}" fill="none" marker-end="url(#ah)"/>'

def dlist_svg(items, header=True, circular=True, hi=None, title=''):
    """doubly linked list: boxes [prev|val|next], arrows both ways, optional header + wrap-around."""
    hi = hi or {}
    w, h, gap, y = 66, 30, 34, 40
    labels = (['H'] if header else []) + [str(v) for v in items]
    out = [DEFS]; x0 = 14; xs = []
    for i, v in enumerate(labels):
        x = x0 + i * (w + gap); xs.append(x)
        c = 'cell dim' if (header and i == 0) else hi.get(i - (1 if header else 0), 'cell')
        out.append(rect(x, y, w, h, c, 4))
        out.append(line(x + 18, y, x + 18, y + h, 'sep')); out.append(line(x + w - 18, y, x + w - 18, y + h, 'sep'))
        out.append(text(x + w / 2, y + h * 0.68, v, 'tc', 'middle', 13))
    for i in range(len(labels) - 1):
        xa, xb = xs[i] + w, xs[i + 1]
        out.append(arrow(xa - 9, y + 10, xb, y + 10)); out.append(arrow(xb + 9, y + 22, xa, y + 22))
    W = xs[-1] + w + 20
    if circular and len(labels) > 1:
        out.append(path_arrow(f"M{xs[-1] + w - 9},{y + h} C{xs[-1] + w - 9},{y + h + 34} {xs[0] + 9},{y + h + 34} {xs[0] + 9},{y + h + 1}"))
        out.append(path_arrow(f"M{xs[0] + 9},{y} C{xs[0] + 9},{y - 30} {xs[-1] + w - 9},{y - 30} {xs[-1] + w - 9},{y - 1}"))
    else:
        out.append(text(xs[0] - 4, y + 12, 'NULL', 'lab', 'end', 10)); out.append(text(xs[-1] + w + 8, y + 12, 'NULL', 'lab', 'start', 10))
        W += 30
    return svg(W, y + h + 50, ''.join(out), title)

def dlist_insert():
    a = dlist_svg([10, 30], True, True, title='before')
    b = dlist_svg([10, 20, 30], True, True, {1: 'cell hi'}, title='after')
    F['dlist-insert'] = ('fig', '<div class="fside"><div class="fside-item"><b>قبل</b>' + a + '</div><div class="fside-item"><b>بعد إدخال 20 بعد العقدة 10</b>' + b + '</div></div>'
        '<figcaption>قايمة circular doubly بـ <b>header (H)</b>. الأسهم العلوية هي <code>next</code> والسفلية <code>prev</code>، والمنحنيات بتقفل الدايرة: <code>H.prev</code> = آخر عقدة و<code>last.next</code> = H. الإدخال 4 كتابات pointers بالظبط: <code>n.prev=p; n.next=p.next; p.next.prev=n; p.next=n</code> — وبنفس الكود سواء القايمة فاضية أو الإدخال في الأول أو الآخر، وده فايدة الـ header.</figcaption>')

def circ_header():
    w, h, gap, y = 46, 28, 30, 36
    out = [DEFS]; xs = []
    labels = ['4', '1', '3', '4', '9']
    for i, v in enumerate(labels):
        x = 14 + i * (w + gap); xs.append(x)
        out.append(rect(x, y, w, h, 'cell dim' if i == 0 else 'cell', 4))
        out.append(line(x + w * 0.7, y, x + w * 0.7, y + h, 'sep'))
        out.append(text(x + w * 0.35, y + h * 0.68, v, 'tc', 'middle', 13))
    out.append(text(xs[0] + w / 2, y - 8, 'header: عدد العقد', 'lab', 'middle', 10))
    for i in range(len(labels) - 1):
        out.append(arrow(xs[i] + w * 0.85, y + h / 2, xs[i + 1], y + h / 2))
    out.append(path_arrow(f"M{xs[-1] + w * 0.85},{y + h} C{xs[-1] + w * 0.85},{y + h + 36} {xs[0] + w * 0.85},{y + h + 36} {xs[0] + w * 0.85},{y + h + 1}"))
    F['circ-header'] = ('fig', svg(xs[-1] + w + 20, y + h + 50, ''.join(out), 'circular header list')
        + '<figcaption>القايمة الدايرية بـ header: آخر عقدة بتشاور على الـ header (مش NULL)، فشرط النهاية بقى <code>p == head</code>. خانة <code>val</code> في الـ header مش محتاجينها لبيانات، فبنخزّن فيها <b>عدد العقد</b> (هنا 4) فالـ length بقى O(1).</figcaption>')

def list_reverse_steps():
    fr = []
    def st(cap, items, cur, prev_idx):
        fr.append(frame(llist(items, {cur: 'hi'} if cur is not None else {}, null=True), cap))
    fr.append(frame(llist(['1', '2', '3', '4'], {}), 'قايمة 1→2→3→4. هنقلب الأسهم واحد واحد من غير ما ننسخ بيانات.'))
    fr.append(frame(llist(['1'], {0: 'hi'}) + llist(['2', '3', '4'], {}), 'الخطوة 1: <code>cur</code> = 1. سيف <code>next</code> = 2، بعدين <code>cur.next = prev (NULL)</code>. القايمة اتقسمت لجزئين: المقلوب [1] والباقي [2,3,4].'))
    fr.append(frame(llist(['2', '1'], {0: 'hi'}) + llist(['3', '4'], {}), 'الخطوة 2: <code>2.next = 1</code>. المقلوب [2→1] والباقي [3,4].'))
    fr.append(frame(llist(['3', '2', '1'], {0: 'hi'}) + llist(['4'], {}), 'الخطوة 3: <code>3.next = 2</code>.'))
    fr.append(frame(llist(['4', '3', '2', '1'], {0: 'hi'}), 'الخطوة 4: <code>4.next = 3</code> والباقي فضي. الراجع <code>prev</code> = 4 = الرأس الجديد. n خطوة، O(1) مساحة.'))
    F['ds-list-reverse'] = ('steps', ''.join(fr))

def cursor_pool():
    rows = [(0, '', 3), (1, '·', 2), (2, 'A', 4), (3, '', 5), (4, 'B', 0), (5, '', 6), (6, '', 7), (7, '', 0)]
    t = '<table class="comp-table"><tr><th>index</th>' + ''.join(f'<th>{r[0]}</th>' for r in rows) + '</tr>'
    t += '<tr><th>val</th>' + ''.join(f'<td>{r[1] or "·"}</td>' for r in rows) + '</tr>'
    t += '<tr><th>next</th>' + ''.join(f'<td>{r[2]}</td>' for r in rows) + '</tr></table>'
    F['cursor-pool'] = ('fig', t + '<figcaption>الـ pool فيه 8 خلايا. القايمة L ليها header في الخلية 1: <b>1 → 2 (A) → 4 (B) → 0</b> والـ 0 معناها «مفيش حاجة بعدها». الـ free list بتبدأ من header الخلية 0: <b>0 → 3 → 5 → 6 → 7 → 0</b>. الـ alloc = اسحب أول خلية من الـ free list، والـ free = رجّعها في أولها: نفس <code>malloc/free</code> بس فوق array.</figcaption>')

dlist_insert(); circ_header(); list_reverse_steps(); cursor_pool()
arr_insert(); arr_rowcol(); sparse_triplet(); tri_pack()


# ---------------- chapter 19: stacks and queues ----------------
def ds19_two_stacks():
    cap = 10
    def row(a_items, b_items, cap_cls):
        cells = [''] * cap
        cls = {}
        for i, v in enumerate(a_items): cells[i] = v; cls[i] = 'hi'
        for k, v in enumerate(b_items): cells[cap - 1 - k] = v; cls[cap - 1 - k] = 'hi2'
        return array_rows([('index', list(range(cap)), {i: 'dim' for i in range(cap)}), ('a', cells, cls)], 40, 50, '')
    fr = [frame(row([], [], {}), 'مصفوفة واحدة سعتها 10. المكدّس A هيبدأ من الشمال (topA = −1) والمكدّس B من اليمين (topB = 10).'),
          frame(row(['3', '8', '1'], ['7', '4'], {}), 'دفعنا 3 و8 و1 في A و7 و4 في B. كل واحد بيكبر ناحية التاني.'),
          frame(row(['3', '8', '1', '5', '6', '9'], ['7', '4', '2', '0'], {}), 'المصفوفة اتملت: <code>topA + 1 == topB</code>. الـ overflow بيحصل <b>بس لما المجموع يوصل 10</b>، حتى لو A واخد 6 وB واخد 4. مع نصّين ثابتين (5+5) كان A هيعمل overflow عند العنصر السادس.')]
    F['ds-two-stacks'] = ('steps', ''.join(fr))

def ds19_shunting():
    toks = 'A + B * C - D'.split()
    prec = {'+': 1, '-': 1, '*': 2, '/': 2}
    st, out, fr = [], [], []
    def tbl(tok, note):
        return ('<table class="comp-table"><tr><th>قرأنا</th><td>' + tok + '</td><th>الـ stack (من الأسفل)</th><td>' + (' '.join(st) or '·') +
                '</td><th>الـ output</th><td>' + (' '.join(out) or '·') + '</td></tr></table>')
    fr.append(frame(tbl('—', ''), 'التعبير: <code>A + B * C − D</code>. الـ stack بيشيل العمليات اللي مستنية، والـ output بيطلع الـ postfix.'))
    notes = {'A': 'معامل: يروح للـ output على طول.', 'B': 'معامل: output.', 'C': 'معامل: output.', 'D': 'معامل: output.'}
    for t in toks:
        if t in prec:
            popped = []
            while st and prec[st[-1]] >= prec[t]:
                popped.append(st[-1]); out.append(st.pop())
            st.append(t)
            n = ('<code>%s</code> أولويتها %s. ' % (t, 'أعلى من اللي على القمة، فتستنى فوقها (ما اتشالش حاجة).' if not popped else
                 'أقل من أو تساوي اللي على القمة (%s)، فاللي على القمة يخرج للـ output الأول.' % ' '.join(popped)))
            fr.append(frame(tbl(t, n), n))
        else:
            out.append(t); fr.append(frame(tbl(t, ''), notes[t]))
    while st:
        out.append(st.pop())
    fr.append(frame(tbl('نهاية', ''), 'خلصت التوكنز: نفرّغ الـ stack كله للـ output. النتيجة: <b>' + ' '.join(out) + '</b>'))
    assert ' '.join(out) == 'A B C * + D -'
    F['ds-shunting'] = ('steps', ''.join(fr))

def ds19_ring():
    import math
    n = 8
    def ring(items, fr_i, rr_i, title):
        cx, cy, R = 100, 100, 70
        o = [DEFS]
        for i in range(n):
            ang = -math.pi / 2 + i * 2 * math.pi / n
            x, y = cx + R * math.cos(ang), cy + R * math.sin(ang)
            c = 'hi' if items[i] else 'cell'
            o.append(rect(x - 17, y - 14, 34, 28, c, 5))
            o.append(text(x, y + 5, items[i] or '', 'tc', 'middle', 13))
            o.append(text(cx + (R + 34) * math.cos(ang), cy + (R + 34) * math.sin(ang) + 4, str(i), 'lab', 'middle', 10))
        lab = []
        if fr_i is not None: lab.append('front=%d' % fr_i)
        if rr_i is not None: lab.append('rear=%d' % rr_i)
        o.append(text(cx, cy + 4, ' '.join(lab) or 'فاضي', 'lab', 'middle', 11))
        return svg(210, 210, ''.join(o), title)
    p1 = ['', '', '', 'C', 'D', 'E', 'F', '']
    p2 = ['H', 'I', '', 'C', 'D', 'E', 'F', 'G']
    p3 = ['H', 'I', 'J', 'C', 'D', 'E', 'F', 'G']
    html = ('<div class="fside"><div class="fside-item"><b>C D E F</b>' + ring(p1, 3, 6, 'a') +
            '</div><div class="fside-item"><b>بعد إضافة G وH وI: REAR لفّ</b>' + ring(p2, 3, 1, 'b') +
            '</div><div class="fside-item"><b>بعد J: ممتلئ</b>' + ring(p3, 3, 2, 'c') + '</div></div>'
            '<figcaption>الـ 8 خلايا دايرة: بعد الخلية 7 بييجي 0 (<code>(i+1) % 8</code>). G راحت 7 ثم H في 0 وI في 1: REAR لفّ وFRONT لسه 3، فالـ queue ماشية 3..7 ثم 0..1. بعد J في الخلية 2 بقى <code>FRONT == (REAR+1) % N</code> يعني ممتلئة. الشرط ده نفسه بيتحقق في الفاضية، عشان كده بنفرّق بينهم بـ count أو بخلية فاضية أو بـ −1 (التلات طرق في الكود).</figcaption>')
    F['ds-ring'] = ('fig', html)

def ds19_q2s():
    def st(items, label):
        o = [DEFS]; h = 28
        for i, v in enumerate(items):
            o.append(rect(10, 120 - (i + 1) * (h + 2), 50, h, 'hi' if i == len(items) - 1 else 'cell', 4))
            o.append(text(35, 120 - (i + 1) * (h + 2) + 19, v, 'tc', 'middle', 13))
        o.append(line(8, 122, 62, 122, 'edge'))
        o.append(text(35, 140, label, 'lab', 'middle', 12))
        return svg(72, 150, ''.join(o), label)
    def two(i, o, cap):
        return frame('<div class="fside"><div class="fside-item">' + st(i, 'IN') + '</div><div class="fside-item">' + st(o, 'OUT') + '</div></div>', cap)
    fr = [two(['1', '2', '3'], [], 'enqueue 1,2,3: كلها بتنزل على IN. القمة = 3 (الأحدث).'),
          two([], ['3', '2', '1'], 'أول dequeue: OUT فاضي، فننقل <b>كل</b> IN لـ OUT (3 pops و3 pushes) فالترتيب بيتعكس: القمة بقت 1 (الأقدم).'),
          two([], ['3', '2'], 'نعمل pop من OUT = 1: ده بالظبط اللي دخل أولًا (FIFO).'),
          two(['4'], ['3', '2'], 'enqueue 4 يروح IN، من غير ما يلمس OUT. الـ 2 هو اللي هيطلع بعده ثم 3 ثم 4.')]
    F['ds-q2s'] = ('steps', ''.join(fr))

def ds19_fibtree():
    def fib(n, hi):
        if n < 2: return N(str(n), cls='node hi' if (n, ) in hi else 'node')
        return N(str(n), fib(n - 1, hi), fib(n - 2, hi), cls='node')
    t = fib(5, set())
    # mark every call that is a repeat of fib(3) or fib(2): easy to show visually by note
    F['ds-fibtree'] = ('fig', tree(t, 30, 46, 14, 'fib(5) call tree') +
        '<figcaption>شجرة النداءات لـ <code>fib(5)</code>: 15 نداء. <code>fib(3)</code> اتحسبت مرتين و<code>fib(2)</code> 3 مرات و<code>fib(1)</code> 5 مرات: نفس المشكلة الفرعية بتتحل تاني وتالت (ولذلك <code>fib(25)</code> محتاجة 242785 نداء، وده أُسّي ≈ 1.618<sup>n</sup>). الحل: memoization أو حلقة بسيطة. لاحظ إن <b>عمق</b> الـ stack هو ارتفاع الشجرة (5)، مش عدد النداءات.</figcaption>')

def ds19_hanoi():
    def pegs(state, cap):
        o = [DEFS]
        for p in range(3):
            x = 70 + p * 140
            o.append(rect(x - 3, 20, 6, 100, 'dim', 2))
            o.append(text(x, 140, 'ABC'[p], 'lab', 'middle', 12))
            for k, d in enumerate(state[p]):
                w = 24 + d * 22
                o.append(rect(x - w / 2, 108 - k * 20, w, 17, 'hi' if d == 1 else 'cell', 4))
                o.append(text(x, 108 - k * 20 + 13, str(d), 'tc', 'middle', 11))
        return frame(svg(420, 150, ''.join(o), 'hanoi'), cap)
    state = [[3, 2, 1], [], []]
    fr = [pegs([p[:] for p in state], '3 أقراص على A. الهدف: ننقلهم لـ C، قرص واحد كل مرة، وما نحطش كبير فوق صغير.')]
    moves = []
    def h(n, a, c, b):
        if n == 0: return
        h(n - 1, a, b, c); moves.append((a, c)); h(n - 1, b, c, a)
    h(3, 0, 2, 1)
    for i, (a, c) in enumerate(moves):
        d = state[a].pop(); assert not state[c] or state[c][-1] > d; state[c].append(d)
        fr.append(pegs([p[:] for p in state], 'حركة %d: القرص %d من %s لـ %s.' % (i + 1, d, 'ABC'[a], 'ABC'[c])))
    assert state == [[], [], [3, 2, 1]]
    F['ds-hanoi'] = ('steps', ''.join(fr))

def ds19_pqarr():
    rows = {1: ['A'], 2: ['B', 'C', 'D'], 3: ['E', 'F'], 4: ['G', 'H']}
    t = '<table class="comp-table"><tr><th>الأولوية</th><th>الـ queue (front ← rear)</th></tr>'
    for k, v in rows.items(): t += '<tr><td>%d</td><td>%s</td></tr>' % (k, ' → '.join(v))
    F['ds-pqarr'] = ('fig', t + '</table><figcaption>priority queue كمصفوفة طوابير: صف لكل أولوية (1 = الأعلى). insert = ضيف في آخر صف الأولوية O(1). delete = أول صف مش فاضي، شيل من أوله: هنا A. لو A مشيت يبقى B (أولوية 2)، وبالنسبة لـ B وC وD بالـ FCFS.</figcaption>')

ds19_two_stacks(); ds19_shunting(); ds19_ring(); ds19_q2s(); ds19_fibtree(); ds19_hanoi(); ds19_pqarr()
