"""Figures for chapters 12-16 (searching). Each trace is produced by
simulating the book's algorithm, so the pictures are the real thing.
Author: Mohammed El Sisi."""
from web.lib.render import *
import random
F = {}
KEYS = list('ASERCHINGXMPL')

def side(*items):
    """several svgs side by side, each with a caption on top"""
    cells = ''.join(f'<div class="fside-item"><small>{t}</small><br>{s}</div>' for t, s in items)
    return f'<div class="fside">{cells}</div>'

# ---------------- chapter 12 ----------------
def mark(t, path=(), new=None, cls_path='node hi2', cls_new='node hi'):
    def walk(n):
        if n is None: return
        n.cls = cls_new if n is new else (cls_path if n in path else 'node')
        walk(n.l); walk(n.r)
    walk(t); return t

def bst_insert_steps():
    root = None; fr = []
    for k in KEYS:
        path = []
        if root is None: root = N(k); new = root
        else:
            h = root
            while True:
                path.append(h)
                if k < h.key:
                    if h.l is None: h.l = N(k); new = h.l; break
                    h = h.l
                else:
                    if h.r is None: h.r = N(k); new = h.r; break
                    h = h.r
        mark(root, path, new)
        dirs = ' ← '.join(p.key for p in path) if path else ''
        cap = f'insert <b>{k}</b>: ' + (f'مشينا في المسار {" → ".join(p.key for p in path)} ووقفنا عند link فاضي، فحطّينا {k} هناك.' if path else 'الشجرة فاضية، فـ A بقت الجذر.')
        fr.append(frame(tree(root, dx=30, dy=46, r=14), cap))
    F['bst-insert'] = ('steps', ''.join(fr))
    # search figure
    t = build_bst(KEYS)
    def path_to(t, k):
        p = []
        while t is not None:
            p.append(t)
            if k == t.key: break
            t = t.l if k < t.key else t.r
        return p
    p = path_to(t, 'H'); mark(t, p[:-1], p[-1]); a = tree(t, dx=28, dy=44, r=13)
    t = build_bst(KEYS); p = path_to(t, 'Q'); mark(t, p, None); b = tree(t, dx=28, dy=44, r=13, show_null=True)
    F['bst-search'] = ('fig', fig(side(('search H (hit)', a), ('search Q (miss)', b)),
        'البحث في BST: كل مقارنة بتنزّلنا مستوى. يمين: لقينا H بعد 5 مقارنات (A، S، E، R، H). شمال: Q مش موجود — عدّينا على A S E R H I N P ووصلنا لـ link فاضي (الخطوط المتقطعة). المكان ده بالظبط هو اللي هيتحط فيه Q لو عملنا insert.'))

def rot_r(h):
    x = h.l; h.l = x.r; x.r = h; return x
def rot_l(h):
    x = h.r; h.r = x.l; x.l = h; return x

def rotation_fig():
    def before():
        return N('S', N('E', N('C', cls='node dim'), N('R', cls='node dim'), cls='node hi'), N('X', cls='node dim'), cls='node hi2')
    b = before(); a = rot_r(before())
    F['rotation'] = ('fig', fig(side(('قبل: rotR(S)', tree(b, dx=36, dy=50)), ('بعد', tree(a, dx=36, dy=50))),
        'الـ right rotation: E طلعت مكان S، وS نزلت يمين E. الحاجة الوحيدة اللي اتنقلت هي R: كانت يمين E وبقت شمال S (وده سليم لأن E &lt; R &lt; S). كل الباقي في مكانه، والـ inorder هو هو: C E R S X. تلات تعيينات pointers بس.'))

def root_insert_steps():
    t = build_bst(list('ASERCHIN')); fr = []
    # insert G at bottom then rotate it up, recording each tree
    def ins(h, k, path):
        if h is None: return N(k)
        path.append(h)
        if k < h.key: h.l = ins(h.l, k, path)
        else: h.r = ins(h.r, k, path)
        return h
    path = []; t = ins(t, 'G', path)
    g = find(t, 'G'); mark(t, path, g)
    fr.append(frame(tree(t, dx=30, dy=44, r=14), 'الأول: G اتحطت تحت عادي (insert في الأوراق). المسار المعلّم هو اللي هنطلع فيه.'))
    while path:
        p = path.pop()
        parent = path[-1] if path else None
        if p.l is g: nr = rot_r(p); d = 'rotR'
        else: nr = rot_l(p); d = 'rotL'
        if parent is None: t = nr
        elif parent.l is p: parent.l = nr
        else: parent.r = nr
        mark(t, path, g)
        fr.append(frame(tree(t, dx=30, dy=44, r=14), f'{d}({p.key}): G طلعت مستوى فوق {p.key}.' + (' وصلت للجذر!' if parent is None else '')))
    F['root-insert'] = ('steps', ''.join(fr))

