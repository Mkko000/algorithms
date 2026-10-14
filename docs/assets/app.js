(function(){
  const C=window.COURSE, all=C.parts.flatMap(p=>p.lessons), KEY='algo-done';
  const side=document.getElementById('side'), main=document.getElementById('main');
  const done=()=>{try{return JSON.parse(localStorage.getItem(KEY)||'[]')}catch(e){return []}};
  const setDone=a=>{localStorage.setItem(KEY,JSON.stringify(a));render();};
  function render(){
    const d=done(); let n=0, cur=(location.hash.match(/#\/(.+)/)||[])[1];
    side.innerHTML=C.parts.map(p=>`<h4>${p.title}</h4>`+p.lessons.map(l=>{n++;return `<a href="#/${l.id}" class="${d.includes(l.id)?'done':''} ${cur===l.id?'on':''}"><span class="n">${d.includes(l.id)?'✓':n}</span>${l.title}</a>`}).join('')).join('');
    const pct=Math.round(100*all.filter(l=>d.includes(l.id)).length/all.length);
    document.getElementById('progBar').style.width=pct+'%';
    document.getElementById('progTxt').textContent=pct+'%';
  }
  function home(){
    const d=done(), next=all.find(l=>!d.includes(l.id))||all[0];
    main.innerHTML=`<section class="hero">
      <p class="meta">كورس تفاعلي · Algorithms in C · Sedgewick Parts 1–4 · C99</p>
      <h1>خوارزميات من الآخر: نفهمها، نكتبها بـ C، ونحسّها بإيدينا</h1>
      <p>الكورس ده ماشي جنب كتاب Robert Sedgewick <b>Algorithms in C (الطبعة التالتة، أجزاء 1–4)</b> فصل بفصل. مش بنلخّص الكتاب وخلاص — بنشرح الفكرة زي ما بتتحكي على قهوة، وبعدين نشوف الكود الحقيقي اللي في الريبو، وبعدين نجرّب بإيدنا.</p>
      <p>ستّ أجزاء: الأساس والتحليل، هياكل البيانات والـ ADTs، الريكيرشن والشجر، الترتيب من الابتدائي لحد الـ radix، جداول الرموز من الـ BST لحد الـ B-tree، وفي الآخر إزاي تتمرّن وتبقى PRO.</p>
      <div class="tip"><b>إزاي تستفيد</b>كل درس فيه شرح + مثال بأرقام + كود C من الريبو + كويز + أحيانًا أداة تفاعلية. خلّص الدرس، دوس «خلصت»، والتقدّم بيتحفظ لوحده في المتصفح.</div>
      <a class="btn main" href="#/${next.id}">${d.length?'كمّل من':'يلا نبدأ بـ'}: ${next.title}</a>
      <div class="cards">${all.map((l,i)=>`<a href="#/${l.id}" class="${d.includes(l.id)?'done':''}"><small>درس ${i+1} · ${l.min} دقيقة${d.includes(l.id)?' · خلصته ✓':''}</small><br><b>${l.title}</b></a>`).join('')}</div>
      <footer>المؤلف: محمد السيسي (Mohammed El Sisi) · ${new Date().getFullYear()}<br>
      مبني على / مقتبس من: R. Sedgewick, Algorithms in C (3rd ed.), Parts 1–4.<br>
      الكود في <code>src/</code> · الكورس في <code>docs/</code></footer></section>`;
  }
  async function lesson(id){
    const i=all.findIndex(l=>l.id===id); if(i<0) return home();
    const l=all[i]; main.innerHTML='<article><p class="meta">بيحمّل…</p></article>';
    let html; try{const r=await fetch('lessons/'+l.file); if(!r.ok) throw 0; html=await r.text();}catch(e){html='<p>الدرس ماحمّلش. لازم تفتح الكورس من سيرفر (مش file://). جرّب: <code>python3 -m http.server</code> من مجلد docs.</p>'}
    const prev=all[i-1], next=all[i+1], isDone=done().includes(id);
    main.innerHTML=`<article><p class="meta">درس ${i+1} من ${all.length} · حوالي ${l.min} دقيقة</p><h1>${l.title}</h1>${html}
      <div class="done-box">${prev?`<a class="btn" href="#/${prev.id}">→ ${prev.title}</a>`:'<span></span>'}
      <button class="btn ${isDone?'':'main'}" id="doneBtn">${isDone?'✓ خلصته (دوس عشان تلغي)':'خلصت الدرس ده'}</button>
      ${next?`<a class="btn" href="#/${next.id}">${next.title} ←</a>`:'<span></span>'}</div></article>`;
    document.getElementById('doneBtn').onclick=()=>{let d=done(); d=d.includes(id)?d.filter(x=>x!==id):d.concat(id); setDone(d); if(!isDone&&next) location.hash='#/'+next.id; else lesson(id);};
    enhance(main); main.focus({preventScroll:true}); window.scrollTo(0,0);
  }
  function hl(s){return s.replace(/(\/\*[\s\S]*?\*\/|\/\/[^\n]*)|("[^"\n]*"|'[^'\n]*')|(#(?:include|define|ifdef|ifndef|if|else|endif|pragma|error)\b|\b(?:typedef|struct|enum|if|else|return|const|static|volatile|void|bool|true|false|for|while|do|break|continue|switch|case|default|sizeof|int|char|float|double|unsigned|long|short|size_t|NULL)\b)|\b([A-Za-z_][A-Za-z0-9_]*)(?=\s*\()|\b(0x[0-9A-Fa-f]+|\d+\.?\d*[fFuUlL]?)\b/g,
    (m,c,str,kw,fn,num)=>c?`<span class="c">${c}</span>`:str?`<span class="s">${str}</span>`:kw?`<span class="k">${kw}</span>`:fn?`<span class="f">${fn}</span>`:`<span class="m">${num}</span>`)}
  function enhance(root){
    root.querySelectorAll('pre').forEach(p=>{const c=p.querySelector('code'); if(c&&!p.dataset.raw){p.dataset.raw=c.textContent;c.innerHTML=hl(c.innerHTML);}
      const b=document.createElement('button');b.className='cp';b.textContent='نسخ';b.onclick=()=>{navigator.clipboard.writeText(p.dataset.raw||p.textContent).then(()=>{b.textContent='اتنسخ ✓';setTimeout(()=>b.textContent='نسخ',1400)})};p.appendChild(b);});
    root.querySelectorAll('a[data-src]').forEach(a=>{a.href=C.repo+a.dataset.src;a.target='_blank';a.rel='noopener';a.classList.add('src');if(!a.textContent)a.textContent='↗ '+a.dataset.src;});
    root.querySelectorAll('.quiz').forEach(q=>q.querySelectorAll('button').forEach(b=>b.onclick=()=>{if(q.classList.contains('answered'))return;q.classList.add('answered');b.classList.add('ok' in b.dataset?'right':'wrong');const ok=q.querySelector('[data-ok]');if(ok)ok.classList.add('right');}));
    root.querySelectorAll('.w[data-w]').forEach(w=>window.WIDGETS&&window.WIDGETS[w.dataset.w]&&window.WIDGETS[w.dataset.w](w));
  }
  document.getElementById('themeBtn').onclick=()=>{const d=document.documentElement;const t=d.dataset.theme==='dark'?'':'dark';d.dataset.theme=t;localStorage.setItem('algo-theme',t||'light')};
  document.getElementById('menuBtn').onclick=()=>side.classList.toggle('open');
  side.addEventListener('click',e=>{if(e.target.closest('a'))side.classList.remove('open')});
  function route(){render();const id=(location.hash.match(/#\/(.+)/)||[])[1];id?lesson(id):home();}
  window.addEventListener('hashchange',route); route();
})();
