"""Figures for Data Structures chapter 21 (efficient binary trees). Author: Mohammed El Sisi.
Every figure that shows a RESULT of an algorithm re-runs that algorithm here in Python and asserts that it
produces the same tree as the tested C program in src/21-efficient-trees/."""
from svglib import *
from figs_ds2 import P, row, lab
F = {}

def pair(a, b):
    return '<div style="display:flex;flex-wrap:nowrap;align-items:center;gap:8px">%s<span style="font-size:1.7em;opacity:.8">⟸</span>%s</div>' % (a, b)

def ser(n):
    if n is None: return '.'
    if n.l is None and n.r is None: return str(n.key)
    return f'{n.key}({ser(n.l)},{ser(n.r)})'

# ---------------------------------------------------------------- BST deletion (3 cases)
def bst_delete():
    def delete(t, k):
        if t is None: return None
        if k < int(t.key): t.l = delete(t.l, k)
        elif k > int(t.key): t.r = delete(t.r, k)
        elif t.l is None: return t.r
        elif t.r is None: return t.l
        else:
            p = t.l
            while p.r: p = p.r
            t.key = p.key; t.l = delete(t.l, int(p.key))
        return t
    base = '50(30(20,40(35,.)),70(60,80))'
    cases = [('الحالة 1: حذف 20 (ورقة)', 20, '50(30(.,40(35,.)),70(60,80))', ('20',)),
             ('الحالة 2: حذف 40 (ابن واحد: 35 بيطلع مكانه)', 40, '50(30(20,35),70(60,80))', ('40',)),
             ('الحالة 3: حذف 50 (ابنين): نبدّله بـ predecessor = 40 (أكبر مفتاح في الشمال) ونحذف 40 من مكانه القديم', 50, '40(30(20,35),70(60,80))', ('50',))]
    parts = []
    for title, k, want, hi in cases:
        t = P(base, hi=hi)
        got = ser(delete(P(base), k))
        assert got == want, (got, want)
        parts.append(lab(title, pair(tree(t, 24, 40, 11, 'قبل'), tree(P(want), 24, 40, 11, 'بعد'))))
    F['ds21-bstdel'] = ('fig', row(*parts, gap=26) + '<figcaption>الحالات التلاتة لحذف عقدة من BST (Thareja 10.1.4). الحالة 3 ممكن تستعمل الـ predecessor (زي هنا وزي RQ 12) أو الـ successor؛ الاتنين بيحافظوا على ترتيب الـ in-order.</figcaption>')

# ---------------------------------------------------------------- AVL rotations
def rotations():
    cs = [('LL: إدخال في شمال شمال → <b>rotation يمين</b>', '30(20(10,.),.)', '20(10,30)', {'30'}),
          ('RR: إدخال في يمين يمين → <b>rotation شمال</b>', '10(.,20(.,30))', '20(10,30)', {'10'}),
          ('LR: شمال ثم يمين → <b>rotation شمال على الابن ثم يمين</b>', '30(10(.,20),.)', '20(10,30)', {'30'}),
          ('RL: يمين ثم شمال → <b>rotation يمين على الابن ثم شمال</b>', '10(.,30(20,.))', '20(10,30)', {'10'})]
    out = []
    for title, a, b, crit in cs:
        out.append(lab(title, pair(tree(P(a, hi=crit, notes={list(crit)[0]: 'critical'}), 44, 42, 12, 'قبل'),
                                  tree(P(b), 44, 42, 12, 'بعد'))))
    F['ds21-rot'] = ('fig', row(*out, gap=28) + '<figcaption>الـ <b>critical node</b> هو أقرب جد للعقدة الجديدة اللي balance factor بتاعه بقى ±2. في الأربع حالات الجذر الجديد هو العقدة الوسطى (20) والترتيب in-order ما اتغيرش: 10 20 30.</figcaption>')

# ---------------------------------------------------------------- AVL build (Q18)
class A:
    def __init__(s, k): s.key = k; s.l = s.r = None; s.h = 1
