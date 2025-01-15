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
    return equality(parser);
}

Expr *parse(Parser* parser){
    return expression(parser);
}