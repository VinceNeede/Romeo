#include "variable.h"
#include <string.h>

Rvariable *newRvariable(char* type, const Token *name, void *pos){
    Rvariable *var = (Rvariable*)malloc(sizeof(Rvariable));
    Token *copy_name = newToken(name->type, name->lexeme, name->literal, name->line);
    var->type = type;
    var->name = copy_name;
    var->pos = pos;
    return var;
}

// Have to think about how to free the variable
void freeRvariable(Rvariable *var){
    // fprintf(stderr, "Not implemented\n");
    if (var == NULL) return;

    if (var->name != NULL){
        freeLiteral(var->name->literal);
        freeToken((Token*)var->name); //cast away the const qualifier
    }

    if (cmp_types(var->type, "string")) free((char*)var->pos);
    else if (cmp_types(var->type, "int")) free((int*)var->pos);
    else if (cmp_types(var->type, "double")) free((double*)var->pos);
    if (var->type != NULL) free(var->type);

    // else
    // if (var->pos != NULL) free(var->pos);
    free(var);
}

Rvariable *newRvariable_from_Literal(const Token* tname, const Literal* lit){
    return newRvariable(strdup(lit->type), tname, lit->data);
}

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
    if (cmp_types(var->type, "int")){
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
    } else {
        fprintf(stderr, "Invalid type for variable\n");
        exit(1);
    }
    
    return newLiteral(var->type, pos);
}