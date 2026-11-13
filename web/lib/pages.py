import json
import re
import tomllib
from dataclasses import dataclass, field
from html import escape, unescape
from pathlib import Path

from web.lib.bidi import isolate_latin
from web.lib.csource import ROOT, header_comment, run_output, whole_file
from web.lib.highlight import highlight_c
from web.lib.margin import build_rows

TEMPLATES = ROOT / "web" / "templates"
GITHUB = "https://github.com/Mkko000/algorithms/blob/main/"
AUTHOR_URL = "https://github.com/Mkko000"
MAINTAINER_URL = "https://github.com/saeeedhany"
PLACEHOLDER = re.compile(r"\{\{(\w+)\}\}")
JS_UI_KEYS = ("step", "play", "pause", "reset", "step_back", "step_forward", "theme_auto", "theme_light",
              "theme_dark", "progress", "done", "mark_done", "start", "continue", "bigoh_title", "bigoh_n",
              "bigoh_focus", "bigoh_at", "bigoh_big", "bigoh_scale", "bigoh_log", "bigoh_linear", "copy", "copied",
              "quiz_title", "quiz_n", "quiz_score", "quiz_retry", "no_results")


@dataclass
class Site:
    course: object
    base: str
    ui: dict
    arabic_only: set = field(default_factory=set)


def load_ui(path: Path) -> dict:
    return tomllib.loads(Path(path).read_text(encoding="utf-8"))


def _linked(text: str, key: str, name: str, href: str) -> str:
    return escape(text).replace("{" + key + "}", f'<a href="{href}">{escape(name)}</a>')


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


def page(site: Site, lang: str, title: str, main: str, twin: str, description: str = "", menu_toc: str = "",
         nav: str = "") -> str:
    ui = site.ui[lang]
    marks = {"home": "", "code": ""}
    if nav:
        key, _, state = nav.partition(":")
        marks[key] = f'aria-current="{state or "page"}"'
    o = other(lang)
    alternate = (f'<link rel="alternate" hreflang="{lang}" href="{escape(url_self(twin, lang, o))}">\n'
                 f'<link rel="alternate" hreflang="{o}" href="{escape(twin)}">')
    done = len(site.course.lessons)
    ui_json = json.dumps({k: ui[k] for k in JS_UI_KEYS}, ensure_ascii=False).replace("</", "<\\/")
    html = fill(
        template("base"),
        lang=lang, dir="rtl" if lang == "ar" else "ltr",
        title=escape(title), description=escape(description or ui["tagline"]),
        assets=asset(site), alternate=alternate, skip=escape(ui["skip"]),
        home=url(site, lang), site_name=escape(ui["site_name"]),
        total=done, progress_tpl=escape(ui["progress"]),
        switch_href=escape(twin), switch_lang=o, switch_label=escape(ui["switch_to"]),
        theme_auto=escape(ui["theme_auto"]), theme_light=escape(ui["theme_light"]), theme_dark=escape(ui["theme_dark"]),
        main=main, credit=_linked(ui["credit"], "author", ui["author_name"], AUTHOR_URL), code_index=url(site, lang, "code"),
        code_index_label=escape(ui["code_index"]), repo=escape(ui["repo"]), ui_json=ui_json,
        contents=escape(ui["contents"]), nav_label=escape(ui["nav_label"]), menu_label=escape(ui["menu_label"]),
        menu_toc=menu_toc, built_by=_linked(ui["built_by"], "name", ui["maintainer_name"], MAINTAINER_URL),
        home_current=marks["home"], code_current=marks["code"],
    )
    if lang == "ar":
        head, _, body = html.partition("<body>")
        html = head + "<body>" + isolate_latin(body)
    return html


def url_self(twin: str, lang: str, twin_lang: str) -> str:
    return twin.replace(f"/{twin_lang}/", f"/{lang}/", 1)


def lesson_number(site: Site, lesson) -> int:
    return site.course.lessons.index(lesson) + 1


def _stats(ui: dict, stats: dict | None) -> str:
    if not stats:
        return ""
    items = "".join(f'<div><dt>{int(stats[k])}</dt><dd>{escape(ui["stat_" + k])}</dd></div>'
                    for k in ("lessons", "programs", "traces") if stats.get(k))
    return f'<dl class="stats">{items}</dl>' if items else ""


