#!/bin/bash

# Output CSV file
CSV_FILE="floyd_results.csv"
echo "nodes,threads,density,total_time,workload_time,overhead" > "$CSV_FILE"

echo ""
echo "Running Floyd-Warshall Benchmarks..."
printf "+--------+---------+---------+--------------+----------------+------------+\n"
printf "| %-6s | %-7s | %-7s | %-12s | %-14s | %-10s |\n" "Nodes" "Threads" "Density" "Total Time" "Workload Time" "Overhead"
printf "+--------+---------+---------+--------------+----------------+------------+\n"

NODE_SIZES=(100 200 300 400 500 1000)
THREAD_COUNTS=(1 2 4 8 16)
DENSITIES=(0.01 0.1 0.25 0.5 0.75 1.0)

for size in "${NODE_SIZES[@]}"; do
    for threads in "${THREAD_COUNTS[@]}"; do
        for density in "${DENSITIES[@]}"; do
            ./floyd_warshall "$size" "$threads" "$density"
        done
    done
done

printf "+--------+---------+---------+--------------+----------------+------------+\n"
echo "Benchmarking complete. Results saved to $CSV_FILE"
