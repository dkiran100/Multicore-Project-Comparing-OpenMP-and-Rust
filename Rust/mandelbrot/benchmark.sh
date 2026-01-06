#!/bin/bash

EXEC=target/release/mandelbrot
OUTFILE="mandelbrot_results_rust.csv"

echo "threads,size,view,total_time_ms,workload_time_ms,overhead_ms" > $OUTFILE

THREADS=(1 2 4 8)
SIZES=(512 1024 2048)
VIEWS=(0)

# Table header
printf "\n+----------+--------+------+---------------+------------------+--------------+\n"
printf "| %-8s | %-6s | %-4s | %-13s | %-16s | %-12s |\n" \
       "Threads" "Size" "View" "Total Time" "Workload Time" "Overhead"
printf "+----------+--------+------+---------------+------------------+--------------+\n"

for t in "${THREADS[@]}"; do
  for s in "${SIZES[@]}"; do
    for v in "${VIEWS[@]}"; do
      save=0
      if [ "$t" -eq 8 ]; then save=1; fi

      OUTPUT_LINE=$($EXEC $t $s $v $save)

      echo "$OUTPUT_LINE" | grep '|' || true
      echo "$OUTPUT_LINE" | grep -v '|' >> $OUTFILE
    done
  done
done

# Table footer
printf "+----------+--------+------+---------------+------------------+--------------+\n"
echo "Mandelbrot benchmarking complete. Results saved to $OUTFILE"
