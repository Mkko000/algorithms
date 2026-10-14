/* قايمة دروس كورس Algorithms in C.
   عشان تضيف درس: ملف في docs/lessons/ + سطر هنا. */
window.COURSE = {
  repo: '../blob/main/',
  parts: [
    { title: 'الأساس', lessons: [
      { id: 'intro',       file: '01-intro.html',       title: 'ليه الخوارزميات؟ وخريطة الكتاب', min: 15 },
      { id: 'union-find',  file: '02-union-find.html',  title: 'Union-Find: مشكلة التوصيل', min: 30 },
      { id: 'analysis',    file: '03-analysis.html',    title: 'تحليل الخوارزميات و Big-Oh', min: 30 }
    ]},
    { title: 'هياكل البيانات والـ ADT', lessons: [
      { id: 'data-structs', file: '04-data-structures.html', title: 'مصفوفات وقوائم ونصوص وجرافات', min: 35 },
      { id: 'adt',          file: '05-adt.html',              title: 'Stacks و Queues و فكرة الـ ADT', min: 30 },
      { id: 'recursion',    file: '06-recursion.html',        title: 'الريكيرشن والشجر و DFS/BFS', min: 40 }
    ]},
    { title: 'الترتيب Sorting', lessons: [
      { id: 'elem-sort',  file: '07-elementary-sorts.html', title: 'Selection و Insertion و Shellsort', min: 30 },
      { id: 'quicksort',  file: '08-quicksort.html',        title: 'Quicksort من الجوّه', min: 35 },
      { id: 'mergesort',  file: '09-mergesort.html',        title: 'Mergesort: الثبات والضمان', min: 25 },
      { id: 'heapsort',   file: '10-heapsort.html',         title: 'الـ Heap و Heapsort', min: 30 },
      { id: 'radix-sort', file: '11-radix-sort.html',       title: 'Radix Sort: LSD و MSD', min: 25 },
      { id: 'special',    file: '12-special-sorts.html',    title: 'ترتيبات خاصة وشبكات', min: 20 }
    ]},
    { title: 'البحث Searching', lessons: [
      { id: 'bst',       file: '13-bst.html',       title: 'جداول الرموز و Binary Search Trees', min: 35 },
      { id: 'balanced',  file: '14-balanced.html',  title: 'شجر متوازنة: Red-Black و Randomized', min: 35 },
      { id: 'hashing',   file: '15-hashing.html',   title: 'Hashing: Chaining و Linear Probing', min: 30 },
      { id: 'tries',     file: '16-tries.html',     title: 'Tries و Ternary Search Tries', min: 30 },
      { id: 'btree',     file: '17-btree.html',     title: 'B-Trees والبحث الخارجي', min: 25 }
    ]},
    { title: 'الخاتمة', lessons: [
      { id: 'practice', file: '18-practice.html', title: 'إزاي تتمرّن وتبقى PRO', min: 15 }
    ]}
  ]
};
