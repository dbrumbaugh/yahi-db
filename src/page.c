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
#include <string.h>
#include "blockio.h"
#include "page.h"
#include "yahi.h"


// At least for now, we won't do any record spanning. So, all updates
// must fit within the bounds of a single block.
int pg_boundscheck(int offset, int length)
{
    return (offset + length < BLOCKSIZE) ? TRUE : FALSE;
}


int pg_getint(page *pg, int offset)
{
    if (pg_boundscheck(offset, sizeof(int))) {
        int result = (int) pg->data[offset];
        return result;
    }

    // gotta figure out a good way to signal the error case here. Perhaps 
    // just require an errno check? Or maybe use a longjump?
    return 0;
}



char *pg_getchar(page *pg, int offset, int length)
{
    // better create a copy and return it, at least until
    // I have a better idea of how concurrency will be managed.
    // Last thing I need is to just return a pointer to the data 
    // in the page buffer, and then have the page get swapped out
    // underneath me!
    
    if (pg_boundscheck(offset, length)) {
        char *result = malloc(sizeof(char) * (length + 1)); // include null term.
        memcpy(result, &(pg->data[offset]), length);

        return result;
    }

    return NULL;
}


double pg_getfloat(page *pg, int offset)
{
    if (pg_boundscheck(offset, sizeof(double))) {
        double result = (double) pg->data[offset];

        return result;
    }

    return 0.0;
}


int pg_setint(page *pg, int offset, int value)
{
    if (pg_boundscheck(offset, sizeof(int))) {
        pg->data[offset] = value;
        pg->modified = TRUE;

        return 1;
    }

    return 0;
}


int pg_setchar(page *pg, int offset, char *value, int length)
{
    if (pg_boundscheck(offset, length)) {
        memcpy(&(pg->data[offset]), value, length);
        pg->modified = TRUE;

        return 1;
    }

    return 0;
}


int pg_setfloat(page *pg, int offset, double value)
{
    if (pg_boundscheck(offset, sizeof(double))) {
        pg->data[offset] = value;
        pg->modified = TRUE;

        return 1;
    }

    return 0;
}
