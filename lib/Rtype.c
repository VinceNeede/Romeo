#include "Rtype.h"
#include "LinkList_string.h"

#define LOOKUP_TABLE_SIZE 23

size_t hash_fun(const key_field* key){
    const char * key_str = (const char*)key->field.function.name;
    size_t hash = 0;
    for (int i = 0; key_str[i] != '\0'; i++){
        hash = 31 * hash + key_str[i];
    }
    return hash;
}
int cmp_fun(const key_field* ht_key, const key_field* cmp_key){
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
}

Rtype *newRtype(key_type key_type, char *name, size_t size, int is_callable){
    Rtype *type = (Rtype*)malloc(sizeof(Rtype));
    type->key_type = key_type;
    type->name = strdup(name);
    type->size = size;
    type->is_callable = is_callable;
    if (key_type == NAME){
        type->look_up_table = (HT_var**)malloc(sizeof(HT_var*));
        *(type->look_up_table) = newHT_var(LOOKUP_TABLE_SIZE, hash_fun, cmp_fun, NULL, freeRvariable);
    } else type->look_up_table = NULL;
    return type;
}

void freeRtype(Rtype *type){
    if (type == NULL) return;
    free(type->name); 
    if (type->look_up_table != NULL){
        freeHT_var(*(type->look_up_table));
        free(type->look_up_table);
    }
    free(type);
}
void free_key_field(key_field *key){
    if (key == NULL) return;
    if (key->type == FUNCTION){
        free(key->field.function.name);
        freeList_string(key->field.function.args_types);
    } else {
        free(key->field.name);
    }
    free(key);
}
Rtype *copyRtype(Rtype *type){
    return newRtype(type->key_type, type->name, type->size, type->is_callable);
}