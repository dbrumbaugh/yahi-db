/* blockio.c
 *
 * A simple block-based I/O library for the yahi-db 
 * project.
 *
 * Copyright 2021, Douglas B. Rumbaugh
 * This code is published under the BSD 3-Clause License,
 * see the LICENSE file in the main project directory
 * for details.
 *
 */
#include "blockio.h"
#include <stdio.h>
#include <stdlib.h>
#include "yahi.h"
#include <sys/stat.h>

/*
 * Calculate and return the appropriate offset,
 * from the beginning of the file, to use for
 * accessing a particular block, based on its
 * ID.
 *
 */
int blk_offset(int blk_no)
{
    return blk_no * BLOCKSIZE;
}


/*
 * Determine the length of the specified file.  It may be worthwhile to track
 * this information within the program somewhere, at some point, to save on
 * syscalls.
 *
 */

off_t blk_flen(FILE *file)
{
    fseeko(file, 0, SEEK_END);
    off_t len = ftello(file);

    return len;
}


/* 
 * Write the data (assumed to be BLOCKSIZE bytes long)
 * to the specified block (by blk_no) in file.
 *
 */
int blk_write(FILE *file, int blk_no, byte* data)
{
    if (data == NULL) return 0;

    int w_offset = blk_offset(blk_no);

    if (w_offset >= blk_flen(file)) return 0;

    fseeko(file, w_offset, SEEK_SET);
    int written = fwrite(data, sizeof(byte), BLOCKSIZE, file);
    //TODO: Modify this to ensure that all bytes are
    //      written.

    return written;
}


/*
 * Read the block (of length BLOCKSIZE) from the 
 * specified file, based on its blk_no, and place the
 * resulting bytes into data
 *
 */
int blk_read(FILE *file, int blk_no, byte* data)
{
    int r_offset = blk_offset(blk_no);
    fseek(file, r_offset, SEEK_SET);
    int read = fread(data, sizeof(byte), BLOCKSIZE, file);
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
int blk_new(FILE *file)
{
    //TODO: Perhaps allocate new block directly in
    //      the buffer pool?
    byte* blk = calloc(BLOCKSIZE, sizeof(byte));
    if (!blk) {
        fprintf(stderr, "MEMORY ERROR!\n");
        exit(-1);
    }
      
    off_t length = blk_flen(file);
    int new_blk_no = length / BLOCKSIZE;

    fseeko(file, 0, SEEK_END);
    int written = fwrite(blk, sizeof(byte), BLOCKSIZE, file);

    if (written == BLOCKSIZE) {
        return new_blk_no;
    }

    return -1;
}









