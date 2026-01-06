#!/bin/bash
set -e

CSV_FILE="floyd_results_rust.csv"
EXEC="./target/release/floyd"

NODES_LIST=(100 200 300 400 500 1000)
THREADS_LIST=(1 2 4 8 16)
DENSITY_LIST=(0.01 0.1 0.25 0.5 0.75 1.0)

# CSV Header
echo "Nodes,Threads,Density,TotalTime,WorkTime,Overhead" > $CSV_FILE

# Table Header
printf "\n+--------+---------+---------+---------------+------------------+------------+\n"
printf "| %-6s | %-7s | %-7s | %-13s | %-16s | %-10s |\n" \
       "Nodes" "Threads" "Density" "Total Time" "Workload Time" "Overhead"
printf "+--------+---------+---------+---------------+------------------+------------+\n"

for nodes in "${NODES_LIST[@]}"; do
    for threads in "${THREADS_LIST[@]}"; do
        for density in "${DENSITY_LIST[@]}"; do

            OUTPUT_LINE=$($EXEC "$nodes" "$density" "$threads")

            # Print table row (line with '|')
            echo "$OUTPUT_LINE" | grep '|' || true

            # Save CSV line (line without '|')
            echo "$OUTPUT_LINE" | grep -v '|' >> "$CSV_FILE"

        done
    done
done

# Table footer
printf "+--------+---------+---------+---------------+------------------+------------+\n"
echo " Floyd-Warshall benchmarking complete. Results saved to $CSV_FILE"
