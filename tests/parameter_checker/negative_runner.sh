#!/bin/bash

# Define Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "========================================"
echo "Starting Advanced 2D Compilation Matrix"
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
    OPT=$4            # e.g., -O0
    TYPE=$5           # CXX or C

    # Updated echo to show both the Standard and the Optimization Level
    echo -n "Checking $TEST_NAME [$STD | $OPT]... "

    # 1. POSITIVE VERIFICATION (Must Compile with -DMAKE_VALID)
    make clean --no-print-directory > /dev/null 2>&1

    # Construct Flags (Now including $OPT)
    if [ "$TYPE" == "CXX" ]; then
        FLAGS="-std=$STD $OPT -D$TEST_NAME -DMAKE_VALID"
        OUT=$(make TEST_SRC=$SRC_DIR USER_CXXFLAGS="$FLAGS" --no-print-directory 2>&1)
    else
        FLAGS="-std=$STD $OPT -D$TEST_NAME -DMAKE_VALID"
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
        FLAGS="-std=$STD $OPT -D$TEST_NAME"
        OUT=$(make TEST_SRC=$SRC_DIR USER_CXXFLAGS="$FLAGS" --no-print-directory 2>&1)
    else
        FLAGS="-std=$STD $OPT -D$TEST_NAME"
        OUT=$(make TEST_SRC=$SRC_DIR USER_CFLAGS="$FLAGS" --no-print-directory 2>&1)
    fi

    if [ $? -eq 0 ]; then
        echo -e "${RED}[ERROR]${NC}"
        echo "   -> Invalid code SUCCEEDED! Type check failed to catch error."
        if [ "$TYPE" == "CXX" ]; then
            echo "      make TEST_SRC=$SRC_DIR USER_CXXFLAGS=\"$FLAGS\" --no-print-directory 2>&1"
        else
            echo "      make TEST_SRC=$SRC_DIR USER_CFLAGS=\"$FLAGS\" --no-print-directory 2>&1"
        fi
        OVERALL_SUCCESS=false
        return
    fi

    echo -e "${GREEN}[OK]${NC}"
}

# ---------------------------------------------------------
# DEFINE MATRICES
# ---------------------------------------------------------
CPP_STANDARDS=("c++17" "gnu++17" "c++20" "gnu++20" "c++23" "gnu++23")
C_STANDARDS=("c11" "gnu11" "c17" "gnu17" "gnu18" "gnu2x" "c23" "gnu23")
C_SCALAR_CHECK_VERSIONS=("gnu99" "c99")

# Define Optimization Matrix
OPT_LEVELS=("-O0" "-O1" "-O2" "-O3" "-Os" "-Ofast" "-Og")


# ---------------------------------------------------------
# C++ NEGATIVE TESTS
# ---------------------------------------------------------
for std in "${CPP_STANDARDS[@]}"; do
    for opt in "${OPT_LEVELS[@]}"; do
        run_check "TEST_FAIL_COLOR"    "src_fail_cpp" "$std" "$opt" "CXX"
        run_check "TEST_FAIL_DURATION" "src_fail_cpp" "$std" "$opt" "CXX"
        run_check "TEST_FAIL_CALLBACK" "src_fail_cpp" "$std" "$opt" "CXX"
        run_check "TEST_FAIL_CONTEXT"  "src_fail_cpp" "$std" "$opt" "CXX"
        run_check "TEST_FAIL_BOOL"     "src_fail_cpp" "$std" "$opt" "CXX"
    done
done

# ---------------------------------------------------------
# C NEGATIVE TESTS
# ---------------------------------------------------------
# 1. Full Checks (Color, Callback, Context) - C11+ Only
for std in "${C_STANDARDS[@]}"; do
    for opt in "${OPT_LEVELS[@]}"; do
        run_check "TEST_FAIL_COLOR"    "src_fail_c" "$std" "$opt" "C"
        run_check "TEST_FAIL_CALLBACK" "src_fail_c" "$std" "$opt" "C"
        run_check "TEST_FAIL_CONTEXT"  "src_fail_c" "$std" "$opt" "C"
        run_check "TEST_FAIL_DURATION" "src_fail_c" "$std" "$opt" "C"
        run_check "TEST_FAIL_BOOL"     "src_fail_c" "$std" "$opt" "C"
    done
done

# 2. Scalar Checks (Float, Bool) - C99+
for std in "${C_SCALAR_CHECK_VERSIONS[@]}"; do
    for opt in "${OPT_LEVELS[@]}"; do
        run_check "TEST_FAIL_COLOR"    "src_fail_c" "$std" "$opt" "C"
        run_check "TEST_FAIL_DURATION" "src_fail_c" "$std" "$opt" "C"
        run_check "TEST_FAIL_BOOL"     "src_fail_c" "$std" "$opt" "C"
    done
done

# ---------------------------------------------------------
# FINAL STATUS
# ---------------------------------------------------------
if [ "$OVERALL_SUCCESS" = true ]; then
    echo "========================================"
    echo -e "${GREEN}All checks passed successfully across all optimizations!${NC}"
    echo "========================================"
    exit 0
else
    echo "========================================"
    echo -e "${RED}Some checks failed.${NC}"
    echo "========================================"
    exit 1
fi