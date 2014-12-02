#!/usr/bin/env bats

EXPECTED=$BATS_TMPDIR"/expected.txt"
RESULT=$BATS_TMPDIR"/result.txt"
TEST_FILES=$BATS_TEST_DIRNAME"/files"

# constant that can be found in `wordcount.c` (lowered by 1, since it usually
# counts the ending '\0' character too
MAX_WORD_LENGTH=100

# use standard UNIX tools to get a similar result as the `wordcount` program
function wordcount_unix_tools {
    sed 's/\s\+/\n/g' $1 |  `# print each word on a separate line` \
        sed '/^$/d' |       `# remove blank lines` \
        sort | uniq -c |    `# get number of repetitions of each word` \
        sed 's/^\s*//g' |   `# remove whitespace from the start of lines` \
        sort -n             `# numeric sort (most common words to bottom)` \
        > $EXPECTED 
}

@test "empty input" {
    echo "" | ./wordcount
}

@test "only whitespace in input" {
    FILE=$TEST_FILES"/wordcount_only_whitespace.txt"
    run bash -c "cat $FILE | ./wordcount"
    [ $status -eq 0 ]
    [ "$output" = "" ]
}

@test "one word per line, no repetitions" {
    FILE=$TEST_FILES"/wordcount_simple.txt"
    wordcount_unix_tools $FILE
    cat $FILE | ./wordcount | sort -n > $RESULT
    diff $EXPECTED $RESULT
}

@test "random words mixed with lots of whitespace" {
    FILE=$TEST_FILES"/wordcount_simple2.txt"
    wordcount_unix_tools $FILE
    cat $FILE | ./wordcount | sort -n > $RESULT
    diff $EXPECTED $RESULT
}

@test "count repetitions in book" {
    FILE=$TEST_FILES"/book.txt"
    wordcount_unix_tools $FILE
    cat $FILE | ./wordcount | sort -n > $RESULT
    diff $EXPECTED $RESULT
}

@test "big file and valgrind" {
    FILE=$TEST_FILES"/book.txt"
    wordcount_unix_tools $FILE
    cat $FILE | ./wordcount | sort -n > $RESULT
    run bash -c "cat $FILE | valgrind ./wordcount | sort -n > $RESULT"
    diff $EXPECTED $RESULT
    [ $status -eq 0 ]
    [[ "$output" =~ "no leaks are possible" ]]
    [[ "$output" =~ " 0 errors from 0 contexts" ]]
}

@test "words exactly $MAX_WORD_LENGTH characters long" {
    FILE=$TEST_FILES"/wordcount_100chars.txt"
    wordcount_unix_tools $FILE
    cat $FILE | ./wordcount | sort -n > $RESULT
    diff $EXPECTED $RESULT
}

@test "words longer than $MAX_WORD_LENGTH characters" {
    FILE=$TEST_FILES"/wordcount_long_words.txt"
    EXPECTED=$TEST_FILES"/wordcount_long_words_result.txt"
    run bash -c "cat $FILE | valgrind ./wordcount | sort -n > $RESULT"
    diff $EXPECTED $RESULT
    [ $status -eq 0 ]
    [[ "$output" =~ "Warning: Some words may be shortened" ]]
    [[ "$output" =~ "no leaks are possible" ]]
    [[ "$output" =~ " 0 errors from 0 contexts" ]]
}
