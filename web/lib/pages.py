import json
import re
import tomllib
from dataclasses import dataclass, field
from html import escape
from pathlib import Path

from web.lib.csource import ROOT, header_comment, run_output, whole_file
from web.lib.highlight import highlight_c

TEMPLATES = ROOT / "web" / "templates"
GITHUB = "https://github.com/Mkko000/algorithms/blob/main/"
PLACEHOLDER = re.compile(r"\{\{(\w+)\}\}")
JS_UI_KEYS = ("step", "play", "pause", "reset", "step_back", "step_forward", "theme_auto", "theme_light",
              "theme_dark", "progress", "done", "mark_done", "start", "continue", "bigoh_title", "bigoh_n",
              "bigoh_focus", "bigoh_at", "bigoh_big")


@dataclass
class Site:
    course: object
    base: str
    ui: dict
    arabic_only: set = field(default_factory=set)


def load_ui(path: Path) -> dict:
    return tomllib.loads(Path(path).read_text(encoding="utf-8"))


def url(site: Site, lang: str, *parts: str) -> str:
    tail = "/".join(p.strip("/") for p in parts if p)
    return f"{site.base}{lang}/" + (f"{tail}/" if tail else "")


def asset(site: Site, path: str = "") -> str:
    return f"{site.base}assets/{path}"


def fill(template: str, **values) -> str:
    def rep(m):
        key = m.group(1)
        if key not in values:
            raise ValueError(f"no value for {{{{{key}}}}}")
        return str(values[key])

    out = PLACEHOLDER.sub(rep, template)
    return out


def template(name: str) -> str:
    return (TEMPLATES / f"{name}.html").read_text(encoding="utf-8")


def other(lang: str) -> str:
    return "en" if lang == "ar" else "ar"


def code_slug(path: str) -> str:
    rel = path[len("src/"):] if path.startswith("src/") else path
    return rel[:-2] if rel.endswith(".c") else rel


def code_url(site: Site, lang: str, path: str) -> str:
    return url(site, lang, "code", code_slug(path))


def page(site: Site, lang: str, title: str, main: str, twin: str, description: str = "") -> str:
    ui = site.ui[lang]
    o = other(lang)
    alternate = (f'<link rel="alternate" hreflang="{lang}" href="{escape(url_self(twin, lang, o))}">\n'
                 f'<link rel="alternate" hreflang="{o}" href="{escape(twin)}">')
    done = len(site.course.lessons)
    ui_json = json.dumps({k: ui[k] for k in JS_UI_KEYS}, ensure_ascii=False).replace("</", "<\\/")
    return fill(
        template("base"),
        lang=lang, dir="rtl" if lang == "ar" else "ltr",
        title=escape(title), description=escape(description or ui["tagline"]),
        assets=asset(site), alternate=alternate, skip=escape(ui["skip"]),
        home=url(site, lang), site_name=escape(ui["site_name"]),
        total=done, progress_tpl=escape(ui["progress"]),
        switch_href=escape(twin), switch_lang=o, switch_label=escape(ui["switch_to"]),
        theme_auto=escape(ui["theme_auto"]), theme_light=escape(ui["theme_light"]), theme_dark=escape(ui["theme_dark"]),
        main=main, credit=escape(ui["credit"]), code_index=url(site, lang, "code"),
        code_index_label=escape(ui["code_index"]), repo=escape(ui["repo"]), ui_json=ui_json,
    )


def url_self(twin: str, lang: str, twin_lang: str) -> str:
    return twin.replace(f"/{twin_lang}/", f"/{lang}/", 1)


def lesson_number(site: Site, lesson) -> int:
    return site.course.lessons.index(lesson) + 1


