/* vim: tabstop=4 shiftwidth=4 expandtab
 * 
 * Various macros for error handling, logging and debugging.
 * Parts of this file were inspired by Zed's Awesome Debug Macros (see
 * "Learning C The Hard Way").
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
#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

/**
 * Prints an error message on stderr. If errno != 0, it prints it too. The
 * function is basically a wrapper of perror(), so it can be used as printf().
 * Example: print_error("Bad value in x: %d\n", x); exit(1);
 *
 * @warning: use it right after the error ocured, the global variable
 * errno could be otherwise rewritten.
 *
 * @param *fmt  Error message to be displayed, with printf-style formatting.
 * @param ...   Variable argument list, with variables to print.
 */
void print_error(const char *fmt, ...);

#ifdef DEBUG
#define debug(...) fprintf(stderr, "[DEBUG] (%s:%d) ", __FILE__, __LINE__); \
        print_error(__VA_ARGS__)
#else
#define debug(...)
#endif

#define log_err(...) fprintf(stderr, "[ERROR] (%s:%d) ", __FILE__, __LINE__); \
        print_error(__VA_ARGS__)

#define log_warn(...) fprintf(stderr, "[WARN] (%s:%d) ", __FILE__, __LINE__); \
        print_error(__VA_ARGS__)

#define log_info(...) fprintf(stderr, "[INFO] (%s:%d) ", __FILE__, __LINE__); \
        print_error(__VA_ARGS__)

/**
 * Check if condition A is true. If not, print the error message (syntax like
 * printf) and errno, then clean errno and goto a label called 'error', which
 * has to be defined in the function that uses this.
 */
#define check(A, ...) if(!(A)) { \
    log_err(__VA_ARGS__); \
    errno = 0; \
    goto error; }

#define check_mem(A) check((A), "Out of memory.")

// to be used in places that are not supposed to happen
#define fail(...)  { log_err(__VA_ARGS__); errno = 0; goto error; }

#endif
