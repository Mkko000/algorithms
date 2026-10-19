/* ============================================================
 *  ex15_48_tst.c  —  Exercises 15.48-15.52, 15.58, 15.62, 15.76-15.78
 *  Chapter 15: Radix Search — multiway tries and TSTs
 *
 *  15.48 Draw the existence trie that results when you insert the words
 *        now is the time for all good people to come the aid of their
 *        party into an initially empty trie. Use 27-way branching.
 *  15.49 Draw the existence TST that results when you insert the same
 *        words into an initially empty TST.
 *  15.50 Draw the 4-way trie that results when you insert items with the
 *        keys 01010011 00000111 00100001 01010001 11101100 00100001
 *        10010101 01001010 into an initially empty trie, using 2-bit bytes.
 *  15.51 Draw the TST that results when you insert the same keys into an
 *        initially empty TST, using 2-bit bytes.
 *  15.52 Draw the TST that results when you insert the same keys into an
 *        initially empty TST, using 4-bit bytes.
 *  15.58 Write a function that prints all the keys in a TST that differ
 *        from the search key in at most k positions, for a given integer k.
 *  15.62 Write a program that prints out all keys in an R-way trie that
 *        have the same initial t bytes as a given search key.
 *  15.76 Draw the 26-way DST that results when you build a text-string
 *        index from the words now is the time for all good people to
 *        come the aid of their party.
 *  15.77 Draw the 26-way trie that results when you build a text-string
 *        index from the same words.
 *  15.78 Draw the TST that results when you build a text-string index
 *        from the same words, in the style of Figure 15.20.
 *
 *  بالعربي: نبني existence trie وTST لكلمات الجملة الكلاسيكية، و4-way trie
 *  وTST لنفس المفاتيح بالـ 2 بت والـ 4 بت بايت، ونطبع المفاتيح اللي بتختلف
 *  في k حروف على الأكثر، واللي ليها نفس أول t بايت، ونبني indexes على النص.
 *
 *  THINKING STEPS
 *   1. Existence R-way trie (Program 15.7): each node has R links; the PATH
 *      spells the key; a non-NULL end-of-string link marks existence. Space
 *      is R * nodes; most links are NULL — that is why TSTs exist.
 *   2. TST (Program 15.8): three links per node (less / equal / greater).
 *      Equal means "consume this character and go deeper"; left/right stay
 *      at the same depth. Nodes = characters that appear, so space is
 *      linear in the total character count of the keys.
 *   3. 15.50-52: treat the 8-bit strings as sequences of 2-bit (R = 4) or
 *      4-bit (R = 16) digits; a TST of digits has the same shape as a
 *      character TST. The 4-way trie has R = 4 links per node.
 *   4. 15.58 Hamming distance <= k: at every TST node, if the search
 *      character equals the node's character go middle with the same k;
 *      otherwise go left/right with the same k AND go middle with k-1
 *      (paying one mismatch). Stop when k < 0.
 *   5. 15.76-78 string index: the "keys" are the SUFFIXES of the text
 *      starting at every word (or every character). A 26-way DST branches
 *      on the character of the search key at each depth and stores one
 *      position per node; a 26-way trie / TST of the suffixes is the
 *      classical string index of Figure 15.20.
 *
 *  Author:  Mohammed El Sisi
 *  Adapted from: R. Sedgewick, "Algorithms in C" (3rd ed.),
 *                Programs 15.7-15.8, Exercises 15.48-15.78
 *  Build:   gcc -std=c99 -Wall -O2 ex15_48_tst.c -o ex15_48_tst
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================= R-way existence trie */
#define R27 27
typedef struct rnode *rlink;
struct rnode { rlink next[R27]; };
static long rnodes;
static int rdigit(const char *s, int d) { return s[d] ? s[d] - 'a' + 1 : 0; }
static rlink rNEW(void) { rnodes++; return calloc(1, sizeof(struct rnode)); }
static rlink rIns(rlink h, const char *v, int w)
{
    int i = rdigit(v, w); if (!h) h = rNEW();
    if (i == 0) { h->next[0] = h; return h; }
    h->next[i] = rIns(h->next[i], v, w + 1); return h;
}
static void rShow(rlink h, char *buf, int d, int indent)
{
    if (!h) return;
    if (h->next[0]) { buf[d] = 0; printf("%*s[%s]\n", indent, "", buf); }
    for (int i = 1; i < R27; i++) if (h->next[i]) {
        buf[d] = (char)('a' + i - 1);
        printf("%*s%c\\\n", indent, "", buf[d]);
        rShow(h->next[i], buf, d + 1, indent + 2);
    }
}
static void rFree(rlink h) { if (!h) return; for (int i = 1; i < R27; i++) rFree(h->next[i]); free(h); }
/* 15.62: keys with the same first t characters as v */
static void rPrefix(rlink h, const char *v, int t, int w, char *buf)
{
    if (!h) return;
    if (w < t) { int i = rdigit(v, w); if (!i || !h->next[i]) return; buf[w] = v[w]; rPrefix(h->next[i], v, t, w + 1, buf); return; }
    if (h->next[0]) { buf[w] = 0; printf(" %s", buf); }
    for (int i = 1; i < R27; i++) if (h->next[i]) { buf[w] = (char)('a' + i - 1); rPrefix(h->next[i], v, t, w + 1, buf); }
}

