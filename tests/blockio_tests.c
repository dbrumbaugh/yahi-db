/*
 * blockio_tests.c
 *
 * A set of unit tests for the functionality of blockio.c .
 *
 * Copyright 2021, Douglas B. Rumbaugh
 *
 * This code is published under the BSD 3-Clause License, see the
 * LICENSE file in the main project directory for details.
 *
 */

#include "blockio.h"

#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

// table record structure for testing.  We'll use this same record
// throughout the test suite, however here we will eschew table stuff
// and simply shuffle bytes around. The goal is to just test the storage
// management features, after all! And these aren't aware of anything
// beyond a byte array and
typedef struct tbl_rec {
    int id;
    char fname[20];
    char lname[20];
    int age;
} tbl_rec;


// Define some test "people" to set up in the file
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


// Test table file
FILE *tbl_file;


START_TEST(allocate_init_block)
{
    // allocate new block
    int blk_no = blk_new(tbl_file);
    
    // return block id must be 0
    ck_assert_int_eq(blk_no, 0);

    // file must be BLOCKSIZE bytes in length
    off_t length = blk_flen(tbl_file);
    ck_assert_int_eq(length, BLOCKSIZE);

    // file must be empty (all nulls)
    rewind(tbl_file);
    int non_nulls = 0;
    for (int i=0; i<length; i++) {
        if ((fgetc(tbl_file)) != 0) {
            non_nulls++;
        }
    }

    ck_assert_int_eq(non_nulls, 0);
}
END_TEST


START_TEST(write_first_block)
{
    // Copy record data into a buffer
    byte *data = calloc(BLOCKSIZE, sizeof(byte));
    memcpy(data, &p1, sizeof(p1));
    memcpy(data + sizeof(p1), &p2, sizeof(p2));

    // write the buffer to the file
    blk_write(tbl_file, 0, data);

    // validate that file is still BLOCKSIZE in length
    off_t length = blk_flen(tbl_file);

    ck_assert_int_eq(length, BLOCKSIZE);

    // validate that the first block in the file
    // is a byte-for-byte replica of the buffer
    rewind(tbl_file);

    for (int i=0; i<BLOCKSIZE; i++) {
        ck_assert_int_eq(data[i], fgetc(tbl_file));
    }

    free(data);
}
END_TEST


START_TEST(read_first_block)
{
    byte *data = calloc(BLOCKSIZE, sizeof(byte));
    int read = blk_read(tbl_file, 0, data);

    ck_assert_int_eq(read, BLOCKSIZE);

    // Verify that the data in the buffer is a match for
    // the file.
    rewind(tbl_file);
    for (int i=0; i<BLOCKSIZE; i++) {
        ck_assert_int_eq(data[i], fgetc(tbl_file));
    }

    free(data);
}
END_TEST


START_TEST(write_nonexistant_block)
{
    byte *data = calloc(BLOCKSIZE, sizeof(byte));
    off_t init_length = blk_flen(tbl_file);

    int blk_no = init_length / BLOCKSIZE;

    int new_blk_no = blk_no + 3;

    int result = blk_write(tbl_file, new_blk_no, data);

    ck_assert_int_eq(result, 0);

    free(data);
}
END_TEST


START_TEST(write_null_block)
{
    byte *data = NULL;

    int result = blk_write(tbl_file, 0, data);

    ck_assert_int_eq(result, 0);
}
END_TEST


START_TEST(create_new_block)
{
    // get initial filesize
    off_t init_len = blk_flen(tbl_file);

    // allocate new block
    int blk_no = blk_new(tbl_file);
    
    // return block id must be 1 greater
    // than previous max.
    int new_blk_no = init_len / BLOCKSIZE;
    ck_assert_int_eq(blk_no, new_blk_no);

    // file must be BLOCKSIZE bytes longer than previous
    off_t len = blk_flen(tbl_file);
    ck_assert_int_eq(init_len + BLOCKSIZE, len);

    // new block must be empty (all nulls)
    fseeko(tbl_file, -BLOCKSIZE, SEEK_END);

    int non_nulls = 0;
    for (int i=0; i<BLOCKSIZE; i++) {
        if ((fgetc(tbl_file)) != 0) {
            non_nulls++;
        }
    }

    ck_assert_int_eq(non_nulls, 0);
}
END_TEST


START_TEST(read_write_to_block)
{
    int blk_no = 0;
    int init_len = blk_flen(tbl_file);
    byte *data = calloc(BLOCKSIZE, sizeof(byte));

    // Read first block and verify correct number of
    // bytes read, and that bytes in the buffer match
    // those in the file.
    int read = blk_read(tbl_file, blk_no, data);
    ck_assert_int_eq(read, BLOCKSIZE);

    rewind(tbl_file);
    for (int i=0; i<BLOCKSIZE; i++) {
        ck_assert_int_eq(data[i], fgetc(tbl_file));
    }

    // Add more data to the buffer, and then write
    // it back to the file at the blk_no. Verify
    // correct return of blk_write, and that the
    // data now matches the new buffer contents.
    memcpy(data + 2*sizeof(p1), &p3, sizeof(p3));
    int write = blk_write(tbl_file, blk_no, data);

    rewind(tbl_file);
    for (int i=0; i<BLOCKSIZE; i++) {
        ck_assert_int_eq(data[i], fgetc(tbl_file));
    }

    // Verify that the file is still the same length as
    // it started.
    int len = blk_flen(tbl_file);
    
    ck_assert_int_eq(init_len, len);

    free(data);
}
END_TEST


Suite *test_suite()
{
    Suite *suite = suite_create("blockio");

    // Test the basic functionality
    TCase *basic = tcase_create("basic");
    
    tcase_add_test(basic, allocate_init_block);
    tcase_add_test(basic, write_first_block);
    tcase_add_test(basic, read_first_block);
    tcase_add_test(basic, write_null_block);
    tcase_add_test(basic, write_nonexistant_block);
    tcase_add_test(basic, create_new_block);
    tcase_add_test(basic, read_write_to_block);

    // TODO: Add stress testing
    TCase *stress = tcase_create("stress");

    suite_add_tcase(suite, basic);
    suite_add_tcase(suite, stress);

    return suite;
}


void initialize_testdb()
{
    mkdir("tests/testdb", 0777);
    remove("tests/testdb/persons.tbl");
    tbl_file = fopen("tests/testdb/persons.tbl", "w+");
}


int run_test_suite()
{
    int failed = 0;
    Suite *suite = test_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return failed;
}


void cleanup()
{
    fclose(tbl_file);
}

int main() 
{
    initialize_testdb();
    int failed = run_test_suite();
    cleanup();

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
