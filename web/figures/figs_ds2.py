"""Figures for Data Structures chapters 20-21 (trees). Author: Mohammed El Sisi."""
from web.lib.render import *
import heapq
F = {}

def P(spec, hi=(), notes=None, cls='node'):
    """'1(2(4,5),3(.,7))' -> N tree. '.' = empty. hi: labels drawn highlighted. notes: label -> small text above node."""
    notes = notes or {}
    pos = [0]
    def rec():
        if spec[pos[0]] == '.':
            pos[0] += 1; return None
        j = pos[0]
        while j < len(spec) and spec[j] not in '(),': j += 1
        lab = spec[pos[0]:j]; pos[0] = j
        n = N(lab, cls=('node hi' if lab in hi else cls), note=notes.get(lab))
        if pos[0] < len(spec) and spec[pos[0]] == '(':
            pos[0] += 1; n.l = rec(); assert spec[pos[0]] == ','; pos[0] += 1; n.r = rec(); assert spec[pos[0]] == ')'; pos[0] += 1
        return n
    t = rec(); assert pos[0] == len(spec), spec
    return t

def row(*svgs, gap=18):
    return '<div style="display:flex;flex-wrap:wrap;gap:%dpx;justify-content:center;align-items:flex-end">%s</div>' % (gap, ''.join(svgs))

def lab(s, svg_s):
    return '<div style="text-align:center">%s<div style="font-size:.85em;opacity:.8">%s</div></div>' % (svg_s, s)

# ---------------------------------------------------------------- ch 20
def terms():
    t = P('1(2(4(8,9),5),3(6(10,11),7(.,12)))', notes={'1': 'root · level 0'})
    F['ds20-terms'] = ('fig', tree(t, 36, 52, 15, 'binary tree 1..12') +
        '<figcaption>شجرة Thareja Fig 9.3: الجذر <b>1</b> (level 0)، <b>2</b> و<b>3</b> إخوات (siblings) وأبناء الجذر (level 1)، الأوراق (leaves) هي <b>5, 8, 9, 10, 11, 12</b>: لا أبناء. العقدة <b>7</b> ليها ابن واحد بس (12) فالشجرة <b>مش full</b>. ارتفاعها 4 (عدد العقد على أطول مسار 1→3→7→12 أو 1→2→4→8).</figcaption>')

def shapes():
    sh = ['x(.,x(.,x))', 'x(.,x(x,.))', 'x(x,x)', 'x(x(.,x),.)', 'x(x(x,.),.)']
    ts = [tree(P(s), 30, 40, 11, 'shape') for s in sh]
    F['ds20-shapes'] = ('fig', row(*ts) + '<figcaption>الأشكال الخمسة المختلفة لشجرة ثنائية بـ 3 عقد (عدد كاتالان C₃ = 5). مع 4 عقد: 14، ومع 10 عقد: 16796. الصيغة: C(n) = (2n)! / ((n+1)! n!).</figcaption>')

def types():
    full = P('A(B(D,E),C(F,G))'); comp = P('A(B(D,E),C(F,.))'); fnc = P('A(B,C(D,E))'); neither = P('A(B(D,.),C(.,G))')
    F['ds20-types'] = ('fig', row(lab('full + complete', tree(full, 30, 42, 12, 'full and complete')),
                                  lab('complete بس (F ملهاش أخ يمين)', tree(comp, 30, 42, 12, 'complete')),
                                  lab('full بس (B ورقة وC ليها أبناء)', tree(fnc, 30, 42, 12, 'full')),
                                  lab('لا full ولا complete', tree(neither, 30, 42, 12, 'neither'))) +
        '<figcaption><b>Full</b>: كل عقدة ليها 0 أو 2 ابن. <b>Complete</b>: كل المستويات ممتلئة ما عدا الأخير، وهو ممتلئ من الشمال. complete مش بالضرورة full، وfull مش بالضرورة complete.</figcaption>')

def memrep():
    rows = [(1, 2, 3), (2, 4, 5), (3, 6, 7), (4, 8, 9), (5, '–1', '–1'), (6, 10, 11), (7, '–1', 12), (8, '–1', '–1'), (9, '–1', '–1'), (10, '–1', '–1'), (11, '–1', '–1'), (12, '–1', '–1')]
    t = '<table class="comp-table"><tr><th>#</th><th>LEFT</th><th>DATA</th><th>RIGHT</th></tr>' + ''.join(
        f'<tr><td>{i}</td><td>{l}</td><td><b>{i}</b></td><td>{r}</td></tr>' for i, l, r in rows) + '</table>'
    seq = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '', '', '10', '11', '', '12']
    arr = array_rows([('index', list(range(1, 16)), {i: 'dim' for i in range(15)}), ('TREE', seq, {})], 30, 50, '')
    F['ds20-mem'] = ('fig', t + '<figcaption>التمثيل <b>الرابط</b> (linked) في array: ROOT = 1. النقص −1 = NULL.</figcaption>' + arr +
        '<figcaption>التمثيل <b>المتتابع</b> (sequential): الأب في i، الشمال 2i، اليمين 2i+1. الخانات 10 و11 و14 فاضية لأن 5 مالوش أبناء و7 مالوش ابن شمال. ده بيضيّع مساحة في الشجرة المائلة (الشجرة اللي عمقها n محتاجة 2ⁿ−1 خانة).</figcaption>')

