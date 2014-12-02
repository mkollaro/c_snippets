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
void tail_minus(FILE * file, unsigned long int lines);

/* print lines starting from line number X */
void tail_plus(FILE * file, unsigned long int line);

void free_2d_array(char ** array, int size);

/*****************************************************************************/
int main(int argc, char *argv[]) {
    params_t params = get_params(argc, argv);
    FILE *input = NULL;
    // if a filename was given, open it, otherwise use stdin
    if(params.filename == NULL) {
        input = stdin;
    }
    else {
        input = fopen(params.filename, "r");
        if (input == NULL) {
            perror("Can't open file");
            exit(EXIT_FAILURE);
        }
    }

    if (params.plus_minus == MINUS) tail_minus(input, params.lines);
    else                            tail_plus(input, params.lines);
    fclose(input);
    return 0;
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
        if (argv[i][0] == '-' || argv[i][0] == '+' ) {
            if(lines_set == true) {
                fputs("Too many parameters\n", stderr);
                exit(EXIT_FAILURE);
            }
            if(argv[i][1] == '-') {  // --5 or --b is invalid
                fputs("Invalid parameter\n", stderr);
                exit(EXIT_FAILURE);
            }
            errno = 0;
            char *end_p;
            result.lines = strtoul(argv[i] + 1, &end_p, 10);
            if (errno != 0) {
                perror("Invalid parameter");
                exit(EXIT_FAILURE);
            }
            if (end_p == argv[i] + 1 || *end_p != '\0') {
                fputs("Invalid parameter\n", stderr);
                exit(EXIT_FAILURE);
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
            if(filename_set == true) {
                fputs("Too many parameters\n", stderr);
                exit(EXIT_FAILURE);
            }
            result.filename = argv[i];
            filename_set = true;
        }
    }
    return result;
}

void tail_minus (FILE * file, unsigned long int lines) {
    char s[MAX_LINE] = {'\0'};
    unsigned long int lines_saved = 0;
    char **line_buffer = (char **)malloc(lines * sizeof(char *));

    unsigned int i = 0;
    for ( ; fgets(s, MAX_LINE, file) != NULL; i++, i = i % lines) {
        if(strlen(s) >= (MAX_LINE-1) && s[MAX_LINE-2] != '\n') {
            fputs("Line too long\n", stderr);
            free_2d_array(line_buffer, lines_saved);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        if (lines > lines_saved) {
            assert(i <= lines);
            line_buffer[i] = (char *)malloc(MAX_LINE * sizeof(char));
            if(line_buffer[i] == NULL) {
                perror("Out of memory");
                free_2d_array(line_buffer, lines_saved);
                fclose(file);
                exit(EXIT_FAILURE);
            }
            strncpy(line_buffer[i], s, MAX_LINE);
            lines_saved++;
        }
        else {
            strncpy(line_buffer[i], s, MAX_LINE);
        }
    }
    unsigned int head = i;
    unsigned int j = 0;
    // print the buffered lines.
    for (int i = head ; j < lines_saved; i++, j++) {
        i = i % lines_saved;
        printf("%s", line_buffer[i]);
    }
    free_2d_array(line_buffer, lines_saved);
}

void tail_plus(FILE * file, unsigned long int line) {
    char s[MAX_LINE+1];
    for (unsigned int i = 0; fgets(s, MAX_LINE, file) != NULL; i++) {
        if (i >= line - 1) // there should be no limit here
            printf("%s", s);
    }
}

void free_2d_array(char **array, int size) {
    for (int i = 0; i < size; i++) {
        free(array[i]);
        array[i] = NULL;
    }
    free(array);
    array = NULL;
}