#include "environment.h"
#include "LinkList_string.h"

size_t hash_var(const key_field * key){    
    const char * key_str;
    switch (key->type){
        case FUNCTION:
            return hash_fun(key);
            break;
        case NAME:
            key_str = (const char*)key->field.name;
            break;
    }
    size_t hash = 0;
    for (int i = 0; key_str[i] != '\0'; i++){
        hash = 31 * hash + key_str[i];
    }
    return hash;
}

int cmp_var (const key_field* ht_key, const key_field* cmp_key){
    if (ht_key->type != cmp_key->type) return 0;
    switch (ht_key->type){
        case FUNCTION:
            return cmp_fun(ht_key, cmp_key);
        case NAME:
            return strcmp((const char*)ht_key->field.name, (const char*)cmp_key->field.name) == 0;
    }
    return 0;
}

Environment * newEnvironment(Environment* enclosing){
    Environment* env = (Environment*)malloc(sizeof(Environment));
    int size = enclosing == NULL ? GLOBAL_ENV_SIZE : LOCAL_ENV_SIZE;
    env->vars  = (HT_var**)malloc(sizeof(HT_var*));
    *env->vars = newHT_var(size, hash_var, cmp_var, NULL, freeRvariable);
    env->enclosing = enclosing;
    return env;
}

Rvariable* get_var(Environment* env, Token* Tkey, int recurse){
    Rvariable* var;
    key_field* key;
    if (strcmp(Tkey->literal->type, "key_field") == 0)
        key = (key_field*)Tkey->literal->data;
    else {  // assume it is a name
        key = (key_field*)malloc(sizeof(key_field));
        key->type = NAME;
        key->field.name = strdup(Tkey->lexeme);
        freeLiteral(Tkey->literal,1);
        Tkey->literal = newLiteral("key_field", (void*)key,1);
    }

    // first search in look up table
    if ( key->type == FUNCTION && key->field.function.args_types!=NULL && key->field.function.args_types->size > 0){
        char * type_first_arg = key->field.function.args_types->head->data;
        Rtype *t = searchHT_Rtype(types, type_first_arg);
        var = searchHT_var(*t->look_up_table, key);
        if (var != NULL) return var; 
    }
    var = searchHT_var(*env->vars, key);
    if (var != NULL){
        if (var->key->type != key->type){
            fprintf(stderr, "Error: key type mismatch\n");
            exit(1);
        }
        return var;
    }
    if (env->enclosing != NULL && recurse) return get_var(env->enclosing, Tkey, recurse);
    return NULL;
}

void freeEnvironment(Environment* env){
    freeHT_var(*env->vars);
    free(env->vars);
    free(env);
}