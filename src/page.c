/* page.c
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

#include <stdio.h>
#include <stdlib.h>
#include "blockio.h"
#include "page.h"
#include "yahi.h"

page **_PAGE_POOL;
int _POOL_SIZE;

void pg_pool_init(int pool_size) 
{
    _PAGE_POOL = calloc(pool_size, sizeof(page));
    _POOL_SIZE = pool_size;
}


/*
page *pg_load(FILE *file, int id)
{
    // Find an unpinned page to evict
    for (int i=0; i<_POOL_SIZE; i++) {
        if (!_PAGE_POOL[i]->pinned) {
            pg_flush(_PAGE_POOL[i]);
            blk_read(file, id, _PAGE_POOL[i]->data);

        }
    }

}


void pg_flush(page* pg)
{

    if (pg->modified) {

    }
}


void pg_erase(page* pg)
{

}


void pg_pin(page* pg)
{
    pg->pinned += 1;
}


void pg_unpin(page* pg) 
{
    pg->pinned -= 1;
}


void pg_modified(page* pg) 
{
    pg->modified = TRUE;
}
*/
