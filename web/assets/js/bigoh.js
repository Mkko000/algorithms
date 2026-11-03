(function () {
  "use strict";
  var UI = {};
  try { UI = JSON.parse(document.getElementById("ui").textContent); } catch (e) {}
  var curves = [
    { name: "1", f: function () { return 1; } },
    { name: "lg N", f: function (n) { return Math.log2(n); } },
    { name: "N", f: function (n) { return n; } },
    { name: "N lg N", f: function (n) { return n * Math.log2(n); } },
    { name: "N²", f: function (n) { return n * n; } },
    { name: "2ᴺ", f: function (n) { return Math.pow(2, n); } }
  ];
  var NS = "http://www.w3.org/2000/svg";
  function el(tag, attrs, text) {
    var e = document.createElementNS(NS, tag);
    Object.keys(attrs).forEach(function (k) { e.setAttribute(k, attrs[k]); });
    if (text !== undefined) e.textContent = text;
    return e;
  }
  function mount(w) {
    var W = 560, H = 260, pad = 34, right = 70;
    w.innerHTML = "";
    var head = document.createElement("div");
    head.className = "trace-ctl";
    var title = document.createElement("strong");
    title.textContent = UI.bigoh_title || "";
    title.style.fontFamily = "var(--font-text)";
    var nl = document.createElement("label");
    nl.className = "trace-preset";
    var nlabel = document.createElement("span");
    nlabel.textContent = UI.bigoh_n || "N";
    var range = document.createElement("input");
    range.type = "range"; range.min = "8"; range.max = "40"; range.value = "20"; range.className = "trace-scrub";
    nl.append(nlabel, range);
    var fl = document.createElement("label");
    fl.className = "trace-preset";
    var flabel = document.createElement("span");
    flabel.textContent = UI.bigoh_focus || "";
    var sel = document.createElement("select");
    curves.forEach(function (c, i) {
      var o = document.createElement("option");
      o.value = String(i); o.textContent = c.name; if (c.name === "N lg N") o.selected = true;
      sel.appendChild(o);
    });
    fl.append(flabel, sel);
    head.append(title, nl, fl);
    var svg = el("svg", { viewBox: "0 0 " + (W + right) + " " + (H + 40), class: "fsvg", role: "img", direction: "ltr" });
    var out = document.createElement("p");
    out.className = "trace-vars";
    var box = document.createElement("div");
    box.className = "trace-scene";
    box.appendChild(svg);
    var frame = document.createElement("div");
    frame.className = "trace";
    var state = document.createElement("div");
    state.className = "trace-state";
    state.append(box, out);
    frame.append(head, state);
    w.appendChild(frame);
    function draw() {
      var n = Number(range.value), focus = Number(sel.value);
      while (svg.firstChild) svg.removeChild(svg.firstChild);
      var ymax = n * n;
      svg.appendChild(el("line", { x1: pad, y1: H + 10, x2: pad + W - pad, y2: H + 10, class: "e" }));
      svg.appendChild(el("line", { x1: pad, y1: 10, x2: pad, y2: H + 10, class: "e" }));
      svg.appendChild(el("text", { x: pad + W - pad, y: H + 30, class: "t-lab", "text-anchor": "end", "font-size": "12" }, "N = " + n));
      curves.forEach(function (c, i) {
        var d = "";
        for (var x = 1; x <= n; x++) {
          var px = pad + (x / n) * (W - 2 * pad);
          var py = H + 10 - (Math.min(c.f(x), ymax * 1.05) / ymax) * H;
          d += (x === 1 ? "M" : "L") + px.toFixed(1) + "," + py.toFixed(1) + " ";
        }
        svg.appendChild(el("path", { d: d, class: i === focus ? "e-active" : "e" }));
        var end = H + 10 - (Math.min(c.f(n), ymax * 1.05) / ymax) * H;
        svg.appendChild(el("text", { x: W - pad + 6, y: Math.max(14, end + 4).toFixed(1), class: i === focus ? "t-ptr" : "t-lab", "font-size": "12" }, c.name));
      });
      var parts = curves.map(function (c) {
        var v = c.f(n);
        var s = v > 1e7 ? (UI.bigoh_big || "") : (v < 100 ? v.toFixed(1).replace(/\.0$/, "") : Math.round(v));
        return "<span>" + c.name + " = <b>" + s + "</b></span>";
      });
      out.innerHTML = "<span>" + String(UI.bigoh_at || "").replace("{n}", n) + "</span>" + parts.join("");
    }
    range.addEventListener("input", draw);
    sel.addEventListener("change", draw);
    draw();
  }
  document.querySelectorAll('.widget[data-widget="bigoh"]').forEach(mount);
})();
