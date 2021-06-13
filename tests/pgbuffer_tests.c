
/*
 * pgbuffer_tests.c
 *
 * A set of unit tests for the functionality of pgbuffer.c .
 *
 * These are fairly limited right now, because the API masks a lot of
 * behaviors. I may end up, for testing purposes, exposing more of the
 * internals of the module so I can properly validate it from this test
 * program.
 *
 * Copyright 2021, Douglas B. Rumbaugh
 *
 * This code is published under the BSD 3-Clause License, see the
 * LICENSE file in the main project directory for details.
 *
 */

#define UNITTEST

#include "pgbuffer.h"
#include "page.h"
#include "table.h"

#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>


int pool_size = 10;
table tbl;
byte empty_blk[BLOCKSIZE];

START_TEST(initialize_pool)
{
    int resp = buff_pool_init(pool_size);
    ck_assert_int_eq(resp, 1);

    ck_assert_int_eq(pool_size, _POOL_SIZE);

    // All initial pages should have no pins,
    // data, tables, and should be unlocked

    for (int i=0; i<pool_size; i++) {
        ck_assert_int_eq(_PAGE_POOL[i]->pinned, 0);
        ck_assert_ptr_eq(_PAGE_POOL[i]->tbl, NULL);
        ck_assert_int_eq(memcmp(&(_PAGE_POOL[i]->data), empty_blk, BLOCKSIZE), 0);

        int locked = sem_trywait(&(_PAGE_POOL[i]->locked));
        ck_assert_int_eq(locked, 0);

        int unlocked = sem_post(&(_PAGE_POOL[i]->locked));
        ck_assert_int_eq(unlocked, 0);
    }

    page **initial_pool = _PAGE_POOL;
    resp = buff_pool_init(pool_size+20);

    // Double initialization should return 0;
    ck_assert_int_eq(resp, 0);

    // Buffer globals should be unchanged from prior to call.
    ck_assert_int_eq(_POOL_SIZE, pool_size);
    ck_assert_ptr_eq(initial_pool, _PAGE_POOL);

    buff_pool_destroy();
}
END_TEST


START_TEST(destroy_pool)
{
    int resp = buff_pool_init(pool_size);
    ck_assert_int_eq(resp, 1);

    ck_assert_int_eq(pool_size, _POOL_SIZE);


    buff_pool_destroy();
    ck_assert_ptr_eq(_PAGE_POOL, NULL);
    ck_assert_int_eq(_POOL_INIT, FALSE);
}
END_TEST


START_TEST(pin_page_not_in_pool)
{
    int blk_no = 0;

    buff_pool_init(pool_size);

    page *pg = buff_pin(&tbl, blk_no);
    
    // Returned page is valid and also has 1 pin.
    ck_assert_ptr_ne(pg, NULL);
    ck_assert_int_eq(pg->pinned, 1);

    // Content of the returned page matches the content in
    // the file.
    fseeko(tbl.file, blk_offset(blk_no), SEEK_SET);
    for (int i=0; i<BLOCKSIZE; i++) {
        ck_assert_int_eq((byte) pg->data[i], (byte) fgetc(tbl.file));
    }

    // verify page is unlocked
    int lock = sem_trywait(&(pg->locked));
    ck_assert_int_eq(lock, 0);

    // verify page modify flag is not set
    ck_assert_int_eq(pg->modified, FALSE);

    // verify page's table pointer is correct
    ck_assert_ptr_eq(pg->tbl, &tbl);

    // verify page's blk_no is correct
    ck_assert_int_eq(pg->blk_id, blk_no);

    // verify we can unlock the page
    int unlock = sem_post(&(pg->locked));
    ck_assert_int_eq(unlock, 0);

    buff_pool_destroy();
}
END_TEST


START_TEST(unpin_page_in_pool)
{
    buff_pool_init(pool_size);
    page *pg = buff_pin(&tbl, 0);
    int err = buff_unpin(&tbl, 0);

    ck_assert_int_eq(err, 1);
    ck_assert_int_eq(pg->pinned, 0);

    // pin page multiple times and then verify that
    // unpin reduces pin count properly.
    page *pg2 = buff_pin(&tbl, 1);
    buff_pin(&tbl, 1);
    buff_pin(&tbl, 1);
    buff_pin(&tbl, 1);

    err = buff_unpin(&tbl, 1);
    ck_assert_int_eq(err, 1);
    ck_assert_int_eq(pg2->pinned, 3);

    buff_pool_destroy();
}
END_TEST


