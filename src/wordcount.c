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
#include <errno.h>

#include "htable.h"
#include "io.h"

/* The table needs to be large enough to distribute its items, ideally so that
 * a single index stores only one item. However, the larger the size, the
 * more memory it consumes. But since an empty item takes up only
 * sizeof(htable_list_t *), we don't need to be too strict about it.
 */
#define HTABLE_SIZE 2000

// Words longer than this on the input will be shortened to this size (and a
// warning will be printed on stderr). Extra +1 for '\0'
#define MAX_WORD_SIZE 100 + 1

int main() {
    htable_t * htable;
    htable = htable_init(HTABLE_SIZE);
    char s[MAX_WORD_SIZE]  = {'\0'};
    if(htable == NULL) {
        perror("Hash table initialization failed");
        return EXIT_FAILURE;
    }

    // read the words and look them up in the hash table
    while(read_word(s, MAX_WORD_SIZE, stdin)) {
        htable_listitem_t *res = htable_lookup(htable, s);
        if(res == NULL) {
            perror("List or list item initialization failed");
            htable_free(&htable);
            return EXIT_FAILURE;
        }
    }

    // print the hash table
    for(htable_iterator_t iterator = htable_begin(htable);
            iterator.ptr != NULL;
            iterator = htable_it_next(iterator)) {

        printf("%d %s\n", iterator.ptr->data, iterator.ptr->key);
    }

    htable_free(&htable);
    return 0;
}