def render_home(site: Site, lang: str, stats: dict | None = None) -> str:
    ui = site.ui[lang]
    parts = []
    n = 0
    for part in site.course.parts:
        rows = []
        for l in part.lessons:
            n += 1
            cls = "row ex" if l.kind == "exercises" else "row"
            note = ""
            if lang == "en" and l.slug in site.arabic_only:
                cls += " todo"
                note = f'<span class="note">{escape(ui["arabic_only_note"])}</span>'
            rows.append(
                f'<li><a class="{cls}" href="{url(site, lang, l.slug)}" data-slug="{l.slug}">'
                f'<span class="num">{n}</span><span class="t">{escape(l.title[lang])}{note}</span>'
                f'<span class="lead" aria-hidden="true"></span>'
                f'<span class="min">{escape(ui["minutes"].format(n=l.minutes))}</span></a></li>')
        parts.append(f'<h2><span>{escape(part.title[lang])}</span></h2>\n<ol>{"".join(rows)}</ol>')
    first = site.course.lessons[0]
    main = fill(template("home"), site_name=escape(ui["site_name"]), tagline=escape(ui["tagline"]),
                intro=escape(ui["intro"]), first_href=url(site, lang, first.slug),
                start_tpl=escape(ui["start"]), continue_tpl=escape(ui["continue"]),
                start_text=escape(ui["start"].format(title=first.title[lang])),
                contents=escape(ui["contents"]), parts="\n".join(parts), stats=_stats(ui, stats))
    return page(site, lang, ui["site_name"], main, url(site, other(lang)), nav="home")


def _toc_list(rendered) -> str:
    items = [e for e in rendered.toc if e[0] in (2, 3)]
    if len(items) < 3:
        return ""
    return "".join(f'<li class="l{lvl}"><a href="#{hid}">{escape(text)}</a></li>' for lvl, hid, text in items)


def _toc(rendered, ui) -> str:
    lis = _toc_list(rendered)
    if not lis:
        return ""
    return (f'<aside class="toc" aria-label="{escape(ui["on_this_page"])}"><div class="toc-inner">'
            f'<p class="toc-title">{escape(ui["on_this_page"])}</p><ol>{lis}</ol></div></aside>')


def _menu_toc(rendered, ui) -> str:
    lis = _toc_list(rendered)
    if not lis:
        return ""
    return f'<div class="menu-toc"><p class="toc-title">{escape(ui["on_this_page"])}</p><ol>{lis}</ol></div>'


BOOK_SECTIONS = re.compile(r"\A\s*<p>([^<:]+):\s*(.+?)</p>\s*", re.S)
SECTION_SPLIT = {"ar": re.compile(r"\s+·\s+"), "en": re.compile(r",\s+(?=\d+\.\d+\s)")}
SECTION_ITEM = re.compile(r"\A(\d+\.\d+)\s+(.+?)(?:\s*\(([^()]*(?:\([^()]*\)[^()]*)*)\))?\Z", re.S)


def parse_sections(html: str, lang: str, ui: dict) -> tuple:
    m = BOOK_SECTIONS.match(html)
    if not m or m.group(1).strip() != ui["book_sections"]:
        return [], html
    entries = []
    for part in SECTION_SPLIT[lang].split(m.group(2).strip()):
        item = SECTION_ITEM.match(part.strip())
        if item:
            entries.append((item.group(1), item.group(2), item.group(3) or ""))
        else:
            entries.append(("", part.strip(), ""))
    return entries, html[m.end():]


def _sections_list(entries: list, ui: dict) -> str:
    if not entries:
        return ""
    lis = "".join(
        f'<li><span class="sec-n">{num}</span><span class="sec-t">{title}{f" <small>{note}</small>" if note else ""}</span></li>'
        for num, title, note in entries)
    nums = escape(ui["sep"].join(num for num, _, _ in entries if num))
    return (f'<details class="book-sections"><summary><span class="bs-label">{escape(ui["book_sections"])}</span>'
            f'<span class="bs-nums">{nums}</span></summary><ol>{lis}</ol></details>')


TAG = re.compile(r"<[^>]+>")
H2_TEXT = re.compile(r'<h2 id="([^"]+)">(.*?)</h2>', re.S)


def _heading_notes(html: str, entries: list, ui: dict) -> tuple:
    notes, used = {}, set()
    for m in H2_TEXT.finditer(html):
        text = unescape(TAG.sub("", m.group(2))).strip()
        for k, (num, title, note) in enumerate(entries):
            if num and k not in used and text.startswith(num + " "):
                sub = f'<p class="n-sub">{note}</p>' if note else ""
                notes[m.group(1)] = (f'<div class="note book"><p class="n-label">{escape(ui["book"])}</p>'
                                     f'<p><b>{num}</b> {title}</p>{sub}</div>')
                used.add(k)
                break
    return notes, [e for k, e in enumerate(entries) if k not in used]


SRC_LIST = re.compile(r'<ul>(\s*<li><a href="[^"]*/code/[^"]*">(?:(?!</a>).)*</a></li>)+\s*</ul>', re.S)


HEAD_ID = re.compile(r'<h([23]) id="([^"]+)">(.*?)</h\1>', re.S)


def _anchors(html: str, label: str) -> str:
    return HEAD_ID.sub(lambda m: (f'<h{m.group(1)} id="{m.group(2)}">{m.group(3)}'
                                  f'<a class="h-anchor" href="#{m.group(2)}" aria-label="{escape(label)}">#</a></h{m.group(1)}>'), html)


