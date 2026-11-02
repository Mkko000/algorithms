/* قايمة دروس كورس Algorithms in C (سادجوك، الطبعة التالتة، الأجزاء 1–4).
   كل درس = ملف في docs/lessons/ + سطر هنا. الترتيب هنا هو ترتيب الكورس،
   والتقدّم بيتحفظ بالـ id (فمتغيّرش الـ id بتاع درس موجود).
   دروس "تمارين محلولة" (ex-*) جاية بعد كل فصل على طول. */
window.COURSE = {
  repo: '../blob/main/',
  parts: [
    { title: 'الأساس (الفصول 1–2)', lessons: [
      { id: 'intro',        file: '01-intro.html',              title: 'ليه الخوارزميات؟ وخريطة الكتاب', min: 15 },
      { id: 'union-find',   file: '02-union-find.html',         title: 'Union-Find: مشكلة التوصيل بكل أنواعها', min: 40 },
      { id: 'ex-ch1',       file: 'ex-01-union-find.html',      title: 'تمارين محلولة: الفصل 1', min: 30 },
      { id: 'analysis',     file: '03-analysis.html',           title: 'تحليل الخوارزميات: القياس ونمو الدوال و Big-Oh', min: 30 },
      { id: 'recurrences',  file: '04-recurrences.html',        title: 'الـ Recurrences والبحث والضمانات والـ Amortized', min: 35 },
      { id: 'ex-ch2',       file: 'ex-02-analysis.html',        title: 'تمارين محلولة: الفصل 2', min: 30 }
    ]},
    { title: 'هياكل البيانات والـ ADT (الفصول 3–4)', lessons: [
      { id: 'data-structs', file: '05-data-structures.html',    title: 'الأنواع والمصفوفات: الأساس اللي تحت كل حاجة', min: 25 },
      { id: 'linked-lists', file: '06-linked-lists.html',       title: 'القوائم المتصلة ومعالجتها والذاكرة', min: 40 },
      { id: 'strings-compound', file: '07-strings-compound.html', title: 'النصوص والهياكل المركّبة والجرافات', min: 30 },
      { id: 'ex-ch3',       file: 'ex-03-data-structures.html', title: 'تمارين محلولة: الفصل 3', min: 35 },
      { id: 'adt',          file: '08-adt.html',                title: 'فكرة الـ ADT والـ Stack وتطبيقاته', min: 35 },
      { id: 'queues-adt',   file: '09-queues-adt.html',         title: 'Generic ADTs والـ Queues والتكرار', min: 35 },
      { id: 'first-class',  file: '10-first-class-adt.html',    title: 'First-class ADTs: الأعداد المركّبة وكثيرات الحدود', min: 30 },
      { id: 'ex-ch4',       file: 'ex-04-adt.html',             title: 'تمارين محلولة: الفصل 4', min: 35 }
    ]},
    { title: 'الريكيرشن والشجر (الفصل 5)', lessons: [
      { id: 'recursion',    file: '11-recursion.html',          title: 'الريكيرشن و Divide-and-Conquer', min: 35 },
      { id: 'dp',           file: '12-dynamic-programming.html',title: 'Dynamic Programming', min: 35 },
      { id: 'trees',        file: '13-trees.html',              title: 'الشجر: تعريفات وخصائص ولفّ', min: 40 },
      { id: 'graph-traversal', file: '14-graph-traversal.html', title: 'DFS و BFS', min: 35 },
      { id: 'ex-ch5',       file: 'ex-05-recursion-trees.html', title: 'تمارين محلولة: الفصل 5', min: 40 }
    ]},
    { title: 'الترتيب Sorting (الفصول 6–11)', lessons: [
      { id: 'elem-sort',    file: '15-elementary-sorts.html',   title: 'Selection و Insertion و Bubble', min: 35 },
      { id: 'shellsort-more', file: '16-shellsort-and-more.html', title: 'Shellsort وترتيب الـ records والقوائم والعدّ', min: 40 },
      { id: 'ex-ch6',       file: 'ex-06-elementary-sorts.html',title: 'تمارين محلولة: الفصل 6', min: 35 },
      { id: 'quicksort',    file: '17-quicksort.html',          title: 'Quicksort من الجوّه', min: 35 },
      { id: 'quicksort-plus', file: '18-quicksort-improvements.html', title: 'تحسينات Quicksort و Selection', min: 40 },
      { id: 'ex-ch7',       file: 'ex-07-quicksort.html',       title: 'تمارين محلولة: الفصل 7', min: 35 },
      { id: 'mergesort',    file: '19-mergesort.html',          title: 'Mergesort: الدمج والتحليل', min: 35 },
      { id: 'mergesort-plus', file: '20-mergesort-more.html',   title: 'Bottom-up والقوائم والأداء', min: 35 },
      { id: 'ex-ch8',       file: 'ex-08-mergesort.html',       title: 'تمارين محلولة: الفصل 8', min: 35 },
      { id: 'heapsort',     file: '21-heapsort.html',           title: 'الـ Heap و Heapsort', min: 40 },
      { id: 'pq-advanced',  file: '22-priority-queues.html',    title: 'PQ ADT كامل و Index heaps و Binomial queues', min: 40 },
      { id: 'ex-ch9',       file: 'ex-09-priority-queues.html', title: 'تمارين محلولة: الفصل 9', min: 35 },
      { id: 'radix-sort',   file: '23-radix-sort.html',         title: 'Radix Sort: البتات والـ MSD', min: 35 },
      { id: 'radix-more',   file: '24-radix-more.html',         title: '3-way radix quicksort و LSD والأداء', min: 40 },
      { id: 'ex-ch10',      file: 'ex-10-radix-sort.html',      title: 'تمارين محلولة: الفصل 10', min: 35 },
      { id: 'special',      file: '25-special-sorts.html',      title: 'Batcher وشبكات الترتيب', min: 35 },
      { id: 'external-sort',file: '26-external-sort.html',      title: 'الترتيب الخارجي والـ Polyphase والمعالجات المتوازية', min: 40 },
      { id: 'ex-ch11',      file: 'ex-11-special-sorts.html',   title: 'تمارين محلولة: الفصل 11', min: 35 }
    ]},
    { title: 'البحث Searching (الفصول 12–16)', lessons: [
      { id: 'symbol-tables',file: '27-symbol-tables.html',      title: 'الـ Symbol Table و Key-indexed و Binary search', min: 40 },
      { id: 'bst',          file: '28-bst.html',                title: 'Binary Search Trees: البحث والإضافة والأداء', min: 40 },
      { id: 'bst-ops',      file: '29-bst-operations.html',     title: 'Root insertion و Select والحذف والـ Join', min: 40 },
      { id: 'ex-ch12',      file: 'ex-12-symbol-tables.html',   title: 'تمارين محلولة: الفصل 12', min: 40 },
      { id: 'balanced',     file: '30-balanced.html',           title: 'Randomized BSTs و Splay trees', min: 40 },
      { id: 'rb-trees',     file: '31-red-black.html',          title: 'شجر 2-3-4 و Red-Black', min: 45 },
      { id: 'skip-lists',   file: '32-skip-lists.html',         title: 'Skip lists ومقارنة كل الطرق', min: 35 },
      { id: 'ex-ch13',      file: 'ex-13-balanced-trees.html',  title: 'تمارين محلولة: الفصل 13', min: 40 },
      { id: 'hashing',      file: '33-hashing.html',            title: 'دوال الـ Hash و Separate chaining', min: 35 },
      { id: 'open-addressing', file: '34-open-addressing.html', title: 'Linear probing و Double hashing و Dynamic tables', min: 40 },
      { id: 'ex-ch14',      file: 'ex-14-hashing.html',         title: 'تمارين محلولة: الفصل 14', min: 35 },
      { id: 'tries',        file: '35-tries.html',              title: 'Digital search trees والـ Tries', min: 35 },
      { id: 'patricia',     file: '36-patricia.html',           title: 'Patricia tries', min: 35 },
      { id: 'tst',          file: '37-tst.html',                title: 'Multiway tries و TSTs وتطبيقات النصوص', min: 40 },
      { id: 'ex-ch15',      file: 'ex-15-radix-search.html',    title: 'تمارين محلولة: الفصل 15', min: 45 },
      { id: 'btree',        file: '38-btree.html',              title: 'البحث الخارجي: ISAM و B-trees', min: 40 },
      { id: 'extendible',   file: '39-extendible-hashing.html', title: 'Extendible hashing والـ Perspective', min: 35 },
      { id: 'ex-ch16',      file: 'ex-16-external-searching.html', title: 'تمارين محلولة: الفصل 16', min: 40 }
    ]},
    { title: 'هياكل بيانات إضافية: من كتب Weiss وThareja وLipschutz (الفصول 17–31)', lessons: [
      { id: 'arrays-matrices', file: '41-arrays-matrices.html', title: 'Arrays والمصفوفات: العناوين والـ Sparse والمثلثية', min: 40 },
      { id: 'ex-ch17', file: 'ex-17-arrays-matrices.html', title: 'تمارين محلولة: الفصل 17', min: 40 },
      { id: 'linked-variants', file: '42-linked-variants.html', title: 'قوايم Header و Circular و Doubly و Cursor وكثيرات الحدود', min: 45 },
      { id: 'ex-ch18', file: 'ex-18-linked-variants.html', title: 'تمارين محلولة: الفصل 18', min: 45 },
      { id: 'stacks-queues-apps', file: '43-stacks-queues-apps.html', title: 'Stacks وQueues: التعبيرات والتطبيقات والـ call stack والـ deque', min: 50 },
      { id: 'ex-ch19', file: 'ex-19-stacks-queues.html', title: 'تمارين محلولة: الفصل 19', min: 45 },
      { id: 'binary-trees', file: '44-binary-trees.html', title: 'الشجر الثنائي: التمثيل والـ traversals وشجر التعبير وHuffman', min: 55 },
      { id: 'ex-ch20', file: 'ex-20-trees.html', title: 'تمارين محلولة: الفصل 20', min: 45 },
      { id: 'efficient-trees', file: '45-efficient-trees.html', title: 'BST وThreaded وAVL وRed-Black وSplay', min: 60 },
      { id: 'ex-ch21', file: 'ex-21-efficient-trees.html', title: 'تمارين محلولة: الفصل 21', min: 55 },
      { id: 'multiway-trees', file: '46-multiway-trees.html', title: 'الأشجار متعددة الفروع: M-way وB وB+ و2-3 وTrie', min: 70 },
      { id: 'ex-ch22', file: 'ex-22-multiway.html', title: 'تمارين الفصل 22', min: 60 }
    ]},
    { title: 'الخاتمة', lessons: [
      { id: 'practice',     file: '40-practice.html',           title: 'تختار إيه إمتى؟ ومشاريع تطبيقية', min: 30 }
    ]}
  ]
};
