#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "../identifier.c"

int main() {
    // Example usage of the generated functions
    const struct identifier* c = get_identifier("class", 5);

    if (c!=NULL) {
        printf("Keyword: %s, TokenType %d\n",c->name, c->value);
    } else {
        printf("Keyword not found.\n");
    }
    return 0;
}