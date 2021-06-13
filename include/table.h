/*
 * table.h
 *
 * yahi-db table operations.
 *
 */

#include <stdio.h>

#define MAX_ATTRS 20
#define MAX_TBL_NAME 20
#define MAX_DB_NAME 20
#define MAX_TBL_CNT 100;

typedef struct schema {
    int record_length;
    int field_lengths[MAX_ATTRS];
    int field_types[MAX_ATTRS];
} schema;


typedef struct table {
   FILE *file; 
   char name[MAX_TBL_NAME];
   char db[MAX_DB_NAME];
   int record_cnt;
   schema fields;
} table;


int tbl_load(char* name, char* database);
table *tbl_create(char* name, char* database, void *schema);

