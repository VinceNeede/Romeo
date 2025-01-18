#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "LinkList_Token.h"
#include "Expr.h"
#include "Stmt.h"
#include "LinkList_Stmt.h"
#include "string_utilities.h"

typedef struct {
    List_Token *list;
    Node_Token *current;
} Parser;

Parser* newParser(List_Token *);
void freeParser(Parser *);
List_Stmt *parse(Parser*);


#endif // PARSER_H