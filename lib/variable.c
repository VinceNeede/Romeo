#include "variable.h"

Rvariable *newRvariable(Rtype type, char *name, void *pos){
    if (searchHT_string(types, type) == NULL){
        fprintf(stderr, "Error: type %s does not exist\n", type);
        return NULL;
    }
    Rvariable *var = (Rvariable*)malloc(sizeof(Rvariable));
    var->type = type;
    var->name = name;
    var->pos = pos;
    return var;
}

// Have to think about how to free the variable
void freeRvariable(Rvariable *var){
    fprintf(stderr, "Not implemented\n");
    free(var);
}