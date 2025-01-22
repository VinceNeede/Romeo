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

int cmp_var (const key_field* ht_key, const key_field* cmp_key){
    if (ht_key->type != cmp_key->type) return 0;
    switch (ht_key->type){
        case FUNCTION:
            int cmp_names = strcmp((const char*)ht_key->field.function.name, (const char*)cmp_key->field.function.name);
            if (cmp_names) return 0;
            List_string *args1 = ht_key->field.function.args_types;
            List_string *args2 = cmp_key->field.function.args_types;
            if (ht_key->field.function.non_optional_args > args2->size) return 0;
            Node_string *current1 = args1->head;
            Node_string *current2 = args2->head;
            int i = 0;
            while (current2 != NULL && !strcmp(current1->data, current2->data)){
                current1 = current1->next;
                current2 = current2->next;
                i++;
            }
            return i >= ht_key->field.function.non_optional_args;
        case NAME:
            return strcmp((const char*)ht_key->field.name, (const char*)cmp_key->field.name) == 0;
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
    if (strcmp(t->name,"string")  == 0){
        size_t size =  strlen((char*)new->data);
        old->pos = realloc(old->pos, size + 1);
        strcpy(old->pos, (char*)new->data);
        ((char*)old->pos)[size] = '\0';
        return;
    }
    if (t->size == 0) fprintf(stderr, "Error: type %s is not implemented fully\n",old->type);
     
    memcpy(old->pos, new->data, t->size);
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