def h(n): return n.h if n else 0
def upd(n): n.h = 1 + max(h(n.l), h(n.r))
def bf(n): return h(n.l) - h(n.r)
def rot_r(y): x = y.l; y.l = x.r; x.r = y; upd(y); upd(x); return x
def rot_l(x): y = x.r; x.r = y.l; y.l = x; upd(x); upd(y); return y
def avl_ins(n, k, log):
    if n is None: return A(k)
    if k < n.key: n.l = avl_ins(n.l, k, log)
    else: n.r = avl_ins(n.r, k, log)
    upd(n); b = bf(n)
    if b > 1:
        if k < n.l.key: log.append(f'LL@{n.key}'); return rot_r(n)
        log.append(f'LR@{n.key}'); n.l = rot_l(n.l); return rot_r(n)
    if b < -1:
        if k > n.r.key: log.append(f'RR@{n.key}'); return rot_l(n)
        log.append(f'RL@{n.key}'); n.r = rot_r(n.r); return rot_l(n)
    return n
def to_n(a, hi=()):
    if a is None: return None
    return N(str(a.key), to_n(a.l, hi), to_n(a.r, hi), cls='node hi' if a.key in hi else 'node', note=None)

def avl_q18():
    seq = [16, 27, 9, 11, 36, 54, 81, 63, 72]
    root = None; frames = []; allrot = []
    for k in seq:
        log = []
        root = avl_ins(root, k, log)
        allrot += log
        def n2(a):
            if a is None: return None
            return N(f'{a.key}', n2(a.l), n2(a.r), cls='node hi' if a.key == k else 'node')
        cap = f'أدخلنا <b>{k}</b>' + (' فحصل عدم توازن وصلحناه بـ <b>%s</b>.' % ' ثم '.join(log) if log else ' (التوازن سليم، مفيش rotation).')
        frames.append(frame(tree(n2(root), 28, 44, 12, 'AVL'), cap))
    assert ser(to_n(root)) == '16(9(.,11),63(36(27,54),81(72,.)))', ser(to_n(root))
    assert allrot == ['RR@27', 'RL@54', 'RR@36'], allrot
    F['ds21-avl18'] = ('steps', ''.join(frames))

# ---------------------------------------------------------------- Threaded tree
def threads():
    spec = '1(2(4(8,9),5),3(6(10,11),7(.,12)))'
    t = P(spec)
    pos = {}
    from svglib import _layout
    _layout(t, 0, [0], pos)
    dx, dy, r = 40, 54, 15
    byk = {}
    def collect(n):
        if n:
            byk[n.key] = pos[id(n)]; collect(n.l); collect(n.r)
    collect(t)
    right = [(5, 1), (8, 4), (9, 2), (10, 6), (11, 3)]
    left = [(5, 2), (7, 3), (9, 4), (10, 1), (11, 6), (12, 7)]
    # check against the C table: right threads / left threads of the in-order threading
    inord = []
    def io(n):
        if n: io(n.l); inord.append(int(n.key)); io(n.r)
    io(t)
    assert inord == [8, 4, 9, 2, 5, 1, 10, 6, 11, 3, 7, 12]
    for a, b in right: assert inord[inord.index(a) + 1] == b
    for a, b in left: assert inord[inord.index(a) - 1] == b
    def with_threads(lst, cls, dash, depth):
        base = tree(t, dx, dy, r, 'threaded')
        xy = lambda k: (10 + dx / 2 + byk[str(k)][0] * dx, 10 + r + byk[str(k)][1] * dy)
        ex = []
        for a, c in lst:
            (x1, y1), (x2, y2) = xy(a), xy(c)
            ex.append(f'<path d="M{x1:.1f},{y1 + r:.1f} Q{(x1 + x2) / 2:.1f},{max(y1, y2) + depth:.1f} {x2:.1f},{y2 + r + 1:.1f}" class="{cls}" style="fill:none"{dash} marker-end="url(#ah)"/>')
        import re
        H = float(re.search(r'viewBox="0 0 [\d.]+ ([\d.]+)"', base).group(1)) + 60
        base = re.sub(r'(viewBox="0 0 [\d.]+ )[\d.]+(")', lambda m: m.group(1) + ('%d' % H) + m.group(2), base, count=1)
        base = re.sub(r'height="[\d.]+"', 'height="%d"' % H, base, count=1)
        return base.replace('</svg>', ''.join(ex) + '</svg>')
    one = with_threads(right, 'arr', '', 38)
    F['ds21-thread'] = ('fig', row(lab('one-way in-order: right threads بس', one), lab('two-way in-order: right + left threads', with_threads_both(t, byk, dx, dy, r, right, left)), gap=24) + '<figcaption>الـ threading لشجرة Thareja (Q16). الخطوط الصلبة (<b>right threads</b>) بتشاور على الـ in-order successor، والمتقطعة (<b>left threads</b>) على الـ predecessor. in-order = 8 4 9 2 5 1 10 6 11 3 7 12: الـ right thread لأي عقدة من غير ابن يمين هو العقدة اللي بعدها في السلسلة دي (5→1، 8→4، 9→2، 10→6، 11→3)، والـ left thread هو اللي قبلها (5→2، 7→3، 9→4، 10→1، 11→6، 12→7). العقدة 8 ملهاش predecessor والعقدة 12 ملهاش successor فخانتهم NULL.</figcaption>')

