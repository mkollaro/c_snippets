#!/usr/bin/env bats
#set -x

CMD="./tail"
EXPECTED=$BATS_TMPDIR"/expected.txt"
RESULT=$BATS_TMPDIR"/result.txt"
TEST_FILES=$BATS_TEST_DIRNAME"/files"
MAIN_FILE=$TEST_FILES"/tail_1to15.txt"

# can be found in tail.c, but in this case we don't include \n and \0
MAX_LINE_LENGTH=1024


@test "non-existent file" {
    run $CMD file_that_doesnt_exist.txt
    [ $status -eq 1 ]
}

@test "empty input" {
    FILE=$TEST_FILES"/empty_file.txt"
    $CMD $FILE
}

@test "unknown parameter" {
    run $CMD -g
    [ $status -eq 1 ]
    [[ "$output" =~ "Invalid parameter" ]]
}

@test "invalid params - two files specified" {
    run $CMD -6 $MAIN_FILE $MAIN_FILE
    [ $status -eq 1 ]
    [[ "$output" =~ "The FILE can be set only once" ]]
}

@test "invalid params - two limits" {
    run $CMD -6 $MAIN_FILE -7
    [ $status -eq 1 ]
    [[ "$output" =~ "Too many parameters" ]]
}

@test "invalid params - use '--n'" {
    run $CMD --6 $MAIN_FILE
    [ $status -eq 1 ]
    [[ "$output" =~ "Invalid parameter" ]]
}

@test "last two lines" {
    $CMD -2 $MAIN_FILE > $RESULT
    tail -2 $MAIN_FILE > $EXPECTED
    diff $EXPECTED $RESULT
}

@test "everything starting from second line" {
    $CMD +2 $MAIN_FILE > $RESULT
    tail -n +2 $MAIN_FILE > $EXPECTED
    diff $EXPECTED $RESULT
}

@test "everything starting from 20th line, in a 15 line long file" {
    $CMD +20 $MAIN_FILE > $RESULT
    tail -n +20 $MAIN_FILE > $EXPECTED
    diff $EXPECTED $RESULT
}

@test "all lines" {
    $CMD +0 $MAIN_FILE > $RESULT
    tail -n +0 $MAIN_FILE > $EXPECTED
    diff $EXPECTED $RESULT
}

@test "no lines" {
    $CMD -0 $MAIN_FILE > $RESULT
    tail -n -0 $MAIN_FILE > $EXPECTED
    diff $EXPECTED $RESULT
}

@test "without parameters" {
    $CMD $MAIN_FILE > $RESULT
    tail $MAIN_FILE > $EXPECTED
    diff $EXPECTED $RESULT
}

@test "big file" {
    FILE=$TEST_FILES"/book.txt"
    $CMD $FILE > $RESULT
    tail $FILE > $EXPECTED
    diff $EXPECTED $RESULT
}

@test "valgrind check" {
    run valgrind $CMD -2 $MAIN_FILE
    [ $status -eq 0 ]
    [[ "$output" =~ "no leaks are possible" ]]
    [[ "$output" =~ " 0 errors from 0 contexts" ]]
}

@test "valgrind check from line 2" {
    run valgrind $CMD +2 $MAIN_FILE
    [ $status -eq 0 ]
    [[ "$output" =~ "no leaks are possible" ]]
    [[ "$output" =~ " 0 errors from 0 contexts" ]]
}

@test "line with $MAX_LINE_LENGTH characters" {
    FILE=$TEST_FILES"/tail_1024chars.txt"
    $CMD $FILE > $RESULT
    tail $FILE > $EXPECTED
    diff $EXPECTED $RESULT
}

@test "line with $MAX_LINE_LENGTH + 1 characters" {
    FILE=$TEST_FILES"/tail_1025chars.txt"
    run valgrind $CMD $FILE
    [ $status -eq 1 ]
    [[ "$output" =~ "Line too long" ]]
    [[ "$output" =~ "no leaks are possible" ]]
    [[ "$output" =~ " 0 errors from 0 contexts" ]]
}
