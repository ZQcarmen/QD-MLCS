$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Split-Path -Parent $scriptDir

Push-Location $repoRoot
try {
  New-Item -ItemType Directory -Force "bin" | Out-Null

  g++ -std=c++17 -O2 `
    -Iinclude `
    src/main.cpp `
    src/point.cpp `
    src/hash.cpp `
    src/loader.cpp `
    src/utils.cpp `
    src/upper.cpp `
    -o bin/QD-MLCS.exe

  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }

  Write-Host "Built $(Join-Path $repoRoot 'bin\QD-MLCS.exe')"
}
finally {
  Pop-Location
}
