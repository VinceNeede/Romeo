#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include "HT_var.h"
#include "types.h"
#include "token.h"

#define GLOBAL_ENV_SIZE 101
#define LOCAL_ENV_SIZE 23


typedef struct env{
    HT_var** vars;
    struct env* enclosing;
} Environment;

Environment * newEnvironment(Environment* enclosing);
void freeEnvironment(Environment* env);
void interpreter_init();
Rvariable* get_var(Environment* env, Token* Tkey, int recurse);
#endif // ENVIRONMENT_H