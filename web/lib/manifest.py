import tomllib
from collections import Counter
from dataclasses import dataclass
from pathlib import Path

LANGS = ("ar", "en")


@dataclass(frozen=True)
class Lesson:
    slug: str
    kind: str
    chapter: int | None
    minutes: int
    title: dict
    part: str


@dataclass(frozen=True)
class Part:
    key: str
    title: dict
    lessons: tuple


@dataclass(frozen=True)
class Course:
    parts: tuple
    lessons: tuple
    by_slug: dict


def load_course(path: Path) -> Course:
    data = tomllib.loads(Path(path).read_text(encoding="utf-8"))
    parts = []
    for p in data.get("part", []):
        lessons = tuple(
            Lesson(
                slug=l["slug"],
                kind=l.get("kind", "lesson"),
                chapter=l.get("chapter"),
                minutes=int(l.get("minutes", 0)),
                title=dict(l["title"]),
                part=p["key"],
            )
            for l in p.get("lesson", [])
        )
        parts.append(Part(key=p["key"], title=dict(p["title"]), lessons=lessons))
    flat = tuple(l for p in parts for l in p.lessons)
    return Course(parts=tuple(parts), lessons=flat, by_slug={l.slug: l for l in flat})


def load_arabic_only(path: Path) -> set:
    path = Path(path)
    if not path.exists():
        return set()
    return {s.strip() for s in path.read_text(encoding="utf-8").splitlines() if s.strip()}


def validate_course(course: Course, content_root: Path, arabic_only: set) -> list:
    root = Path(content_root)
    errors = []
    for slug, n in Counter(l.slug for l in course.lessons).items():
        if n > 1:
            errors.append(f"duplicate slug {slug}")
    for lesson in course.lessons:
        s = lesson.slug
        for lang in LANGS:
            if not lesson.title.get(lang):
                errors.append(f"{s}: missing {lang} title")
        if not (root / "ar" / f"{s}.md").exists():
            errors.append(f"{s}: missing ar/{s}.md")
        has_en = (root / "en" / f"{s}.md").exists()
        if not has_en and s not in arabic_only:
            errors.append(f"{s}: missing en/{s}.md")
        if has_en and s in arabic_only:
            errors.append(f"{s}: listed in arabic_only.txt but en/{s}.md exists")
    known = set(course.by_slug)
    for s in sorted(arabic_only - known):
        errors.append(f"{s}: listed in arabic_only.txt but not in course.toml")
    return errors
