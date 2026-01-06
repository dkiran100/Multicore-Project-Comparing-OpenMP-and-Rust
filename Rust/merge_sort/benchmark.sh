#!/bin/bash

PROGRAM="./target/release/merge_sort"
OUTPUT="mergesort_results_rust.csv"
THREADS=(1 2 4 8 16)
SIZES=(10 100 1000 10000 100000 1000000 10000000)

# CSV header
echo "size,threads,total_time,workload_time,overhead" > "$OUTPUT"

# Terminal table header
printf "\n+------------+----------+---------------+------------------+------------+\n"
printf "| %-10s | %-8s | %-13s | %-16s | %-10s |\n" \
       "Array Size" "Threads" "Total Time" "Workload Time" "Overhead"
printf "+------------+----------+---------------+------------------+------------+\n"

# Outer loop: sizes
for s in "${SIZES[@]}"; do
    # Inner loop: threads
    for t in "${THREADS[@]}"; do
        OUTPUT_LINE=$($PROGRAM "$t" "$s")

        # Print formatted row directly (the first line from Rust program)
        echo "$OUTPUT_LINE" | grep '|' || true

        # Append second line (CSV) to results.csv
        echo "$OUTPUT_LINE" | grep -v '|' >> "$OUTPUT"
    done
done

# Footer
printf "+------------+----------+---------------+------------------+------------+\n"
echo "Benchmarking complete. Results saved to $OUTPUT"
