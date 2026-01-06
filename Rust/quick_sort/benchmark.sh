#!/bin/bash

PROGRAM="./target/release/quick_sort"
THREADS=(1 2 4 8 16)
SIZES=(10 100 1000 10000 100000 1000000 10000000)

OUTPUT="quicksort_results_rust.csv"
echo "size,threads,total_time,workload_time,overhead" > "$OUTPUT"

# Print header
printf "\n+------------+----------+---------------+------------------+------------+\n"
printf "| %-10s | %-8s | %-13s | %-16s | %-10s |\n" \
       "Array Size" "Threads" "Total Time" "Workload Time" "Overhead"
printf "+------------+----------+---------------+------------------+------------+\n"

for s in "${SIZES[@]}"; do
    for t in "${THREADS[@]}"; do
        OUTPUT_LINE=$($PROGRAM "$t" "$s")

        # Table line
        echo "$OUTPUT_LINE" | grep '|' || true

        # CSV line
        echo "$OUTPUT_LINE" | grep -v '|' >> "$OUTPUT"
    done
done

printf "+------------+----------+---------------+------------------+------------+\n"
echo " Quicksort benchmarking complete. Results saved to $OUTPUT"
