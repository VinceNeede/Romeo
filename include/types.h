#include "HT_string.h"
#define MAX_TYPES 100

typedef char* Rtype;

extern HT_string *types;

void types_init();
int cmp_types(const char *a, const char *b);
void free_types();