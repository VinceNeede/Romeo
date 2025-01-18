#include "parser.h"

Parser* newParser(List_Token *list){
    Parser *parser = (Parser*)malloc(sizeof(Parser));
    parser->list = list;
    parser->current = list->head;
    return parser;
}

void freeParser(Parser *parser){
    // freeList_Token(parser->list);
    free(parser);
}

// int advance_parser(Parser* parser){
//     parser->current = parser->current->next;
//     if (parser->current == NULL) return 1;
//     return 0;
// }

int match_type(Parser* parser, TokenType type){
    return parser->current->data->type == type;
}

Expr *expression(Parser*); // forward declaration for Primary

Expr* primary(Parser* parser){
    Token *token = parser->current->data;
    TokenType type = token->type;
    Expr *res;

    switch (type){
    case FALSE:
        res = newLiteralExpr(newToken(FALSE,"false",NULL,0));
        break;
    case TRUE:
        res = newLiteralExpr(newToken(TRUE,"true",NULL,0));
        break;
    case NIL:
        res = newLiteralExpr(newToken(NIL,"nil",NULL,0));
        break;
    case NUMBER:
        res = newLiteralExpr(token);
        break;
    case STRING:
        res = newLiteralExpr(token);
        break;
    case LEFT_PAREN:
        parser->current = parser->current->next;
        res = expression(parser);
        if(!match_type(parser, RIGHT_PAREN)){
            fprintf(stderr,"Expect ')' after expression\n");
            exit(1);
        }
        parser->current = parser->current->next;
        return newGroupingExpr(res);
    case IDENTIFIER:
        res = newVariableExpr(token);
        break;
    default:
        break;
    }
    parser->current = parser->current->next;
    return res;
}

Expr *unary(Parser* parser){
    Token *token = parser->current->data;
    if(match_type(parser, MINUS) || match_type(parser, BANG)){
        parser->current = parser->current->next;
        return newUnaryExpr(token, primary(parser));    // possible to add recurrence with unary(parser)
    }
    return primary(parser);
}

Expr* factor(Parser* parser){
    Expr* expr = unary(parser);
    while(match_type(parser, STAR) || match_type(parser, SLASH)){
        Token *token = parser->current->data;
        parser->current = parser->current->next;
        expr = newBinaryExpr(expr, token, unary(parser));
    }
    return expr;
}

Expr *term(Parser* parser){
    Expr* expr = factor(parser);
    while(match_type(parser, PLUS) || match_type(parser, MINUS)){
        Token *token = parser->current->data;
        parser->current = parser->current->next;
        expr = newBinaryExpr(expr, token, factor(parser));
    }
    return expr;
}

Expr *comparison(Parser* parser){
    Expr* expr = term(parser);
    while(match_type(parser, GREATER) || match_type(parser, GREATER_EQUAL)
        || match_type(parser, LESS) || match_type(parser, LESS_EQUAL)){
        Token *token = parser->current->data;
        parser->current = parser->current->next;
        expr = newBinaryExpr(expr, token, term(parser));
    }
    return expr;
}

Expr *equality(Parser* parser){
    Expr* expr = comparison(parser);
    while(match_type(parser,BANG_EQUAL) || match_type(parser,EQUAL_EQUAL)){
        Token *token = parser->current->data;
        parser->current = parser->current->next;
        expr = newBinaryExpr(expr, token, comparison(parser));
    }
    return expr;
}

Expr *expression(Parser* parser){
    Expr *expr = equality(parser);
    if (match_type(parser, EQUAL)){
        parser->current = parser->current->next;
        if (expr->type == EXPR_VARIABLE){
            Token *token = expr->expr.variable.name;
            Expr* tmp;
            tmp = newAssignExpr(token, expression(parser));
            freeExpr(expr);
            expr = tmp;    
        }
    }
    return expr;
}

Stmt *varDeclaration(Parser* parser){
    Token *type = parser->current->data;
    Token *name = parser->current->next->data;
    Expr *initializer = NULL;
    Stmt* res;

    parser->current = parser->current->next->next;

    if (parser->current->data->type == EQUAL){
        parser->current = parser->current->next;
        initializer = expression(parser);
    }
    res = newVarStmt(type, name, initializer);
    return res;
}


Stmt *statement(Parser*); //forward declaration for block

List_Stmt* block(Parser* parser){
    List_Stmt *stmts = newList_Stmt();
    parser->current = parser->current->next;
    while (!match_type(parser, RIGHT_BRACE) && !match_type(parser, TOKEN_EOF)){
        add_Stmt(stmts, statement(parser));
    }
    if (match_type(parser, TOKEN_EOF)){
        fprintf(stderr, "Expect '}' at the end of block\n");
        exit(1);
    }
    parser->current = parser->current->next;
    return stmts;
}

Stmt *statement(Parser* parser){
    if(match_type(parser, PRINT)){
        parser->current = parser->current->next;
        return newPrintStmt(expression(parser));
    }
    if (match_type(parser,TYPE) && parser->current->next->data->type == IDENTIFIER){
        return varDeclaration(parser);
    }
    if (match_type(parser,LEFT_BRACE)) return newBlockStmt(block(parser));
    return newExpressionStmt(expression(parser));
}

List_Stmt *parse(Parser* parser){
    List_Stmt *stmts = newList_Stmt();
    while (!match_type(parser, TOKEN_EOF)){
        add_Stmt(stmts,statement(parser));
    }
    return stmts;
}