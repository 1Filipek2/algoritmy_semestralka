#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"

SIZES=(50 100 200 500)
REPEATS=5
SEED=42

if ! command -v /usr/bin/time >/dev/null 2>&1; then
  echo "Chyba: pre meranie casu a pamate je potrebny /usr/bin/time." >&2
  exit 1
fi

BUILD_DIR="$(mktemp -d)"
cleanup() {
  rm -rf "$BUILD_DIR"
}
trap cleanup EXIT

gcc -std=c11 -O2 "$ROOT_DIR/zimne_riesenie.c" -o "$BUILD_DIR/zimne_riesenie"
gcc -std=c11 -O2 "$ROOT_DIR/letne_riesenie.c" -lm -o "$BUILD_DIR/letne_riesenie"

RESULTS_CSV=""
for size in "${SIZES[@]}"; do
  for run in $(seq 1 "$REPEATS"); do
    metrics=$({ /usr/bin/time -f "%e %M" "$BUILD_DIR/zimne_riesenie" < <(python3 "$ROOT_DIR/generate_matrix_input.py" --size "$size" --seed "$SEED") > /dev/null; } 2>&1)
    read -r seconds rss_kb <<< "$metrics"
    RESULTS_CSV+="${size},zimne,${run},${seconds},${rss_kb}"$'\n'
  done

  for run in $(seq 1 "$REPEATS"); do
    metrics=$({ /usr/bin/time -f "%e %M" "$BUILD_DIR/letne_riesenie" < <(python3 "$ROOT_DIR/generate_matrix_input.py" --size "$size" --seed "$SEED") > /dev/null; } 2>&1)
    read -r seconds rss_kb <<< "$metrics"
    RESULTS_CSV+="${size},letne,${run},${seconds},${rss_kb}"$'\n'
  done
done

BENCHMARK_RESULTS="$RESULTS_CSV" BENCHMARK_REPEATS="$REPEATS" python3 - << 'PY'
import os
from collections import defaultdict

times = defaultdict(list)
mem = defaultdict(list)
sizes = ["50", "100", "200", "500"]
solutions = ["zimne", "letne"]
repeats = int(os.environ.get("BENCHMARK_REPEATS", "0"))

for line in os.environ.get("BENCHMARK_RESULTS", "").splitlines():
    if not line.strip():
        continue
    size, solution, run, seconds, rss_kb = line.split(",")
    times[(size, solution)].append(float(seconds))
    mem[(size, solution)].append(int(rss_kb))

def avg(values):
    return sum(values) / len(values) if values else float("nan")

header = (
    f"{'Velkost':<10} {'Program':<8} "
    f"{'Cas min (s)':>10} {'Cas priem (s)':>10} {'Cas max (s)':>10} "
    f"{'Pamat min (kb)':>14} {'Pamat priem (kb)':>16} {'Pamat max (kb)':>14}"
)
print(header)
print("-" * len(header))

for size in sizes:
    for solution in solutions:
        times_key = (size, solution)
        mem_key = (size, solution)
        t_values = times[times_key]
        m_values = mem[mem_key]

        avg_time = avg(t_values)
        min_time = min(t_values)
        max_time = max(t_values)
        avg_mem = avg(m_values)
        min_mem = min(m_values)
        max_mem = max(m_values)

        print(
            f"{(size + 'x' + size):<10} {solution:<8} "
            f"{min_time:10.3f} {avg_time:10.3f} {max_time:10.3f} "
            f"{min_mem:14d} {avg_mem:16.0f} {max_mem:14d}"
        )

    print(f"Testov pre rozmer {size}x{size}: {repeats * len(solutions)}")
PY


