#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"

cd "$REPO_ROOT"
mkdir -p bin

g++ -std=c++17 -O2 \
  -Iinclude \
  src/main.cpp \
  src/point.cpp \
  src/hash.cpp \
  src/loader.cpp \
  src/utils.cpp \
  src/upper.cpp \
  -o bin/QD-MLCS

echo "Built $REPO_ROOT/bin/QD-MLCS"
