#include "scanner.h"
#include "parser.h"
#include "interpreter.h"
#include "types.h"

int main(){
    const char* sources[] = {"y=2+3*2","x=y*y","print(x)",NULL};//,"print(x)",NULL, "z=x+5", "print(z)",NULL};
    const char** sources_ptr = sources;
    Scanner* scanner;
    Parser *parser;
    List_Stmt* stmts;
    types_init();    
    interpreter_init();
    
    while (*sources_ptr != NULL){
        scanner = newScanner(*sources_ptr);
        scanTokens(scanner);

        parser = newParser(scanner->tokens);

        stmts = parse(parser);

        Interpret(stmts);

        freeScanner(scanner);
        freeParser(parser);
        freeList_Stmt(stmts);
        sources_ptr++;
    }
    freeInterpreter();
    free_types();
    return 0;
}