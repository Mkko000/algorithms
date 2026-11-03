import argparse
import json
import re
import shutil
import sys
from pathlib import Path

from web.figures import Figure, all_figures
from web.lib.content import ContentError, RenderContext, render_page
from web.lib.csource import ROOT, SourceError
from web.lib.figlang import load_figure_text, localize
from web.lib.manifest import LANGS, load_arabic_only, load_course
from web.lib.pages import (Site, asset, code_slug, load_ui, render_code_index, render_code_page, render_home,
                           render_lesson, render_missing, render_not_found, render_root)
from web.lib.trace import frames_json, load_templates, section_html
from web.traces import all_traces

MARKER = ".site-build"
SRC_LINK = re.compile(r"src:(src/[^)\"\s]+\.c)")


def programs() -> list:
    return sorted(str(p.relative_to(ROOT)) for p in (ROOT / "src").glob("*/*.c")) + \
        sorted(str(p.relative_to(ROOT)) for p in (ROOT / "src").glob("*/exercises/*.c"))


def prepare(out: Path):
    if out.exists():
        if not (out / MARKER).exists():
            raise SystemExit(f"{out} exists and was not made by this build; refusing to replace it")
        shutil.rmtree(out)
    out.mkdir(parents=True)
    (out / MARKER).write_text("", encoding="utf-8")
    shutil.copytree(ROOT / "web" / "assets", out / "assets")


def write(path: Path, text: str):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def build(out: Path, base: str = "/", only: str | None = None) -> dict:
    if not base.endswith("/"):
        base += "/"
    course = load_course(ROOT / "content" / "course.toml")
    site = Site(course=course, base=base, ui=load_ui(ROOT / "content" / "ui.toml"),
                arabic_only=load_arabic_only(ROOT / "content" / "arabic_only.txt"))
    base_figures = all_figures()
    fig_text = load_figure_text(ROOT / "content" / "en" / "figures.toml")
    figures_by_lang = {"ar": base_figures,
                       "en": {n: Figure(n, f.kind, localize(f.html, fig_text)) for n, f in base_figures.items()}}
    traces = all_traces()
    templates = {lang: load_templates(ROOT / "content" / lang / "traces.toml") for lang in LANGS}
    prepare(out)
    for name, t in traces.items():
        write(out / "assets" / "traces" / f"{name}.json",
              json.dumps(frames_json(t), ensure_ascii=False, separators=(",", ":")))

    used = {}
    pages = 0
    for lang in LANGS:
        ui = site.ui[lang]
        labels = {k[len("preset_"):]: v for k, v in ui.items() if k.startswith("preset_")}

        def trace_maker(name, lang=lang, ui=ui, labels=labels):
            return lambda: section_html(traces[name], templates[lang][name], ui, labels,
                                        data_url=asset(site, f"traces/{name}.json"))

        trace_map = {name: trace_maker(name) for name in traces}
        for lesson in course.lessons:
            if only and lesson.slug != only:
                continue
            md_path = ROOT / "content" / lang / f"{lesson.slug}.md"
            target = out / lang / lesson.slug / "index.html"
            if not md_path.exists():
                if lang == "en" and lesson.slug in site.arabic_only:
                    write(target, render_missing(site, lesson))
                    pages += 1
                    continue
                raise SystemExit(f"missing {md_path.relative_to(ROOT)}")
            text = md_path.read_text(encoding="utf-8")
            ctx = RenderContext(lang, figures_by_lang[lang], trace_map, ui, str(md_path.relative_to(ROOT)))
            rendered = render_page(text, ctx)
            for src in rendered.sources | set(SRC_LINK.findall(text)) | {traces[t].source for t in rendered.traces}:
                used.setdefault(src, [])
                if lesson not in used[src]:
                    used[src].append(lesson)
            write(target, render_lesson(site, lang, lesson, rendered))
            pages += 1
        write(out / lang / "index.html", render_home(site, lang))
        pages += 1

    paths = programs()
    order = {l.slug: k for k, l in enumerate(course.lessons)}
    for lang in LANGS:
        for path in paths:
            lessons = sorted(used.get(path, []), key=lambda l: order[l.slug])
            write(out / lang / "code" / code_slug(path) / "index.html", render_code_page(site, lang, path, lessons))
            pages += 1
        write(out / lang / "code" / "index.html", render_code_index(site, lang, paths))
        pages += 1
    write(out / "index.html", render_root(site))
    write(out / "404.html", render_not_found(site))
    return {"pages": pages + 2, "programs": len(paths), "traces": len(traces)}


def main(argv=None):
    ap = argparse.ArgumentParser(prog="python -m web.build")
    ap.add_argument("--out", default="_site")
    ap.add_argument("--base", default="/")
    ap.add_argument("--only")
    a = ap.parse_args(argv)
    try:
        stats = build(ROOT / a.out if not Path(a.out).is_absolute() else Path(a.out), a.base, a.only)
    except (ContentError, SourceError, ValueError) as e:
        print(f"build failed: {e}", file=sys.stderr)
        return 1
    print(f"built {stats['pages']} pages, {stats['programs']} programs, {stats['traces']} traces into {a.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
