(function () {
  "use strict";
  var root = document.documentElement;
  var UI = {};
  try { UI = JSON.parse(document.getElementById("ui").textContent); } catch (e) {}

  function store(key, value) {
    try {
      if (value === null) localStorage.removeItem(key); else localStorage.setItem(key, value);
    } catch (e) {}
  }
  function read(key) {
    try { return localStorage.getItem(key); } catch (e) { return null; }
  }
  function fmt(tpl, args) {
    return String(tpl || "").replace(/\{(\w+)\}/g, function (m, k) { return k in args ? args[k] : m; });
  }
  function reduced() {
    return window.matchMedia && window.matchMedia("(prefers-reduced-motion: reduce)").matches;
  }

  function setupTheme() {
    var btn = document.querySelector(".theme-toggle");
    if (!btn) return;
    var order = ["auto", "light", "dark"];
    function current() {
      var t = root.dataset.theme;
      return t === "light" || t === "dark" ? t : "auto";
    }
    function label() { btn.textContent = btn.dataset[current()] || btn.textContent; }
    btn.addEventListener("click", function () {
      var next = order[(order.indexOf(current()) + 1) % order.length];
      if (next === "auto") { delete root.dataset.theme; store("algo-theme", null); }
      else { root.dataset.theme = next; store("algo-theme", next); }
      label();
    });
    label();
  }

  function setupLang() {
    var a = document.querySelector(".lang-switch");
    if (a) a.addEventListener("click", function () { store("algo-lang", a.getAttribute("hreflang")); });
    store("algo-lang", root.lang);
  }

  function doneList() {
    try {
      var v = JSON.parse(read("algo-done") || "[]");
      return Array.isArray(v) ? v : [];
    } catch (e) { return []; }
  }
  function setupProgress() {
    var done = doneList();
    var prog = document.querySelector(".progress");
    if (prog && done.length) {
      prog.textContent = fmt(prog.dataset.tpl, { done: done.length, total: prog.dataset.total });
    }
    var rows = document.querySelectorAll(".row[data-slug]");
    var firstOpen = null;
    rows.forEach(function (r) {
      var isDone = done.indexOf(r.dataset.slug) >= 0;
      r.classList.toggle("done", isDone);
      if (isDone) r.querySelector(".num").textContent = "✓";
      if (!isDone && !firstOpen) firstOpen = r;
    });
    var start = document.querySelector(".start-link");
    if (start && firstOpen && done.length) {
      start.href = firstOpen.href;
      start.textContent = fmt(start.dataset.continue, { title: firstOpen.querySelector(".t").textContent });
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
      if (i >= 0) list.splice(i, 1); else list.push(btn.dataset.slug);
      store("algo-done", JSON.stringify(list));
      paint();
      if (prog) prog.textContent = list.length ? fmt(prog.dataset.tpl, { done: list.length, total: prog.dataset.total }) : "";
    });
    paint();
  }

  function setupQuizzes() {
    document.querySelectorAll(".quiz").forEach(function (q) {
      q.querySelectorAll(".opts button").forEach(function (b) {
        b.addEventListener("click", function () {
          if (q.classList.contains("answered")) return;
          q.classList.add("answered");
          var ok = q.querySelector("[data-ok]");
          if (ok) ok.classList.add("right");
          if (b !== ok) b.classList.add("wrong");
          q.querySelectorAll(".opts button").forEach(function (x) { x.setAttribute("aria-disabled", "true"); });
          var why = q.querySelector(".why");
          if (why) why.hidden = false;
        });
      });
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
    var k = 0, timer = null;
    function stop() {
      if (timer) { clearInterval(timer); timer = null; }
      if (opts.playBtn) opts.playBtn.textContent = "▶ " + (UI.play || "");
    }
    function go(n) {
      var total = opts.total();
      k = Math.max(0, Math.min(total - 1, n));
      opts.show(k);
      if (k >= total - 1) stop();
    }
    function play() {
      if (timer) { stop(); return; }
      if (k >= opts.total() - 1) go(0);
      if (opts.playBtn) opts.playBtn.textContent = "❚❚ " + (UI.pause || "");
      timer = setInterval(function () { go(k + 1); }, reduced() ? 1400 : 900);
    }
    return {
      go: go, play: play, stop: stop,
      index: function () { return k; },
      key: function (e) {
        var t = e.target.tagName;
        if (t === "SELECT" || t === "INPUT") return;
        if (e.key === "ArrowRight") { stop(); go(k + 1); e.preventDefault(); }
        else if (e.key === "ArrowLeft") { stop(); go(k - 1); e.preventDefault(); }
        else if (e.key === " " && t !== "BUTTON") { play(); e.preventDefault(); }
        else if (e.key === "Home") { stop(); go(0); e.preventDefault(); }
      }
    };
  }

  function setupSteps() {
    document.querySelectorAll(".steps").forEach(function (st) {
      var frames = st.querySelectorAll(".frame");
      if (frames.length < 2) { if (frames[0]) frames[0].classList.add("on"); return; }
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
      st.appendChild(bar);
      st.tabIndex = 0;
      var p = player({
        total: function () { return frames.length; },
        playBtn: playBtn,
        show: function (k) {
          frames.forEach(function (f, i) { f.classList.toggle("on", i === k); });
          count.textContent = (UI.step || "") + " " + (k + 1) + " / " + frames.length;
        }
      });
      bar.addEventListener("click", function (e) {
        var a = e.target.closest("button");
        if (!a) return;
        if (a.dataset.a === "play") { p.play(); return; }
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
      try { meta = JSON.parse(sec.querySelector('script[type="application/json"]').textContent); } catch (e) { return; }
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

      function frames() { return data ? data.presets[preset] : [null]; }
      function load() {
        if (data) return Promise.resolve(data);
        if (!loading) {
          sec.classList.add("loading");
          loading = fetch(meta.url).then(function (r) { return r.json(); }).then(function (d) {
            data = d; sec.classList.remove("loading"); return d;
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
          html += "<span>" + n + " = <b>" + String(f.vars[n]).replace(/[<&]/g, "") + "</b></span>";
        });
        vars.innerHTML = html;
        var on = null;
        lines.forEach(function (ln) {
          var hit = f.line !== null && Number(ln.dataset.n) === f.line;
          ln.classList.toggle("on", hit);
          if (hit) on = ln;
        });
        if (on && code) {
          var delta = on.getBoundingClientRect().top - code.getBoundingClientRect().top;
          code.scrollTop = Math.max(0, code.scrollTop + delta - code.clientHeight / 2 + on.offsetHeight / 2);
        }
        scrub.max = String(frames().length - 1);
        scrub.value = String(k);
        count.textContent = (count.dataset.label || "") + " " + (k + 1) + " / " + frames().length;
      }
      var p = player({ total: function () { return frames().length; }, playBtn: playBtn, show: show });
      function withData(fn) { return function () { var args = arguments; load().then(function () { fn.apply(null, args); }); }; }
      sec.querySelector(".trace-ctl").addEventListener("click", withData(function (e) {
        var a = e.target.closest("button");
        if (!a) return;
        if (a.dataset.a === "play") { p.play(); return; }
        p.stop();
        if (a.dataset.a === "back") p.go(p.index() - 1);
        if (a.dataset.a === "fwd") p.go(p.index() + 1);
        if (a.dataset.a === "reset") p.go(0);
      }));
      scrub.addEventListener("input", withData(function () { p.stop(); p.go(Number(scrub.value)); }));
      if (select) select.addEventListener("change", withData(function () { p.stop(); preset = select.value; p.go(0); }));
      sec.addEventListener("keydown", function (e) {
        if (["ArrowRight", "ArrowLeft", " ", "Home"].indexOf(e.key) < 0) return;
        var t = e.target.tagName;
        if (t === "SELECT" || t === "INPUT") return;
        if (e.key === " " && t === "BUTTON") return;
        e.preventDefault();
        load().then(function () { p.key({ key: e.key, target: e.target, preventDefault: function () {} }); });
      });
    });
  }

  setupTheme();
  setupLang();
  setupProgress();
  setupQuizzes();
  setupSteps();
  setupTraces();
})();
