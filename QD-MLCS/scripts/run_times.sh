#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"
BIN="$REPO_ROOT/bin/QD-MLCS"

if [[ ! -x "$BIN" ]]; then
  "$SCRIPT_DIR/build.sh"
fi

mkdir -p "$REPO_ROOT/artifacts/log"
"$BIN" "$REPO_ROOT/datas/instance4/dna_vl-1.seq" \
  < "$REPO_ROOT/inputs/times.in" \
  > "$REPO_ROOT/artifacts/log/times.log"
