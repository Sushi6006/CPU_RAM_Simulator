#!/bin/bash

SCRIPT_COUNT=$(find -name "*.sh"  | wc -l)
if [ $SCRIPT_COUNT -gt 0 ]; then
    echo -e "Found $SCRIPT_COUNT scripts. Excellent! Removing executable flags\n"
    find -name "*.sh"  -exec chmod -x {} \;
fi

DIRTY_COUNT=$(find . -regex '.*\.\(o|so|a\)' | wc -l)
EXE_COUNT=$(find .  -type f | wc -l)

if [ $DIRTY_COUNT -gt 0 ]; then
    echo "$DIRTY_COUNT dirty files found. Please do not commit object files"
    echo "Dirty files are:"
    find . -regex '.*\.\(o|so|a\)'
    echo -e "-----------------------------------\n"
fi

if [ $EXE_COUNT -gt 0 ]; then
    echo "$EXE_COUNT executable files found. Please do not commit executables"
    echo "Executable files are:"
    find .  -type f
    echo -e "-----------------------------------\n"
fi

echo "running make"
make

if [ -f ./scheduler ]; then
    echo "OK -- ./scheduler found"
else
    echo "BAD -- no executable named scheduler found, please fix"
    exit 1
fi

echo ""

# execute against basic tests
CASES_DIR="tests/cases"
SCENARIOS_DIR="tests/scenarios"


FAIL=0

i=1
for f in $(ls ${CASES_DIR}/*.in); do
    echo "Test #${i} -- running $(cat ${CASES_DIR}/testcase${i}.in)"
    $(cat ${CASES_DIR}/testcase${i}.in) 2>/tmp/stderr >/tmp/stdout
    EXIT_CODE=$?
    if [ $EXIT_CODE -ne 0 ]; then
        echo "Test #${i} WARNING: did not exit normally -- exit code: $EXIT_CODE"
        echo "========== STDOUT ============"
        cat /tmp/stdout;
        echo "=============================="
        echo "---------- STDERR ------------"
        cat /tmp/stderr;
        echo "------------------------------"
    fi

    diff ${CASES_DIR}/testcase${i}.out /tmp/stdout &> /tmp/test-output
    if [ $? -ne 0 ]; then
        echo -e "Test #${i} FAILED"
        echo "========== DIFF OUTPUT ============"
        cat /tmp/test-output
        echo "==================================="
        ((FAIL++))
        echo "========== STDOUT ============"
        cat /tmp/stdout; rm /tmp/stdout
        echo "=============================="
        echo "---------- STDERR ------------"
        cat /tmp/stderr; rm /tmp/stderr
        echo -e "------------------------------\n"
    else
        echo -e "Test #${i} PASSED\n"
    fi
    ((i++))
    rm -f /tmp/test-output /tmp/stdout /tmp/stderr
done


# run benchmark
if [ -f benchmark-cs.txt ]; then
    echo "Running \`./scheduler -f benchmark-cs.txt -a rr -s 100 -m v -q 10\`"
    ./scheduler -f benchmark-cs.txt -a rr -s 100 -m v -q 10 2>/tmp/stderr >/tmp/stdout
    echo "========== STDOUT ============"
    cat /tmp/stdout; rm /tmp/stdout
    echo "=============================="
    echo "---------- STDERR ------------"
    cat /tmp/stderr; rm /tmp/stderr
    echo "------------------------------"

    echo "Running \`./scheduler -f benchmark-cs.txt -a ff -s 100 -m v\`"
    ./scheduler -f benchmark-cs.txt -a ff -s 100 -m v 2>/tmp/stderr >/tmp/stdout
    echo "========== STDOUT ============"
    cat /tmp/stdout; rm /tmp/stdout
    echo "=============================="
    echo "---------- STDERR ------------"
    cat /tmp/stderr; rm /tmp/stderr
    echo "------------------------------"

    echo "Running \`./scheduler -f benchmark-cs.txt -a cs -s 100 -m v\`"
    ./scheduler -f benchmark-cs.txt -a cs -s 100 -m v 2>/tmp/stderr >/tmp/stdout
    echo "========== STDOUT ============"
    cat /tmp/stdout; rm /tmp/stdout
    echo "=============================="
    echo "---------- STDERR ------------"
    cat /tmp/stderr; rm /tmp/stderr
    echo "------------------------------"
    echo ""
else
    echo "benchmark-cs.txt not found, skipping benchmark-cs"
fi

if [ -f benchmark-cm.txt ]; then
    echo "Running \`./scheduler -f benchmark-cm.txt -a rr -s 100 -m v -q 10\`"
    ./scheduler -f benchmark-cm.txt -a rr -s 100 -m v -q 10 2>/tmp/stderr >/tmp/stdout
    echo "========== STDOUT ============"
    cat /tmp/stdout; rm /tmp/stdout
    echo "=============================="
    echo "---------- STDERR ------------"
    cat /tmp/stderr; rm /tmp/stderr
    echo "------------------------------"

    echo "Running \`./scheduler -f benchmark-cm.txt -a rr -s 100 -m p -q 10\`"
    ./scheduler -f benchmark-cm.txt -a rr -s 100 -m p -q 10 2>/tmp/stderr >/tmp/stdout
    echo "========== STDOUT ============"
    cat /tmp/stdout; rm /tmp/stdout
    echo "=============================="
    echo "---------- STDERR ------------"
    cat /tmp/stderr; rm /tmp/stderr
    echo "------------------------------"

    echo "Running \`./scheduler -f benchmark-cm.txt -a rr -s 100 -m cm -q 10\`"
    ./scheduler -f benchmark-cm.txt -a rr -s 100 -m cm -q 10 2>/tmp/stderr >/tmp/stdout
    echo "========== STDOUT ============"
    cat /tmp/stdout; rm /tmp/stdout
    echo "=============================="
    echo "---------- STDERR ------------"
    cat /tmp/stderr; rm /tmp/stderr
    echo "------------------------------"
    echo ""
else
    echo "benchmark-cm.txt not found, skipping benchmark-cm"
fi


if [ $FAIL -gt 0 ]; then
    exit 1
fi