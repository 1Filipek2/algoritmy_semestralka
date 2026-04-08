# Semester Project

Refactored matrix multiplication assignment from winter with test scripts for benchmarking time and memory usage.

## What it does

- Multiplies two matrices
- Computes row sums of the result
- Finds rows with maximum sum

## How to run

```bash
mkdir cmake-build-debug && cd cmake-build-debug
cmake .. && make
./letne_riesenie
```

Or with generated input and benchmarks:

```bash
python3 generate_matrix_input.py --size 100 --seed 42 | ./cmake-build-debug/letne_riesenie
./run_benchmarks.sh
```

