#ifndef MEMBLOCK_H
#define MEMBLOCK_H

#define HT_MEMBLOCK_SIZE 101

#include <stdlib.h>
#include <stdint.h>
#include "LinkList_string.h"

typedef struct ht_MemBlock HT_MemBlock;

typedef struct memblock{
    uintptr_t start;
    size_t size;
    int count;
} MemBlock;

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

typedef struct Rvariable{
    char* type;
    key_field *key;
    MemBlock *block;
} Rvariable;

void freeMemBlock(MemBlock *block);
MemBlock *copyMemBlock(MemBlock *block);
void memblock_init();
int free_memblocks(); // returns 0 in case there is still memory allocated on exit
Rvariable *newRvariable(Rtype* type, key_field *key, void *pos);
void assignRvariable(Rvariable *var, const Literal *value);
void freeRvariable(Rvariable *var);
// MemBlock *newMemBlock(uintptr_t start, size_t size);
// size_t hash_memblock(const start);
// int cmp_memblock(const startA, const startB);
// void freeMemBlock(MemBlock *block);

#endif // MEMBLOCK_H