/* ========================================================= TST (Program 15.8) */
typedef struct tnode *tlink;
struct tnode { int c; int end; tlink l, m, r; };      /* c is a digit (0..R-1) or a char */
static long tnodes;
static tlink tNEW(int c) { tlink x = calloc(1, sizeof *x); x->c = c; tnodes++; return x; }
static tlink tIns(tlink h, const int *s, int d, int n) /* s[0..n-1] digits, then end */
{
    int ch = (d < n) ? s[d] : -1;                    /* -1 = end-of-key sentinel */
    if (!h) h = tNEW(ch);
    if      (ch < h->c) h->l = tIns(h->l, s, d, n);
    else if (ch > h->c) h->r = tIns(h->r, s, d, n);
    else if (d < n)     h->m = tIns(h->m, s, d + 1, n);
    else                h->end = 1;
    return h;
}
static void tFree(tlink h) { if (h) { tFree(h->l); tFree(h->m); tFree(h->r); free(h); } }
/* print a TST node by node: "=x" middle child (next character), "<x" / ">x" left / right
   sibling at the SAME character position; "$" = end of a key (the word so far exists) */
static char sym(int c, int base) { return c < 0 ? '$' : base == 26 ? (char)('a' + c) : (char)(c < 10 ? '0' + c : 'a' + c - 10); }
static void tShowR(tlink h, char *buf, int d, int indent, int base, char how)
{
    if (!h) return;
    printf("%*s%c%c", indent, "", how, sym(h->c, base));
    if (h->c < 0) { buf[d] = 0; printf("   [%s]", buf); }
    else if (h->end) { buf[d] = sym(h->c, base); buf[d + 1] = 0; printf("   [%s]", buf); }
    printf("\n");
    if (h->c >= 0) buf[d] = sym(h->c, base);
    tShowR(h->l, buf, d, indent + 2, base, '<');
    if (h->c >= 0) { buf[d] = sym(h->c, base); tShowR(h->m, buf, d + 1, indent + 2, base, '='); }
    tShowR(h->r, buf, d, indent + 2, base, '>');
}
static void tShow(tlink h, char *buf, int d, int indent, int base) { tShowR(h, buf, d, indent, base, ' '); }
static int tHeight(tlink h) { if (!h) return 0; int a = tHeight(h->l), b = tHeight(h->m), c = tHeight(h->r); int m = a > b ? a : b; return 1 + (m > c ? m : c); }

/* 15.58: Hamming distance <= k */
static void tHamm2(tlink h, const char *v, int d, int k, char *buf, int len)
{
    if (!h || k < 0) return;
    tHamm2(h->l, v, d, k, buf, len);
    if (h->c < 0) {
        int rem = (int)strlen(v) - d; if (rem < 0) rem = -rem;
        if (rem <= k) { buf[len] = 0; printf(" %s", buf); }
    } else {
        buf[len] = (char)('a' + h->c);
        int pay = (d < (int)strlen(v) && v[d] - 'a' == h->c) ? 0 : 1;
        if (h->end) {
            int rem = (int)strlen(v) - (d + 1); if (rem < 0) rem = -rem;
            if (pay + rem <= k) { buf[len + 1] = 0; printf(" %s", buf); }
        }
        if (h->m) tHamm2(h->m, v, d + 1, k - pay, buf, len + 1);
    }
    tHamm2(h->r, v, d, k, buf, len);
}

