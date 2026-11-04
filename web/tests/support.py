from pathlib import Path

from web.lib.csource import CACHE

SAMPLE_C = r'''/* ============================================
 *  sample.c  a fixture
 * ============================================ */
#include <stdio.h>
#define less(A,B) ((A) < (B))

int partition(int a[], int l, int r);

/* the partition step */
int partition(int a[], int l, int r)
{
    char close = '}';
    const char *open = "{";
    (void)close; (void)open;
    return l < r ? l : r;
}

//>>demo
static int twice(int x) { return 2 * x; }
//<<demo

typedef struct { int key; } rec;

int main(void)
{
    printf("1\n2\n3\n");
    return twice(0) + partition(0, 0, 0);
}
'''


def sample_c() -> str:
    d = CACHE / "test"
    d.mkdir(parents=True, exist_ok=True)
    p = d / "sample.c"
    if not p.exists() or p.read_text(encoding="utf-8") != SAMPLE_C:
        p.write_text(SAMPLE_C, encoding="utf-8")
    return str(p)


def line_of(text: str, needle: str) -> int:
    for i, line in enumerate(text.splitlines(), 1):
        if needle in line:
            return i
    raise ValueError(needle)
