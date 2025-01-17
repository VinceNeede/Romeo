#include "scanner.h"
#include "parser.h"
#include "interpreter.h"
#include "types.h"

int main(){
    const char* source="y=2+3*2\nx=y*y\nprint(x)\n";
    Scanner* scanner;
    Parser *parser;
    List_Stmt* stmts;
    types_init();    
    interpreter_init();

    scanner = newScanner(source);
    scanTokens(scanner);
    
    parser = newParser(scanner->tokens);

    stmts = parse(parser);

    Interpret(stmts);

    freeScanner(scanner);
    freeParser(parser);
    freeList_Stmt(stmts);

    freeInterpreter();
    free_types();
    return 0;
}