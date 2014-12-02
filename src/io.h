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
#ifndef __IO_H__
#define __IO_H__

#include <stdbool.h>

/**
 * Read a word (characters separeted from others by `isspace` characters -
 * spaces, tabs, newlines, etc). If the input word is longer than (max-1), it
 * will ignore the extra characters, write '\0' to the out[max-1] position, but
 * still read trough the rest, leaving the file pointing to the beginning of
 * the next word. A warning will be printed on stderr (only once) if this
 * occurs.
 *
 * @param s: Output string buffer which has size 'max' (including the space
 *      needed for '\0'), so the number of saved characters will be (max-1).
 * @param file: Input file pointer.
 * @return: Number of characters in the original word (will be bigger than
 *      'max' if the lenght of the input word is bigger than 'max'. 0 if EOF
 *      was reached.
 */
int read_word(char *out, unsigned int max, FILE *file);

#endif /* __IO_H__ */
