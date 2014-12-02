/* vim: tabstop=4 shiftwidth=4 expandtab
 *
 * Hash table where the keys are strings and values are unsigned integers.
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
#ifndef __HTABLE_H__
#define __HTABLE_H__

#include <stdbool.h>


typedef struct htable               htable_t;
typedef struct htable_iterator      htable_iterator_t;
typedef struct htable_list          htable_list_t;
typedef struct htable_listitem      htable_listitem_t;


struct htable {
    unsigned int size;
    htable_list_t *list;
};

struct htable_iterator {
    htable_t *htable;
    unsigned int index;
    htable_listitem_t *ptr;
};

struct htable_list {
    htable_listitem_t *head;
    htable_listitem_t *tail;
};

struct htable_listitem {
    char *key;
    unsigned int data;
    htable_listitem_t *next;
};


/**
 * Allocate space for the hash table.
 * @param size  Desired size of the table, recommended is 2000.
 * @return  Pointer to the created table or NULL if malloc failed.
 */
htable_t * htable_init(unsigned int size);

/* Free space after table and all its contents. */
void htable_free(htable_t *htable);

/**
 * Find the key in the htable. If found, increase the count (data) of the kay.
 * If not found, create its item and set count (data) to 1.
 * Additionally, if the list at the index doesn't exist, allocate space for it.
 * @param key The key string - it will be copied over to heap memory.
 * @return The found/created listitem of the key.
 *
 */
htable_listitem_t * htable_lookup(htable_t *htable, const char *key);

/* Create a hash code for the key, to be used as an index in the table. */
unsigned int htable_hash_function(const char *str, unsigned int htable_size);


/* Return the first item in the htable. */
htable_iterator_t htable_begin(htable_t *htable);

/* Return the last item in the htable. */
htable_iterator_t htable_end(htable_t *htable);

/* Return the next item in the htable. */
htable_iterator_t htable_it_next(htable_iterator_t iterator);

/**
 * Check if the iterators are equal.
 * @return 1 if equal, 0 if not
 */
bool htable_it_eq(htable_iterator_t iterator1, htable_iterator_t iterator2);

#endif /* __HTABLE_H__ */
