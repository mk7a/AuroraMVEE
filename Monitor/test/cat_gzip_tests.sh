#!/bin/bash

# Cat
for file in random_*.txt; do
    echo "Testing $file..."

    echo "Baseline cat:"
    sudo perf stat -e task-clock -r 10 cat "$(pwd)/$file" 1>/dev/null
    echo "MVEE cat:"
    sudo perf stat -e task-clock -r 10 ../../cmake-build-debug/MVEE cat "$(pwd)/$file" 1>/dev/null
done

# Gzip
for file in random_*.txt; do
    echo "Testing $file..."

    echo "Baseline gzip:"
    sudo perf stat -e task-clock -r 10 gzip -k -f "$(pwd)/$file"
    echo "MVEE gzip:"
    sudo perf stat -e task-clock -r 10 ../../cmake-build-debug/MVEE gzip -k -f "$(pwd)/$file"
done

# Cleanup
rm ./*.gz