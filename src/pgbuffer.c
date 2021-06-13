/* pgbuffer.c
 *
 * A simple buffer management library for the yahi-db project.
 *
 * FIXME: I'm ignoring it for now, but there will be some major work that needs
 * to be done in here for concurrency controls. We'll need to make sure that
 * between when find_pg finds the page in the buffer, and the page gets pinned,
 * it doesn't get flushed out and replaced. That'd be... bad. I'm sure there
 * are other concurrency problems too--that's just the first one to spring to
 * my mind.
 *
 * Copyright 2021, Douglas B. Rumbaugh 
 * This code is published under the BSD 3-Clause License, see the LICENSE file 
 * in the main project directory for details.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blockio.h"
#include "pgbuffer.h"
#include "page.h"
#include "table.h"
#include "yahi.h"

page **_PAGE_POOL = NULL;
int _POOL_SIZE = 0;
int _POOL_INIT = FALSE;

int buff_pool_init(int pool_size) 
{
    // You can only initialize the pool once.
    if (_POOL_INIT) {
        return 0;
    }

    _PAGE_POOL = calloc(pool_size, sizeof(size_t));
    if (!_PAGE_POOL) return 0;

    _POOL_SIZE = pool_size;

    for (int i=0; i<_POOL_SIZE; i++) {
        _PAGE_POOL[i] = calloc(1, sizeof(page));
        sem_init(&(_PAGE_POOL[i]->locked), FALSE, 1);
    }

    _POOL_INIT = TRUE;
    return 1;
}


void buff_pool_destroy()
{
    for (int i=0; i<_POOL_SIZE; i++) {
        buff_flush(_PAGE_POOL[i]);
        free(_PAGE_POOL[i]);
    }

    free(_PAGE_POOL);

    _PAGE_POOL = NULL;
    _POOL_INIT = FALSE;
}


page *buff_find_pg(table *tbl, int blk_no)
{
    // search through page pool to see if it is in the buffer,
    // TODO: Replace the page_pool with a better data structure
    //       than array. Maybe a treemap or a hashmap?

    page *pg = NULL;
    for (int i=0; i<_POOL_SIZE; i++) {

        if (_PAGE_POOL[i]->tbl->name == tbl->name &&
                _PAGE_POOL[i]->blk_id == blk_no) {
            pg = _PAGE_POOL[i];
            return pg;
        }
    }

    return NULL;
}


page *buff_find_and_load_pg(table *tbl, int blk_no)
{
    page *pg = buff_find_pg(tbl, blk_no);

    if (!pg) {
        pg = buff_load(tbl, blk_no);
    }

    return pg;
}



page *buff_load(table *tbl, int blk_no)
{
    // Find an unpinned page to evict
    int loaded = FALSE;

    while (!loaded) {
        for (int i=0; !loaded && i<_POOL_SIZE; i++) {
            if (!_PAGE_POOL[i]->pinned) {

                // Write the contents of the evicted page back to disk
                buff_flush(_PAGE_POOL[i]);

                // Read the new data, and initialize the page
                blk_read(tbl->file, blk_no, _PAGE_POOL[i]->data);
                _PAGE_POOL[i]->blk_id = blk_no;
                _PAGE_POOL[i]->pinned = 0;
                sem_init(&_PAGE_POOL[i]->locked, 0, 1);
                _PAGE_POOL[i]->tbl = tbl;
                _PAGE_POOL[i]->modified = FALSE;

                return _PAGE_POOL[i];
            }
        }
    }

    return NULL;
}


void buff_flush(page* pg)
{
    if (pg->modified) {
        blk_write(pg->tbl->file, pg->blk_id, pg->data);
    }
}


void buff_erase(page* pg)
{
    memset(pg, 0, sizeof(page));
}


page *buff_pin(table *tbl, int blk_no)
{
    page *pg = buff_find_and_load_pg(tbl, blk_no);
    pg->pinned += 1;

    return pg;
}


int buff_unpin(table *tbl, int blk_no) 
{
    page *pg = buff_find_pg(tbl, blk_no);

    if (!pg) return 0;
    if (pg->pinned == 0) return -1;

    pg->pinned -= 1;
    return 1;
}


int buff_lock(table *tbl, int blk_no) 
{
    page *pg = buff_find_pg(tbl, blk_no);
    if (!pg) return 0;

    sem_wait(&pg->locked);
    return 1;
}


int buff_unlock(table *tbl, int blk_no) 
{
    page *pg = buff_find_pg(tbl, blk_no);
    if (!pg) return 0;

    int err = sem_close(&pg->locked);
    if (err != 0) return -1;

    return 1;
}


int buff_modified(table *tbl, int blk_no) 
{
    page *pg = buff_find_pg(tbl, blk_no);
    if (!pg) return 0;

    pg->modified = TRUE;
    return 1;
}
