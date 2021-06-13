/* blockio.h
 *
 * A block-based IO library for the yahi-db 
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
#include "yahi.h"

#define BLOCKSIZE 200
#define MAX_TBL_NAME 20

off_t blk_flen(FILE *file);
int blk_offset(int blk_no);
int blk_write(FILE *file, int blk_no, byte* data);
int blk_read(FILE *file, int blk_no, byte* data);
int blk_new(FILE *file);









