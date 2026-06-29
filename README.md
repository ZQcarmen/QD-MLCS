# QD-MLCS

Standalone package for the `QD-MLCS` implementation used in the paper experiments.

## What This Version Computes

`QD-MLCS` computes the **MLCS length** with:

- incremental prefix expansion
- successor-table based state transition
- pairwise-DP upper bounds
- heuristic lower-bound estimation

This package focuses on reproducing the `QD-MLCS` code path and benchmark runs, and it can optionally backtrack one MLCS path or enumerate all MLCS strings through command-line flags.

## Repository Layout

```text
QD-MLCS/
├─ include/       # headers
├─ src/           # core C++ sources
├─ scripts/       # build and experiment scripts
├─ inputs/        # incremental-length schedules
├─ datas/         # benchmark instances copied from the original project
├─ examples/      # tiny smoke-test input
└─ bin/           # build output (generated)
```

## Build

### Linux

```bash
bash ./scripts/build.sh
```

## Quick Run

### Tiny example

```bash
bash ./scripts/build.sh
./bin/QD-MLCS ./examples/sample.seq < ./examples/sample.in
```

To print one backtracked MLCS path, add `--trace`:

```bash
./bin/QD-MLCS --trace ./examples/sample.seq < ./examples/sample.in
```

To print all distinct MLCS strings, add `--all-mlcs`:

```bash
./bin/QD-MLCS --all-mlcs ./examples/sample.seq < ./examples/sample.in
```

The `scripts/` directory also includes benchmark helpers used for paper experiments; when run, they write outputs under `artifacts/`.

## Input File Format

Sequence files use the following format:

```text
# comment
seqnum : 4
alphabets : ACGT
length : 100

sequences%start
ACGT...
TGCA...
...
%end
```

The incremental schedule files in `inputs/` are read from stdin:

```text
25
25
25
25
0
```

Each positive integer increases the working prefix length by that amount. `0` terminates the run.

## Notes Before Publishing

- Add your preferred open-source `LICENSE` file before pushing the repository to GitHub.
- The benchmark directories under `datas/` are included to keep the package self-contained for the paper.
