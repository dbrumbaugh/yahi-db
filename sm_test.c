#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "yahi.h"
#include "storagemanager.h"

int main(int argc, char **argv)
{
    mkdir("testdb", 0777);

    dbfile *test_tbl = malloc(sizeof(dbfile));
    test_tbl->file = fopen("testdb/persons.tbl", "w+");
    memcpy(test_tbl->filename, "testdb/persons.tbl", 18);
    test_tbl->max_id = -1;

    sm_new_blk(test_tbl);

    return 0;

}
