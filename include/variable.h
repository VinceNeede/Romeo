#ifndef VARIABLE_H
#define VARIABLE_H
#include "forward_decl.h"
#include "types.h"
#include "token.h"
#include "memblock.h"


Literal *var_to_literal(Rvariable* var);
Rvariable *copyvar(Rvariable *var);
#endif // VARIABLE_H