def size(t): return 0 if t is None else 1 + size(t.l) + size(t.r)
def partR(h, k):
    t = size(h.l)
    if t > k: h.l = partR(h.l, k); h = rot_r(h)
    if t < k: h.r = partR(h.r, k - t - 1); h = rot_l(h)
    return h

def delete_steps():
    t = build_bst(KEYS); fr = []
    e = find(t, 'E'); mark(t, (), e); g = find(t, 'G'); g.cls = 'node hi2'
    fr.append(frame(tree(t, dx=30, dy=44, r=14), 'عايزين نمسح E (المظلّل) وليها ولدين. الحل في Program 12.15: نجيب أصغر حاجة في الشجرة اليمين (G، المحدّد) ونخلّيها مكان E.'))
    r = partR(e.r, 0)
    e.r = r; mark(t, (), e); find(t, 'G').cls = 'node hi2'
    fr.append(frame(tree(t, dx=30, dy=44, r=14), 'partR(E→r, 0): بالـ rotations طلّعنا أصغر عنصر في الشجرة اليمين (G) لجذرها. دلوقتي G مالهاش ولد شمال أكيد.'))
    r.l = e.l; t.r.l = r; mark(t, (), r)
    fr.append(frame(tree(t, dx=30, dy=44, r=14), 'joinLR: علّقنا الشجرة الشمال بتاعة E (اللي فيها C) شمال G، وG خدت مكان E. خلاص، E اتمسحت والـ BST لسه سليمة.'))
    F['bst-delete'] = ('steps', ''.join(fr))

# ---------------- chapter 13 ----------------
def splay_fig():
    def chain():
        return build_bst(list('87654321'))
    # rotate-to-root of '1'
    def to_root(h, k):
        if h is None or h.key == k: return h
        if k < h.key: h.l = to_root(h.l, k); return rot_r(h)
        h.r = to_root(h.r, k); return rot_l(h)
    def splay(h, k):
        if h is None or h.key == k: return h
        if k < h.key:
            if h.l is None: return h
            if k < h.l.key: h.l.l = splay(h.l.l, k); h = rot_r(h)
            elif k > h.l.key: h.l.r = splay(h.l.r, k); h.l = rot_l(h.l)
            return rot_r(h) if h.l else h
        if h.r is None: return h
        if k > h.r.key: h.r.r = splay(h.r.r, k); h = rot_l(h)
        elif k < h.r.key: h.r.l = splay(h.r.l, k); h.r = rot_r(h.r)
        return rot_l(h) if h.r else h
    a = chain(); find(a, '1').cls = 'node hi'
    b = to_root(chain(), '1'); b.cls = 'node hi'
    c = splay(chain(), '1'); c.cls = 'node hi'
    F['splay'] = ('fig', fig(side(('قبل: سلسلة (بعد insert بالترتيب)', tree(a, dx=24, dy=34, r=11)), ('rotate-to-root عادي', tree(b, dx=24, dy=34, r=11)), ('splay (rotations مزدوجة)', tree(c, dx=24, dy=34, r=11))),
        'ليه الـ splay أحسن من مجرد «طلّعه للجذر»؟ بحثنا عن 1 في سلسلة طولها 8. الـ rotations العادية طلّعت 1 للجذر بس السلسلة فضلت بنفس الطول تقريبًا. الـ splay بيعمل rotations اتنين اتنين (zig-zig) من فوق — النتيجة إن كل عقدة على المسار عمقها <b>اتقسم تقريبًا على 2</b>. وده سر الـ amortized bound بتاع Property 13.5.'))

class Node4:
    def __init__(self, keys, kids=None): self.keys, self.kids = keys, kids or []

def t234_insert(root, k, events):
    def split(n, parent):
        a, m, b = n.keys
        L = Node4([a], n.kids[:2]); R = Node4([b], n.kids[2:])
        if parent is None: events.append(f'الجذر 4-node [{a} {m} {b}] اتقسم و{m} بقى جذر جديد (الشجرة طولت مستوى).'); return Node4([m], [L, R])
        i = parent.kids.index(n); parent.kids[i:i + 1] = [L, R]; parent.keys.insert(i, m)
        events.append(f'الـ 4-node [{a} {m} {b}] اتقسم في السكة، و{m} طلع للأب.')
        return parent
    if root is None: return Node4([k])
    if len(root.keys) == 3: root = split(root, None)
    n = root
    while n.kids:
        i = sum(1 for x in n.keys if x < k)
        c = n.kids[i]
        if len(c.keys) == 3:
            split(c, n); i = sum(1 for x in n.keys if x < k); c = n.kids[i]
        n = c
    n.keys.append(k); n.keys.sort()
    return root

