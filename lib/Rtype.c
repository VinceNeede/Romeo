#include "Rtype.h"

Rtype *newRtype(const char *name, size_t size, int is_callable){
    Rtype *type = (Rtype*)malloc(sizeof(Rtype));
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
Rtype *copyRtype(Rtype *type){
    return newRtype(type->name, type->size, type->is_callable);
}