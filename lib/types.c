#include "types.h"
#include "string_utilities.h"

HT_string *types = NULL;

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

void update_types(char *a, const char *b){
    fprintf(stderr, "Error: type %s already exists\n",a);
}

void freeRtype(Rtype type){
    free(type); 
}

void types_init(){
    types = newHT_string(MAX_TYPES, hash_types, cmp_types, update_types, freeRtype);
    addHT_string(types, literal_string_to_string("int"),0);
    addHT_string(types, literal_string_to_string("double"),0);
    addHT_string(types, literal_string_to_string("string"),0);
}

void free_types(){
    freeHT_string(types);
}