def to_m(n, hi=None):
    return M(n.keys, [to_m(c, hi) for c in n.kids], 'mnode hi' if n is hi else 'mnode')

def t234_steps():
    root = None; fr = []
    for k in 'ASEARCHINGEXAMPLE':
        ev = []; root = t234_insert(root, k, ev)
        # find leaf containing new key (last placed)
        cap = f'insert <b>{k}</b>. ' + (' '.join(ev) if ev else 'مفيش 4-node في السكة، فدخل في الورقة على طول.')
        fr.append(frame(mtree(to_m(root), kw=20, gap=10, dy=50), cap))
    F['t234-insert'] = ('steps', ''.join(fr))
    return root

# --- red-black: faithful simulation of Program 13.6 ---
class RB:
    def __init__(s, k): s.key, s.l, s.r, s.red = k, None, None, True
def isred(x): return x is not None and x.red
def rb_insert(h, k, sw, ev):
    if h is None: return RB(k)
    if isred(h.l) and isred(h.r):
        h.red = True; h.l.red = False; h.r.red = False; ev.append(f'color flip عند {h.key} (كان 4-node: قسمناه)')
    if k < h.key:
        h.l = rb_insert(h.l, k, False, ev)
        if h.red and isred(h.l) and sw: ev.append(f'rotR({h.key}) — أول rotation من الاتنين (حالة zig-zag)'); h = rr(h)
        if isred(h.l) and isred(h.l.l): ev.append(f'rotR({h.key}) وتبديل ألوان (أحمرين ورا بعض)'); h = rr(h); h.red = False; h.r.red = True
    else:
        h.r = rb_insert(h.r, k, True, ev)
        if h.red and isred(h.r) and not sw: ev.append(f'rotL({h.key}) — أول rotation من الاتنين (حالة zig-zag)'); h = rl(h)
        if isred(h.r) and isred(h.r.r): ev.append(f'rotL({h.key}) وتبديل ألوان (أحمرين ورا بعض)'); h = rl(h); h.red = False; h.l.red = True
    return h
def rr(h): x = h.l; h.l = x.r; x.r = h; return x
def rl(h): x = h.r; h.r = x.l; x.l = h; return x
def rb_to_n(h, new=None):
    if h is None: return None
    return N(h.key, rb_to_n(h.l, new), rb_to_n(h.r, new), 'node hi' if h is new else 'node', h.red)

def rb_steps():
    root = None; fr = []
    for k in 'ASEARCHINGEXAMPLE':
        ev = []; root = rb_insert(root, k, False, ev)
        if root.red: root.red = False
        cap = f'insert <b>{k}</b>: ' + ('، '.join(ev) if ev else 'مفيش flips ولا rotations — دخل كعقدة حمرا في الآخر.')
        fr.append(frame(tree(rb_to_n(root), dx=26, dy=46, r=13), cap))
    F['rb-insert'] = ('steps', ''.join(fr))
    out = []
    def ino(h):
        if h: ino(h.l); out.append(h.key + ('*' if h.red else '')); ino(h.r)
    ino(root); return ' '.join(out)

def rb_cases():
    def mk(spec):
        # spec: nested tuples (key, red, left, right)
        if spec is None: return None
        k, red, l, r = spec
        return N(k, mk(l), mk(r), 'node', red)
    flip_b = mk(('M', False, ('E', True, None, None), ('R', True, None, None)))
    flip_a = mk(('M', True, ('E', False, None, None), ('R', False, None, None)))
    zz_b = mk(('S', False, ('M', True, ('E', True, None, None), None), ('X', False, None, None)))
    zz_a = mk(('M', False, ('E', True, None, None), ('S', True, None, ('X', False, None, None))))
    zg_b = mk(('S', False, ('E', True, None, ('M', True, None, None)), ('X', False, None, None)))
    zg_m = mk(('S', False, ('M', True, ('E', True, None, None), None), ('X', False, None, None)))
    F['rb-cases'] = ('fig', fig(
        side(('4-node: قبل', tree(flip_b, dx=34, dy=44)), ('color flip', tree(flip_a, dx=34, dy=44))) +
        side(('أحمرين على خط واحد', tree(zz_b, dx=34, dy=44)), ('rotR(S) + ألوان', tree(zz_a, dx=34, dy=44))) +
        side(('أحمرين zig-zag', tree(zg_b, dx=34, dy=44)), ('rotL(E) الأول…', tree(zg_m, dx=34, dy=44)), ('…وبعدين rotR(S)', tree(zz_a, dx=34, dy=44))),
        'الحالات التلاتة في Program 13.6 (الحلقة الحمرا = عقدة حمرا، والخط الأحمر = link أحمر). (1) عقدة سودا بولدين أحمر = 4-node: نقلب الألوان، وده split في 2-3-4. (2) لو الـ flip عمل أحمرين ورا بعض على نفس الاتجاه: rotation واحدة. (3) لو في اتجاهين (zig-zag): rotation تحت الأول تحوّلها للحالة (2)، وبعدين rotation فوق.'))

