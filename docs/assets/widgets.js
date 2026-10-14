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

  window.WIDGETS=W;
})();
