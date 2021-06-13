/*
 * types.c
 *
 * yahi-db field data types
 *
 */

#include "yahi.h"
#include <stdlib.h>
#include <string.h>

int tp_getasint(byte* record, int offset)
{
    int value = *(record + offset);
    return value;
}


double tp_getasfloat(byte* record, int offset)
{
    double value = *(record + offset);
    return value;
}


char *tp_getaschar(byte* record, int offset, int length)
{
    char *value = malloc(sizeof(char) * length);
    memcpy(value, record+offset, length);

    return value;
}
