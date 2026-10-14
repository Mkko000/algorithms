#!/bin/bash
set -e
ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"
mkdir -p bin
fail=0
while IFS= read -r -d '' f; do
  rel="${f#src/}"
  out="bin/${rel%.c}"
  mkdir -p "$(dirname "$out")"
  if gcc -std=c99 -Wall -Wextra -O2 "$f" -o "$out" -lm; then
    echo "OK  $rel"
  else
    echo "FAIL $rel"; fail=1
  fi
done < <(find src -name '*.c' -print0 | sort -z)
exit $fail
