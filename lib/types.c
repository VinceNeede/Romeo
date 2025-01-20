#include "types.h"
#include "string_utilities.h"
#include "callable.h"

HT_Rtype *types = NULL;

size_t hash_types(const char *key){
    size_t hash = 0;
    for (int i = 0; key[i] != '\0'; i++){
        hash = (hash << 5) + key[i];
    }
    return hash;
}

int cmp_types(const char *a, const char *b){
    return strcmp(a, b) == 0;
}

void update_types(Rtype *a, const Rtype *b){
    fprintf(stderr, "Error: type %s already exists\n",a->name);
}


void types_init(){
    types = newHT_Rtype(MAX_TYPES, hash_types, cmp_types, update_types, freeRtype);
    
    Rtype *rint = newRtype(NAME,"int",sizeof(int),0);
    Rtype *rdouble = newRtype(NAME,"double",sizeof(double),0);
    Rtype *rstring = newRtype(NAME,"string",0,0);
    Rtype *rbool = newRtype(NAME,"bool",1,0);
    Rtype *rfunction = newRtype(FUNCTION,"function",sizeof(Callable),1);
    
    addHT_Rtype(types, rint, 0);
    addHT_Rtype(types, rdouble, 0);
    addHT_Rtype(types, rstring, 0);
    addHT_Rtype(types, rbool, 0);
    addHT_Rtype(types, rfunction, 0);
} 

void free_types(){
    freeHT_Rtype(types);
}