/* buffermanager.h
 *
 * A simple buffer management library for the yahi-db 
 * project.
 *
 * Copyright 2021, Douglas B. Rumbaugh
 * This code is published under the BSD 3-Clause License,
 * see the LICENSE file in the main project directory
 * for details.
 *
 */

#pragma once

#include <stdio.h>
#include <semaphore.h>
#include "blockio.h"
#include "yahi.h"

typedef struct page {
    byte data[BLOCKSIZE];
    int blk_id;
    FILE *file;
    
    int modified;
    int pinned;
    sem_t locked;
} page;

void pg_pool_init(int pool_size);
page *pg_load(FILE *file, int id);
void pg_flush(page* pg);
void pg_pin(page* pg);
void pg_unpin(page* pg);
void pg_modified(page* pg);



