/* storageman.h
 *
 * A simple storage management library for the yahi-db 
 * project.
 *
 * Copyright 2021, Douglas B. Rumbaugh
 * This code is published under the BSD 3-Clause License,
 * see the LICENSE file in the main project directory
 * for details.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "yahi.h"
#include "storageman.h"

/*
 * Calculate and return the appropriate offset,
 * from the beginning of the file, to use for
 * accessing a particular block, based on its
 * ID.
 *
 */
int sm_blk_offset(int id)
{
    return id * BLOCKSIZE;
}


/* 
 * Write the data (assumed to be BLOCKSIZE bytes long)
 * to the specified block (by id) in file.
 *
 */
int sm_write(dbfile *file, int id, byte* data)
{
    int w_offset = sm_blk_offset(id);
    fseek(file->file, w_offset, SEEK_SET);
    int written = fwrite(data, BLOCKSIZE, 1, file->file);
    //TODO: Modify this to ensure that all bytes are
    //      written.

    return written;
}


/*
 * Read the block (of length BLOCKSIZE) from the 
 * specified file, based on its id, and place the
 * resulting bytes into data
 *
 */
int sm_read(dbfile *file, int id, byte* data)
{
    int r_offset = sm_blk_offset(id);
    fseek(file->file, r_offset, SEEK_SET);
    int read = fread(data, BLOCKSIZE, 1, file->file);
    //TODO: Modify this to ensure that all bytes are
    //      read.

    return read;
}


/*
 * Create a new, empty block of size BLOCKSIZE
 * and add it to the end of file. Returns the ID
 * of the newly created block.
 *
 */
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









