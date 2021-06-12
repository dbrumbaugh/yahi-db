/*
 * table.h
 *
 * yahi-db table operations.
 *
 */

#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

table tables[MAX_TBL_CNT];
int tbl_cnt;

void tbl_init() 
{
    tbl_cnt = 0;

}

int tbl_load(char* name, char* database) 
{
    if (tbl_cnt == MAX_TBL_CNT) {
        return -1;
    }

    char fname[MAX_TBL_NAME + MAX_DB_NAME + 5];
    strcat(fname, database);
    strcat(fname, "/");
    strcat(fname, name);
    strcat(fname, ".tbl");

    FILE *tbl_file = fopen(fname, "w+");


    int records = 0;
    schema fields;

    table new_tbl = {
        .name = *name,
        .db = *database,
        .record_cnt = records,
        .fields = fields,
    };

    memcpy(tables + tbl_cnt, &new_tbl, sizeof(table));
}


table *tbl_create(char* name, char* database, schema *fields)
{
    char fname[MAX_TBL_NAME + MAX_DB_NAME + 5];
    strcat(fname, database);
    strcat(fname, "/");
    strcat(fname, name);
    strcat(fname, ".tbl");

    FILE *tbl_file = fopen(fname, "w+");
    int namelen = strnlen(name, MAX_TBL_NAME);
    int dblen = strnlen(database, MAX_DB_NAME);

    fwrite(database, dblen, 1, tbl_file);
    fwrite(name, namelen, 1, tbl_file);


}

