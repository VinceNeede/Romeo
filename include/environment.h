#include "HT_var.h"
#include "types.h"
#include "token.h"

#define GLOBAL_ENV_SIZE 100
#define LOCAL_ENV_SIZE 20


typedef struct env{
    HT_var* vars;
    struct env* enclosing;
} Environment;

Environment * newEnvironment(Environment* enclosing);
void freeEnvironment(Environment* env);
void update_var_from_Literal(Rvariable* old, const Literal* new);
void interpreter_init();
