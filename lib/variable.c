#include "variable.h"
#include "callable.h"
#include <string.h>


Literal *var_to_literal(Rvariable* var){
    // new memory is allocated and the value is copied
    if (strcmp(var->type, "string") == 0){
        return newLiteral(var->type, strdup((char*)var->block->start), 1);
    } else if (strcmp(var->type, "function") == 0){
        return newLiteral(var->type, (void*)var->block->start, 0);
    }
    void *pos = malloc(var->block->size);
    memcpy(pos, (void*)var->block->start, var->block->size);
    return newLiteral(var->type, pos, 1);
}

Rvariable *copyvar(Rvariable *var){
    fprintf(stderr, "Not implemented\n");
    return NULL;
}