def with_threads_both(t, byk, dx, dy, r, right, left):
    import re
    base = tree(t, dx, dy, r, 'threaded')
    xy = lambda k: (10 + dx / 2 + byk[str(k)][0] * dx, 10 + r + byk[str(k)][1] * dy)
    ex = []
    for a, c in right:
        (x1, y1), (x2, y2) = xy(a), xy(c)
        ex.append(f'<path d="M{x1 + 4:.1f},{y1 + r:.1f} Q{(x1 + x2) / 2 + 6:.1f},{max(y1, y2) + 34:.1f} {x2 + 4:.1f},{y2 + r + 1:.1f}" class="arr" style="fill:none" marker-end="url(#ah)"/>')
    for a, c in left:
        (x1, y1), (x2, y2) = xy(a), xy(c)
        ex.append(f'<path d="M{x1 - 4:.1f},{y1 + r:.1f} Q{(x1 + x2) / 2 - 6:.1f},{max(y1, y2) + 52:.1f} {x2 - 4:.1f},{y2 + r + 1:.1f}" class="arr hi" style="fill:none" stroke-dasharray="4 3" marker-end="url(#ah)"/>')
    H = float(re.search(r'viewBox="0 0 [\d.]+ ([\d.]+)"', base).group(1)) + 70
    base = re.sub(r'(viewBox="0 0 [\d.]+ )[\d.]+(")', lambda m: m.group(1) + ('%d' % H) + m.group(2), base, count=1)
    base = re.sub(r'height="[\d.]+"', 'height="%d"' % H, base, count=1)
    return base.replace('</svg>', ''.join(ex) + '</svg>')

# ---------------------------------------------------------------- Red-black insertion 1..7
class R:
    def __init__(s, k): s.key = k; s.l = s.r = s.p = None; s.red = True
def rb_rot_l(root, x):
    y = x.r; x.r = y.l
    if y.l: y.l.p = x
    y.p = x.p
    if x.p is None: root = y
    elif x is x.p.l: x.p.l = y
    else: x.p.r = y
    y.l = x; x.p = y; return root
def rb_rot_r(root, x):
    y = x.l; x.l = y.r
    if y.r: y.r.p = x
    y.p = x.p
    if x.p is None: root = y
    elif x is x.p.r: x.p.r = y
    else: x.p.l = y
    y.r = x; x.p = y; return root
def rb_insert(root, k, cases):
    z = R(k); y = None; x = root
    while x: y = x; x = x.l if k < x.key else x.r
    z.p = y
    if y is None: root = z
    elif k < y.key: y.l = z
    else: y.r = z
    while z.p and z.p.red:
        g = z.p.p
        if z.p is g.l:
            u = g.r
            if u and u.red: cases.append('I1'); z.p.red = u.red = False; g.red = True; z = g
            else:
                if z is z.p.r: cases.append('I2'); z = z.p; root = rb_rot_l(root, z)
                cases.append('I3'); z.p.red = False; g.red = True; root = rb_rot_r(root, g)
        else:
            u = g.l
            if u and u.red: cases.append('I1'); z.p.red = u.red = False; g.red = True; z = g
            else:
                if z is z.p.l: cases.append('I2'); z = z.p; root = rb_rot_r(root, z)
                cases.append('I3'); z.p.red = False; g.red = True; root = rb_rot_l(root, g)
    root.red = False
    return root
