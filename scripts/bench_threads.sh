#!/bin/bash
# Renders each scene at 1920x1080 with 1, 2, 4, 8 and 12 threads and prints the
# best of 3 render times. Usage: scripts/bench_threads.sh [scene ...]
BIN=./build/Bin/Assignment2
SCENES=${@:-dragon buddha bunny}
for s in $SCENES; do
  for t in 1 2 4 8 12; do
    best=$(for r in 1 2 3; do
      $BIN --in $s.ray --width 1920 --height 1080 --threads $t 2>/dev/null |
        awk '/Ray-traced/ {print $2}'
    done | sort -g | head -1)
    echo "$s,$t,$best"
  done
done
