#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "token.h"

int main() {
    const char* source = "var x = 10;\nprint(x);";
    Scanner* scanner = newScanner(source);
    List_Token* tokens = scanTokens(scanner);

    for (int i = 0; i < tokens->size; i++) {
        Token* token = getIndex_Token(tokens, i);
        char* tokenStr = TokenToString(token);
        printf("Token: %s\n", tokenStr);
        free(tokenStr);
    }

    freeScanner(scanner);
    return 0;
}