def t234_vs_rb(root234, rb_inorder):
    pass

def skiplist_fig():
    random.seed(4)
    keys = sorted(random.sample(range(10, 99), 12))
    lv = []
    for _ in keys:
        j = 1
        while random.random() < 0.5 and j < 4: j += 1
        lv.append(j)
    L = max(lv); w = 40; gap = 16; out = [DEFS]; H = L * 34 + 30
    x0 = 10
    for j in range(L): out.append(rect(x0, H - 30 - j * 34, 26, 24, 'cell hi2', 3))
    out.append(text(x0 + 13, H - 4, 'head', 'lab', 'middle', 10))
    xs = []
    for i, (k, l) in enumerate(zip(keys, lv)):
        x = x0 + 40 + i * (w + gap); xs.append(x)
        for j in range(l): out.append(rect(x, H - 30 - j * 34, w, 24, 'cell hi' if (k == keys[7]) else 'cell', 3))
        out.append(text(x + w / 2, H - 13, k, 'tc', 'middle', 12))
    for j in range(L):
        prev = x0 + 26; y = H - 18 - j * 34
        for x, l in zip(xs, lv):
            if l > j: out.append(arrow(prev, y, x - 2, y, 'arr')); prev = x + w
        out.append(arrow(prev, y, prev + 20, y, 'arr')); out.append(text(prev + 28, y + 4, '∅', 'lab', 'middle', 11))
    W = xs[-1] + w + 50
    F['skiplist'] = ('fig', fig(svg(W, H + 6, ''.join(out)), f'Skip list فيها {len(keys)} مفتاح. كل عقدة عدد links بتاعها عشوائي (احتمال ½ إنها تطلع مستوى كمان)، فحوالي نصهم مستوى 1، ربعهم مستوى 2… البحث عن {keys[7]} (المظلّل): ابدأ من أعلى مستوى في head، امشي يمين لحد ما الجاي يبقى أكبر، انزل مستوى، كرّر. كأنها binary search على قايمة.'))

# ---------------- chapter 14 ----------------
BOOKH = dict(zip('ASERCHINGXMPL', [7, 3, 9, 9, 8, 4, 11, 7, 10, 12, 0, 8, 6]))
BOOKH2 = dict(zip('ASERCHINGXMPL', [1, 3, 1, 5, 5, 5, 3, 3, 2, 3, 5, 4, 2]))

def chaining_fig():
    Mb = 5; heads = {i: [] for i in range(Mb)}
    for k in KEYS:
        heads[(ord(k) - 64) * 11 % Mb].insert(0, k)
    out = [DEFS]; y0 = 10
    for i in range(Mb):
        y = y0 + i * 38
        out.append(rect(10, y, 30, 28, 'cell hi2', 3)); out.append(text(25, y + 19, i, 'tc', 'middle', 12))
        x = 60; prev = 40
        for k in heads[i]:
            out.append(arrow(prev, y + 14, x - 2, y + 14)); out.append(rect(x, y, 30, 28, 'cell', 3)); out.append(text(x + 15, y + 19, k, 'tc', 'middle', 12))
            prev = x + 30; x += 50
        out.append(text(prev + 14, y + 19, '∅', 'lab', 'middle', 11))
    W = 60 + 50 * max(len(v) for v in heads.values()) + 30
    F['chaining'] = ('fig', fig(svg(W, Mb * 38 + 14, ''.join(out)), 'Separate chaining بـ M = 5 قوايم و13 مفتاح (h = 11k mod 5، وk رقم الحرف). كل مفتاح جديد بيتحط في <b>أول</b> قايمته (Program 14.3). البحث = حساب h + sequential search في قايمة واحدة طولها في المتوسط N/M ≈ 2.6.'))

