#ifndef KXS_STRING_SET_H
#define KXS_STRING_SET_H

#include <stdlib.h>
#include <string.h>
#include "xstring.h"

#define STRING_SET_HASHSIZE   (101)

typedef struct string_set_node_t_ {
    struct string_set_node_t_ *left;
    struct string_set_node_t_ *right;
    struct string_set_node_t_ *parent;
    string_t *key;
} string_set_node_t;

typedef struct string_set_t_ {
    string_set_node_t *hashtable[STRING_SET_HASHSIZE];
    int count;
} string_set_t;

#endif /* KXS_STRING_SET_H */
