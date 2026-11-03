import importlib
from dataclasses import dataclass

from web.lib.render import map_svg_classes

MODULES = ("figs_a", "figs_b", "figs_c", "figs_ex", "figs_ds", "figs_ds2", "figs_ds3", "figs_ds4", "figs_ds5")


@dataclass(frozen=True)
class Figure:
    name: str
    kind: str
    html: str


_all = None


def all_figures() -> dict:
    global _all
    if _all is None:
        found = {}
        for mod in MODULES:
            for name, (kind, html) in importlib.import_module(f"web.figures.{mod}").F.items():
                if name in found:
                    raise ValueError(f"duplicate figure {name}")
                found[name] = Figure(name, kind, map_svg_classes(html))
        _all = found
    return _all
