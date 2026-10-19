/* أدوات تفاعلية لكورس Algorithms in C.
   تتنادى من الدرس بـ <div class="w" data-w="الاسم"></div> */
(function(){
  const $=(w,s)=>w.querySelector(s);
  const W={};

  /* ====== 1) منحنيات Big-Oh ====== */
  W['bigoh']=w=>{
    const curves=[
      {name:'1',      f:n=>1,           c:'#8b949e'},
      {name:'lg N',   f:n=>Math.log2(n),c:'#22d3ee'},
      {name:'N',      f:n=>n,           c:'#2ecc71'},
      {name:'N lg N', f:n=>n*Math.log2(n),c:'#a78bfa'},
      {name:'N²',     f:n=>n*n,         c:'#f59e0b'},
      {name:'2ᴺ',     f:n=>Math.pow(2,Math.min(n,20)),c:'#f87171'}
    ];
    w.innerHTML=`<h5>منحنيات النمو — حرّك N وشوف مين بيموت الأول</h5>
      <label>N max <input id="n" type="range" min="8" max="40" value="20" style="width:180px"></label>
      <span id="nv" style="font-family:JetBrains Mono">20</span>
      <svg class="bigoh-svg" id="svg" viewBox="0 0 560 280"></svg>
      <div class="out" id="o"></div>`;
    const draw=()=>{
      const Nmax=+$(w,'#n').value; $(w,'#nv').textContent=Nmax;
      const svg=$(w,'#svg'); svg.innerHTML='';
      const Wd=520,H=220,pad=30;
      // axes
      svg.innerHTML=`<line x1="${pad}" y1="${H+20}" x2="${pad+Wd}" y2="${H+20}" stroke="#2a323c"/>
        <line x1="${pad}" y1="20" x2="${pad}" y2="${H+20}" stroke="#2a323c"/>`;
      const ymax=Nmax*Nmax; // scale to N^2 so 2^N blows up visibly
      const pts=curves.map(c=>{
        let d='';
        for(let i=1;i<=Nmax;i++){
          const x=pad+(i/Nmax)*Wd;
          const yv=Math.min(c.f(i),ymax*1.05);
          const y=H+20-(yv/ymax)*H;
          d+=(i===1?'M':'L')+x+','+y+' ';
        }
        return `<path d="${d}" fill="none" stroke="${c.c}" stroke-width="2"/><text x="${pad+Wd+4}" y="${H+20-(Math.min(c.f(Nmax),ymax)/ymax)*H}" fill="${c.c}" font-size="11">${c.name}</text>`;
      }).join('');
      svg.innerHTML+=pts;
      const n=Nmax;
      $(w,'#o').innerHTML=`عند N=${n}: &nbsp; lgN≈${Math.log2(n).toFixed(1)} &nbsp;·&nbsp; N lgN≈${(n*Math.log2(n)).toFixed(0)} &nbsp;·&nbsp; N²=${n*n} &nbsp;·&nbsp; 2ᴺ=${n<=20?Math.pow(2,n):'💥 كبير أوي'}`;
    };
    $(w,'#n').oninput=draw; draw();
  };

  /* ====== 2) Sorting visualizer ====== */
  W['sortviz']=w=>{
    w.innerHTML=`<h5>محاكي الترتيب — شوف الفَرق بعينك</h5>
      <div class="row">
        <label>الخوارزمية
          <select id="algo"><option value="insertion">Insertion</option>
          <option value="selection">Selection</option>
          <option value="bubble">Bubble</option>
          <option value="shell">Shellsort</option>
          <option value="quick">Quicksort</option>
          <option value="merge">Mergesort</option>
          <option value="heap">Heapsort</option></select>
        </label>
        <button class="btn" id="shuf">خلط</button>
        <button class="btn main" id="step">خطوة</button>
        <button class="btn" id="run">شغّل</button>
        <button class="btn" id="reset">من الأول</button>
      </div>
      <div class="viz" id="v" style="height:140px;display:flex;align-items:flex-end;gap:2px;padding:10px"></div>
      <div class="out" id="o">دوس «خلط» وبعدين «خطوة» أو «شغّل».</div>`;
    let a=[], steps=[], si=0, timer=null;
    const draw=(hi=-1,hj=-1,done=[])=>{
      const v=$(w,'#v'); const mx=Math.max(...a,1);
      v.innerHTML=a.map((x,i)=>`<div class="bar ${done.includes(i)?'done':(i===hi||i===hj)?'on':''}" style="height:${Math.round(20+100*x/mx)}px">${x}</div>`).join('');
    };
    const shuffle=()=>{
      a=Array.from({length:16},(_,i)=>i+1);
      for(let i=a.length-1;i>0;i--){const j=Math.floor(Math.random()*(i+1));[a[i],a[j]]=[a[j],a[i]];}
      steps=[]; si=0; if(timer){clearInterval(timer);timer=null;}
      draw(); $(w,'#o').textContent='اتخلط. اختار خوارزمية ودوس خطوة.';
    };
    const gen=()=>{
      const algo=$(w,'#algo').value; const b=a.slice(); steps=[];
      const snap=(i,j,arr)=>steps.push({a:arr.slice(),i,j});
      if(algo==='insertion'){
        for(let i=1;i<b.length;i++){let v=b[i],j=i; while(j>0&&b[j-1]>v){b[j]=b[j-1];j--;snap(j,i,b);} b[j]=v;snap(j,i,b);}
      } else if(algo==='selection'){
        for(let i=0;i<b.length;i++){let m=i; for(let j=i+1;j<b.length;j++){snap(m,j,b); if(b[j]<b[m])m=j;} [b[i],b[m]]=[b[m],b[i]]; snap(i,m,b);}
      } else if(algo==='bubble'){
        for(let i=0;i<b.length;i++) for(let j=b.length-1;j>i;j--){snap(j-1,j,b); if(b[j]<b[j-1]){[b[j],b[j-1]]=[b[j-1],b[j]]; snap(j-1,j,b);}}
      } else if(algo==='shell'){
        let h=1; while(h<=b.length/9)h=3*h+1;
        for(;h>0;h=Math.floor(h/3)) for(let i=h;i<b.length;i++){let v=b[i],j=i; while(j>=h&&b[j-h]>v){b[j]=b[j-h];j-=h;snap(j,i,b);} b[j]=v;snap(j,i,b);}
      } else if(algo==='quick'){
        const qs=(l,r)=>{if(r<=l)return; let i=l-1,j=r,v=b[r]; for(;;){while(b[++i]<v); while(v<b[--j]) if(j===l)break; if(i>=j)break; [b[i],b[j]]=[b[j],b[i]]; snap(i,j,b);} [b[i],b[r]]=[b[r],b[i]]; snap(i,r,b); qs(l,i-1); qs(i+1,r);};
        qs(0,b.length-1);
      } else if(algo==='merge'){
        const aux=b.slice();
        const merge=(l,m,r)=>{let i=l,j=m+1,k=l; while(i<=m&&j<=r){snap(i,j,b); aux[k++]=b[i]<=b[j]?b[i++]:b[j++];} while(i<=m)aux[k++]=b[i++]; while(j<=r)aux[k++]=b[j++]; for(k=l;k<=r;k++){b[k]=aux[k];} snap(l,r,b);};
        const ms=(l,r)=>{if(r<=l)return; const m=(l+r)>>1; ms(l,m); ms(m+1,r); merge(l,m,r);}; ms(0,b.length-1);
      } else if(algo==='heap'){
        const sink=(k,n)=>{while(2*k+1<n){let j=2*k+1; if(j+1<n&&b[j]<b[j+1])j++; if(b[k]>=b[j])break; [b[k],b[j]]=[b[j],b[k]]; snap(k,j,b); k=j;}};
        for(let k=(b.length>>1)-1;k>=0;k--) sink(k,b.length);
        for(let n=b.length-1;n>0;n--){[b[0],b[n]]=[b[n],b[0]]; snap(0,n,b); sink(0,n);}
      }
      si=0; $(w,'#o').textContent=`اتبنى ${steps.length} خطوة. يلا.`;
    };
    const step=()=>{
      if(!steps.length) gen();
      if(si>=steps.length){draw(-1,-1,a.map((_,i)=>i)); $(w,'#o').textContent='خلصت ✓'; if(timer){clearInterval(timer);timer=null;} return;}
      const s=steps[si++]; a=s.a; draw(s.i,s.j); $(w,'#o').textContent=`خطوة ${si}/${steps.length}`;
    };
    $(w,'#shuf').onclick=shuffle;
    $(w,'#step').onclick=()=>{if(timer){clearInterval(timer);timer=null;} step();};
    $(w,'#run').onclick=()=>{if(timer){clearInterval(timer);timer=null;return;} if(!steps.length)gen(); timer=setInterval(step,80);};
    $(w,'#reset').onclick=()=>{steps=[];si=0;draw();};
    $(w,'#algo').onchange=()=>{steps=[];si=0;};
    shuffle();
  };

  /* ====== 3) BST insert visualizer ====== */
  W['bstviz']=w=>{
    w.innerHTML=`<h5>BST: اكتب حرف ودوس Insert — الشجرة بتتبني قدامك</h5>
      <div class="row">
        <label>المفتاح <input id="k" maxlength="1" value="S" style="width:60px;text-transform:uppercase"></label>
        <button class="btn main" id="ins">Insert</button>
        <button class="btn" id="clr">فضّي</button>
        <button class="btn" id="demo">Demo: ASEARCHING</button>
      </div>
      <div class="viz" id="v" style="min-height:120px"></div>
      <div class="out" id="o">الشجرة فاضي. ابدأ بحرف.</div>`;
    let root=null;
    const insert=(h,k)=>{if(!h)return{k,l:null,r:null}; if(k<h.k)h.l=insert(h.l,k); else if(k>h.k)h.r=insert(h.r,k); return h;};
    const render=()=>{
      const v=$(w,'#v');
      if(!root){v.textContent='(فاضي)';return;}
      // simple level-order text tree
      let levels=[],q=[{n:root,d:0}];
      while(q.length){const {n,d}=q.shift(); if(!levels[d])levels[d]=[]; levels[d].push(n?n.k:'.'); if(n){q.push({n:n.l,d:d+1});q.push({n:n.r,d:d+1});}}
      // trim trailing empty levels
      while(levels.length&&levels[levels.length-1].every(x=>x==='.'))levels.pop();
      v.innerHTML=levels.map((lv,d)=>`<div style="text-align:center;margin:4px 0">${lv.map(x=>x==='.'?`<span class="tree-node" style="opacity:.2">·</span>`:`<span class="tree-node">${x}</span>`).join(' ')}</div>`).join('');
    };
    $(w,'#ins').onclick=()=>{const k=$(w,'#k').value.trim().toUpperCase(); if(!k)return; root=insert(root,k); render(); $(w,'#o').textContent=`اتضاف ${k}. inorder بيطبع المفاتيح مرتّبة.`; $(w,'#k').value=''; $(w,'#k').focus();};
    $(w,'#clr').onclick=()=>{root=null;render();$(w,'#o').textContent='اتفضّت.';};
    $(w,'#demo').onclick=()=>{root=null; for(const c of 'ASEARCHING') root=insert(root,c); render(); $(w,'#o').textContent='ASEARCHING — لاحظ إن الشجرة مش متوازنة بالضرورة.';};
    render();
  };

  /* ====== 4) Heap visualizer ====== */
  W['heapviz']=w=>{
    w.innerHTML=`<h5>Binary Heap (max): Insert و DelMax</h5>
      <div class="row">
        <label>قيمة <input id="v" type="number" value="42" style="width:80px"></label>
        <button class="btn main" id="ins">Insert (fixUp)</button>
        <button class="btn" id="del">DelMax (fixDown)</button>
        <button class="btn" id="clr">فضّي</button>
      </div>
      <div class="viz" id="viz" style="min-height:100px"></div>
      <div class="out" id="o">الـ heap فاضي. الاندكس 1 هو الجذر (مش بنستخدم 0).</div>`;
    let pq=[null]; // 1-indexed
    const draw=()=>{
      const v=$(w,'#viz');
      if(pq.length<=1){v.textContent='(فاضي)';return;}
      // show array + tree levels
      const arr=pq.slice(1).map((x,i)=>`<span class="tree-node">${i+1}:${x}</span>`).join(' ');
      let html=`<div style="margin-bottom:8px">array: ${arr}</div>`;
      let levels=[], q=[{i:1,d:0}];
      while(q.length){const {i,d}=q.shift(); if(i>=pq.length)continue; if(!levels[d])levels[d]=[]; levels[d].push(pq[i]); if(2*i<pq.length)q.push({i:2*i,d:d+1}); if(2*i+1<pq.length)q.push({i:2*i+1,d:d+1});}
      html+=levels.map(lv=>`<div style="text-align:center;margin:4px 0">${lv.map(x=>`<span class="tree-node">${x}</span>`).join(' ')}</div>`).join('');
      v.innerHTML=html;
    };
    const fixUp=k=>{while(k>1&&pq[k>>1]<pq[k]){[pq[k],pq[k>>1]]=[pq[k>>1],pq[k]];k>>=1;}};
    const fixDown=k=>{const N=pq.length-1; while(2*k<=N){let j=2*k; if(j<N&&pq[j]<pq[j+1])j++; if(pq[k]>=pq[j])break; [pq[k],pq[j]]=[pq[j],pq[k]]; k=j;}};
    $(w,'#ins').onclick=()=>{const v=+$(w,'#v').value; pq.push(v); fixUp(pq.length-1); draw(); $(w,'#o').textContent=`اتضاف ${v} واتعمل fixUp.`;};
    $(w,'#del').onclick=()=>{if(pq.length<=1)return; const m=pq[1]; pq[1]=pq.pop(); if(pq.length>1)fixDown(1); draw(); $(w,'#o').textContent=`DelMax رجع ${m}.`;};
    $(w,'#clr').onclick=()=>{pq=[null];draw();};
    draw();
  };

  /* ====== 5) Union-Find visualizer ====== */
  W['ufviz']=w=>{
    w.innerHTML=`<h5>Union-Find (weighted + path halving) — وصّل أرقام</h5>
      <div class="row">
        <label>p <input id="p" type="number" value="3" min="0" max="9" style="width:60px"></label>
        <label>q <input id="q" type="number" value="4" min="0" max="9" style="width:60px"></label>
        <button class="btn main" id="un">Union</button>
        <button class="btn" id="rst">Reset</button>
      </div>
      <div class="viz" id="v"></div>
      <div class="out" id="o">10 عناصر (0..9). كل واحد root لنفسه في الأول.</div>`;
    let id, sz;
    const init=()=>{id=Array.from({length:10},(_,i)=>i); sz=Array(10).fill(1); draw(); $(w,'#o').textContent='اتعمل Reset.';};
    const root=i=>{while(i!==id[i]){id[i]=id[id[i]]; i=id[i];} return i;};
    const draw=()=>{
      // group by root
      const groups={};
      for(let i=0;i<10;i++){const r=(()=>{let x=i;while(x!==id[x])x=id[x];return x;})(); (groups[r]=groups[r]||[]).push(i);}
      $(w,'#v').innerHTML=Object.entries(groups).map(([r,ms])=>
        `<div style="margin:6px 0"><span class="tree-node" style="background:var(--acc2);color:#04121a">root ${r}</span> ← ${ms.map(m=>`<span class="tree-node">${m}</span>`).join(' ')}</div>`
      ).join('')+`<div style="margin-top:8px;color:var(--mute)">id[] = [${id.join(', ')}]</div>`;
    };
    $(w,'#un').onclick=()=>{
      let p=+$(w,'#p').value, q=+$(w,'#q').value;
      let i=root(p), j=root(q);
      if(i===j){$(w,'#o').textContent=`${p} و ${q} أصلًا متوصلين (root=${i}).`; draw(); return;}
      if(sz[i]<sz[j]){id[i]=j; sz[j]+=sz[i];} else {id[j]=i; sz[i]+=sz[j];}
      $(w,'#o').textContent=`Union(${p},${q}) — اتربط الأصغر تحت الأكبر.`;
      draw();
    };
    $(w,'#rst').onclick=init; init();
  };

  /* ====== 6) Hash table probe demo ====== */
  W['hashviz']=w=>{
    w.innerHTML=`<h5>Linear Probing — اكتب مفتاح وشوف بيروح فين</h5>
      <div class="row">
        <label>مفتاح <input id="k" value="SEARCH" style="width:120px;text-transform:uppercase"></label>
        <button class="btn main" id="ins">Insert</button>
        <button class="btn" id="clr">فضّي</button>
      </div>
      <div class="viz" id="v" style="display:flex;flex-wrap:wrap;gap:4px"></div>
      <div class="out" id="o">M=16. hash = (127*h + c) % M لكل حرف.</div>`;
    const M=16; let st=Array(M).fill(null);
    const hash=s=>{let h=0,a=127; for(const c of s) h=(a*h+c.charCodeAt(0))%M; return h;};
    const draw=(hi=-1)=>{
      $(w,'#v').innerHTML=st.map((x,i)=>`<span class="tree-node ${i===hi?'red':''}" style="min-width:48px">${i}:${x||'.'}</span>`).join('');
    };
    $(w,'#ins').onclick=()=>{
      const k=$(w,'#k').value.trim().toUpperCase(); if(!k)return;
      let i=hash(k), start=i, probes=0;
      while(st[i]!==null){ if(st[i]===k){$(w,'#o').textContent=`${k} موجود في ${i}`; draw(i); return;} i=(i+1)%M; probes++; if(i===start){$(w,'#o').textContent='الجدول مليان!';return;}}
      st[i]=k; draw(i);
      $(w,'#o').textContent=`hash("${k}")=${hash(k)} → اتزقّ في slot ${i} بعد ${probes} probe.`;
    };
    $(w,'#clr').onclick=()=>{st=Array(M).fill(null);draw();};
    draw();
  };

  /* ====== 7) Basic recurrences (Section 2.5) ====== */
  W['recur']=w=>{
    const R=[
      {n:'C(N) = C(N−1) + N, C(1)=1',  f:null, cf:N=>N*(N+1)/2, cfn:'N(N+1)/2', why:'لوب بيشيل عنصر واحد كل مرة (زي selection sort)'},
      {n:'C(N) = C(N/2) + 1, C(1)=1',   f:null, cf:N=>Math.log2(N)+1, cfn:'lg N + 1', why:'بنقسم على 2 وخطوة واحدة (binary search)'},
      {n:'C(N) = C(N/2) + N, C(1)=0',   f:null, cf:N=>2*N, cfn:'≈ 2N', why:'بنقسم على 2 بس بنلمس كل العناصر (quickselect في المتوسط شبهه)'},
      {n:'C(N) = 2C(N/2) + N, C(1)=0',  f:null, cf:N=>N*Math.log2(N), cfn:'N lg N', why:'قسّم نصين وادمج خطي (mergesort)'},
      {n:'C(N) = 2C(N/2) + 1, C(1)=1',  f:null, cf:N=>2*N-1, cfn:'≈ 2N', why:'قسّم نصين وخطوة واحدة (أقصى عنصر بالـ divide & conquer)'}
    ];
    const memo=new Map();
    const C=(k,N)=>{ if(N<=1) return (k===0||k===1||k===4)?1:0; const key=k+':'+N; if(memo.has(key)) return memo.get(key);
      const h=Math.floor(N/2), c=N-h; let v;
      if(k===0) { v=0; for(let i=2;i<=N;i++) v+=i; v+=1; }
      else if(k===1) v=C(1,h)+1; else if(k===2) v=C(2,h)+N; else if(k===3) v=C(3,h)+C(3,c)+N; else v=C(4,h)+C(4,c)+1;
      memo.set(key,v); return v; };
    w.innerHTML=`<h5>المعادلات التكرارية الخمسة — احسبها بالظبط وقارنها بالحل</h5>
      <div class="row"><label>المعادلة <select id="r" style="width:260px">${R.map((r,i)=>`<option value="${i}">${r.n}</option>`).join('')}</select></label>
      <label>N = 2ⁿ ، n <input id="n" type="range" min="1" max="20" value="10" style="width:160px"></label></div>
      <div class="viz" id="t"></div><div class="out" id="o"></div>`;
    const draw=()=>{ const k=+$(w,'#r').value, n=+$(w,'#n').value; let rows='n     N         C(N) بالظبط       الحل المغلق\n';
      for(let i=Math.max(1,n-5);i<=n;i++){const N=2**i; rows+=`${String(i).padEnd(5)} ${String(N).padEnd(9)} ${String(C(k,N)).padEnd(17)} ${Math.round(R[k].cf(N))}\n`;}
      const N=2**n+3; rows+=`\nN مش قوة 2 (N=${N}): C(N)=${C(k,N)} ، الحل ≈ ${Math.round(R[k].cf(N))}`;
      $(w,'#t').innerHTML='<pre style="margin:0;background:none;border:0;padding:0">'+rows+'</pre>';
      $(w,'#o').innerHTML=`الحل: <b>${R[k].cfn}</b> — ${R[k].why}.`; };
    $(w,'#r').onchange=draw; $(w,'#n').oninput=draw; draw();
  };

  /* ====== 8) Partitioning step by step (Program 7.2) ====== */
  W['partviz']=w=>{
    w.innerHTML=`<h5>الـ Partition خطوة بخطوة — i من الشمال، j من اليمين، الـ pivot هو آخر عنصر</h5>
      <div class="row"><label>المدخل <input id="in" value="ASORTINGEXAMPLE" style="width:200px;text-transform:uppercase"></label>
      <button class="btn main" id="st">خطوة</button><button class="btn" id="all">كمّل للآخر</button><button class="btn" id="rs">من الأول</button></div>
      <div class="viz" id="v"></div><div class="out" id="o"></div>`;
    let a,i,j,v,phase,done,l,r;
    const init=()=>{a=$(w,'#in').value.toUpperCase().replace(/[^A-Z]/g,'').split('');if(a.length<2)a='ASORTINGEXAMPLE'.split('');l=0;r=a.length-1;v=a[r];i=l-1;j=r;phase=0;done=false;draw('الـ pivot = '+v+' (آخر عنصر). دوس «خطوة».');};
    const draw=msg=>{$(w,'#v').innerHTML=a.map((c,k)=>`<span class="bar ${k===r&&!done?'pivot':''} ${k===i||k===j?'on':''} ${done&&k===i?'done':''}" style="height:${24+ (c.charCodeAt(0)-64)*4}px">${c}</span>`).join('')+`<div style="margin-top:6px">i=${i} &nbsp; j=${j}</div>`;$(w,'#o').innerHTML=msg;};
    const step=()=>{ if(done) return false;
      if(phase===0){ do{i++;}while(a[i]<v); phase=1; draw(`i وقف عند ${a[i]} (مش أصغر من ${v}).`); return true;}
      if(phase===1){ do{j--;}while(v<a[j] && j>l); phase=2; draw(`j وقف عند ${a[j]} (مش أكبر من ${v})${j===l?' — أو وصل لأول المصفوفة':''}.`); return true;}
      if(i>=j){ [a[i],a[r]]=[a[r],a[i]]; done=true; draw(`i ≥ j: خلاص. بدّل a[i] مع الـ pivot → ${v} في مكانه النهائي (${i}). الشمال كله ≤ ${v} واليمين ≥ ${v}.`); return false;}
      [a[i],a[j]]=[a[j],a[i]]; phase=0; draw(`بدّل ${a[j]} و ${a[i]} وكمّل.`); return true; };
    $(w,'#st').onclick=step; $(w,'#all').onclick=()=>{while(step());}; $(w,'#rs').onclick=init; init();
  };

  /* ====== 9) LSD radix sort, pass by pass (Program 10.4) ====== */
  W['lsdviz']=w=>{
    const words0=['now','for','tip','ilk','dim','tag','jot','sob','nob','sky','hut','ace','bet','men','egg','few','jay','owl','joy','rap','gig','wee','was','cab','wad','caw','cue','fee','tap','ago'];
    let words, pass;
    w.innerHTML=`<h5>LSD radix: رتّب على آخر حرف، بعدين اللي قبله، بعدين الأول — كل مرة ترتيب «ثابت»</h5>
      <div class="row"><button class="btn main" id="nx">المرور الجاي</button><button class="btn" id="rs">من الأول</button></div>
      <div class="viz" id="v" style="columns:5;font-size:14px"></div><div class="out" id="o"></div>`;
    const draw=msg=>{const col=3-pass; $(w,'#v').innerHTML=words.map(x=>x.split('').map((c,k)=>k===col?`<b style="color:var(--acc)">${c}</b>`:c).join('')).join('<br>'); $(w,'#o').innerHTML=msg;};
    const init=()=>{words=words0.slice();pass=0;draw('دي الكلمات قبل أي حاجة. المرور الأول هيرتّب على الحرف التالت.');};
    $(w,'#nx').onclick=()=>{ if(pass>=3){draw('خلاص: 3 مرورات = 3 حروف. الكلمات مترتّبة بالكامل.');return;}
      const d=2-pass; const buckets=Array.from({length:26},()=>[]); words.forEach(x=>buckets[x.charCodeAt(d)-97].push(x)); words=buckets.flat(); pass++;
      draw(pass<3?`مرور ${pass}: اترتبوا على الحرف رقم ${d+1} (key-indexed counting، ثابت). الحرف اللي ملوّن هو اللي عليه الدور.`:'مرور 3 على الحرف الأول — والقايمة كلها بقت مترتبة، لأن كل مرور ثابت (stable) فبيحافظ على ترتيب المرور اللي قبله للمتساويين.'); };
    $(w,'#rs').onclick=init; init();
  };

  /* ====== 10) Linear probing vs double hashing ====== */
  W['probeviz']=w=>{
    const M=31;
    w.innerHTML=`<h5>Linear probing ضد Double hashing — نفس المفاتيح، شوف الـ clusters</h5>
      <div class="row"><label>عدد المفاتيح <input id="n" type="range" min="5" max="29" value="20" style="width:160px"></label><span id="nv"></span>
      <button class="btn" id="sh">مفاتيح جديدة</button></div>
      <div class="viz" id="v"></div><div class="out" id="o"></div>`;
    let keys=[];
    const gen=()=>{keys=[];while(keys.length<29){const k=Math.floor(Math.random()*1000);if(!keys.includes(k))keys.push(k);}};
    const build=(n,dbl)=>{const t=Array(M).fill(null);let tot=0;for(let q=0;q<n;q++){const k=keys[q];let i=k%M,s=dbl?(7-k%7):1,p=1;while(t[i]!==null){i=(i+s)%M;p++;}t[i]=k;tot+=p;}return {t,avg:tot/n};};
    const row=(t)=>t.map(x=>`<span class="tree-node ${x===null?'':'red'}" style="min-width:34px;padding:2px 3px;font-size:11px">${x===null?'·':x}</span>`).join('');
    const draw=()=>{const n=+$(w,'#n').value; $(w,'#nv').textContent=`α = ${n}/${M} = ${(n/M).toFixed(2)}`; const L=build(n,0),D=build(n,1);
      const clus=t=>{let mx=0,c=0;for(let k=0;k<2*M;k++){if(t[k%M]!==null){c++;mx=Math.max(mx,Math.min(c,M));}else c=0;}return mx;};
      $(w,'#v').innerHTML=`<div>linear:</div><div style="display:flex;flex-wrap:wrap;gap:2px">${row(L.t)}</div><div style="margin-top:8px">double (step = 7 − k mod 7):</div><div style="display:flex;flex-wrap:wrap;gap:2px">${row(D.t)}</div>`;
      $(w,'#o').innerHTML=`متوسط الـ probes وقت الإدخال: linear <b>${L.avg.toFixed(2)}</b> (أطول cluster ${clus(L.t)}) · double <b>${D.avg.toFixed(2)}</b> (أطول cluster ${clus(D.t)}). زوّد α لحد 0.9 وشوف linear بيتعب إزاي.`;};
    $(w,'#n').oninput=draw; $(w,'#sh').onclick=()=>{gen();draw();}; gen(); draw();
  };

  /* ====== 11) Batcher sorting network ====== */
  W['netviz']=w=>{
    w.innerHTML=`<h5>شبكة Batcher للترتيب — كل خط رأسي مقارِن (compare-exchange)</h5>
      <div class="row"><label>N <select id="n"><option>4</option><option selected>8</option><option>16</option></select></label>
      <button class="btn main" id="go">شغّل أرقام عشوائية</button><button class="btn" id="z1">جرّب 0/1 كلهم (مبدأ 0-1)</button></div>
      <svg id="s" viewBox="0 0 600 260" style="width:100%;direction:ltr;background:var(--code);border-radius:12px"></svg><div class="out" id="o"></div>`;
    const net=N=>{const c=[];for(let p=1;p<N;p+=p)for(let k=p;k>0;k=k>>1)for(let j=k%p;j+k<N;j+=k+k)for(let i=0;i<k;i++)if(Math.floor((i+j)/(p+p))===Math.floor((i+j+k)/(p+p)))c.push([i+j,i+j+k]);return c;};
    const draw=(vals)=>{const N=+$(w,'#n').value,c=net(N),H=240/N,X=520/(c.length+1);let g='';
      for(let i=0;i<N;i++)g+=`<line x1="40" y1="${12+i*H+H/2}" x2="580" y2="${12+i*H+H/2}" stroke="#4b5563"/>`;
      c.forEach(([a,b],k)=>{const x=50+(k+1)*X;g+=`<line x1="${x}" y1="${12+a*H+H/2}" x2="${x}" y2="${12+b*H+H/2}" stroke="#22d3ee" stroke-width="2"/><circle cx="${x}" cy="${12+a*H+H/2}" r="3" fill="#22d3ee"/><circle cx="${x}" cy="${12+b*H+H/2}" r="3" fill="#22d3ee"/>`;});
      if(vals){const out=vals.slice();c.forEach(([a,b])=>{if(out[b]<out[a])[out[a],out[b]]=[out[b],out[a]];});
        for(let i=0;i<N;i++){g+=`<text x="8" y="${16+i*H+H/2}" fill="#e5e7eb" font-size="${N>8?9:12}">${vals[i]}</text><text x="584" y="${16+i*H+H/2}" fill="#86efac" font-size="${N>8?9:12}">${out[i]}</text>`;}}
      $(w,'#s').innerHTML=g; return c;};
    $(w,'#go').onclick=()=>{const N=+$(w,'#n').value,v=Array.from({length:N},()=>Math.floor(Math.random()*90)+10);const c=draw(v);$(w,'#o').innerHTML=`N=${N}: ${c.length} مقارن. الأرقام دخلت شمال وطلعت يمين مترتّبة — والشبكة نفسها ماتعرفش حاجة عن الأرقام (nonadaptive).`;};
    $(w,'#z1').onclick=()=>{const N=+$(w,'#n').value,c=draw(null);let ok=0;for(let m=0;m<(1<<N);m++){const b=Array.from({length:N},(_,i)=>(m>>i)&1);c.forEach(([x,y])=>{if(b[y]<b[x])[b[x],b[y]]=[b[y],b[x]];});if(b.every((t,i)=>i===0||b[i-1]<=t))ok++;}
      $(w,'#o').innerHTML=`جرّبنا كل الـ ${1<<N} مدخل من أصفار وواحدات: اترتّب منهم ${ok}. بمبدأ 0-1 (Property 11.1) ده كفاية نتأكد إن الشبكة بترتّب <b>أي</b> مدخل.`;};
    $(w,'#n').onchange=()=>$(w,'#go').onclick(); $(w,'#go').onclick();
  };


  window.WIDGETS=W;
})();
