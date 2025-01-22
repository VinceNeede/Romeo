#include "variable.h"
#include "callable.h"
#include <string.h>

Rvariable *newRvariable(char* type, key_field *key, void *pos){
    Rvariable *var = (Rvariable*)malloc(sizeof(Rvariable));
    var->type = strdup(type);
    var->key = key;
    var->pos = pos;
    return var;
}

// Have to think about how to free the variable
void freeRvariable(Rvariable *var){
    // fprintf(stderr, "Not implemented\n");
    if (var == NULL) return;

    if (var->key != NULL) free_key_field(var->key);

    if (cmp_types(var->type, "string")) free((char*)var->pos);
    else if (cmp_types(var->type, "int")) free((int*)var->pos);
    else if (cmp_types(var->type, "double")) free((double*)var->pos);
    else if (cmp_types(var->type, "function")) freeCallable((Callable*)var->pos);
    else if (cmp_types(var->type, "bool")) free((int*)var->pos);
    else fprintf(stderr, "cannot free void*\n");

    if (var->type != NULL) free(var->type);

    free(var);
}

// Rvariable *newRvariable_from_Literal(const Token* tname, const Literal* lit){
//     return newRvariable(lit->type, tname, lit->data);
// }

int *get_int_var(Rvariable* var){
    return (int*)var->pos;
}

double *get_double_var(Rvariable* var){
    return (double*)var->pos;
}

char *get_string_var(Rvariable* var){
    return (char*)var->pos;
}

Literal *var_to_literal(Rvariable* var){
    void *pos;
    int free_data = 1;
    if (cmp_types(var->type, "int") || cmp_types(var->type, "bool")){
        pos = malloc(sizeof(int));
        *(int*)pos = *(int*)var->pos;
    } else if (cmp_types(var->type, "double")){
        pos = malloc(sizeof(double));
        *(double*)pos = *(double*)var->pos;
    } else if (cmp_types(var->type, "string")){
        size_t len = strlen((char*)var->pos);
        pos = malloc( len + 1);
        strcpy((char*)pos, (char*)var->pos);
        ((char*)pos)[len] = '\0';
    } else if (cmp_types(var->type, "function")){
        pos = var->pos;
        free_data = 0;
    } else {
        fprintf(stderr, "Invalid type for variable\n");
        exit(1);
    }
    
    return newLiteral(var->type, pos, free_data);
}

Rvariable *copyvar(Rvariable *var){
    fprintf(stderr, "Not implemented\n");
    return NULL;
}
