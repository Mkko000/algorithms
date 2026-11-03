import importlib

MODULES = ("partition",)

_all = None


def all_traces() -> dict:
    global _all
    if _all is None:
        found = {}
        for mod in MODULES:
            t = importlib.import_module(f"web.traces.{mod}").build()
            found[t.name] = t
        _all = found
    return _all