/* ========================================================= 4-way (R-way digit) trie */
typedef struct mnode *mlink;
struct mnode { mlink next[4]; int leaf; unsigned key; };
static long mnodes;
static mlink mNEW(void) { mnodes++; return calloc(1, sizeof(struct mnode)); }
static int mdigit(unsigned v, int d, int bits) { return (int)((v >> (8 - bits * (d + 1))) & ((1u << bits) - 1)); }
static mlink mIns(mlink h, unsigned v, int d, int bits, int digits)
{
    if (d == digits) { if (!h) { h = mNEW(); h->leaf = 1; h->key = v; } return h; }
    if (!h) h = mNEW();
    int i = mdigit(v, d, bits);
    h->next[i] = mIns(h->next[i], v, d + 1, bits, digits);
    return h;
}
static void mShow(mlink h, char *path, int d)
{
    if (!h) return;
    if (h->leaf) { path[d] = 0; printf("       leaf %-8s : ", d ? path : "(root)"); for (int i = 7; i >= 0; i--) putchar('0' + ((h->key >> i) & 1)); printf("\n"); return; }
    for (int i = 0; i < 4; i++) if (h->next[i]) { path[d] = (char)('0' + i); mShow(h->next[i], path, d + 1); }
}
static void mFree(mlink h) { if (!h) return; for (int i = 0; i < 4; i++) mFree(h->next[i]); free(h); }

/* ========================================================= 26-way DST for a string index (15.76) */
typedef struct dnode *dlink;
struct dnode { int pos; dlink next[26]; };
static dlink dIns(dlink h, const char *text, int pos, int d)
{
    if (!h) { h = calloc(1, sizeof *h); h->pos = pos; return h; }
    char ch = text[pos + d];
    if (!ch) return h;                               /* exact duplicate suffix */
    int i = ch - 'a'; if (i < 0 || i >= 26) return h;
    h->next[i] = dIns(h->next[i], text, pos, d + 1);
    return h;
}
static void dShow(dlink h, const char *text, int indent)
{
    if (!h) return;
    printf("%*spos %2d \"%.*s...\"\n", indent, "", h->pos, 12, text + h->pos);
    for (int i = 0; i < 26; i++) if (h->next[i]) { printf("%*s%c\\\n", indent, "", 'a' + i); dShow(h->next[i], text, indent + 2); }
}
static void dFree(dlink h) { if (!h) return; for (int i = 0; i < 26; i++) dFree(h->next[i]); free(h); }

/* helpers: pack a letter-word into digit array for the letter TST */
static int packWord(const char *s, int *out) { int n = 0; for (; s[n]; n++) out[n] = s[n] - 'a'; return n; }
static int packBits(unsigned v, int bits, int *out)
{
    int digits = 8 / bits; for (int d = 0; d < digits; d++) out[d] = mdigit(v, d, bits); return digits;
}

