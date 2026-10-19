"""Fill <!--fig:NAME--> ... <!--/fig:NAME--> markers in docs/lessons/*.html
with the generated inline-SVG figures. Re-runnable. Author: Mohammed El Sisi.
Usage: python3 tools/fill.py"""
import re, glob, os, sys
sys.path.insert(0, os.path.dirname(__file__))
F = {}
for mod in ('figs_a', 'figs_b', 'figs_c', 'figs_ex'):
    try:
        F.update(__import__(mod).F)
    except ModuleNotFoundError as e:
        if e.name != mod: raise
root = os.path.join(os.path.dirname(__file__), '..', 'docs', 'lessons')
used, missing = set(), set()
pat = re.compile(r'<!--fig:([\w-]+)-->.*?<!--/fig:\1-->', re.S)
for path in sorted(glob.glob(os.path.join(root, '*.html'))):
    s = open(path, encoding='utf-8').read()
    def rep(m):
        name = m.group(1)
        if name not in F:
            missing.add(name); return m.group(0)
        used.add(name); kind, html = F[name]
        inner = f'<figure class="fig">{html}</figure>' if kind == 'fig' else f'<div class="steps">{html}</div>'
        return f'<!--fig:{name}-->{inner}<!--/fig:{name}-->'
    t = pat.sub(rep, s)
    if t != s: open(path, 'w', encoding='utf-8').write(t)
print(f'{len(F)} figures defined, {len(used)} placed')
if missing: print('MISSING:', ' '.join(sorted(missing)))
unused = set(F) - used
if unused: print('unused:', ' '.join(sorted(unused)))
