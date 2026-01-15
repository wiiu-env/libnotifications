#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

echo "========================================"
echo "Starting Compilation Matrix"
echo "========================================"

# ------------------------------------------------------------------
# 1. POSITIVE TESTS (Should Compile)
# ------------------------------------------------------------------

# C++ Standards
CPP_STANDARDS=("gnu++11" "gnu++14" "gnu++17" "gnu++2a")

for std in "${CPP_STANDARDS[@]}"; do
    echo "[PASS-CHECK] C++ Source with -std=$std"
    make clean --no-print-directory
    make TEST_SRC=src_cpp USER_CXXFLAGS="-std=$std" --no-print-directory
    echo "  -> SUCCESS"
done

# C Standards
C_STANDARDS=("gnu99" "gnu11" "gnu17")

for std in "${C_STANDARDS[@]}"; do
    echo "[PASS-CHECK] C Source with -std=$std"
    make clean --no-print-directory
    make TEST_SRC=src_c USER_CFLAGS="-std=$std" --no-print-directory
    echo "  -> SUCCESS"
done

# ------------------------------------------------------------------
# 2. NEGATIVE TESTS (Should FAIL to Compile)
# ------------------------------------------------------------------
echo "========================================"
echo "Starting Negative Tests (Expect Errors)"
echo "========================================"

FAIL_CASES=(
    "src_fail/fail_color"
    "src_fail/fail_duration"
    "src_fail/fail_callback"
    "src_fail/fail_bool"
)

# Temporarily turn off 'set -e' so we can capture the failure
set +e

for test_dir in "${FAIL_CASES[@]}"; do
    echo "[FAIL-CHECK] Testing $test_dir (Should fail build)"
    
    make clean --no-print-directory > /dev/null 2>&1
    
    # Run Make and capture output to prevent cluttering logs, 
    # unless you want to see the specific error.
    OUTPUT=$(make TEST_SRC=$test_dir --no-print-directory 2>&1)
    EXIT_CODE=$?

    if [ $EXIT_CODE -eq 0 ]; then
        echo "  -> ERROR: Build succeeded but SHOULD HAVE FAILED!"
        echo "     (Type checks failed to catch the invalid parameter)"
        exit 1
    else
        echo "  -> SUCCESS: Build failed as expected."
        # Optional: Check if the output contains the specific warning message?
        # echo "$OUTPUT" | grep "NotificationModule_SetDefaultValue expects"
    fi
done

# Re-enable exit on error
set -e

echo "========================================"
echo "All tests passed successfully!"
echo "========================================"