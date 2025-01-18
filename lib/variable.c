#include "variable.h"
#include <string.h>

Rvariable *newRvariable(Rtype type, const Token *name, void *pos){
    if (searchHT_string(types, type) == NULL){
        fprintf(stderr, "Error: type %s does not exist\n", type);
        return NULL;
    }
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
    if (strcmp(var->type, "string") == 0) free((char*)var->pos);
    else if (strcmp(var->type, "int") == 0) free((int*)var->pos);
    else if (strcmp(var->type, "double") == 0) free((double*)var->pos);
    // else
    // if (var->pos != NULL) free(var->pos);
    free(var);
}

Rvariable *newRvariable_from_Literal(const Token* tname, const Literal* lit){
    return newRvariable(lit->type, tname, lit->data);
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
    if (strcmp(var->type, "int") == 0){
        pos = malloc(sizeof(int));
        *(int*)pos = *(int*)var->pos;
    } else if (strcmp(var->type, "double") == 0){
        pos = malloc(sizeof(double));
        *(double*)pos = *(double*)var->pos;
    } else if (strcmp(var->type, "string") == 0){
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