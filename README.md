# Semester Project

Refactored matrix multiplication assignment from winter with support for two algorithms and a built-in benchmark.

## What it does

- Multiplies two matrices and computes row sums of the result
- Finds rows with maximum sum
- Supports standard O(n³) and Strassen O(n^2.807) multiplication
- Built-in benchmark mode measuring time and memory for both algorithms

## How to run

```bash
mkdir cmake-build-debug && cd cmake-build-debug
cmake .. && make
./letne_riesenie
```

## Modes

1. Manual — user provides matrix dimensions and values
2. Auto — user provides dimensions, values are randomly generated
3. Benchmark — automated time and memory test across multiple matrix sizes
