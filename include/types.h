/*
 * types.h
 *
 * yahi-db field data types
 *
 */

#pragma once

#include "yahi.h"

#define INT 0
#define CHAR 1
#define FLOAT 2

int tp_getasint(byte* record, int offset);
double tp_getasfloat(byte* record, int offset);
char *tp_getaschar(byte* record, int offset);
