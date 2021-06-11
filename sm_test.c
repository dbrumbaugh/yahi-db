/*
 * sm_test.c
 *
 * A set of tests for the functionality of
 * storageman.c .
 *
 * Not currently automated--the generated database 
 * file should be checked for correctness manually.
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
#include <sys/stat.h>
#include "yahi.h"
#include "storageman.h"


typedef struct tbl_rec {
    int id;
    char fname[20];
    char lname[20];
    int age;
} tbl_rec;

int main(int argc, char **argv)
{
    // Initial Setup of test database directory,
    // file, and db_file struct.
    mkdir("testdb", 0777);
    dbfile *test_tbl = malloc(sizeof(dbfile));
    test_tbl->file = fopen("testdb/persons.tbl", "w+");
    memcpy(test_tbl->filename, "testdb/persons.tbl", 18);
    test_tbl->max_id = -1;


    // Allocate a new block and insert it at the 
    // start of the test file.
    sm_new_blk(test_tbl);


    // Define some test "people" to set up in
    // the file.
    tbl_rec p1 = {
        .id = 0,
        .fname = "John",
        .lname = "Smith",
        .age = 40
    };


    tbl_rec p2 = {
        .id = 1,
        .fname = "Lucy",
        .lname = "Adams",
        .age = 40
    };

    tbl_rec p3 = {
        .id = 2,
        .fname = "Tommy",
        .lname = "Adams",
        .age = 8
    };

    // Write the first two people to the first block
    // of the file.
    byte *data = calloc(BLOCKSIZE, sizeof(byte));
    memcpy(data, &p1, sizeof(p1));
    memcpy(data + sizeof(p1), &p2, sizeof(p2));

    sm_write(test_tbl, 0, data);


    // Read in the first block of the file, and then
    // add the third person to it.
    byte *data2 = calloc(BLOCKSIZE, sizeof(byte));
    sm_read(test_tbl, 0, data2);
    memcpy(data2 + 2*sizeof(p1), &p3, sizeof(p3));
    
    
    // Create a new block, and write the modified
    // version of the first block into this second 
    // block.
    int new_id = sm_new_blk(test_tbl);
    sm_write(test_tbl, 1, data2);


    // Close out the file and free up any
    // allocated memory.
    fclose(test_tbl->file);
    free(data);
    free(data2);
    free(test_tbl);

    return 0;
}
