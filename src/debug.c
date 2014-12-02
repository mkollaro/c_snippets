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
#include <stdarg.h>
#include <errno.h>

void print_error(const char *fmt, ...) {
    int errno_tmp = errno;  // the following commands could rewrite errno
    va_list ap;
    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap); // print the variable argument list
    if(errno_tmp != 0) {
        errno = errno_tmp;
        perror(" ");
    }
    else
        fprintf(stderr, "\n");
    va_end(ap);
}
