#include "environment.h"
#include "LinkList_string.h"

size_t hash_var(const key_field * key){    
    const char * key_str;
    switch (key->type){
        case FUNCTION:
            key_str = (const char*)key->field.function.name;
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

int cmp_var (const key_field* key1, const key_field* key2){
    if (key1->type != key2->type) return 0;
    switch (key1->type){
        case FUNCTION:
            int cmp_names = strcmp((const char*)key1->field.function.name, (const char*)key2->field.function.name);
            if (cmp_names) return 0;
            List_string *args1 = key1->field.function.args_types;
            List_string *args2 = key2->field.function.args_types;
            if (args1->size != args2->size) return 0;
            Node_string *current1 = args1->head;
            Node_string *current2 = args2->head;
            while (current1 != NULL && !strcmp(current1->data, current2->data)){
                current1 = current1->next;
                current2 = current2->next;
            }
            return current1 == NULL;
        case NAME:
            return strcmp((const char*)key1->field.name, (const char*)key2->field.name) == 0;
    }
    return 0;
}

void update_var_from_Literal(Rvariable* old, const Literal* new){
    // A lot to add here, for now just update the value
    if (!cmp_types(old->type,new->type)){
        fprintf(stderr, "Error: type mismatch\n");
        exit(1);
    }
    Rtype *t = searchHT_Rtype(types, old->type);
    if (t == NULL){
        fprintf(stderr, "Error: type %s not found\n",old->type);
        exit(1);
    }
    memcpy(old->pos, new->data, t->size);
}

Environment * newEnvironment(Environment* enclosing){
    Environment* env = (Environment*)malloc(sizeof(Environment));
    int size = enclosing == NULL ? GLOBAL_ENV_SIZE : LOCAL_ENV_SIZE;
    env->vars = newHT_var(size, hash_var, cmp_var, NULL, freeRvariable);
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
    var = searchHT_var(env->vars, key);
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
    freeHT_var(env->vars);
    free(env);
}