#!/bin/bash

EXEC=./man
OUTFILE="mandelbrot_results.csv"

# Initialize CSV
echo "threads,size,view,total_time_ms,workload_time_ms,overhead_ms" > $OUTFILE

THREADS=(1 2 4 8 16)
SIZES=(512 1024 2048)
VIEWS=(0)
SAVE_IMAGE_THREADS=8

# Terminal Header
printf "\n+----------+--------+------+---------------+------------------+--------------+\n"
printf "| %-8s | %-6s | %-4s | %-13s | %-16s | %-12s |\n" \
       "Threads" "Size" "View" "Total Time" "Workload Time" "Overhead"
printf "+----------+--------+------+---------------+------------------+--------------+\n"

# Run Benchmarks
for t in "${THREADS[@]}"; do
  for s in "${SIZES[@]}"; do
    for v in "${VIEWS[@]}"; do
      save=0
      if [ "$t" -eq "$SAVE_IMAGE_THREADS" ]; then save=1; fi
      OUTPUT_LINE=$($EXEC $t $s $v $save)

      # Print formatted output (first line with '|')
      echo "$OUTPUT_LINE" | grep '|' || true

      # Append to CSV (second line without '|')
      echo "$OUTPUT_LINE" | grep -v '|' >> $OUTFILE
    done
  done
done

# Terminal Footer
printf "+----------+--------+------+---------------+------------------+--------------+\n"
echo " Mandelbrot benchmarking complete. Results saved to $OUTFILE"
