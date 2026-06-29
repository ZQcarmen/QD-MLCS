#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"
BIN="$REPO_ROOT/bin/QD-MLCS"

if [[ ! -x "$BIN" ]]; then
  "$SCRIPT_DIR/build.sh"
fi

for t in 1 2 3; do
  mkdir -p "$REPO_ROOT/artifacts/log_last/num$t"
  for num in 300 400 500 600 700 800 900 1000 1500 2000 5 10 20 50 100; do
    echo "num: $num time: $t"
    "$BIN" "$REPO_ROOT/datas/instance$num/dna_vl-1.seq" \
      < "$REPO_ROOT/inputs/90.in" \
      > "$REPO_ROOT/artifacts/log_last/num$t/$num.log"
  done
done
