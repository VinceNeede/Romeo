#include "LinkList_Token.h"
#include "token.h"
#include <string.h>

typedef struct Scanner {
    const char* source;
    int source_length;
    int start;
    int current;
    int line;
    List_Token* tokens;
} Scanner;

Scanner* newScanner(const char* source);
void freeScanner(Scanner* scanner);
List_Token* scanTokens(Scanner* scanner);