import re
import sys
from pathlib import Path

from web.lib.csource import ROOT
from web.lib.manifest import load_course

SITE_URL = "https://mkko000.github.io/algorithms/"
PLACEHOLDER = re.compile(r"\{\{(\w+)\}\}")


def counts() -> dict:
    programs = len(list((ROOT / "src").glob("*/*.c"))) + len(list((ROOT / "src").glob("*/exercises/*.c")))
    return {"programs": programs}


def chapters(course, lang: str) -> list:
    seen = {}
    for lesson in course.lessons:
        if lesson.chapter and lesson.kind == "lesson" and lesson.chapter not in seen:
            seen[lesson.chapter] = lesson
    out = []
    for n in sorted(seen):
        folders = sorted((ROOT / "src").glob(f"{n:02d}-*"))
        folder = f"src/{folders[0].name}/" if folders else ""
        out.append((n, seen[n].title[lang], folder, seen[n].slug))
    return out


def chapter_map(course, lang: str) -> str:
    lines = []
    for n, title, folder, slug in chapters(course, lang):
        where = f" (`{folder}`)" if folder else ""
        lines.append(f"{n}. [{title}]({SITE_URL}{lang}/{slug}/){where}")
    return "\n".join(lines)


def render_readme(template: str, course, lang: str, numbers: dict) -> str:
    values = {
        "chapter_map": chapter_map(course, lang),
        "program_count": str(numbers["programs"]),
        "lesson_count": str(sum(1 for l in course.lessons if l.kind == "lesson")),
        "exercise_page_count": str(sum(1 for l in course.lessons if l.kind == "exercises")),
        "site_url": SITE_URL,
    }

    def rep(m):
        if m.group(1) not in values:
            raise ValueError(f"unknown placeholder {m.group(1)}")
        return values[m.group(1)]

    return PLACEHOLDER.sub(rep, template)


def write_all() -> None:
    course = load_course(ROOT / "content" / "course.toml")
    for lang, name in (("en", "README.md"), ("ar", "README.ar.md")):
        tpl = (ROOT / "web" / "readme" / f"{name}.in").read_text(encoding="utf-8")
        (ROOT / name).write_text(render_readme(tpl, course, lang, counts()), encoding="utf-8")


if __name__ == "__main__":
    write_all()
    sys.exit(0)
