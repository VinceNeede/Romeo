#include "scanner.h"
#include "parser.h"
#include "interpreter.h"

int main(){
    const char* source = "print(\"Hello, World!\")";
    Scanner* scanner;
    Parser *parser;
    List_Stmt* stmts;

    scanner = newScanner(source);
    scanTokens(scanner);
    parser = newParser(scanner->tokens);

    stmts = parse(parser);

    Interpret(stmts);

    freeScanner(scanner);
    freeParser(parser);
    freeList_Stmt(stmts);
    return 0;
}