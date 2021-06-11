/* storagemanager.h
 *
 * A simple storage management library for the yahi-db 
 * project.
 *
 * Copyright 2021, Douglas Rumbaugh
 *
 */

#pragma once

#include <stdio.h>
#include "yahi.h"

#define BLOCKSIZE 200
#define MAX_TBL_NAME 20

typedef struct block {
    int id;
    char filename[MAX_TBL_NAME + 4];
} block;


typedef struct dbfile {
    FILE* file;
    char filename[MAX_TBL_NAME + 4];
    int max_id;
} dbfile;

int sm_blk_offset(int id);
int sm_write(dbfile *file, int id, byte* data);
int sm_read(dbfile *file, int id, byte* data);
int sm_new_blk(dbfile *file);









