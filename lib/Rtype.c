#include "Rtype.h"

Rtype *newRtype(const char *name, size_t size){
    Rtype *type = (Rtype*)malloc(sizeof(Rtype));
    type->name = strdup(name);
    type->size = size;
    return type;
}
void freeRtype(Rtype *type){
    if (type == NULL) return;
    free(type->name); 
    free(type);
}
