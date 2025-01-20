#ifndef RTYPE_H
#define RTYPE_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    size_t size;
    int is_callable;
} Rtype;

Rtype *newRtype(const char *name, size_t size, int is_callable);
void freeRtype(Rtype *type);
Rtype *copyRtype(Rtype *type);
#endif // RTYPE_H