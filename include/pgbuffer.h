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
#include "table.h"
#include "page.h"
#include "blockio.h"
#include "yahi.h"

int buff_pool_init(int pool_size);
void buff_pool_destroy();

page *buff_find_pg(table *tbl, int blk_no);
page *buff_find_and_load_pg(table *tbl, int blk_no);
page *buff_load(table *tbl, int blk_no);
void buff_flush(page* pg);

page *buff_pin(table *tbl, int blk_no);
int buff_unpin(table *tbl, int blk_no);

int buff_lock(table *tbl, int blk_no);
int buff_unlock(table *tbl, int blk_no);

int buff_modified(table *tbl, int blk_no);

#ifdef UNITTEST
int _POOL_SIZE;
page **_PAGE_POOL;
int _POOL_INIT;
#endif