def preorder_steps():
    spec = 'A(B(D(G,H(.,L)),E),C(.,F(I,J(K,.))))'
    order = ['A', 'B', 'D', 'G', 'H', 'L', 'E', 'C', 'F', 'I', 'J', 'K']
    fr = []
    for k in range(len(order)):
        t = P(spec, hi=set(order[:k + 1]))
        fr.append(frame(tree(t, 30, 44, 13, 'preorder'), 'pre-order (N L R): زرنا %s' % ' '.join(order[:k + 1])))
    F['ds20-preorder'] = ('steps', ''.join(fr))

def exprtree():
    t = P('%(-(+(a,b),*(c,d)),/(^(e,f),-(g,h)))')
    F['ds20-exprtree'] = ('fig', tree(t, 34, 48, 14, 'expression tree') +
        '<figcaption>شجرة التعبير <code>((a+b)−(c*d)) % ((e^f)/(g−h))</code> (Example 9.2): العمليات عقد داخلية والمعاملات أوراق. in-order يرجّع التعبير (مع أقواس)، pre-order بيرجّع prefix، post-order بيرجّع postfix: <code>a b + c d * − e f ^ g h − / %</code>.</figcaption>')

def huffman_steps():
    w = [7, 9, 11, 14, 18, 21, 27, 29, 35, 40]
    ch = 'ABCDEFGHIJ'
    items = [(w[i], i, ch[i], None) for i in range(10)]   # (weight, seq, label, node)
    forest = [N(f'{ch[i]}{w[i]}') for i in range(10)]
    ws = list(w)
    seq = 10
    pq = [(w[i], i, forest[i]) for i in range(10)]
    pq.sort(key=lambda x: (x[0], x[1]))
    fr = []
    def show(cap):
        trees = [tree(n, 22, 38, 11, 'subtree') for _, _, n in pq]
        return frame(row(*trees, gap=10), cap)
    fr.append(show('الغابة الأولية: 10 أشجار كل واحدة عقدة واحدة بوزنها.'))
    step = 0
    while len(pq) > 1:
        a = pq.pop(0); b = pq.pop(0)
        step += 1
        n = N(str(a[0] + b[0]), a[2], b[2])
        pq.append((a[0] + b[0], seq, n)); seq += 1
        pq.sort(key=lambda x: (x[0], x[1]))
        fr.append(show('خطوة %d: ندمج أصغر اتنين (%d و%d) في عقدة وزنها %d.' % (step, a[0], b[0], a[0] + b[0])))
    root = pq[0][2]
    # codes
    codes = {}
    def walk(n, c):
        if n.l is None: codes[n.key[0]] = c; return
        walk(n.l, c + '0'); walk(n.r, c + '1')
    walk(root, '')
    expect = {'A': '11000', 'B': '11001', 'C': '0110', 'D': '0111', 'E': '1101', 'F': '010', 'G': '100', 'H': '101', 'I': '111', 'J': '00'}
    assert codes == expect, codes
    F['ds20-huffman'] = ('steps', ''.join(fr))

def general():
    g = P('A(B(E,F),C,D)') if False else None
    # general tree A(B(E,F),C,D(G,H(I))) drawn as a multiway tree, then left-child/right-sibling binary form
    gt = M(['A'], [M(['B'], [M(['E']), M(['F'])]), M(['C']), M(['D'], [M(['G']), M(['H'], [M(['I'])])])])
    b = P('A(B(E(.,F),C(.,D(G(.,H(I,.)),.))),.)')
    # check against the C program: pre = A B E F C D G H I, in = E F B C G I H D A
    def pre(n): return [] if n is None else [n.key] + pre(n.l) + pre(n.r)
    def ino(n): return [] if n is None else ino(n.l) + [n.key] + ino(n.r)
    assert ' '.join(pre(b)) == 'A B E F C D G H I' and ' '.join(ino(b)) == 'E F B C G I H D A'
    F['ds20-general'] = ('fig', row(lab('الشجرة العامة', mtree(gt, 16, 14, 52, 'general tree')), lab('الشجرة الثنائية المقابلة', tree(b, 28, 44, 12, 'binary form'))) +
        '<figcaption>الابن الأول = <b>left</b>، والأخ التالي = <b>right</b>. النتيجة: pre-order للغابة = pre-order للثنائية، وpost-order للغابة = <b>in-order</b> للثنائية (E F B C G I H D A).</figcaption>')

terms(); shapes(); types(); memrep(); preorder_steps(); exprtree(); huffman_steps(); general()
