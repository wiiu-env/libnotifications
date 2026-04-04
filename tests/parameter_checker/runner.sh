#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

echo "========================================"
echo "Starting 2D Compilation Matrix"
echo "========================================"

# Optimization Levels Array
OPT_LEVELS=("-O0" "-O1" "-O2" "-O3" "-Os" "-Ofast" "-Og")

# C++ Standards
CPP_STANDARDS=("c++11" "gnu++11" "c++14" "gnu++14" "c++17" "gnu++17" "c++20" "gnu++20" "c++23" "gnu++23")

# C Standards
C_STANDARDS=("c99" "gnu99" "c11" "gnu11" "c17" "gnu17" "c23" "gnu23")

# ------------------------------------------------------------------
# 1. POSITIVE TESTS (Should Compile without errors or warnings)
# ------------------------------------------------------------------

for std in "${CPP_STANDARDS[@]}"; do
    for opt in "${OPT_LEVELS[@]}"; do
        echo -n "[PASS-CHECK] C++ Source with -std=$std | $opt... "
        make clean --no-print-directory > /dev/null 2>&1

        set +e # Turn off exit-on-error temporarily
        OUTPUT=$(make TEST_SRC=src_cpp USER_CXXFLAGS="-std=$std $opt" --no-print-directory 2>&1)
        EXIT_CODE=$?
        set -e # Turn it back on

        if [ $EXIT_CODE -ne 0 ]; then
            echo -e "\n  -> ERROR: Build failed!"
            echo "================ COMPILER OUTPUT ================"
            echo "$OUTPUT"
            echo "================================================="
            exit 1
        else
            echo "SUCCESS"
        fi
    done
done

for std in "${C_STANDARDS[@]}"; do
    for opt in "${OPT_LEVELS[@]}"; do
        echo -n "[PASS-CHECK] C Source with -std=$std | $opt... "
        make clean --no-print-directory > /dev/null 2>&1

        set +e # Turn off exit-on-error temporarily
        OUTPUT=$(make TEST_SRC=src_c USER_CFLAGS="-std=$std $opt" --no-print-directory 2>&1)
        EXIT_CODE=$?
        set -e # Turn it back on

        if [ $EXIT_CODE -ne 0 ]; then
            echo -e "\n  -> ERROR: Build failed!"
            echo "================ COMPILER OUTPUT ================"
            echo "$OUTPUT"
            echo "================================================="
            exit 1
        else
            echo "SUCCESS"
        fi
    done
done

# Re-enable exit on error
set -e

echo "========================================"
echo "All tests passed successfully!"
echo "========================================"