(function () {
  "use strict";
  var root = document.documentElement;
  var UI = {};
  try {
    UI = JSON.parse(document.getElementById("ui").textContent);
  } catch (e) {}

  function store(key, value) {
    try {
      if (value === null) localStorage.removeItem(key);
      else localStorage.setItem(key, value);
    } catch (e) {}
  }
  function read(key) {
    try {
      return localStorage.getItem(key);
    } catch (e) {
      return null;
    }
  }
  function fmt(tpl, args) {
    return String(tpl || "").replace(/\{(\w+)\}/g, function (m, k) {
      return k in args ? args[k] : m;
    });
  }
  function reduced() {
    return (
      window.matchMedia &&
      window.matchMedia("(prefers-reduced-motion: reduce)").matches
    );
  }

  function setupTheme() {
    var buttons = document.querySelectorAll(".theme-toggle");
    if (!buttons.length) return;
    var order = ["auto", "light", "dark"];
    function current() {
      var t = root.dataset.theme;
      return t === "light" || t === "dark" ? t : "auto";
    }
    function label() {
      buttons.forEach(function (b) {
        var text = b.dataset[current()] || "";
        var span = b.querySelector(".theme-text");
        if (span) span.textContent = text;
        b.setAttribute("aria-label", text);
        b.setAttribute("title", text);
      });
    }
    buttons.forEach(function (btn) {
      btn.addEventListener("click", function () {
        var next = order[(order.indexOf(current()) + 1) % order.length];
        if (next === "auto") {
          delete root.dataset.theme;
          store("algo-theme", null);
        } else {
          root.dataset.theme = next;
          store("algo-theme", next);
        }
        label();
      });
    });
    label();
  }

  function setupLang() {
    document.querySelectorAll(".lang-switch").forEach(function (a) {
      a.addEventListener("click", function () {
        store("algo-lang", a.getAttribute("hreflang"));
      });
    });
    store("algo-lang", root.lang);
  }

  function setupMenu() {
    var head = document.querySelector(".site-head");
    var btn = document.querySelector(".menu-btn");
    var panel = document.getElementById("menu");
    if (!head || !btn || !panel) return;
    function set(open) {
      head.classList.toggle("menu-open", open);
      btn.setAttribute("aria-expanded", open ? "true" : "false");
    }
    btn.addEventListener("click", function () {
      set(!head.classList.contains("menu-open"));
    });
    panel.addEventListener("click", function (e) {
      if (e.target.closest("a")) set(false);
    });
    document.addEventListener("keydown", function (e) {
      if (e.key === "Escape" && head.classList.contains("menu-open")) {
        set(false);
        btn.focus();
      }
    });
    document.addEventListener("click", function (e) {
      if (head.classList.contains("menu-open") && !head.contains(e.target))
        set(false);
    });
  }

  function setupOutline() {
    var links = document.querySelectorAll(".toc a");
    if (!links.length || !("IntersectionObserver" in window)) return;
    var byId = {};
    links.forEach(function (a) {
      byId[decodeURIComponent(a.hash.slice(1))] = a;
    });
    var heads = [];
    Object.keys(byId).forEach(function (id) {
      var h = document.getElementById(id);
      if (h) heads.push(h);
    });
    function mark(id) {
      links.forEach(function (a) {
        a.classList.toggle("on", a === byId[id]);
      });
    }
    var obs = new IntersectionObserver(
      function () {
        var best = null;
        heads.forEach(function (h) {
          if (h.getBoundingClientRect().top < 140) best = h;
        });
        mark(best ? best.id : heads.length ? heads[0].id : null);
      },
      { rootMargin: "0px 0px -60% 0px", threshold: [0, 1] },
    );
    heads.forEach(function (h) {
      obs.observe(h);
    });
  }

  function doneList() {
    try {
      var v = JSON.parse(read("algo-done") || "[]");
      return Array.isArray(v) ? v : [];
    } catch (e) {
      return [];
    }
  }
  function setupProgress() {
    var done = doneList();
    var prog = document.querySelector(".progress");
    if (prog && done.length) {
      prog.textContent = fmt(prog.dataset.tpl, {
        done: done.length,
        total: prog.dataset.total,
      });
    }
    var rows = document.querySelectorAll(".row[data-slug]");
    var firstOpen = null;
    rows.forEach(function (r) {
      var isDone = done.indexOf(r.dataset.slug) >= 0;
      r.classList.toggle("done", isDone);
      if (isDone) r.querySelector(".num").textContent = "✓";
      if (!isDone && !firstOpen) firstOpen = r;
    });
    document.querySelectorAll(".contents h2").forEach(function (h) {
      var list = h.nextElementSibling;
      if (!list || !done.length) return;
      var all = list.querySelectorAll(".row[data-slug]");
      var n = list.querySelectorAll(".row.done").length;
      var tag = h.querySelector(".part-done") || document.createElement("span");
      tag.className = "part-done" + (n === all.length ? " full" : "");
      tag.textContent = n + " / " + all.length;
      h.appendChild(tag);
    });
    if (firstOpen && done.length) firstOpen.classList.add("next-up");
    var start = document.querySelector(".start-link");
    if (start && firstOpen && done.length) {
      start.href = firstOpen.href;
      start.textContent = fmt(start.dataset.continue, {
        title: firstOpen.querySelector(".t").textContent,
      });
    }
    var btn = document.querySelector(".done-btn");
    if (!btn) return;
    function paint() {
      var on = doneList().indexOf(btn.dataset.slug) >= 0;
      btn.setAttribute("aria-pressed", on ? "true" : "false");
      btn.textContent = on ? "✓ " + btn.dataset.done : btn.dataset.label;
    }
    btn.addEventListener("click", function () {
      var list = doneList();
      var i = list.indexOf(btn.dataset.slug);
      if (i >= 0) list.splice(i, 1);
      else list.push(btn.dataset.slug);
      store("algo-done", JSON.stringify(list));
      paint();
      if (prog)
        prog.textContent = list.length
          ? fmt(prog.dataset.tpl, {
              done: list.length,
              total: prog.dataset.total,
            })
          : "";
    });
    paint();
  }

  function setupQuizzes() {
    var groups = [];
    document.querySelectorAll(".quiz").forEach(function (q) {
      var last = groups[groups.length - 1];
      if (last && q.previousElementSibling === last[last.length - 1])
        last.push(q);
      else groups.push([q]);
    });
    groups.forEach(function (g) {
      var set = document.createElement("section");
      set.className = "quiz-set";
      var head = document.createElement("div");
      head.className = "quiz-head";
      var title = document.createElement("p");
      title.className = "quiz-title";
      title.textContent = UI.quiz_title || "";
      var score = document.createElement("span");
      score.className = "quiz-score";
      score.setAttribute("aria-live", "polite");
      head.append(title, score);
      g[0].parentNode.insertBefore(set, g[0]);
      set.appendChild(head);
      g.forEach(function (q, i) {
        if (g.length > 1) {
          var n = document.createElement("p");
          n.className = "quiz-n";
          n.textContent = fmt(UI.quiz_n, { n: i + 1, total: g.length });
          q.insertBefore(n, q.firstChild);
        }
        set.appendChild(q);
      });
      var retry = document.createElement("button");
      retry.type = "button";
      retry.className = "quiz-retry";
      retry.textContent = UI.quiz_retry || "";
      retry.hidden = true;
      head.appendChild(retry);

      function update() {
        var answered = g.filter(function (q) {
          return q.classList.contains("answered");
        });
        var right = g.filter(function (q) {
          return q.dataset.result === "right";
        }).length;
        score.textContent =
          answered.length === g.length
            ? fmt(UI.quiz_score, { right: right, total: g.length })
            : "";
        set.classList.toggle(
          "all-right",
          answered.length === g.length && right === g.length,
        );
        retry.hidden = answered.length === 0;
      }
      retry.addEventListener("click", function () {
        g.forEach(function (q) {
          q.classList.remove("answered");
          delete q.dataset.result;
          q.querySelectorAll(".opts button").forEach(function (b) {
            b.classList.remove("right", "wrong");
            b.removeAttribute("aria-disabled");
          });
          var why = q.querySelector(".why");
          if (why) why.hidden = true;
        });
        update();
        var first = g[0].querySelector(".opts button");
        if (first) first.focus();
      });
      g.forEach(function (q) {
        q.querySelectorAll(".opts button").forEach(function (b) {
          var t = document.createElement("span");
          t.className = "opt-t";
          while (b.firstChild) t.appendChild(b.firstChild);
          b.appendChild(t);
          b.addEventListener("click", function () {
            if (q.classList.contains("answered")) return;
            q.classList.add("answered");
            var ok = q.querySelector("[data-ok]");
            if (ok) ok.classList.add("right");
            if (b !== ok) b.classList.add("wrong");
            q.dataset.result = b === ok ? "right" : "wrong";
            q.querySelectorAll(".opts button").forEach(function (x) {
              x.setAttribute("aria-disabled", "true");
            });
            var why = q.querySelector(".why");
            if (why) why.hidden = false;
            update();
          });
        });
      });
    });
  }

  function copyText(text) {
    if (navigator.clipboard && window.isSecureContext)
      return navigator.clipboard.writeText(text);
    return new Promise(function (resolve, reject) {
      var t = document.createElement("textarea");
      t.value = text;
      t.setAttribute("readonly", "");
      t.style.position = "fixed";
      t.style.opacity = "0";
      document.body.appendChild(t);
      t.select();
      var ok = false;
      try {
        ok = document.execCommand("copy");
      } catch (e) {}
      document.body.removeChild(t);
      if (ok) resolve();
      else reject();
    });
  }

  function setupCopy() {
    if (!UI.copy) return;
    var blocks = [];
    document.querySelectorAll(".lesson-body figure.code").forEach(function (f) {
      blocks.push([f, f.querySelector("pre")]);
    });
    document.querySelectorAll(".lesson-body pre").forEach(function (pre) {
      if (
        pre.closest("figure, .trace, .code-about") ||
        pre.classList.contains("code-about")
      )
        return;
      var wrap = document.createElement("div");
      wrap.className = "pre-wrap";
      pre.parentNode.insertBefore(wrap, pre);
      wrap.appendChild(pre);
      blocks.push([wrap, pre]);
    });
    blocks.forEach(function (pair) {
      var box = pair[0],
        pre = pair[1];
      if (!pre) return;
      var b = document.createElement("button");
      b.type = "button";
      b.className = "copy-btn";
      b.textContent = UI.copy;
      var timer = null;
      b.addEventListener("click", function () {
        copyText(pre.textContent.replace(/\n$/, "")).then(
          function () {
            b.textContent = UI.copied || UI.copy;
            b.classList.add("ok");
            clearTimeout(timer);
            timer = setTimeout(function () {
              b.textContent = UI.copy;
              b.classList.remove("ok");
            }, 1600);
          },
          function () {},
        );
      });
      box.classList.add("has-copy");
      box.appendChild(b);
    });
  }

  function setupFilter() {
    var box = document.querySelector(".prog-filter");
    if (!box) return;
    var input = box.querySelector("input");
    var empty = document.querySelector(".prog-empty");
    var groups = document.querySelectorAll(".prog-group");
    box.hidden = false;
    function norm(t) {
      return String(t || "")
        .toLowerCase()
        .replace(/[_\-./]+/g, " ")
        .trim();
    }
    var rows = [];
    document.querySelectorAll(".prog-list li").forEach(function (li) {
      rows.push([li, norm(li.textContent)]);
    });
    input.addEventListener("input", function () {
      var words = norm(input.value).split(/\s+/).filter(Boolean);
      rows.forEach(function (r) {
        r[0].hidden = !words.every(function (w) {
          return r[1].indexOf(w) >= 0;
        });
      });
      var any = false;
      groups.forEach(function (g) {
        g.querySelectorAll(".prog-list").forEach(function (list) {
          var visible = list.querySelector("li:not([hidden])") !== null;
          list.hidden = !visible;
          var label = list.previousElementSibling;
          if (label && label.classList.contains("prog-sub"))
            label.hidden = !visible;
        });
        var show = g.querySelector(".prog-list:not([hidden])") !== null;
        g.hidden = !show;
        if (show) any = true;
      });
      if (empty) empty.hidden = any;
    });
    input.addEventListener("keydown", function (e) {
      if (e.key === "Escape" && input.value) {
        input.value = "";
        input.dispatchEvent(new Event("input"));
      }
    });
  }

  function button(text, action, label, cls) {
    var b = document.createElement("button");
    b.type = "button";
    b.textContent = text;
    b.dataset.a = action;
    if (label) b.setAttribute("aria-label", label);
    if (cls) b.className = cls;
    return b;
  }

  function player(opts) {
    var k = 0,
      timer = null;
    function stop() {
      if (timer) {
        clearInterval(timer);
        timer = null;
      }
      if (opts.playBtn) opts.playBtn.textContent = "▶ " + (UI.play || "");
    }
    function go(n) {
      var total = opts.total();
      k = Math.max(0, Math.min(total - 1, n));
      opts.show(k);
      if (k >= total - 1) stop();
    }
    function play() {
      if (timer) {
        stop();
        return;
      }
      if (k >= opts.total() - 1) go(0);
      if (opts.playBtn) opts.playBtn.textContent = "❚❚ " + (UI.pause || "");
      timer = setInterval(
        function () {
          go(k + 1);
        },
        reduced() ? 1400 : 900,
      );
    }
    return {
      go: go,
      play: play,
      stop: stop,
      index: function () {
        return k;
      },
      key: function (e) {
        var t = e.target.tagName;
        if (t === "SELECT" || t === "INPUT") return;
        if (e.key === "ArrowRight") {
          stop();
          go(k + 1);
          e.preventDefault();
        } else if (e.key === "ArrowLeft") {
          stop();
          go(k - 1);
          e.preventDefault();
        } else if (e.key === " " && t !== "BUTTON") {
          play();
          e.preventDefault();
        } else if (e.key === "Home") {
          stop();
          go(0);
          e.preventDefault();
        }
      },
    };
  }

  function orient(bar) {
    var rtl = document.documentElement.dir === "rtl";
    bar.dir = rtl ? "rtl" : "ltr";
    var back = bar.querySelector('[data-a="back"]'),
      fwd = bar.querySelector('[data-a="fwd"]');
    if (back) back.textContent = rtl ? "›" : "‹";
    if (fwd) fwd.textContent = rtl ? "‹" : "›";
  }

  function setupSteps() {
    document.querySelectorAll(".steps").forEach(function (st) {
      var frames = st.querySelectorAll(".frame");
      if (frames.length < 2) {
        if (frames[0]) frames[0].classList.add("on");
        return;
      }
      var bar = document.createElement("div");
      bar.className = "stepbar";
      var back = button("‹", "back", UI.step_back);
      var playBtn = button("▶ " + (UI.play || ""), "play", null, "play");
      var fwd = button("›", "fwd", UI.step_forward);
      var reset = button("⟲", "reset", UI.reset);
      var count = document.createElement("span");
      count.className = "count";
      count.setAttribute("aria-live", "polite");
      bar.append(reset, back, playBtn, fwd, count);
      orient(bar);
      st.appendChild(bar);
      st.tabIndex = 0;
      var p = player({
        total: function () {
          return frames.length;
        },
        playBtn: playBtn,
        show: function (k) {
          frames.forEach(function (f, i) {
            f.classList.toggle("on", i === k);
          });
          count.textContent =
            (UI.step || "") + " " + (k + 1) + " / " + frames.length;
        },
      });
      bar.addEventListener("click", function (e) {
        var a = e.target.closest("button");
        if (!a) return;
        if (a.dataset.a === "play") {
          p.play();
          return;
        }
        p.stop();
        if (a.dataset.a === "back") p.go(p.index() - 1);
        if (a.dataset.a === "fwd") p.go(p.index() + 1);
        if (a.dataset.a === "reset") p.go(0);
      });
      st.addEventListener("keydown", p.key);
      p.go(0);
    });
  }

  function setupTraces() {
    document.querySelectorAll(".trace").forEach(function (sec) {
      var meta;
      try {
        meta = JSON.parse(
          sec.querySelector('script[type="application/json"]').textContent,
        );
      } catch (e) {
        return;
      }
      var scene = sec.querySelector(".trace-scene");
      var say = sec.querySelector(".trace-say");
      var vars = sec.querySelector(".trace-vars");
      var count = sec.querySelector(".trace-count");
      var scrub = sec.querySelector(".trace-scrub");
      var select = sec.querySelector(".trace-preset select");
      var playBtn = sec.querySelector('[data-a="play"]');
      var code = sec.querySelector(".trace-code");
      var lines = sec.querySelectorAll(".trace-code .ln");
      var data = meta.presets ? meta : null;
      var loading = null;
      var preset = select ? select.value : Object.keys(meta.says)[0];

      function frames() {
        return data ? data.presets[preset] : [null];
      }
      function load() {
        if (data) return Promise.resolve(data);
        if (!loading) {
          sec.classList.add("loading");
          loading = fetch(meta.url)
            .then(function (r) {
              return r.json();
            })
            .then(function (d) {
              data = d;
              sec.classList.remove("loading");
              return d;
            });
        }
        return loading;
      }
      function show(k) {
        var f = frames()[k];
        if (!f) return;
        scene.innerHTML = f.svg;
        say.textContent = meta.says[preset][k];
        var html = "";
        Object.keys(f.vars).forEach(function (n) {
          html +=
            "<span>" +
            n +
            " = <b>" +
            String(f.vars[n]).replace(/[<&]/g, "") +
            "</b></span>";
        });
        vars.innerHTML = html;
        var on = null;
        lines.forEach(function (ln) {
          var hit = f.line !== null && Number(ln.dataset.n) === f.line;
          ln.classList.toggle("on", hit);
          if (hit) on = ln;
        });
        if (on && code) {
          var delta =
            on.getBoundingClientRect().top - code.getBoundingClientRect().top;
          code.scrollTop = Math.max(
            0,
            code.scrollTop +
              delta -
              code.clientHeight / 2 +
              on.offsetHeight / 2,
          );
        }
        scrub.max = String(frames().length - 1);
        scrub.value = String(k);
        count.textContent =
          (count.dataset.label || "") + " " + (k + 1) + " / " + frames().length;
      }
      var p = player({
        total: function () {
          return frames().length;
        },
        playBtn: playBtn,
        show: show,
      });
      function withData(fn) {
        return function () {
          var args = arguments;
          load().then(function () {
            fn.apply(null, args);
          });
        };
      }
      orient(sec.querySelector(".trace-ctl"));
      sec.querySelector(".trace-ctl").addEventListener(
        "click",
        withData(function (e) {
          var a = e.target.closest("button");
          if (!a) return;
          if (a.dataset.a === "play") {
            p.play();
            return;
          }
          p.stop();
          if (a.dataset.a === "back") p.go(p.index() - 1);
          if (a.dataset.a === "fwd") p.go(p.index() + 1);
          if (a.dataset.a === "reset") p.go(0);
        }),
      );
      scrub.addEventListener(
        "input",
        withData(function () {
          p.stop();
          p.go(Number(scrub.value));
        }),
      );
      if (select)
        select.addEventListener(
          "change",
          withData(function () {
            p.stop();
            preset = select.value;
            p.go(0);
          }),
        );
      sec.addEventListener("keydown", function (e) {
        if (["ArrowRight", "ArrowLeft", " ", "Home"].indexOf(e.key) < 0) return;
        var t = e.target.tagName;
        if (t === "SELECT" || t === "INPUT") return;
        if (e.key === " " && t === "BUTTON") return;
        e.preventDefault();
        load().then(function () {
          p.key({
            key: e.key,
            target: e.target,
            preventDefault: function () {},
          });
        });
      });
    });
  }

  setupTheme();
  setupLang();
  setupMenu();
  setupOutline();
  setupProgress();
  setupQuizzes();
  setupCopy();
  setupFilter();
  setupSteps();
  setupTraces();
})();
