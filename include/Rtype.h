#ifndef RTYPE_H
#define RTYPE_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    size_t size;
} Rtype;

Rtype *newRtype(const char *name, size_t size);
void freeRtype(Rtype *type);
#endif // RTYPE_H