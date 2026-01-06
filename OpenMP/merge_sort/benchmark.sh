#!/bin/bash

# Compile the merge sort program
gcc -Wall -std=c99 -fopenmp -o merge_sort merge_sort.c
if [ $? -ne 0 ]; then
    echo "Compilation failed. Please check your code."
    exit 1
fi

# Initialize results file
echo "array_size,threads,total_time,workload_time,overhead" > merge_sort_results.csv



ARRAY_SIZES=(10 100 1000 10000 100000 1000000 10000000)
THREAD_COUNTS=(1 2 4 8 16)

# Print table header once
echo ""
echo "+------------+----------+---------------+---------------+---------------+"
printf "| %-10s | %-8s | %-13s | %-13s | %-13s |\n" "Array Size" "Threads" "Total Time" "Workload Time" "Overhead"
echo "+------------+----------+---------------+---------------+---------------+"


# Run benchmarks
for size in "${ARRAY_SIZES[@]}"; do
    for threads in "${THREAD_COUNTS[@]}"; do
        ./merge_sort "$threads" "$size"
    done
done

echo "+------------+----------+---------------+---------------+---------------+"

echo "Benchmark complete. Results saved to merge_sort_results.csv"