def probe_steps(double=False):
    Mt = 13; T = [None] * Mt; fr = []
    for k in KEYS:
        i = BOOKH[k]; step = BOOKH2[k] if double else 1; probes = []
        while T[i] is not None: probes.append(i); i = (i + step) % Mt
        T[i] = k
        c = {p: 'bad' for p in probes}; c[i] = 'hi'
        hdr = list(range(Mt))
        cap = f'<b>{k}</b>: h = {BOOKH[k]}' + (f'، h₂ = {step}' if double else '') + (f'. اصطدم في {", ".join(map(str, probes))} ودخل في {i}.' if probes else f'. الخانة فاضية، دخل على طول.')
        fr.append(frame(array_rows([('', hdr, {x: 'dim' for x in range(Mt)}), ('', [x or '' for x in T], c)], 28, 0), cap))
    F['double-hash' if double else 'linear-probe'] = ('steps', ''.join(fr))

# ---------------- chapter 15 ----------------
CODE = {ch: format(ord(ch) - 64, '05b') for ch in 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'}
DSTK = list('ASERCHINGXMP')

def dst_fig():
    root = None
    def ins(h, k, d):
        if h is None: return N(k)
        if CODE[k][d] == '0': h.l = ins(h.l, k, d + 1)
        else: h.r = ins(h.r, k, d + 1)
        return h
    for k in DSTK: root = ins(root, k, 0)
    # draw with structural layout (not inorder): use mtree on binary
    def to_m(h):
        if h is None: return None
        kids = [to_m(h.l), to_m(h.r)]
        n = M([h.key], [k if k else M([], [], 'virtual') for k in kids] if any(kids) else [], 'mnode')
        return n
    F['dst'] = ('fig', fig(bintree_struct(root, lambda n: n.key), 'Digital search tree للمفاتيح A S E R C H I N G X M P (أكواد 5 بت: A=00001، S=10011…). كل مستوى بنبص على بت: البت الأول يقرّر شمال (0) ولا يمين (1) من الجذر، التاني من المستوى اللي بعده… لاحظ إن الشجرة <b>مش</b> BST: مثلًا الـ inorder مش مترتب. بس طولها محكوم بعدد البتات.'))

def bintree_struct(root, label, cls=lambda n: 'node', dx=30, dy=46, r=14, edge_lab=True, leafbox=None):
    """layout a binary tree by structure (each null keeps half the width)."""
    widths = {}
    def w(n, d):
        if n is None: return 1
        if n.l is None and n.r is None: return 2
        widths[id(n)] = w(n.l, d + 1) + w(n.r, d + 1); return widths[id(n)]
    total = w(root, 0); out = [DEFS]; maxd = [0]; U = dx / 2
    def place(n, x0, d):
        if n is None: return None
        maxd[0] = max(maxd[0], d)
        ww = widths.get(id(n), 2); cx = 10 + (x0 + ww / 2) * U; cy = 10 + r + d * dy
        if n.l is not None or n.r is not None:
            lw = 1 if n.l is None else (widths.get(id(n.l), 2))
            for c, xx, b in ((n.l, x0, '0'), (n.r, x0 + lw, '1')):
                if c is None: continue
                pt = place(c, xx, d + 1)
                out.insert(1, line(cx, cy, pt[0], pt[1], 'edge'))
                if edge_lab: out.append(text((cx + pt[0]) / 2 + (-6 if b == '0' else 6), (cy + pt[1]) / 2, b, 'elab', 'middle', 10))
        lab = label(n)
        if lab is None:
            out.append(f'<circle cx="{cx:.1f}" cy="{cy:.1f}" r="5" class="dot"/>')
        elif leafbox and n.l is None and n.r is None:
            out.append(rect(cx - 13, cy - 12, 26, 24, 'cell hi', 3)); out.append(text(cx, cy + 5, lab, 'tc', 'middle', 12))
        else:
            out.append(f'<circle cx="{cx:.1f}" cy="{cy:.1f}" r="{r}" class="{cls(n)}"/>'); out.append(text(cx, cy + 5, lab, 'tn', 'middle', 12))
        return (cx, cy)
    place(root, 0, 0)
    return svg(total * U + 20, (maxd[0] + 1) * dy + 20, ''.join(out))

def trie_build(keys):
    def ins(h, k, d):
        if h is None: return N(k)
        if h.l is None and h.r is None and h.key is not None:
            if h.key == k: return h
            old = h; h = N(None)
            # split until bits differ
            if CODE[old.key][d] == '0': h.l = old
            else: h.r = old
        if CODE[k][d] == '0': h.l = ins(h.l, k, d + 1)
        else: h.r = ins(h.r, k, d + 1)
        return h
    root = None
    for k in keys: root = ins(root, k, 0)
    return root

def trie_steps():
    fr = []
    for i in range(1, len(DSTK) + 1):
        t = trie_build(DSTK[:i]); k = DSTK[i - 1]
        fr.append(frame(bintree_struct(t, lambda n: n.key, leafbox=True), f'insert <b>{k}</b> = {CODE[k]}: انزل حسب البتات لحد ما توصل لـ link فاضي (حطه) أو لورقة فيها مفتاح تاني (افصلهم بعقد داخلية لحد أول بت مختلف).'))
    F['trie-insert'] = ('steps', ''.join(fr))

def patricia_fig():
    # Program 15.5 simulation with header node, bits counted from the left (bit 0 = leftmost of 5)
    class P:
        def __init__(s, k, b): s.key, s.bit, s.l, s.r = k, b, None, None
    def bit(k, b): return CODE[k][b] == '1' if 0 <= b < 5 else False
    head = P(None, -1); head.l = head.r = head
    head.key = '`'  # a key of all zeros
    CODE['`'] = '00000'
    def search(k):
        p = head; t = head.l
        while p.bit < t.bit: p = t; t = t.r if bit(k, t.bit) else t.l
        return t
    def insert(k):
        t = search(k)
        if t.key == k: return
        i = 0
        while bit(k, i) == bit(t.key, i): i += 1
        p = head; x = head.l
        while p.bit < x.bit and x.bit < i: p = x; x = x.r if bit(k, x.bit) else x.l
        n = P(k, i)
        n.l = x if bit(k, i) else n; n.r = n if bit(k, i) else x
        if p is head or not bit(k, p.bit): p.l = n
        else: p.r = n
        if p is head: p.l = n
    for k in DSTK: insert(k)
    # convert to drawable tree: downward links only; upward links become notes
    def conv(p, x):
        if x.bit <= p.bit: return None
        n = N(f'{x.key}', None, None, 'node', False)
        n.bitno = x.bit
        n.l = conv(x, x.l); n.r = conv(x, x.r)
        ups = []
        if x.l.bit <= x.bit: ups.append(f'0↑{x.l.key if x.l.key != "`" else "∅"}')
        if x.r.bit <= x.bit: ups.append(f'1↑{x.r.key if x.r.key != "`" else "∅"}')
        n.up = ' '.join(ups); return n
    root = conv(head, head.l)
    def lab(n): return n.key
    svgs = bintree_struct(root, lab, dx=44, dy=58, r=15)
    # add bit numbers and up-link notes via a second pass is complex; list them in caption table
    rows = []
    def walk(n):
        if n is None: return
        rows.append(f'<tr><td>{n.key}</td><td>{CODE[n.key]}</td><td>{n.bitno}</td><td dir="ltr"><code>{n.up or "—"}</code></td></tr>'); walk(n.l); walk(n.r)
    walk(root)
    tbl = '<table class="comp-table"><tr><th>العقدة</th><th>الكود</th><th>بتفرّع على بت رقم</th><th>links لفوق (0 = شمال، 1 = يمين)</th></tr>' + ''.join(rows) + '</table>'
    F['patricia'] = ('fig', fig(svgs + tbl, 'Patricia trie (Program 15.5) للمفاتيح A S E R C H I N G X M P: N عقدة بالظبط، كل عقدة فيها مفتاح ورقم البت اللي بتتفرّع عليه. الخطوط المرسومة هي الـ links «لتحت». الـ links اللي بترجع لفوق (لعقدة رقم البت بتاعها أصغر أو مساوي) مكتوبة في الجدول: لما البحث يمشي في link زي ده، يبقى وصل لمفتاح واحد بس يقارنه بالمفتاح كله. «∅» = الـ header (مفتاح كله أصفار).'))

def tst_fig():
    words = 'she sells sea shells by the sea shore'.split()
    class T:
        def __init__(s, c): s.c, s.l, s.m, s.r, s.end = c, None, None, None, False
    root = None
    def ins(h, w, d):
        c = w[d]
        if h is None: h = T(c)
        if c < h.c: h.l = ins(h.l, w, d)
        elif c > h.c: h.r = ins(h.r, w, d)
        elif d + 1 < len(w): h.m = ins(h.m, w, d + 1)
        else: h.end = True
        return h
    for w in words: root = ins(root, w, 0)
    widths = {}
    def wd(h):
        if h is None: return 0
        s = wd(h.l) + wd(h.m) + wd(h.r); widths[id(h)] = max(1, s); return widths[id(h)]
    total = wd(root); U = 30; dy = 40; out = [DEFS]; maxd = [0]
    def place(h, x0, d):
        maxd[0] = max(maxd[0], d)
        ww = widths[id(h)]; xx = x0; cx = None
        pts = []
        for c, kind in ((h.l, 'l'), (h.m, 'm'), (h.r, 'r')):
            if c is not None:
                pts.append((place(c, xx, d + 1), kind)); xx += widths[id(c)]
            elif kind == 'm' and h.l is None and h.r is None: pass
        cx = 10 + (x0 + ww / 2) * U; cy = 20 + d * dy
        for (px, py), kind in pts: out.insert(1, line(cx, cy, px, py, 'edge hi' if kind == 'm' else 'edge'))
        out.append(f'<circle cx="{cx:.1f}" cy="{cy:.1f}" r="12" class="node{" hi" if h.end else ""}"/>'); out.append(text(cx, cy + 5, h.c, 'tn', 'middle', 12))
        return (cx, cy)
    place(root, 0, 0)
    F['tst'] = ('fig', fig(svg(total * U + 20, (maxd[0] + 1) * dy + 20, ''.join(out)), 'TST للكلمات she sells sea shells by the shore: كل عقدة حرف وتلات links — أصغر (شمال)، <b>مساوي</b> (الخط الملوّن: نتقدّم للحرف الجاي)، أكبر (يمين). العقد الزرقا آخر كلمة. البحث عن «shells»: s، h (مساوي)، e، l، l، s — بنقرا كل حرف مرة واحدة تقريبًا، ومفيش مصفوفات 27 link فاضية زي الـ multiway trie.'))

# ---------------- chapter 16 ----------------
BKEYS = [int(x, 8) for x in '706 176 601 153 513 773 742 373 524 766 275 737 574 434 641 207 001 277 061 736 526 562 017 107 147'.split()]

def btree_steps(Mp=4):
    """Book-style B tree (Program 16.3): items only in external pages, internal pages
    hold (copy of first key, link). A page splits when it reaches M entries."""
    class Pg:
        def __init__(s, leaf): s.leaf, s.k, s.c = leaf, [], []
    root = Pg(True); H = [0]; fr = []
    def ins(p, key):
        if p.leaf:
            i = 0
            while i < len(p.k) and p.k[i] < key: i += 1
            p.k.insert(i, key)
        else:
            i = 0
            while i + 1 < len(p.k) and p.k[i + 1] <= key: i += 1
            r = ins(p.c[i], key)
            if r is None: return None
            p.k.insert(i + 1, r.k[0]); p.c.insert(i + 1, r)
        if len(p.k) < Mp: return None
        q = Pg(p.leaf); h = Mp // 2
        q.k = p.k[h:]; p.k = p.k[:h]
        if not p.leaf: q.c = p.c[h:]; p.c = p.c[:h]
        return q
    def tom(p, hl):
        lab = [format(x, '03o') for x in p.k]
        return M(lab, [tom(c, hl) for c in p.c], 'mnode hi' if p in hl else ('mnode' if not p.leaf else 'mnode'))
    for n, key in enumerate(BKEYS[:16]):
        before = set()
        r = ins(root, key); ev = ''
        if r is not None:
            nr = Pg(False); nr.k = [root.k[0], r.k[0]]; nr.c = [root, r]; root = nr; ev = ' الجذر اتقسم، فعملنا جذر جديد فوق: الشجرة طولت مستوى (وكل الأوراق لسه على نفس العمق).'
        # detect split pages: highlight pages containing the new key's page siblings - simply highlight leaf that holds key
        def leaf_of(p):
            while not p.leaf:
                i = 0
                while i + 1 < len(p.k) and p.k[i + 1] <= key: i += 1
                p = p.c[i]
            return p
        hl = {leaf_of(root)}
        msg = f'insert <b>{format(key, "03o")}</b>.' + (ev if ev else '')
        fr.append(frame(mtree(tom(root, hl), kw=30, gap=12, dy=56), msg))
    F['btree-insert'] = ('steps', ''.join(fr))

def btree_split_fig():
    full = M(['153', '176', '373', '513'], [], 'mnode hi')
    a = M(['153', '176'], [], 'mnode hi'); b = M(['373', '513'], [], 'mnode hi2')
    par = M(['153', '373'], [a, b], 'mnode')
    F['btree-split'] = ('fig', fig(side(('صفحة مليانة (M = 4 entries)', mtree(full, kw=34)), ('بعد الـ split', mtree(par, kw=34, dy=60))),
        'الـ split في B tree: لما صفحة توصل M entries، نقسمها نصّين، كل نص M/2، ونحط في الأب entry جديد بأول مفتاح في النص التاني ولينك ليه. لو الأب نفسه اتملى، نقسمه هو كمان، ولو الجذر اتقسم، الشجرة تطول من <b>فوق</b> — عشان كده كل الأوراق دايمًا على نفس العمق.'))

def extendible_fig():
    # keys from the book's octal list, using the leading bits of a 9-bit key
    Mp = 4; d = [0]; dirs = [None]
    class Pg:
        def __init__(s, k): s.k, s.keys = k, []
    p0 = Pg(0); dirs[0] = p0
    def bits(x, n): return x >> (9 - n) if n else 0
    fr = []
    def draw(cap, hi=None):
        D = 1 << d[0]; out = [DEFS]; pages = []
        for p in dirs:
            if p not in pages: pages.append(p)
        rowh = 30; H = max(D, len(pages)) * rowh + 30
        for i in range(D):
            y = 20 + i * rowh
            out.append(rect(10, y, 50, 24, 'cell hi2', 3)); out.append(text(35, y + 17, format(i, f'0{d[0]}b') if d[0] else '·', 'tc', 'middle', 11))
        for j, p in enumerate(pages):
            y = 20 + j * rowh
            out.append(rect(170, y, 4 * 38, 24, 'cell hi' if p is hi else 'cell', 3))
            out.append(text(160, y + 17, f'k={p.k}', 'lab', 'end', 10))
            for t, key in enumerate(sorted(p.keys)): out.append(text(170 + 19 + t * 38, y + 17, format(key, '03o'), 'tc', 'middle', 11))
        for i in range(D):
            j = pages.index(dirs[i]); out.append(arrow(60, 32 + i * rowh, 128, 32 + j * rowh))
        out.append(text(35, 12, f'directory d={d[0]}', 'elab', 'middle', 10))
        fr.append(frame(svg(340, H, ''.join(out)), cap))
    for n, key in enumerate(BKEYS[:14]):
        ev = []
        while True:
            p = dirs[bits(key, d[0])]
            if len(p.keys) < Mp: p.keys.append(key); break
            if p.k == d[0]:
                dirs = [x for x in dirs for _ in (0, 1)]; d[0] += 1; ev.append('الصفحة مليانة وعمقها = d، فضاعفنا الـ directory')
            q = Pg(p.k + 1); p.k += 1
            old = p.keys; p.keys = []
            for i in range(len(dirs)):
                if dirs[i] is p and (i >> (d[0] - p.k)) & 1: dirs[i] = q
            for x in old: dirs[bits(x, d[0])].keys.append(x)
            ev.append('قسمنا الصفحة على البت رقم ' + str(p.k))
        draw(f'insert <b>{format(key, "03o")}</b> ({format(key, "09b")}). ' + ('، '.join(ev) + '.' if ev else 'دخل في صفحته على طول.'), dirs[bits(key, d[0])])
    F['extendible'] = ('steps', ''.join(fr))

def isam_fig():
    out = [DEFS]
    idx = ['001', '207', '434', '641']
    out.append(rect(130, 10, 4 * 40, 26, 'cell hi2', 3))
    for i, k in enumerate(idx): out.append(text(150 + i * 40, 28, k, 'tc', 'middle', 11))
    pages = [['001', '017', '061', '107'], ['207', '275', '277', '373'], ['434', '513', '524', '601'], ['641', '706', '742', '773']]
    for i, pg in enumerate(pages):
        x = 10 + i * 105
        out.append(line(150 + i * 40, 36, x + 48, 80, 'edge'))
        out.append(rect(x, 80, 96, 26, 'cell', 3)); out.append(text(x + 48, 98, ' '.join(pg), 'tc', 'middle', 10))
    out.append(rect(220, 130, 96, 26, 'cell bad', 3)); out.append(text(268, 148, '526 562 574', 'tc', 'middle', 10)); out.append(arrow(268, 106, 268, 128))
    out.append(text(330, 148, 'overflow', 'lab', 'start', 10))
    F['isam'] = ('fig', fig(svg(440, 170, ''.join(out)), 'ISAM: index ثابت فوق (أول مفتاح في كل صفحة) وصفحات بيانات مترتبة. البحث: صفحة index + صفحة بيانات = probes قليلة وثابتة. بس لما نضيف 526 و562 و574 والصفحة بتاعتهم مليانة، بيروحوا لصفحة <b>overflow</b> (الأحمر). ومع كتر الإضافات الـ overflow chains بتطول والبحث بيتقل — ودي بالظبط المشكلة اللي الـ B tree اتعمل يحلّها.'))

for f in (bst_insert_steps, rotation_fig, root_insert_steps, delete_steps, splay_fig, t234_steps, rb_cases, skiplist_fig,
          chaining_fig, lambda: probe_steps(False), lambda: probe_steps(True), dst_fig, trie_steps, patricia_fig, tst_fig,
          btree_steps, btree_split_fig, extendible_fig, isam_fig):
    f()
RB_INORDER = rb_steps()
