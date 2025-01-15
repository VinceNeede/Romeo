#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "parser.h"
#include "ASTPrinter.h"

int main(void){
    const char* source = "3==2+(4*5-19)";
    Scanner* scanner;
    Parser *parser;
    Expr* final_expr;

    scanner = newScanner(source);
    scanTokens(scanner);
    parser = newParser(scanner->tokens);

    final_expr = parse(parser);

    ASTPrinter(final_expr);

    freeScanner(scanner);
    freeParser(parser);
    freeExpr(final_expr);

    return 0;
}