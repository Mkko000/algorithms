"""Insert empty figure markers into lessons (idempotent). Author: Mohammed El Sisi."""
import os, sys
root = os.path.join(os.path.dirname(__file__), '..', 'docs', 'lessons')
def place(fname, anchor, name, where='before'):
    p = os.path.join(root, fname); s = open(p, encoding='utf-8').read()
    mk = f'<!--fig:{name}--><!--/fig:{name}-->'
    if f'<!--fig:{name}-->' in s: return
    if anchor not in s:
        print('ANCHOR NOT FOUND', fname, name, anchor[:40]); return
    i = s.index(anchor)
    if where == 'after': i += len(anchor)
    s = s[:i] + mk + '\n' + s[i:]
    open(p, 'w', encoding='utf-8').write(s)
if __name__ == '__main__':
    import placements
