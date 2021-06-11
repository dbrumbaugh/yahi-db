/* storagemanager.h
 *
 * A simple storage management library for the yahi-db 
 * project.
 *
 * Copyright 2021, Douglas Rumbaugh
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "yahi.h"
#include "storagemanager.h"

int sm_blk_offset(int id)
{
    return id * BLOCKSIZE;
}


int sm_write(dbfile *file, int id, byte* data)
{
    int w_offset = sm_blk_offset(id);
    fseek(file->file, w_offset, SEEK_SET);
    int written = fwrite(data, BLOCKSIZE, 1, file->file);
    //TODO: Modify this to ensure that all bytes are
    //      written.

    return written;
}


int sm_read(dbfile *file, int id, byte* data)
{
    int r_offset = sm_blk_offset(id);
    fseek(file->file, r_offset, SEEK_SET);
    int read = fread(data, BLOCKSIZE, 1, file->file);
    //TODO: Modify this to ensure that all bytes are
    //      read.

    return read;
}


int sm_new_blk(dbfile *file)
{
    //TODO: Perhaps allocate new block directory in
    //      the buffer pool?
    byte* blk = calloc(BLOCKSIZE, sizeof(byte));
    if (!blk) {
        fprintf(stderr, "MEMORY ERROR!\n");
        exit(-1);
    }
      
    fseek(file->file, 0, SEEK_END);

    file->max_id++;
    int stat = sm_write(file, file->max_id, blk);

    return file->max_id;
}









