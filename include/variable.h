#include "types.h"

typedef struct{
    Rtype type;
    char *name;     // can become a token
    void *pos;
} Rvariable;

Rvariable *newRvariable(Rtype type, char *name, void *pos);
void freeRvariable(Rvariable *var);