int main(void)
{
    const char *words[] = { "now","is","the","time","for","all","good","people","to","come","the","aid","of","their","party" };
    int nwords = 15;
    char buf[64];

    /* ---------- 15.48 / 15.62 ---------- */
    rnodes = 0; rlink rt = NULL;
    for (int i = 0; i < nwords; i++) rt = rIns(rt, words[i], 0);
    printf("15.48 27-way existence trie for the sentence (end-of-string = [word]):\n");
    rShow(rt, buf, 0, 5);
    printf("     %ld nodes x %d links = %ld links (most NULL)\n", rnodes, R27, rnodes * R27);
    printf("15.62 keys with the same first 2 characters as \"th\":"); rPrefix(rt, "th", 2, 0, buf); printf("\n");
    printf("      keys with the same first 3 characters as \"peo\":"); rPrefix(rt, "peo", 3, 0, buf); printf("\n");

    /* ---------- 15.49 / 15.58 ---------- */
    tnodes = 0; tlink tt = NULL; int dig[32];
    for (int i = 0; i < nwords; i++) { int n = packWord(words[i], dig); tt = tIns(tt, dig, 0, n); }
    printf("15.49 existence TST for the same words (%ld nodes, height %d; '=' next char, '<' '>' siblings, '$' end):\n", tnodes, tHeight(tt));
    tShow(tt, buf, 0, 5, 26);
    printf("15.58 keys within Hamming distance 1 of \"the\":"); tHamm2(tt, "the", 0, 1, buf, 0); printf("\n");
    printf("      keys within Hamming distance 2 of \"tome\":"); tHamm2(tt, "tome", 0, 2, buf, 0); printf("\n");
    tFree(tt); tt = NULL;

    /* ---------- 15.50 / 15.51 / 15.52 ---------- */
    { const char *ks[] = { "01010011","00000111","00100001","01010001","11101100","00100001","10010101","01001010" };
      unsigned u[8]; int n = 0;
      for (int i = 0; i < 8; i++) { unsigned v = (unsigned)strtoul(ks[i], NULL, 2); int seen = 0; for (int j = 0; j < n; j++) if (u[j] == v) seen = 1; if (!seen) u[n++] = v; }
      mnodes = 0; mlink mt = NULL;
      for (int i = 0; i < n; i++) mt = mIns(mt, u[i], 0, 2, 4);
      printf("15.50 4-way trie (2-bit bytes) for the 8-bit keys (%ld nodes):\n", mnodes);
      mShow(mt, buf, 0); mFree(mt);
      tnodes = 0; for (int i = 0; i < n; i++) { int nd = packBits(u[i], 2, dig); tt = tIns(tt, dig, 0, nd); }
      printf("15.51 TST with 2-bit bytes (%ld nodes):\n", tnodes);
      tShow(tt, buf, 0, 5, 4); tFree(tt); tt = NULL;
      tnodes = 0; for (int i = 0; i < n; i++) { int nd = packBits(u[i], 4, dig); tt = tIns(tt, dig, 0, nd); }
      printf("15.52 TST with 4-bit bytes (%ld nodes):\n", tnodes);
      tShow(tt, buf, 0, 5, 16); tFree(tt); tt = NULL; }

    /* ---------- 15.76 / 15.77 / 15.78 string indexes ---------- */
    { /* the text without spaces (26-letter alphabet); keys = suffixes starting at each word */
      char text[256] = ""; int starts[32], ns = 0;
      for (int i = 0; i < nwords; i++) { starts[ns++] = (int)strlen(text); strcat(text, words[i]); }
      printf("15.76-78 text (spaces removed) = \"%s\", one suffix per word start\n", text);
      dlink dt = NULL;
      for (int i = 0; i < ns; i++) dt = dIns(dt, text, starts[i], 0);
      printf("15.76 26-way DST string index (each node holds one text position):\n");
      dShow(dt, text, 5); dFree(dt);
      /* 15.77/15.78: cut every suffix right after it becomes unique (Figure 15.20 style):
         sort the suffixes, a suffix needs max(lcp with left, lcp with right) + 1 characters */
      int ord[32]; for (int i = 0; i < ns; i++) ord[i] = starts[i];
      for (int i = 1; i < ns; i++) { int x = ord[i], j = i; while (j > 0 && strcmp(text + ord[j - 1], text + x) > 0) { ord[j] = ord[j - 1]; j--; } ord[j] = x; }
      char keys[32][64]; int kpos[32]; int same = 0;
      for (int i = 0; i < ns; i++) {
          int a = 0, b = 0;
          if (i > 0)      { const char *x = text + ord[i], *y = text + ord[i - 1]; while (x[a] && x[a] == y[a]) a++; }
          if (i + 1 < ns) { const char *x = text + ord[i], *y = text + ord[i + 1]; while (x[b] && x[b] == y[b]) b++; }
          int L = (a > b ? a : b) + 1; if (L > 60) L = 60;
          if (!text[ord[i] + L - 1]) same++;
          memcpy(keys[i], text + ord[i], L); keys[i][L] = 0; kpos[i] = ord[i];
      }
      rnodes = 0; rlink ri = NULL;
      for (int i = 0; i < ns; i++) ri = rIns(ri, keys[i], 0);
      printf("15.77 26-way trie string index: %ld nodes for %d positions; each leaf [prefix] -> position:\n", rnodes, ns);
      for (int i = 0; i < ns; i++) printf("       [%-9s] -> pos %2d  \"%.14s...\"\n", keys[i], kpos[i], text + kpos[i]);
      rShow(ri, buf, 0, 5);
      rFree(ri);
      /* insert in TEXT order (the way we scan the text); sorted order would make the TST a list */
      tlink ts = NULL; for (int i = 0; i < ns; i++) { int n = packWord(keys[i], dig); ts = tIns(ts, dig, 0, n); }
      int hs = tHeight(ts); tFree(ts);
      tnodes = 0; tlink ti = NULL;
      for (int p = 0; p < ns; p++) for (int i = 0; i < ns; i++) if (kpos[i] == starts[p]) { int n = packWord(keys[i], dig); ti = tIns(ti, dig, 0, n); }
      printf("15.78 TST string index for the same cut suffixes, inserted in text order (%ld nodes, height %d;\n"
             "      inserted in sorted order the height would be %d: the first-level BST degenerates):\n", tnodes, tHeight(ti), hs);
      tShow(ti, buf, 0, 5, 26); tFree(ti); (void)same; }

    rFree(rt);
    return 0;
}
