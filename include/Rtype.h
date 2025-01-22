#ifndef RTYPE_H
#define RTYPE_H

#include <stdlib.h>
#include <string.h>
#include "LinkList_Rtype.h"
#include "HT_var.h"

typedef struct List_string List_string;
void freeList_string(List_string *);

typedef enum{
    NAME,
    FUNCTION
} key_type;

typedef struct key_field{
    key_type type;
    union{
        char* name;
        struct{
            char* name;
            List_string* args_types;
            int non_optional_args;
        }function;
    }field;
} key_field;

typedef struct Rtype{
    key_type key_type;
    char * name;
    size_t size;
    int is_callable;
    HT_var ** look_up_table;
} Rtype;

Rtype *newRtype(key_type key_type, char *name, size_t size, int is_callable);
void freeRtype(Rtype *type);
Rtype *copyRtype(Rtype *type);
void free_key_field(key_field *key);
size_t hash_fun(const key_field* key);
int cmp_fun(const key_field* ht_key, const key_field* cmp_key);
#endif // RTYPE_H