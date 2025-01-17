#include "types.h"
#include "token.h"

typedef struct{
    Rtype type;
    const Token *name;     // can become a token
    void *pos;
} Rvariable;

Rvariable *newRvariable(Rtype type, const Token *name, void *pos);
Rvariable *newRvariable_from_Literal(const Token* tname, const Literal* lit);
void freeRvariable(Rvariable *var);

int *get_int_var(Rvariable* var);
double *get_double_var(Rvariable* var);
char *get_string_var(Rvariable* var);
Literal *var_to_literal(Rvariable* var);