#include "LinkList_Token.h"
#include "token.h"
#include <string.h>

typedef struct Scanner {
    char* source;
    size_t source_length;
    size_t start;
    size_t current;
    int line;
    List_Token* tokens;
} Scanner;

Scanner* newScanner();
void set_source(Scanner* scanner, const char* source);
void freeScanner(Scanner* scanner);
List_Token* scanTokens(Scanner* scanner);