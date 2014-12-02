/* vim: tabstop=4 shiftwidth=4 expandtab
 *
 * Similar to the `tail` program in UNIX. Print the last n (default is 10)
 * lines of a file or standard input.
 *
 * Copyright 2009 Martina Kollarova
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "debug.h"

#define PLUS 1
#define MINUS 0

// Simplified implementation - ignore the extra characters on a line longer
// than this
// 1024 characters + '\n' + '\0'
#define MAX_LINE 1026

typedef struct params {
    unsigned long lines;
    bool plus_minus;  // which mode is used
    char *filename;
} params_t;

params_t get_params(int argc, char *argv[]);

/* print last X lines */
int tail_minus(FILE *file, unsigned long int lines);

/* print lines starting from line number X */
void tail_plus(FILE *file, unsigned long int line);

void free_2d_array(char **array[], int size);

void print_help();

/*****************************************************************************/
int main(int argc, char *argv[]) {
    params_t params = get_params(argc, argv);
    FILE *input = NULL;
    if(params.filename == NULL) input = stdin;
    else {
        input = fopen(params.filename, "r");
        check(input, "Can't open file '%s'", params.filename);
    }
    if (params.plus_minus == MINUS) {
        int res = tail_minus(input, params.lines);
        if(res != 0) goto error;
    }
    else {
        tail_plus(input, params.lines);
    }

    fclose(input);
    return 0;
error:
    if(input) fclose(input);
    exit(EXIT_FAILURE);
}
/*****************************************************************************/

params_t get_params(int argc, char *argv[]) {
    params_t result = {
        .lines = 10,
        .plus_minus = MINUS,
        .filename = NULL
    };
    // used to test if some parameter wasn't given twice
    bool lines_set = false;
    bool filename_set = false;

    for(int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-h", 2) == 0) {
            print_help();
            exit(EXIT_SUCCESS);
        }
        else if (argv[i][0] == '-' || argv[i][0] == '+' ) {
            check(lines_set == false, "Too many parameters");
            // --5 or --b is invalid
            check(argv[i][1] != '-', "Invalid parameter %s", argv[i]);

            char *end_p;
            result.lines = strtoul(argv[i] + 1, &end_p, 10);
            check(errno == 0, "Invalid parameter %s", argv[i]);
            if (end_p == argv[i] + 1 || *end_p != '\0') {
                fail("Invalid parameter %s", argv[i]);
            }
            lines_set = true;
            result.plus_minus = (argv[i][0] == '+')? PLUS : MINUS;

            if(result.lines == 0) {
                if (result.plus_minus == PLUS) {
                    result.lines = 1;  // +0, special case, print all lines
                }
                else {
                    exit(EXIT_SUCCESS);  // -0, don't print anything
                }
            }
        }
        else {
            check(filename_set == false, "The FILE can be set only once");
            result.filename = argv[i];
            filename_set = true;
        }
    }
    return result;
error:
    print_help();
    exit(EXIT_FAILURE);
}

int tail_minus (FILE * file, unsigned long int lines_requested) {
    char s[MAX_LINE] = {'\0'};
    unsigned long int lines_saved = 0;
    char **line_buffer = (char **)calloc(lines_requested, sizeof(char *));
    check_mem(line_buffer);

    unsigned int i;
    for (i = 0; fgets(s, MAX_LINE, file) != NULL;
            i++, i = i % lines_requested) {
        if(strlen(s) >= (MAX_LINE-1) && s[MAX_LINE-2] != '\n') {
            fail("Line too long (longer lines not implemented)");
        }

        if (lines_requested > lines_saved) {
            // create new line in buffer, since we still read less lines than
            // we want to print
            assert(i <= lines_requested);
            line_buffer[i] = (char *)malloc(MAX_LINE * sizeof(char));
            check_mem(line_buffer[i]);
            strncpy(line_buffer[i], s, MAX_LINE);
            lines_saved++;
        }
        else { // overwrite older line, since we won't need it
            strncpy(line_buffer[i], s, MAX_LINE);
        }
    }
    // print the buffered lines.
    unsigned int j = 0;
    for ( ; j < lines_saved; i++, j++) {
        i = i % lines_saved;
        printf("%s", line_buffer[i]);
    }
    free_2d_array(&line_buffer, lines_requested);
    return 0;
error:
    free_2d_array(&line_buffer, lines_requested);
    return -1;
}

void tail_plus(FILE *file, unsigned long int line) {
    char s[MAX_LINE+1];
    for (unsigned int i = 0; fgets(s, MAX_LINE, file) != NULL; i++) {
        // print forever if necessary (if stdio doesn't end)
        if (i >= line - 1) printf("%s", s);
    }
}

void free_2d_array(char **array_p[], int size) {
    // assumes that the array is initialized with zeros
    char **array = *array_p;
    if(array == NULL) return;
    for (int i = 0; i < size; i++) {
        if(array[i]) free(array[i]);
        array[i] = NULL;
    }
    free(array);
    *array_p = NULL;
}

void print_help() {
    puts("Usage: tail [OPTIONS] [FILE]\n"
         "Print the last 10 lines of FILE to standard output. "
         "If no FILE is given, read standard input.\n"
         "-h\tshow usage information\n"
         "-X\toutput the last X lines\n"
         "+X\toutput all the lines starting from the Xth line "
         "(`tail +1` will print everything)");
}