def _source_lists(html: str) -> str:
    return SRC_LIST.sub(lambda m: m.group(0).replace("<ul>", '<ul class="src-links">', 1), html)


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
        context += f"{escape(ui['sep'])}{escape(ui['chapter'].format(n=lesson.chapter))}"
    meta = f'{ui["lesson_n_of"].format(n=i + 1, total=len(lessons))}{ui["sep"]}{ui["about_minutes"].format(n=lesson.minutes)}'
    entries, body = parse_sections(rendered.html, lang, ui)
    heading_notes, rest = _heading_notes(body, entries, ui)
    body = build_rows(_source_lists(resolve_sources(body, site, lang)), heading_notes)
    main = fill(template("lesson"), context=context, title=escape(lesson.title[lang]), meta=escape(meta),
                sections=_sections_list(rest, ui), body=_anchors(body, ui["section_link"]),
                prev=_pager_link(site, lang, prev, "prev", ui["prev"]),
                next=_pager_link(site, lang, nxt, "next", ui["next"]),
                slug=lesson.slug, mark_done=escape(ui["mark_done"]), done=escape(ui["done"]),
                toc=_toc(rendered, ui))
    return page(site, lang, f'{lesson.title[lang]} | {ui["site_name"]}', main, url(site, other(lang), lesson.slug),
                menu_toc=_menu_toc(rendered, ui))


def render_missing(site: Site, lesson) -> str:
    ui = site.ui["en"]
    main = fill(template("missing"), title=escape(lesson.title["en"]), message=escape(ui["not_translated"]),
                href=url(site, "ar", lesson.slug), lang="ar", link=escape(ui["read_in_arabic"]),
                extra=f'<a href="{url(site, "en")}">{escape(ui["go_home"])}</a>')
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
        items = "".join(f'<li><a href="{url(site, lang, l.slug)}"><span class="num">{lesson_number(site, l)}</span>'
                        f'<span class="t">{escape(l.title[lang])}</span></a></li>' for l in used_by)
        used = f'<h2>{escape(ui["used_in"])}</h2><ul class="used-in">{items}</ul>'
    name = Path(path).name
    main = fill(template("code"), index_href=url(site, lang, "code"), index_label=escape(ui["code_index"]),
                name=escape(name), path=escape(path), about=about_html, source_label=escape(ui["source"]),
                source=source, output_label=escape(ui["output_of"]), output=output, used=used,
                github=GITHUB + path, github_label=escape(ui["view_on_github"]))
    return page(site, lang, f'{name} | {ui["site_name"]}', main, code_url(site, other(lang), path),
                description=about.split("\n")[0] if about else "", nav="code:true")


def _program_row(site: Site, lang: str, path: str, used: dict) -> str:
    name = Path(path).name
    lessons = used.get(path) or []
    uses = f'<span class="uses">{escape(lessons[0].title[lang])}</span>' if lessons else '<span class="uses"></span>'
    return (f'<li><a class="prog" href="{code_url(site, lang, path)}"><code dir="ltr">{escape(name)}</code>'
            f'<span class="lead" aria-hidden="true"></span>{uses}</a></li>')


def render_code_index(site: Site, lang: str, paths: list, used: dict | None = None) -> str:
    ui = site.ui[lang]
    used = used or {}
    groups = {}
    for p in sorted(paths):
        groups.setdefault(Path(p).parts[1], []).append(p)
    blocks = []
    for folder, items in groups.items():
        num, _, _ = folder.partition("-")
        title = escape(ui["chapter"].format(n=int(num))) if num.isdigit() else escape(folder)
        main_items = [p for p in items if "/exercises/" not in p]
        ex_items = [p for p in items if "/exercises/" in p]
        body = f'<ol class="prog-list">{"".join(_program_row(site, lang, p, used) for p in main_items)}</ol>'
        if ex_items:
            body += (f'<p class="prog-sub">{escape(ui["exercises_label"])}</p>'
                     f'<ol class="prog-list">{"".join(_program_row(site, lang, p, used) for p in ex_items)}</ol>')
        blocks.append(f'<section class="prog-group"><h2>{title}<small dir="ltr">{escape(folder)}</small></h2>{body}</section>')
    main = fill(template("codeindex"), title=escape(ui["code_index"]), intro=escape(ui["code_index_intro"]),
                groups="\n".join(blocks), filter_label=escape(ui["filter_programs"]), no_results=escape(ui["no_results"]))
    return page(site, lang, f'{ui["code_index"]} | {ui["site_name"]}', main, url(site, other(lang), "code"), nav="code")


def render_not_found(site: Site) -> str:
    ui = site.ui["en"]
    main = fill(template("missing"), title=escape(ui["not_found"]), message="", href=url(site, "en"), lang="en",
                link=escape(ui["go_home"]), extra="")
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
        'if(l!=="ar"&&l!=="en"){l="ar"}'
        f'location.replace("{site.base}"+l+"/")}})()</script></head>'
        f'<body><main class="wrap missing"><h1>{escape(site.ui["en"]["site_name"])}</h1><p>{links}</p></main></body></html>'
    )
