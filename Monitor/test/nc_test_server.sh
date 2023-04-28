#!/bin/bash


for file in random_*.txt; do
  echo "Testing $file..."

  echo "Baseline nc:"
  sudo perf stat -e task-clock -r 10 bash -c "<$file nc -l -p 4444"

  echo "MVEE nc:"
  sudo perf stat -e task-clock -r 10 bash -c "<$file ../../cmake-build-debug/MVEE nc -l -p 4444"
done
