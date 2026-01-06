
# Rust vs OpenMP Benchmarking
This project compares parallel performance between **Rust (with Rayon)** and **C (with OpenMP)** implementations of four classical algorithms as part of the final project for *CSCI-GA 3033: Multicore Processors & Architecture*.

## 📁 Project Structure

```
Project/
├── Rust/
│   ├── floyd/               # Floyd-Warshall in Rust
│   ├── merge_sort/          # Merge Sort in Rust
│   ├── quick_sort/          # Quick Sort in Rust
│   └── mandelbrot/          # Mandelbrot Set in Rust
│       └── *.csv            # Rust benchmark results
├── OpenMP/
│   ├── floyd/               # Floyd-Warshall in C (OpenMP)
│   ├── merge_sort/          # Merge Sort in C (OpenMP)
│   ├── quick_sort/          # Quick Sort in C (OpenMP)
│   └── mandelbrot/          # Mandelbrot Set in C (OpenMP)
│       └── *.csv            # OpenMP benchmark results
```

Each subdirectory contains:
- `main.rs` or `.c` source files
- `benchmark.sh` scripts for automated execution
- Output `.csv` results (e.g., execution time, threads, overhead)

---

## 🚀 Running the Benchmarks

### ✅ Rust (via Rayon)
Each Rust benchmark is a standalone Cargo project.

```bash
cd Project/Rust/<benchmark>
cargo build --release
chmod +x benchmark.sh
./benchmark
```

Example:
```bash
cd Project/Rust/merge_sort
cargo run --release
chmod +x benchmark.sh
./benchmark.sh
```

### ✅ C (via OpenMP)
Each OpenMP benchmark contains a C file with a bash script for compilation and execution.

```bash
cd Project/OpenMP/<benchmark>
gcc -Wall -std=c99 -fopenmp -o <benchmark> <benchmark>.c
chmod +x benchmark.sh
./benchmark.sh
```

You can also compile manually:
```bash
gcc -Wall -std=c99 -fopenmp -o merge_sort merge_sort.c

```

---

## 📊 Benchmark Summary

| Benchmark       | Description                             |
|------------------|-----------------------------------------|
| Merge Sort       | Recursive divide-and-conquer sort       |
| Quick Sort       | Recursive partitioning sort             |
| Floyd-Warshall   | All-pairs shortest path (graph-based)   |
| Mandelbrot Set   | Parallel pixel evaluation for fractals  |

CSV outputs include columns like:
- Threads
- Problem size
- Execution time
- Overhead
- Speedup

These can be used for analysis and plotting with tools like `matplotlib` or `pandas`.

---

## 📎 Notes

- Some directories contain image output from Mandelbrot (`.ppm` format).
- `.DS_Store` and `target/` files are OS and build artifacts and can be ignored.

___
