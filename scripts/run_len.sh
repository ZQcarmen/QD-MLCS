#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"
BIN="$REPO_ROOT/bin/QD-MLCS"

if [[ ! -x "$BIN" ]]; then
  "$SCRIPT_DIR/build.sh"
fi

for t in 1 2 3; do
  mkdir -p "$REPO_ROOT/artifacts/log_last/len$t"
  l=100
  for i in {1..10}; do
    len=$(( l * i ))
    echo "len: $len time $t"
    "$BIN" "$REPO_ROOT/datas/instance4/dna_vl-1.seq" \
      < "$REPO_ROOT/inputs/$len.in" \
      > "$REPO_ROOT/artifacts/log_last/len$t/$len.log"
  done
done
