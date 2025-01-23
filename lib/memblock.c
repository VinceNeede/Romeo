#include "memblock.h"
#include "interpreter.h"
#include "HT_MemBlock.h"
#include "Rtype.h"
#include "callable.h"

HT_MemBlock *memblocks = NULL;

size_t hash_memblock(const uintptr_t start){
    return start;
}

int cmp_memblock(const uintptr_t startA, const uintptr_t startB){
    return startA == startB;
}

// void update_MemBlock(MemBlock *a, const MemBlock *b){
//     a->count++;
// }

MemBlock *newMemBlock(uintptr_t start, size_t size){
    MemBlock *block = (MemBlock*)malloc(sizeof(MemBlock));
    block->start = start;
    block->size = size;
    block->count = 1;
    return block;
}

void freeMemBlock(MemBlock *block){
    free((void*)block->start);
    free(block);
}

MemBlock* copyMemBlock(MemBlock *block){
    fprintf(stderr, "Not implemented\n");
    return NULL;
}

void memblock_init(){
    memblocks = newHT_MemBlock(HT_MEMBLOCK_SIZE, hash_memblock, cmp_memblock, NULL, freeMemBlock);
}

Rvariable *newRvariable(Rtype* type, key_field *key, void *pos){
    Rvariable *var = (Rvariable*)malloc(sizeof(Rvariable));
    var->type = type->name;
    var->key = key;
    if (type->size == 0){
        fprintf(stderr, "Error: Cannot allocate memory for type %s\n", type->name);
        exit(1);
    }
    if (pos == NULL){
        pos = malloc(type->size); //since it is just allocated we can assume it is not in the hashtable
        var->block = NULL;
    } else {
        var->block = searchHT_MemBlock(memblocks, (uintptr_t)pos);
    }

    if (var->block == NULL){
        var->block = newMemBlock((uintptr_t)pos, type->size);
        addHT_MemBlock(&memblocks, var->block, 0);
    } else {
        var->block->count++;
    }
    return var;
}

void freeRvariable(Rvariable *var){
    if (var == NULL) return;
    if (var->key->type == FUNCTION){
        Callable * callee = (Callable*)var->block->start;
        if (callee->params != NULL) freeList_Stmt(callee->params);
        if (callee->body != NULL) freeList_Stmt(callee->body);
        if (callee->return_type != NULL) free(callee->return_type);
    }
    if (var->key != NULL) free_key_field(var->key);
    var->block->count--;
    if (var->block->count == 0){
        rmHT_MemBlock(memblocks, var->block->start);
    }

    free(var);
}

void assignRvariable(Rvariable *var, const Literal *value){
    if (var->block->size == 0){
        fprintf(stderr, "Error: Cannot assign value to type %s\n", var->type);
        exit(1);
    }
    if (strcmp(var->type, value->type) != 0){
        fprintf(stderr, "Error: Cannot assign value of type %s to variable of type %s\n", value->type, var->type);
        exit(1);
    }
    memcpy((void*)var->block->start, value->data, var->block->size);
}

int free_memblocks(){
    int final_count = memblocks->count;
    freeHT_MemBlock(memblocks);

    return final_count;
}