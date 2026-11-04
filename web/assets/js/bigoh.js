(function () {
  "use strict";
  var UI = {};
  try { UI = JSON.parse(document.getElementById("ui").textContent); } catch (e) {}
  var NS = "http://www.w3.org/2000/svg";
  var CURVES = [
    { name: "1", f: function () { return 1; } },
    { name: "lg N", f: function (n) { return Math.log2(n); } },
    { name: "N", f: function (n) { return n; } },
    { name: "N lg N", f: function (n) { return n * Math.log2(n); } },
    { name: "N²", f: function (n) { return n * n; } },
    { name: "N³", f: function (n) { return n * n * n; } },
    { name: "2ᴺ", f: function (n) { return Math.pow(2, n); } }
  ];
  var W = 640, H = 340, M = { left: 58, right: 84, top: 18, bottom: 36 };
  var PW = W - M.left - M.right, PH = H - M.top - M.bottom;
  var SUP = { "0": "⁰", "1": "¹", "2": "²", "3": "³", "4": "⁴", "5": "⁵", "6": "⁶", "7": "⁷", "8": "⁸", "9": "⁹", "-": "⁻" };
  var uid = 0;

  function el(tag, attrs, text) {
    var e = document.createElementNS(NS, tag);
    Object.keys(attrs).forEach(function (k) { e.setAttribute(k, attrs[k]); });
    if (text !== undefined) e.textContent = text;
    return e;
  }
  function html(tag, cls, text) {
    var e = document.createElement(tag);
    if (cls) e.className = cls;
    if (text !== undefined) e.textContent = text;
    return e;
  }
  function pow10(k) { return "10" + String(k).split("").map(function (c) { return SUP[c]; }).join(""); }
  function fmt(v) {
    if (v >= 1e9) {
      var k = Math.floor(Math.log10(v));
      var m = v / Math.pow(10, k);
      return m.toFixed(1).replace(/\.0$/, "") + " × " + pow10(k);
    }
    if (v >= 100 || Number.isInteger(v)) return Math.round(v).toLocaleString("en-US");
    return v.toFixed(1).replace(/\.0$/, "");
  }
  function niceStep(span, target) {
    var raw = span / target, mag = Math.pow(10, Math.floor(Math.log10(raw)));
    var r = raw / mag;
    return (r <= 1 ? 1 : r <= 2 ? 2 : r <= 5 ? 5 : 10) * mag;
  }

  function mount(host) {
    var state = { n: 32, log: true, focus: 3, cursor: null };
    var clipId = "bigoh-clip-" + (++uid);
    host.innerHTML = "";
    var frame = html("div", "trace bigoh");
    var head = html("div", "trace-ctl bigoh-ctl");
    head.dir = document.documentElement.dir || "ltr";
    var title = html("strong", "bigoh-title", UI.bigoh_title || "");
    var nLabel = html("label", "trace-preset");
    var nText = html("span", "", UI.bigoh_n || "N");
    var range = html("input", "trace-scrub");
    range.type = "range"; range.min = "8"; range.max = "64"; range.step = "1"; range.value = String(state.n);
    range.setAttribute("aria-label", UI.bigoh_n || "N");
    var nValue = html("output", "bigoh-n", String(state.n));
    nValue.dir = "ltr";
    nLabel.append(nText, range, nValue);
    var scale = html("div", "bigoh-scale");
    scale.setAttribute("role", "group");
    scale.setAttribute("aria-label", UI.bigoh_scale || "");
    var logBtn = html("button", "", UI.bigoh_log || "log");
    var linBtn = html("button", "", UI.bigoh_linear || "linear");
    [logBtn, linBtn].forEach(function (b) { b.type = "button"; });
    logBtn.dataset.scale = "log"; linBtn.dataset.scale = "linear";
    scale.append(logBtn, linBtn);
    var fLabel = html("label", "trace-preset");
    var sel = html("select");
    CURVES.forEach(function (c, i) {
      var o = html("option", "", c.name);
      o.value = String(i);
      sel.appendChild(o);
    });
    sel.value = String(state.focus);
    fLabel.append(html("span", "", UI.bigoh_focus || ""), sel);
    head.append(title, nLabel, scale, fLabel);

    var box = html("div", "trace-scene bigoh-scene");
    var svg = el("svg", { viewBox: "0 0 " + W + " " + H, class: "fsvg bigoh-svg", role: "img", direction: "ltr", tabindex: "0" });
    svg.setAttribute("aria-label", UI.bigoh_title || "");
    box.appendChild(svg);
    var out = html("p", "trace-vars bigoh-out");
    out.dir = "ltr";
    out.setAttribute("aria-live", "polite");
    var body = html("div", "trace-state");
    body.append(box, out);
    frame.append(head, body);
    host.appendChild(frame);

    function domain() {
      var top = Math.max.apply(null, CURVES.map(function (c) { return c.f(state.n); }));
      if (state.log) return { lo: -0.4, hi: Math.ceil(Math.log10(top)) || 1 };
      var focusTop = Math.max(CURVES[state.focus].f(state.n), state.n) * 1.15;
      var step = niceStep(focusTop, 5);
      return { lo: 0, hi: Math.ceil(focusTop / step) * step, step: step };
    }
    function xOf(n) { return M.left + (n - 1) / (state.n - 1) * PW; }
    function yOf(v, d) {
      var t = state.log ? (Math.log10(v) - d.lo) / (d.hi - d.lo) : (v - d.lo) / (d.hi - d.lo);
      return M.top + PH - t * PH;
    }

    function draw() {
      var d = domain();
      while (svg.firstChild) svg.removeChild(svg.firstChild);
      var defs = el("defs", {});
      var clip = el("clipPath", { id: clipId });
      clip.appendChild(el("rect", { x: M.left, y: M.top, width: PW, height: PH }));
      defs.appendChild(clip);
      svg.appendChild(defs);
      svg.appendChild(el("rect", { x: M.left, y: M.top, width: PW, height: PH, class: "plot-area" }));

      var xs = niceStep(state.n - 1, 8);
      for (var x = 0; x <= state.n; x += xs) {
        if (x < 1) continue;
        var px = xOf(x);
        svg.appendChild(el("line", { x1: px, y1: M.top, x2: px, y2: M.top + PH, class: "e-grid" }));
        svg.appendChild(el("text", { x: px, y: M.top + PH + 18, class: "t-lab x-tick", "text-anchor": "middle", "font-size": "11" }, String(x)));
      }
      if (state.log) {
        var every = Math.ceil((d.hi + 1) / 7);
        for (var k = 0; k <= d.hi; k += every) {
          var py = yOf(Math.pow(10, k), d);
          svg.appendChild(el("line", { x1: M.left, y1: py, x2: M.left + PW, y2: py, class: "e-grid" }));
          svg.appendChild(el("text", { x: M.left - 8, y: py + 4, class: "t-lab y-tick", "text-anchor": "end", "font-size": "11" }, pow10(k)));
        }
      } else {
        for (var v = 0; v <= d.hi + 1e-9; v += d.step) {
          var py2 = yOf(v, d);
          svg.appendChild(el("line", { x1: M.left, y1: py2, x2: M.left + PW, y2: py2, class: "e-grid" }));
          svg.appendChild(el("text", { x: M.left - 8, y: py2 + 4, class: "t-lab y-tick", "text-anchor": "end", "font-size": "11" }, fmt(v)));
        }
      }
      svg.appendChild(el("line", { x1: M.left, y1: M.top + PH, x2: M.left + PW, y2: M.top + PH, class: "e" }));
      svg.appendChild(el("line", { x1: M.left, y1: M.top, x2: M.left, y2: M.top + PH, class: "e" }));
      svg.appendChild(el("text", { x: M.left + PW + 10, y: M.top + PH + 18, class: "t-lab", "text-anchor": "start", "font-size": "11", "font-style": "italic" }, "N"));

      var curves = el("g", { "clip-path": "url(#" + clipId + ")" });
      var labels = [];
      CURVES.forEach(function (c, i) {
        var pts = [], last = null;
        var steps = 240;
        for (var q = 0; q <= steps; q++) {
          var n = 1 + (state.n - 1) * q / steps;
          var val = c.f(n);
          if (state.log && val <= 0) continue;
          var p = [xOf(n), yOf(val, d)];
          pts.push(p[0].toFixed(1) + "," + p[1].toFixed(1));
          if (p[1] >= M.top) last = p;
        }
        var endVal = c.f(state.n);
        var endY = yOf(endVal, d);
        curves.appendChild(el("polyline", { points: pts.join(" "), class: i === state.focus ? "e-active bigoh-line" : "e bigoh-line", fill: "none" }));
        var lx, ly, anchor = "start";
        if (endY >= M.top) { lx = M.left + PW + 8; ly = endY; }
        else if (last) { lx = Math.min(last[0], M.left + PW - 4); ly = M.top + 12; anchor = "end"; }
        else { lx = M.left + 4; ly = M.top + 12; }
        labels.push({ i: i, x: lx, y: ly, anchor: anchor, name: c.name });
      });
      svg.appendChild(curves);

      var right = labels.filter(function (l) { return l.anchor === "start"; }).sort(function (a, b) { return a.y - b.y; });
      var gap = 16;
      for (var r = 1; r < right.length; r++) if (right[r].y - right[r - 1].y < gap) right[r].y = right[r - 1].y + gap;
      var floor = M.top + PH;
      for (var u = right.length - 1; u >= 0; u--) {
        var limit = u === right.length - 1 ? floor : right[u + 1].y - gap;
        if (right[u].y > limit) right[u].y = limit;
      }
      var top = labels.filter(function (l) { return l.anchor === "end"; }).sort(function (a, b) { return b.x - a.x; });
      for (var t = 1; t < top.length; t++) top[t].y = top[t - 1].y + gap;
      labels.forEach(function (l) {
        svg.appendChild(el("text", { x: l.x, y: l.y + 4, class: (l.i === state.focus ? "t-ptr" : "t-lab") + " curve-label",
                                     "text-anchor": l.anchor, "font-size": "12" }, l.name));
      });

      var at = state.cursor || state.n;
      if (state.cursor) {
        var cx = xOf(at);
        svg.appendChild(el("line", { x1: cx, y1: M.top, x2: cx, y2: M.top + PH, class: "e-null" }));
        var dots = el("g", { "clip-path": "url(#" + clipId + ")" });
        CURVES.forEach(function (c, i) {
          var val = c.f(at);
          if (state.log && val <= 0) return;
          dots.appendChild(el("circle", { cx: cx, cy: yOf(val, d), r: i === state.focus ? 4 : 3, class: i === state.focus ? "s-focus" : "s-idle" }));
        });
        svg.appendChild(dots);
      }
      var parts = ["<span>" + String(UI.bigoh_at || "N = {n}").replace("{n}", "<b>" + at + "</b>") + "</span>"];
      CURVES.forEach(function (c) { parts.push("<span>" + c.name + " = <b>" + fmt(c.f(at)) + "</b></span>"); });
      out.innerHTML = parts.join("");
      nValue.textContent = String(state.n);
      logBtn.setAttribute("aria-pressed", state.log ? "true" : "false");
      linBtn.setAttribute("aria-pressed", state.log ? "false" : "true");
    }

    function cursorFrom(evt) {
      var r = svg.getBoundingClientRect();
      var x = (evt.clientX - r.left) / r.width * W;
      var n = Math.round(1 + (x - M.left) / PW * (state.n - 1));
      return Math.max(1, Math.min(state.n, n));
    }
    range.addEventListener("input", function () { state.n = Number(range.value); state.cursor = null; draw(); });
    sel.addEventListener("change", function () { state.focus = Number(sel.value); draw(); });
    logBtn.addEventListener("click", function () { state.log = true; draw(); });
    linBtn.addEventListener("click", function () { state.log = false; draw(); });
    svg.addEventListener("pointermove", function (e) { state.cursor = cursorFrom(e); draw(); });
    svg.addEventListener("pointerdown", function (e) { state.cursor = cursorFrom(e); draw(); });
    svg.addEventListener("pointerleave", function () { state.cursor = null; draw(); });
    svg.addEventListener("keydown", function (e) {
      if (e.key !== "ArrowLeft" && e.key !== "ArrowRight") return;
      e.preventDefault();
      var c = state.cursor || state.n;
      state.cursor = Math.max(1, Math.min(state.n, c + (e.key === "ArrowRight" ? 1 : -1)));
      draw();
    });
    svg.addEventListener("blur", function () { state.cursor = null; draw(); });
    draw();
  }

  document.querySelectorAll('.widget[data-widget="bigoh"]').forEach(mount);
})();