def rb_ser(n):
    if n is None: return '.'
    c = 'r' if n.red else 'b'
    if not n.l and not n.r: return f'{n.key}{c}'
    return f'{n.key}{c}({rb_ser(n.l)},{rb_ser(n.r)})'
def rb_to_n(n):
    if n is None: return None
    return N(str(n.key), rb_to_n(n.l), rb_to_n(n.r), cls='node' if n.red else 'node bk', red=n.red)
def rb_seven():
    root = None; frames = []; allc = []
    for k in range(1, 8):
        c = []
        root = rb_insert(root, k, c); allc += c
        cap = f'أدخلنا <b>{k}</b>' + (' وصلّحنا بالحالات: <b>%s</b>' % ' ثم '.join(c) if c else (' (جذر أسود)' if k == 1 else ' (الأب أسود: مفيش مشكلة)'))
        frames.append(frame(tree(rb_to_n(root), 30, 46, 13, 'red-black'), cap))
    assert rb_ser(root) == '2b(1b,4r(3b,6b(5r,7r)))', rb_ser(root)
    assert allc == ['I3', 'I1', 'I3', 'I1', 'I3'], allc
    F['ds21-rb7'] = ('steps', ''.join(frames))

# ---------------------------------------------------------------- Splay
class S:
    def __init__(s, k): s.key = k; s.l = s.r = s.p = None
def s_rot(x):
    p = x.p; g = p.p
    if p.l is x:
        p.l = x.r
        if x.r: x.r.p = p
        x.r = p
    else:
        p.r = x.l
        if x.l: x.l.p = p
        x.l = p
    p.p = x; x.p = g
    if g:
        if g.l is p: g.l = x
        else: g.r = x
def s_to_n(n, hi):
    if n is None: return None
    return N(str(n.key), s_to_n(n.l, hi), s_to_n(n.r, hi), cls='node hi' if n.key == hi else 'node')
def s_ser(n):
    if n is None: return '.'
    if not n.l and not n.r: return str(n.key)
    return f'{n.key}({s_ser(n.l)},{s_ser(n.r)})'
def s_root(x):
    while x.p: x = x.p
    return x
def splay_steps():
    nodes = {k: S(k) for k in range(1, 8)}
    # inserting 1..7 in order into a BST: the chain 7(6(5(4(3(2(1)))))) after splaying each new key to the root
    for k in range(2, 8):
        nodes[k].l = nodes[k - 1]; nodes[k - 1].p = nodes[k]
    assert s_ser(nodes[7]) == '7(6(5(4(3(2(1,.),.),.),.),.),.)'
    frames = [frame(tree(s_to_n(nodes[7], 1), 24, 40, 11, 'chain'), 'الحالة الابتدائية بعد إدخال 1..7 (كل إدخال بيعمل splay للمفتاح الجديد للجذر فبيطلع chain شمالي): عمق 1 = 6.')]
    def splay(x, tag):
        steps = []
        while x.p:
            p = x.p; g = p.p
            if g is None: s_rot(x); steps.append('zig')
            elif (g.l is p) == (p.l is x): s_rot(p); s_rot(x); steps.append('zig-zig')
            else: s_rot(x); s_rot(x); steps.append('zig-zag')
            frames.append(frame(tree(s_to_n(s_root(x), x.key), 24, 40, 11, 'splay'), f'{tag}: بعد الخطوة <b>{steps[-1]}</b> المفتاح {x.key} أقرب للجذر.'))
        return steps
    st = splay(nodes[1], 'search(1)')
    assert st == ['zig-zig'] * 3 and s_ser(nodes[1]) == '1(.,6(4(2(.,3),5),7))', s_ser(nodes[1])
    st = splay(nodes[2], 'search(2)')
    assert st == ['zig-zig', 'zig'] and s_ser(nodes[2]) == '2(1,4(3,6(5,7)))', (st, s_ser(nodes[2]))
    F['ds21-splay'] = ('steps', ''.join(frames))

bst_delete(); rotations(); avl_q18(); threads(); rb_seven(); splay_steps()
