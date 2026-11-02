"""Build the Data-Structures lessons (docs/lessons/*.html) from tools/ds_src/*.html.
Author: Mohammed El Sisi.

A source lesson is a normal lesson fragment plus two kinds of directives, so the code and
the program output shown in the course are ALWAYS the real, tested ones from src/:

  <!--code:src/17-arrays-matrices/array_ops.c:insert-->   -> <pre><code> of the lines between
                                                            //>>insert  and  //<<insert
  <!--code:src/17-arrays-matrices/array_ops.c-->           -> the whole file
  <!--out:src/17-arrays-matrices/array_ops.c:12-->         -> compile + run, first 12 lines of output

Usage:  python3 tools/ds_build.py            (rebuild everything)
        python3 tools/ds_build.py 17         (only sources starting with 17)
"""
import re, os, sys, subprocess, html, hashlib
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
SRC = os.path.join(ROOT, 'tools', 'ds_src')
OUT = os.path.join(ROOT, 'docs', 'lessons')
BIN = '/tmp/ds_build_bin'
os.makedirs(BIN, exist_ok=True)

def extract(path, mark):
    lines = open(os.path.join(ROOT, path), encoding='utf-8').read().split('\n')
    if not mark:
        body = lines
        # drop the big header comment so the lesson shows the code itself
        if body and body[0].startswith('/*'):
            for i, l in enumerate(body):
                if '*/' in l: body = body[i + 1:]; break
        return '\n'.join(body).strip('\n')
    a = next((i for i, l in enumerate(lines) if '//>>' + mark in l), None)
    b = next((i for i, l in enumerate(lines) if '//<<' + mark in l), None)
    if a is None or b is None: raise SystemExit(f'marker {mark} not found in {path}')
    return '\n'.join(lines[a + 1:b])

def run(path, n):
    exe = os.path.join(BIN, hashlib.md5(path.encode()).hexdigest())
    r = subprocess.run(['gcc', '-std=c99', '-Wall', '-Wextra', '-O2', os.path.join(ROOT, path), '-o', exe, '-lm'],
                       capture_output=True, text=True)
    if r.returncode: raise SystemExit(f'compile failed {path}\n{r.stderr}')
    p = subprocess.run([exe], capture_output=True, text=True, timeout=60)
    if p.returncode: raise SystemExit(f'run failed {path} rc={p.returncode}\n{p.stderr}')
    out = p.stdout.rstrip('\n').split('\n')
    cut = out[:n]
    if len(out) > n: cut.append('...')
    return '\n'.join(cut)

def build(name):
    s = open(os.path.join(SRC, name), encoding='utf-8').read()
    s = re.sub(r'<!--code:([^:>]+)(?::([\w-]+))?-->',
               lambda m: '<pre><code>' + html.escape(extract(m.group(1), m.group(2))) + '</code></pre>', s)
    s = re.sub(r'<!--out:([^:>]+)(?::(\d+))?-->',
               lambda m: '<pre><code>' + html.escape(run(m.group(1), int(m.group(2) or 40))) + '</code></pre>', s)
    open(os.path.join(OUT, name), 'w', encoding='utf-8').write(s)
    return len(s)

if __name__ == '__main__':
    pref = sys.argv[1] if len(sys.argv) > 1 else ''
    for n in sorted(os.listdir(SRC)):
        if n.endswith('.html') and n.startswith(pref):
            print(f'{n}: {build(n)} bytes')