def render_home(site: Site, lang: str) -> str:
    ui = site.ui[lang]
    parts = []
    n = 0
    for part in site.course.parts:
        rows = []
        for l in part.lessons:
            n += 1
            cls = "row ex" if l.kind == "exercises" else "row"
            rows.append(
                f'<li><a class="{cls}" href="{url(site, lang, l.slug)}" data-slug="{l.slug}">'
                f'<span class="num">{n}</span><span class="t">{escape(l.title[lang])}</span>'
                f'<span class="lead" aria-hidden="true"></span>'
                f'<span class="min">{escape(ui["minutes"].format(n=l.minutes))}</span></a></li>')
        parts.append(f'<h2>{escape(part.title[lang])}</h2>\n<ol>{"".join(rows)}</ol>')
    first = site.course.lessons[0]
    main = fill(template("home"), site_name=escape(ui["site_name"]), tagline=escape(ui["tagline"]),
                intro=escape(ui["intro"]), first_href=url(site, lang, first.slug),
                start_tpl=escape(ui["start"]), continue_tpl=escape(ui["continue"]),
                start_text=escape(ui["start"].format(title=first.title[lang])),
                contents=escape(ui["contents"]), parts="\n".join(parts))
    return page(site, lang, ui["site_name"], main, url(site, other(lang)))


def _toc(rendered, ui) -> str:
    items = [e for e in rendered.toc if e[0] in (2, 3)]
    if len(items) < 3:
        return ""
    lis = "".join(f'<li class="l{lvl}"><a href="#{hid}">{escape(text)}</a></li>' for lvl, hid, text in items)
    return f'<aside class="toc" aria-label="{escape(ui["on_this_page"])}"><p>{escape(ui["on_this_page"])}</p><ol>{lis}</ol></aside>'


def _pager_link(site, lang, lesson, cls, label) -> str:
    if lesson is None:
        return "<span></span>"
    return (f'<a class="{cls}" href="{url(site, lang, lesson.slug)}" rel="{cls}">'
            f'<small>{escape(label)}</small><span>{escape(lesson.title[lang])}</span></a>')


def render_lesson(site: Site, lang: str, lesson, rendered) -> str:
    ui = site.ui[lang]
    lessons = site.course.lessons
    i = lessons.index(lesson)
    prev = lessons[i - 1] if i > 0 else None
    nxt = lessons[i + 1] if i + 1 < len(lessons) else None
    part = next(p for p in site.course.parts if p.key == lesson.part)
    context = f'<a href="{url(site, lang)}">{escape(part.title[lang])}</a>'
    if lesson.chapter:
        context += f", {escape(ui['chapter'].format(n=lesson.chapter))}"
    meta = f'{ui["lesson_n_of"].format(n=i + 1, total=len(lessons))}, {ui["about_minutes"].format(n=lesson.minutes)}'
    main = fill(template("lesson"), context=context, title=escape(lesson.title[lang]), meta=escape(meta),
                body=resolve_sources(rendered.html, site, lang),
                prev=_pager_link(site, lang, prev, "prev", ui["prev"]),
                next=_pager_link(site, lang, nxt, "next", ui["next"]),
                slug=lesson.slug, mark_done=escape(ui["mark_done"]), done=escape(ui["done"]),
                toc=_toc(rendered, ui))
    return page(site, lang, f'{lesson.title[lang]} | {ui["site_name"]}', main, url(site, other(lang), lesson.slug))


def render_missing(site: Site, lesson) -> str:
    ui = site.ui["en"]
    main = fill(template("missing"), title=escape(lesson.title["en"]), message=escape(ui["not_translated"]),
                href=url(site, "ar", lesson.slug), lang="ar", link=escape(ui["read_in_arabic"]))
    return page(site, "en", f'{lesson.title["en"]} | {ui["site_name"]}', main, url(site, "ar", lesson.slug))


CODE_CAPTION = re.compile(r'(<figure class="(?:code|output)" dir="ltr" data-src="(src/[^"]+\.c)"><figcaption>)([^<]*)(</figcaption>)')
LINK_SCHEME = re.compile(r'href="(lesson|src):([^"#]+)"')
TRACE_SRC = re.compile(r'(<span class="trace-src" data-src="(src/[^"]+\.c)">)([^<]*)(</span>)')


def resolve_sources(html: str, site: Site, lang: str) -> str:
    def link(m):
        if not (ROOT / m.group(2)).exists():
            return m.group(0)
        return f'{m.group(1)}<a href="{code_url(site, lang, m.group(2))}">{m.group(3)}</a>{m.group(4)}'

    def link_code_only(m):
        if m.group(1).startswith('<figure class="output"'):
            return m.group(0)
        return link(m)

    def scheme(m):
        kind, target = m.group(1), m.group(2)
        if kind == "lesson":
            if target not in site.course.by_slug:
                raise ValueError(f"link to unknown lesson {target}")
            return f'href="{url(site, lang, target)}"'
        if not (ROOT / target).exists():
            raise ValueError(f"link to missing file {target}")
        if not target.endswith(".c"):
            return f'href="{GITHUB}{target}"'
        return f'href="{code_url(site, lang, target)}"'

    html = LINK_SCHEME.sub(scheme, html)
    return TRACE_SRC.sub(link, CODE_CAPTION.sub(link_code_only, html))


