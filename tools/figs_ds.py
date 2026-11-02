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
