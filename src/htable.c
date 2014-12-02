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
#include <assert.h>

#include "htable.h"

/*
 * The hash table consists of one integer showing its size and a fixed array of
 * lists. The hash of the key specifies the index of the main array. If the
 * list on that index is empty or the key isn't found in the list, it will
 * create an item for the key and save its data there. If the key is found in
 * the list, it will just update its data.
 *
 * The diagram below shows how htable->table is represented in memory, though
 * it's not very accurate, since 'htable_list_t' contains the pointers 'head'
 * and 'tail', but the diagram only shows 'head' (just imagine another arrow
 * from each, to its last item).
 *
 *   +-------------+
 *   |htable_list_t|-->[key,data,next]-->[key,data,next]-->[key,data,next]--|
 *   +-------------+
 *   |htable_list_t|-->[key,data,next]-->[key,data,next]--|
 *   +-------------+
 *   |htable_list_t|--|
 *   +-------------+
 */

htable_t * htable_init(unsigned int size) {
    // alloc space for the table
    htable_t *htable = malloc(sizeof(htable_t));
    if(htable == NULL)
        return NULL;

    // alloc an array of pointers, pointing to lists
    htable->list = (htable_list_t *)calloc(size, sizeof(htable_list_t));
    if(htable->list == NULL) {
        free(htable);
        return NULL;
    }
    htable->size = size;
    return htable;
}

/* Free space after keys and items */
static void htable_clear(htable_t *htable) {
    for(unsigned int i = 0; i < htable->size; i++) {
        if(htable->list[i].head == NULL || htable->list[i].tail == NULL) {
            assert(htable->list[i].head == NULL &&
                   htable->list[i].tail == NULL);
            continue;
        }
        // cycle trough the list and free its items
        while (htable->list[i].head != NULL) {
            htable_listitem_t *tmp;
            tmp = htable->list[i].head;

            htable->list[i].head = htable->list[i].head->next;
            free(tmp->key);
            free(tmp);
        }
    }
}

void htable_free(htable_t **htable) {
    htable_clear(*htable);
    free((*htable)->list);
    free(*htable);
    *htable = NULL;
}

htable_listitem_t * htable_lookup(htable_t *htable, const char *key) {
    htable_listitem_t *result = NULL;
    unsigned int i = htable_hash_function(key, htable->size);
    bool found = false;
    htable_listitem_t *tmp;
    tmp = htable->list[i].head;

    // search for the key, if found, increase its count and return the item
    while(htable->list[i].head != NULL) {
        if(strcmp(htable->list[i].head->key, key) == 0) {
            htable->list[i].head->data++;
            result = htable->list[i].head;
            found = true;
            break;
        }
        htable->list[i].head = htable->list[i].head->next;
    }
    htable->list[i].head = tmp;

    if(!found) {
        htable_listitem_t *tmp = NULL;
        if(htable->list[i].tail != NULL) { // if this is not the first item
            tmp = htable->list[i].tail;
        }

        // alloc space for one listitem
        htable->list[i].tail = (htable_listitem_t *)
                                malloc(sizeof(htable_listitem_t));
        if(htable->list[i].tail == NULL)
            return NULL;

        // alloc space for the string
        htable->list[i].tail->key = (char *)
                                     malloc((strlen(key) + 1) * sizeof(char));
        strcpy(htable->list[i].tail->key, key);

        htable->list[i].tail->data = 1;
        htable->list[i].tail->next = NULL;

        result = htable->list[i].tail;

        if(tmp != NULL) {
            // reconnect the previous item
            tmp->next = htable->list[i].tail;
        }
        else {
            // if this is the first item in the list, set htable = tail = item
            htable->list[i].head = htable->list[i].tail;
        }
    }
    return result;
}

unsigned int htable_hash_function(const char *key, unsigned int htable_size) {
    unsigned long int h = 0;
    unsigned char *p;

    for(p=(unsigned char*)key; *p!='\0'; p++)
        h = 31*h + *p;
    return h % htable_size;
}
