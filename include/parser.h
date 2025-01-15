#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "LinkList_Token.h"
#include "Expr.h"

typedef struct {
    List_Token *list;
    Node_Token *current;
} Parser;

Parser* newParser(List_Token *);
void freeParser(Parser *);
Expr *parse(Parser*);


#endif // PARSER_H