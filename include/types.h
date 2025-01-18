#ifndef TYPES_H
#define TYPES_H

#include "Rtype.h"
#include "HT_Rtype.h"
#define MAX_TYPES 100


extern HT_Rtype *types;

void types_init();
int cmp_types(const char *a, const char *b);
void free_types();

#endif // TYPES_H