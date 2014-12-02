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
#include <string.h>
#include <stdbool.h>

#include "htable.h"


htable_iterator_t htable_begin(htable_t * htable) {
    htable_iterator_t iterator = {
        .htable = htable,
        .index  = 0,
        .ptr    = NULL,
    };

    // cycle trough the htable until we find a non-empty list.
    for(unsigned int i = 0; i < htable->size; i++) {
        if(htable->list[i].head != NULL) {
            iterator.index = i;
            iterator.ptr = htable->list[i].head;
            break;
        }
    }
    return iterator;
}

htable_iterator_t htable_end(htable_t * htable) {
    htable_iterator_t iterator = {
        .htable = htable,
        .index  = 0,
        .ptr    = NULL,
    };

    // cycle trough the htable from the end until we find an non-empty list.
    for(int i = htable->size - 1; i >= 0 ; i--) {
        if(htable->list[i].tail != NULL) {
            iterator.index = i;
            iterator.ptr = htable->list[i].tail;
            break;
        }
    }
    return iterator;
}

htable_iterator_t htable_it_next(htable_iterator_t iterator) {
    htable_iterator_t result = {
        .htable = iterator.htable,
        .index  = 0,
        .ptr    = NULL
    };

    if(iterator.ptr->next != NULL) {
        // return the next item in the same list if it's not empty
        result.index = iterator.index;
        result.ptr = iterator.ptr->next;
    }
    else {
        // find the next list that's not empty end return a pointer to its
        // first item
        unsigned int i;
        for(i = iterator.index + 1; i < iterator.htable->size; i++) {
            if(iterator.htable->list[i].head != NULL) {
                result.index = i;
                result.ptr = iterator.htable->list[i].head;
                break;
            }
        }
    }
    return result;
}

bool htable_it_eq(htable_iterator_t iterator1, htable_iterator_t iterator2) {
    return (iterator1.htable == iterator2.htable &&
            iterator1.index  == iterator2.index  &&
            iterator1.ptr    == iterator2.ptr) ? 1 : 0;
}