START_TEST(unpin_page_not_in_pool)
{
    buff_pool_init(pool_size);
    int err = buff_unpin(&tbl, 1);

    ck_assert_int_eq(err, 0);

    buff_pool_destroy();
}
END_TEST


START_TEST(pin_page_in_pool)
{
    buff_pool_init(pool_size);
    buff_pin(&tbl, 0);
    buff_pin(&tbl, 1);
    buff_pin(&tbl, 2);

    // Verify that block_no 2 is in the pool currently.
    page *pg = buff_find_pg(&tbl, 2);
    ck_assert_ptr_ne(pg, NULL);
    ck_assert_int_eq(pg->pinned, 1);

    // Pin page again
    page *pg2 = buff_pin(&tbl, 2);

    // Verify that pg and pg2 are pointers to the same thing
    ck_assert_ptr_eq(pg, pg2);

    // Verify that the page has been pinned twice
    ck_assert_int_eq(pg2->pinned, 2);

    buff_pool_destroy();
}
END_TEST


START_TEST(pin_page_with_full_pool)
{
    int pool_size = 3;
    // Fill up the pool and ensure that one of the pages
    // is unpinned and able to be swapped out.
    buff_pool_init(pool_size);
    buff_pin(&tbl, 0);
    buff_pin(&tbl, 1);
    buff_pin(&tbl, 2);

    buff_unpin(&tbl, 1);

    // Verify pin status
    ck_assert_int_eq(_PAGE_POOL[0]->pinned, 1);
    ck_assert_int_eq(_PAGE_POOL[1]->pinned, 0);
    ck_assert_int_eq(_PAGE_POOL[2]->pinned, 1);

    // Request pinning of new page
    page *pg = buff_pin(&tbl, 4);

    // verify pin status
    ck_assert_int_eq(_PAGE_POOL[0]->pinned, 1);
    ck_assert_int_eq(_PAGE_POOL[1]->pinned, 1);
    ck_assert_int_eq(_PAGE_POOL[2]->pinned, 1);

    // verify that the three pages are the correct ones
    ck_assert_int_eq(_PAGE_POOL[0]->blk_id, 0);
    ck_assert_int_eq(_PAGE_POOL[1]->blk_id, 4);
    ck_assert_int_eq(_PAGE_POOL[2]->blk_id, 2);

    // verify that the returned page pointer matches
    // where it should be.
    ck_assert_ptr_eq(pg, _PAGE_POOL[1]);

    // verify that blk_no 1 is no longer present
    page *pg2 = buff_find_pg(&tbl, 1);
    ck_assert_ptr_eq(pg2, NULL);

    buff_pool_destroy();
}
END_TEST


Suite *test_suite()
{
    Suite *suite = suite_create("pgbuffer");

    // Test the basic functionality
    TCase *basic = tcase_create("basic");
    
    tcase_add_test(basic, initialize_pool);
    tcase_add_test(basic, destroy_pool);
    tcase_add_test(basic, pin_page_in_pool);
    tcase_add_test(basic, unpin_page_in_pool);
    tcase_add_test(basic, unpin_page_not_in_pool);
    tcase_add_test(basic, pin_page_not_in_pool);
    tcase_add_test(basic, pin_page_with_full_pool);

    // TODO: Add stress testing
    TCase *stress = tcase_create("stress");

    suite_add_tcase(suite, basic);
    suite_add_tcase(suite, stress);

    return suite;
}


void initialize_table()
{
    char *filename = "tests/testdb/persons.tbl";
    strcpy(tbl.db, "tests/testdb");

    remove(filename);
    tbl.file = fopen(filename, "w+");

    // We want a lot of stuff in here, and I don't feel like
    // making a formal test dataset just yet. So, random numbers
    // it is. We can pretty easily compare buffer contents to 
    // the underlying file, so no need to have a super-specific
    // state of the table at start.
    
    srand(time(NULL));
    int tbl_size = 1000;

    for (int i=0; i<tbl_size; i++) {
        fputc(rand(), tbl.file);
    }

}


int run_test_suite()
{
    int failed = 0;
    Suite *suite = test_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_VERBOSE);
    failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return failed;
}


void cleanup()
{
    fclose(tbl.file);
}

int main() 
{
    initialize_table();
    memset(empty_blk, 0, BLOCKSIZE);
    int failed = run_test_suite();
    cleanup();

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
