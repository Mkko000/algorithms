#!/usr/bin/env python3
"""Mechanical inventory of the five source books (listings / examples / exercises per chapter).
Usage: python3 tools/coverage_scan.py BOOKS_DIR > inventory.json
Heuristic counts (OCR/two-column text) - used to build COVERAGE.md; see that file for caveats."""
import re, sys, json, os
B = sys.argv[1] if len(sys.argv) > 1 else '/workspace/books'
def L(name): return open(os.path.join(B, name), encoding='utf8', errors='ignore').read().split('\n')

def seq_count(lines, pat=r'^\s*(\d+)[\.\)]\s', prefix=None):
    """count items in sequential-numbered runs (1,2,3,...)."""
    exp, total, best = 1, 0, 0
    for ln in lines:
        m = re.match(pat, ln)
        if not m: continue
        n = int(m.group(1))
        if n == exp: exp += 1; total += 1
        elif n == 1: exp = 2; total += 1
    return total

def inc(lines): return sum(1 for l in lines if '#include' in l)
def mains(lines): return sum(1 for l in lines if re.search(r'\b(int|void)\s+main\s*\(', l))

books = {}

# ---------------- Thareja ----------------
t = L('ds-book-4.txt')
tst = {1:600,2:2741,3:3877,4:6565,5:7821,6:9156,7:12464,8:14460,9:16038,10:17105,11:19811,12:20720,13:21998,14:24332,15:26543,16:27840}
tst_end = {**{k: tst[k+1] for k in range(1,16)}, 16: 28584}
chs = {}
for c in range(1,17):
    a, b = tst[c]-1, tst_end[c]-1
    seg = t[a:b]
    pes = [i for i,l in enumerate(seg) if re.search(r'Programming Examples?\s*$', l)]
    ex = sorted({m for l in seg for m in re.findall(r'Example (%d\.\d+)' % c, l)}, key=lambda s:int(s.split('.')[1]))
    # exercises
    def region(start_pat, end_pats):
        s = next((i for i,l in enumerate(seg) if re.match(start_pat, l.strip())), None)
        if s is None: return []
        e = len(seg)
        for i in range(s+1, len(seg)):
            if any(re.match(p, seg[i].strip()) for p in end_pats): e = i; break
        return seg[s:e]
    rq = region(r'Review Questions', [r'Programming Exercises', r'Multiple-choice', r'Multiple Choice', r'True or False'])
    pe_ex = region(r'Programming Exercises', [r'Multiple-choice', r'Multiple Choice', r'True or False', r'Fill in the Blanks'])
    chs[c] = dict(code_programs=inc(seg), programming_examples=len([1 for l in seg if re.match(r'^\s*\d+\.\s+Write (a )?(program|an algorithm|a function)', l)]) ,
                  PE_blocks=len(pes), solved_examples=ex, review_q=seq_count(rq), prog_ex=seq_count(pe_ex))
# answer key counts
key = t[29244:]
keyc = {}
cur = None; sec = None
for l in key:
    m = re.match(r'^\s*Chapter (\d+)\s*$', l)
    if m: cur = int(m.group(1)); keyc[cur] = dict(mcq=0, tf=0, fib=0); continue
    if cur is None: continue
    s = l.strip()
    if s.startswith('Multiple-choice'): sec='mcq'; continue
    if s.startswith('True or False'): sec='tf'; continue
    if s.startswith('Fill in the Blanks'): sec='fib'; continue
    if sec == 'mcq': keyc[cur]['mcq'] += len(re.findall(r'\d+\.\s*\([a-d]\)', s))
    elif sec == 'tf': keyc[cur]['tf'] += len(re.findall(r'\d+\.\s*(True|False)', s))
    elif sec == 'fib': keyc[cur]['fib'] += len(re.findall(r'(?:^|\s)(\d+)\.\s', s))
for c in chs: chs[c].update(keyc.get(c, {}))
books['Thareja'] = chs

# ---------------- Bandyopadhyay & Dey ----------------
b = L('ds-using-c.txt')
bst = {1:270,2:600,3:1410,4:2400,5:3214,6:4015,7:5280,8:6363,9:7801,10:8735,11:10265,12:10859,13:12252}
bend = {**{k: bst[k+1] for k in range(1,13)}, 13: len(b)}
chs = {}
for c in range(1,14):
    seg = b[bst[c]-1:bend[c]-1]
    ex = sorted({m for l in seg for m in re.findall(r'Example (%d\.\d+)' % c, l)}, key=lambda s:int(s.split('.')[1]))
    ei = [i for i,l in enumerate(seg) if re.match(r'^\s*(EXERCISES|Exercises)\s*$', l)]
    exr = seg[ei[-1]:] if ei else []
    chs[c] = dict(code_programs=inc(seg)+mains(seg), solved_examples=ex, exercises=seq_count(exr))
books['Bandyopadhyay'] = chs

# ---------------- Pal & Halder ----------------
p = L('dsa-with-c.txt')
pst = {1:390,2:749,3:1399,4:2737,5:4025,6:5686,7:6570,8:7290,9:9673,10:11108,11:12903,12:13457}
pend = {**{k: pst[k+1] for k in range(1,12)}, 12: 14560}
chs = {}
for c in range(1,13):
    seg = p[pst[c]-1:pend[c]-1]
    ei = [i for i,l in enumerate(seg) if re.match(r'^\s*Exercises\s*$', l)]
    exr = seg[ei[-1]:] if ei else []
    chs[c] = dict(code_programs=inc(seg)+mains(seg), exercises=seq_count(exr),
                  examples=len([1 for l in seg if re.match(r'^\s*(Example|EXAMPLE)\b', l)]))
books['Pal'] = chs

# ---------------- Weiss ----------------
w = L('DataStructures.txt')
wst = {1:610,2:2990,3:4247,4:6345,5:9769,6:12324,7:14410,8:17290,9:18606,10:21982,11:24000,12:27262}
# find chapter 11 start
for i,l in enumerate(w):
    if i>22000 and re.match(r'^11\.1 ', l): wst[11]=i+1; break
wend = {**{k: wst[k+1] for k in range(1,12)}, 12: len(w)}
chs = {}
for c in range(1,13):
    seg = w[wst[c]-1:wend[c]-1]
    figs = sorted({m for l in seg for m in re.findall(r'^Figure (%d\.\d+)' % c, l.strip())}, key=lambda s:int(s.split('.')[1]))
    exs = sorted({int(m.group(1)) for l in seg for m in [re.match(r'^%d\.(\d+)\s' % c, l.strip())] if m})
    chs[c] = dict(figures=len(figs), exercise_ids=len(exs), max_ex=max(exs) if exs else 0)
books['Weiss'] = chs
json.dump(books, sys.stdout, indent=1)
