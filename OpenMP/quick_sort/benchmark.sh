#!/bin/bash

# Output CSV file
CSV_FILE="quick_sort_results.csv"
echo "Threads,ArraySize,TotalTime,WorkloadTime,Overhead" > $CSV_FILE

# Array sizes and thread counts
ARRAY_SIZES=(10 100 1000 10000 100000 1000000 10000000)
THREAD_COUNTS=(1 2 4 8 16)
EXEC="./quick_sort"

# Print table header once
printf "\n+-------------+----------+---------------+---------------+---------------+\n"
printf "| %-11s | %-8s | %-13s | %-13s | %-13s |\n" "Array Size" "Threads" "Total Time" "Workload Time" "Overhead"
printf "+-------------+----------+---------------+---------------+---------------+\n"

# Run benchmarks
for SIZE in "${ARRAY_SIZES[@]}"; do
    for THREADS in "${THREAD_COUNTS[@]}"; do
        $EXEC $SIZE $THREADS
    done
done

# Footer
printf "+-------------+----------+---------------+---------------+---------------+\n"
echo "Benchmarking complete. Results saved to $CSV_FILE"
