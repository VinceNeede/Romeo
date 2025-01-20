#include "Rtype.h"


Rtype *newRtype(key_type key_type, char *name, size_t size, int is_callable){
    Rtype *type = (Rtype*)malloc(sizeof(Rtype));
    type->key_type = key_type;
    type->name = strdup(name);
    type->size = size;
    type->is_callable = is_callable;
    return type;
}

void freeRtype(Rtype *type){
    if (type == NULL) return;
    free(type->name); 
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