def render_code_page(site: Site, lang: str, path: str, used_by: list) -> str:
    ui = site.ui[lang]
    snip = whole_file(path)
    about = header_comment(path)
    about_html = f'<pre class="code-about" dir="ltr">{escape(about)}</pre>' if about else ""
    source = (f'<figure class="code" dir="ltr" data-src="{escape(path)}"><figcaption>{escape(path)}</figcaption>'
              f'<pre><code>{highlight_c(snip.text)}</code></pre></figure>')
    out = run_output(path, 200)
    output = (f'<figure class="output" dir="ltr"><figcaption>{escape(ui["output"])}</figcaption>'
              f'<pre>{escape(out)}</pre></figure>')
    used = ""
    if used_by:
        items = "".join(f'<li><a href="{url(site, lang, l.slug)}">{escape(l.title[lang])}</a></li>' for l in used_by)
        used = f'<h2>{escape(ui["used_in"])}</h2><ul class="used-in">{items}</ul>'
    name = Path(path).name
    main = fill(template("code"), index_href=url(site, lang, "code"), index_label=escape(ui["code_index"]),
                name=escape(name), path=escape(path), about=about_html, source_label=escape(ui["source"]),
                source=source, output_label=escape(ui["output_of"]), output=output, used=used,
                github=GITHUB + path, github_label=escape(ui["view_on_github"]))
    return page(site, lang, f'{name} | {ui["site_name"]}', main, code_url(site, other(lang), path),
                description=about.split("\n")[0] if about else "")


def render_code_index(site: Site, lang: str, paths: list) -> str:
    ui = site.ui[lang]
    groups = {}
    for p in sorted(paths):
        groups.setdefault(Path(p).parts[1], []).append(p)
    blocks = []
    for folder, items in groups.items():
        lis = "".join(f'<li><a href="{code_url(site, lang, p)}">{escape(code_slug(p).split("/", 1)[1])}.c</a></li>'
                      for p in items)
        blocks.append(f'<h2 dir="ltr">{escape(folder)}</h2><ul class="code-list" dir="ltr">{lis}</ul>')
    main = fill(template("codeindex"), title=escape(ui["code_index"]), intro=escape(ui["code_index_intro"]),
                groups="\n".join(blocks))
    return page(site, lang, f'{ui["code_index"]} | {ui["site_name"]}', main, url(site, other(lang), "code"))


def render_not_found(site: Site) -> str:
    ui = site.ui["en"]
    main = fill(template("missing"), title=escape(ui["not_found"]), message="", href=url(site, "en"), lang="en",
                link=escape(ui["go_home"]))
    return page(site, "en", ui["site_name"], main, url(site, "ar"))


def render_root(site: Site) -> str:
    links = " ".join(f'<a href="{url(site, l)}" hreflang="{l}" lang="{l}">{escape(site.ui[l]["lang_name"])}</a>'
                     for l in ("ar", "en"))
    return (
        '<!doctype html><html lang="ar"><head><meta charset="utf-8">'
        '<meta name="viewport" content="width=device-width, initial-scale=1">'
        f'<title>{escape(site.ui["en"]["site_name"])}</title>'
        f'<link rel="stylesheet" href="{asset(site, "css/tokens.css")}"><link rel="stylesheet" href="{asset(site, "css/base.css")}">'
        '<script>(function(){var l=null;try{l=localStorage.getItem("algo-lang")}catch(e){}'
        'if(l!=="ar"&&l!=="en"){l=(navigator.language||"").slice(0,2)==="en"?"en":"ar"}'
        f'location.replace("{site.base}"+l+"/")}})()</script></head>'
        f'<body><main class="wrap missing"><h1>{escape(site.ui["en"]["site_name"])}</h1><p>{links}</p></main></body></html>'
    )
