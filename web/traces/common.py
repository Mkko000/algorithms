import random

from web.lib.csource import extract_function


def letters(word: str) -> list:
    return [ord(c) for c in word]


def shown(values, as_letters: bool) -> list:
    return [chr(v) if as_letters else str(v) for v in values]


def one(v, as_letters: bool) -> str:
    return chr(v) if as_letters else str(v)


def line_with(snippet, needle: str) -> int:
    for k, text in enumerate(snippet.text.split("\n")):
        if needle in text:
            return snippet.first_line + k
    raise KeyError(f"{snippet.path}: no line with {needle!r}")


def seeded(n: int, seed: int = 7, lo: int = 1, hi: int = 99) -> list:
    rng = random.Random(seed)
    return [rng.randint(lo, hi) for _ in range(n)]


def c_array(values) -> str:
    return "{" + ", ".join(str(v) for v in values) + "}"


def snippet(source: str, fn: str):
    return extract_function(source, fn)
