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

arr_insert(); arr_rowcol(); sparse_triplet(); tri_pack()
