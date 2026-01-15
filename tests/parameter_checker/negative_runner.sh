#!/bin/bash

# Define Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "========================================"
echo "Starting Advanced Compilation Matrix"
echo "========================================"

# Track overall status
OVERALL_SUCCESS=true

# ---------------------------------------------------------
# Helper Function: Run a specific Negative Test
# ---------------------------------------------------------
run_check() {
    TEST_NAME=$1      # e.g., TEST_FAIL_COLOR
    SRC_DIR=$2        # e.g., src_fail_cpp
    STD=$3            # e.g., gnu++17
    TYPE=$4           # CXX or C

    echo -n "Checking $TEST_NAME [$STD]... "

    # 1. POSITIVE VERIFICATION (Must Compile with -DMAKE_VALID)
    make clean --no-print-directory > /dev/null 2>&1
    
    # Construct Flags
    if [ "$TYPE" == "CXX" ]; then
        FLAGS="-std=$STD -D$TEST_NAME -DMAKE_VALID"
        OUT=$(make TEST_SRC=$SRC_DIR USER_CXXFLAGS="$FLAGS" --no-print-directory 2>&1)
    else
        FLAGS="-std=$STD -D$TEST_NAME -DMAKE_VALID"
        OUT=$(make TEST_SRC=$SRC_DIR USER_CFLAGS="$FLAGS" --no-print-directory 2>&1)
    fi
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}[ERROR]${NC}"
        echo "   -> Valid code failed to build! Setup is broken."
        echo "$OUT"
        OVERALL_SUCCESS=false
        return
    fi

    # 2. NEGATIVE VERIFICATION (Must FAIL without -DMAKE_VALID)
    make clean --no-print-directory > /dev/null 2>&1
    
    if [ "$TYPE" == "CXX" ]; then
        FLAGS="-std=$STD -D$TEST_NAME"
        OUT=$(make TEST_SRC=$SRC_DIR USER_CXXFLAGS="$FLAGS" --no-print-directory 2>&1)
    else
        FLAGS="-std=$STD -D$TEST_NAME"
        OUT=$(make TEST_SRC=$SRC_DIR USER_CFLAGS="$FLAGS" --no-print-directory 2>&1)
    fi

    if [ $? -eq 0 ]; then
        echo -e "${RED}[ERROR]${NC}"
        echo "   -> Invalid code SUCCEEDED! Type check failed to catch error."
		echo "make TEST_SRC=$SRC_DIR USER_CFLAGS="$FLAGS" --no-print-directory 2>&1"
        OVERALL_SUCCESS=false
        return
    fi

    echo -e "${GREEN}[OK]${NC}"
}

# ---------------------------------------------------------
# C++ NEGATIVE TESTS
# ---------------------------------------------------------
# Note: C++ Type checks are only active on C++17 and newer.
CPP_FULL_CHECK_VERSIONS=("gnu++17" "gnu++20" "gnu++23")

for std in "${CPP_FULL_CHECK_VERSIONS[@]}"; do
    run_check "TEST_FAIL_COLOR"    "src_fail_cpp" "$std" "CXX"
    run_check "TEST_FAIL_DURATION" "src_fail_cpp" "$std" "CXX"
    run_check "TEST_FAIL_CALLBACK" "src_fail_cpp" "$std" "CXX"
    run_check "TEST_FAIL_CONTEXT"  "src_fail_cpp" "$std" "CXX"
    run_check "TEST_FAIL_BOOL"     "src_fail_cpp" "$std" "CXX"
done

# ---------------------------------------------------------
# C NEGATIVE TESTS
# ---------------------------------------------------------
# C11 and newer have support for _Generic (Full Checks)
C_FULL_CHECK_VERSIONS=("gnu11" "gnu17" "gnu18" "gnu2x")
# C99 only supports built-in compatibility checks (Scalar only: float, bool)
C_SCALAR_CHECK_VERSIONS=("gnu99")

# 1. Full Checks (Color, Callback, Context) - C11+ Only
for std in "${C_FULL_CHECK_VERSIONS[@]}"; do
    run_check "TEST_FAIL_COLOR"    "src_fail_c" "$std" "C"
    run_check "TEST_FAIL_CALLBACK" "src_fail_c" "$std" "C"
    run_check "TEST_FAIL_CONTEXT"  "src_fail_c" "$std" "C"
	run_check "TEST_FAIL_DURATION" "src_fail_c" "$std" "C"
    run_check "TEST_FAIL_BOOL"     "src_fail_c" "$std" "C"
done

# 2. Scalar Checks (Float, Bool) - C99+
for std in "${C_SCALAR_CHECK_VERSIONS[@]}"; do
    run_check "TEST_FAIL_COLOR"    "src_fail_c" "$std" "C"
    run_check "TEST_FAIL_DURATION" "src_fail_c" "$std" "C"
    run_check "TEST_FAIL_BOOL"     "src_fail_c" "$std" "C"
done

# ---------------------------------------------------------
# FINAL STATUS
# ---------------------------------------------------------
if [ "$OVERALL_SUCCESS" = true ]; then
    echo "========================================"
    echo -e "${GREEN}All checks passed successfully!${NC}"
    echo "========================================"
    exit 0
else
    echo "========================================"
    echo -e "${RED}Some checks failed.${NC}"
    echo "========================================"
    exit 1
fi