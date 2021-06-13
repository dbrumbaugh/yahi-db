/* page.h
 *
 * Pages represent the "in-memory" view of a block of data on disk,
 * and track information such as what table they belong to, whether
 * the block is "pinned" (which means that is is being used by the database),
 * or locked (for concurrency), etc.
 *
 * A finite number of pages are maintained in a page buffer, in the pgbuffer
 * module. This module handles loading and unloading pages. The page module
 * itself simply contains the page structure, and routines for interacting with
 * it. All client interactions with these components should be through the code
 * in pgbuffer.
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
#include "table.h"
#include "blockio.h"
#include "yahi.h"

typedef struct page {
    byte data[BLOCKSIZE];
    int blk_id;
    table* tbl;
    
    int modified;
    int pinned;
    sem_t locked;
} page;

int pg_getint(page *pg, int offset);
char *pg_getchar(page *pg, int offset, int length);
double pg_getfloat(page *pg, int offset);

int pg_setint(page *pg, int offset, int value);
int pg_setchar(page *pg, int offset, char *value, int length);
int pg_setfloat(page *pg, int offset, double value);
