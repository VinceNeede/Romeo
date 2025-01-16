#include "scanner.h"
#include "parser.h"
#include "interpreter.h"

int main(void){
    const char* source = "2+(2.5*2-10)";
    Scanner* scanner;
    Parser *parser;
    Expr* final_expr;

    scanner = newScanner(source);
    scanTokens(scanner);
    parser = newParser(scanner->tokens);

    final_expr = parse(parser);

    Literal* result = Interpret(final_expr);
    switch (result->type){
        case C_INT:
            printf("%d\n", *((int*)result->data));
            break;
        case C_DOUBLE:
            printf("%lf\n", *((double*)result->data));
            break;
        default:
            fprintf(stderr, "Invalid type for result\n");
            exit(1);
    }

    freeScanner(scanner);
    freeParser(parser);
    freeExpr(final_expr);
    freeLiteral(result);
    return 0;
}