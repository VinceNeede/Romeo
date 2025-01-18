#include "environment.h"

size_t hash_var(const Token * key){
    const char * key_str = (const char*)key->literal->data;
    size_t hash = 0;
    for (int i = 0; key_str[i] != '\0'; i++){
        hash = 31 * hash + key_str[i];
    }
    return hash;
}

int cmp_var (const Token * key1, const Token * key2){
    return strcmp((const char*)key1->literal->data, (const char*)key2->literal->data) == 0;
}

void update_var_from_Literal(Rvariable* old, const Literal* new){
    // A lot to add here, for now just update the value
    if (!cmp_types(old->type,new->type)){
        fprintf(stderr, "Error: type mismatch\n");
        exit(1);
    }
    if (cmp_types(old->type,"int")){
        *get_int_var(old) = *((int*)new->data);
    } else if (cmp_types(old->type,"double")){
        *get_double_var(old) = *((double*)new->data);
    } else if (cmp_types(old->type,"string")){
        fprintf(stderr, "Not implemented yet");
        exit(1);
    } else {
        fprintf(stderr, "Error: not implemented type\n");
        exit(1);
    }
}

Environment * newEnvironment(Environment* enclosing){
    Environment* env = (Environment*)malloc(sizeof(Environment));
    int size = enclosing == NULL ? GLOBAL_ENV_SIZE : LOCAL_ENV_SIZE;
    env->vars = newHT_var(size, hash_var, cmp_var, NULL, freeRvariable);
    env->enclosing = enclosing;
    return env;
}

Rvariable* get_var(Environment* env, const Token* key){
    Rvariable* var = searchHT_var(env->vars, key);
    if (var != NULL) return var;
    if (env->enclosing != NULL) return get_var(env->enclosing, key);
    return NULL;
}

void freeEnvironment(Environment* env){
    freeHT_var(env->vars);
    free(env);
}