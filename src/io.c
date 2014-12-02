/* vim: tabstop=4 shiftwidth=4 expandtab
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
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#include "io.h"

static bool warning_printed = false;


int read_word(char *out, unsigned int max, FILE *file) {
    assert(max > 2); // have space for at least one char + '\0'
    // read first character
    int c = '\0';
    while (c = getc(file), isspace(c) && c != EOF) {}
    if (isspace(c) || c == EOF || c == '\0')
        return 0;  // file is empty or contains only spaces
    else
        out[0] = (char)c;

    // read the rest of the characters until a whitespace or EOF is found
    bool word_trimmed = false;
    unsigned int i;
    for (i = 1; c = getc(file), !isspace(c) && c != EOF; i++) {
        if (i < (max - 1)) {
            out[i] = (char)c;
        }
        else {
            word_trimmed = true;
            if (!warning_printed) {
                warning_printed = true;
                fputs("Warning: Some words may be shortened\n", stderr);
            }
        }
    }

    assert(word_trimmed || i <= (max -1));
    if (word_trimmed) out[max-1] = '\0';
    else              out[i] = '\0';
    return i;
}
