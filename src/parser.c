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
    Expr *res=NULL;

    switch (type){
    case FALSE:
        res = newLiteralExpr(newLiteral("string", strdup("false"),1));
        break;
    case TRUE:
        res = newLiteralExpr(newLiteral("string", strdup("true"),1));
        break;
    case NIL:
        res = newLiteralExpr(newLiteral("string", strdup("nil"),1));
        break;
    case NUMBER:
        res = newLiteralExpr(token->literal);
        break;
    case STRING:
        res = newLiteralExpr(token->literal);
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

Expr* finishCall(Parser* parser, Expr* callee){
    List_Expr *arguments = newList_Expr();
    if (parser->current->next->data->type != RIGHT_PAREN){
        do{
            parser->current = parser->current->next;
            if (arguments->size >= 255){
                fprintf(stderr, "Cannot have more than 255 arguments\n");
                exit(1);
            }
            add_Expr(arguments, expression(parser));
        } while (match_type(parser, COMMA));
    } else parser->current = parser->current->next;

    if (!match_type(parser, RIGHT_PAREN)){
        fprintf(stderr, "Expect ')' after arguments\n");
        exit(1);
    }
    parser->current = parser->current->next;
    arguments->free_lit_from = NULL;
    return newCallExpr(callee, arguments);
}

Expr* call(Parser * parser){
    Expr* expr = primary(parser);
    while(1){
        if (match_type(parser, LEFT_PAREN)){
            expr = finishCall(parser, expr);
        } else break;
    }
    return expr;
}

Expr* reference(Parser* parser){
    if (match_type(parser, AMPERSAND)){
        parser->current = parser->current->next;
        Token *token = parser->current->data;
        if (token->type != IDENTIFIER){
            fprintf(stderr, "Expect identifier after &\n");
            exit(1);
        }
        parser ->current = parser->current->next;
        return newReferenceExpr(token);
    }
    return call(parser);
}

Expr *unary(Parser* parser){
    if(match_type(parser, MINUS) || match_type(parser, BANG)){
        Literal *lit = newLiteral("string", strdup(match_type(parser,MINUS) ? "negate" : "not"),1);
        List_Expr *arguments = newList_Expr();
        Token* Tcallee = newToken(IDENTIFIER, (char*)lit->data, lit, parser->current->data->line,1);
        Expr* callee = newVariableExpr(Tcallee);

        parser->current = parser->current->next;
        add_Expr(arguments, primary(parser));       // why not call?
        // arguments ->free_lit_from = NULL;
        return newCallExpr(callee, arguments);
    }
    return reference(parser);
}

Expr *binary_to_call(Parser* parser, Expr* left, char* operator, Expr* (*next_func)(Parser*)){
    Literal *lit = newLiteral("string", strdup(operator),1);
    List_Expr *arguments = newList_Expr();
    Token* Tcallee = newToken(IDENTIFIER, (char*)lit->data, lit, parser->current->data->line,1);
    Expr* callee = newVariableExpr(Tcallee);

    parser->current = parser->current->next;
    add_Expr(arguments, left);
    add_Expr(arguments, next_func(parser));
    // arguments ->free_lit_from = NULL;
    return newCallExpr(callee, arguments);
}

Expr* factor(Parser* parser){
    Expr* expr = unary(parser);
    while(match_type(parser, STAR) || match_type(parser, SLASH)){
        expr = binary_to_call(parser, expr, match_type(parser, STAR) ? "mul" : "div", unary);
    }
    return expr;
}

Expr *term(Parser* parser){
    Expr* expr = factor(parser);
    while(match_type(parser, PLUS) || match_type(parser, MINUS)){
        expr = binary_to_call(parser, expr, match_type(parser, PLUS) ? "add" : "sub", factor);
    }
    return expr;
}

Expr *comparison(Parser* parser){
    Expr* expr = term(parser);
    char* operator;
    while(match_type(parser, GREATER) || match_type(parser, GREATER_EQUAL)
        || match_type(parser, LESS) || match_type(parser, LESS_EQUAL)){
        switch (parser->current->data->type){
        case GREATER:
            operator = "gt";
            break;
        case GREATER_EQUAL:
            operator = "ge";
            break;
        case LESS:
            operator = "lt";
            break;
        case LESS_EQUAL:
            operator = "le";
            break;
        }
        expr = binary_to_call(parser, expr, operator, term);
    }
    return expr;
}

Expr *equality(Parser* parser){
    Expr* expr = comparison(parser);
    while(match_type(parser,BANG_EQUAL) || match_type(parser,EQUAL_EQUAL)){
        expr = binary_to_call(parser, expr, match_type(parser, BANG_EQUAL) ? "neq" : "eq", comparison);
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
            freeExpr(expr,0);
            expr = tmp;    
        }
    }
    return expr;
}

Stmt *statement(Parser*); //forward declaration
Stmt *funDeclaration(Parser* parser, Token* type, Token* name);



Stmt *varDeclaration(Parser* parser){
    Token *type = parser->current->data;
    Token *name = parser->current->next->data;
    Expr *initializer = NULL;
    Stmt* res;

    parser->current = parser->current->next->next;

    if (cmp_types(type->literal->data, "function")){
        return funDeclaration(parser, type, name);
    }
    if (parser->current->data->type == EQUAL){
        parser->current = parser->current->next;
        initializer = expression(parser);
    }
    key_field *key = (key_field*)malloc(sizeof(key_field));
    key->type = NAME;
    key->field.name = strdup((char*)name->literal->data);
    freeLiteral(name->literal,1);
    name->literal = newLiteral("key_field", (void*)key,1);
    res = newVarStmt(type, name, initializer);
    return res;
}



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

Stmt *funDeclaration(Parser* parser, Token* type, Token* name){
    List_Stmt *params = newList_Stmt();
    List_Stmt *body;
    Stmt* stmt;
    List_string *args_types = newList_string();
    char* return_type;
    int optional_args = 0;

    if (!match_type(parser, LEFT_PAREN)){
        fprintf(stderr, "Expect '(' after function name\n");
        exit(1);
    }

    parser->current = parser->current->next;

    while (!match_type(parser, RIGHT_PAREN) && !match_type(parser, TOKEN_EOF)){
        if (params->size >= 255){
            fprintf(stderr, "Cannot have more than 255 parameters\n");
            exit(1);
        }
        stmt = statement(parser);
        if (stmt->type != STMT_VAR){
            fprintf(stderr, "Expect parameter declaration\n");
            exit(1);
        }
        if (stmt->stmt.var.initializer != NULL){
            optional_args++;
        } else if (optional_args > 0){
            fprintf(stderr, "Non-optional parameters cannot follow optional parameters\n");
            exit(1);
        }
        add_string(args_types, strdup((char*)stmt->stmt.var.type->literal->data));
        add_Stmt(params, stmt);
        if (match_type(parser, COMMA)) parser->current = parser->current->next;
    } while (!match_type(parser, RIGHT_PAREN));

    if (!match_type(parser, RIGHT_PAREN)){
        fprintf(stderr, "Expect ')' after parameters\n");
        exit(1);
    }
    parser->current = parser->current->next;
    if (match_type(parser, ARROW)){
        parser->current = parser->current->next;
        if (!match_type(parser,TYPE)){
            fprintf(stderr, "Expect return type after arrow\n");
            exit(1);
        }
        return_type = strdup((char*)parser->current->data->literal->data);
        freeLiteral(parser->current->data->literal,1);
        parser->current = parser->current->next;
    } else return_type = strdup("void");
    
    if (!match_type(parser, LEFT_BRACE)){
        fprintf(stderr, "Expect '{' before function body\n");
        exit(1);
    }
    body = block(parser);
    key_field *key = (key_field*)malloc(sizeof(key_field));
    key->type = FUNCTION;
    key->field.function.name = strdup((char*)name->literal->data);
    key->field.function.args_types = args_types;
    key->field.function.non_optional_args = params->size - optional_args;
    freeLiteral(name->literal,1);
    name->literal = newLiteral("key_field", (void*)key,1);
    Literal *value = newLiteral("function", (void*)newCallable(params, body, return_type),1);
    free(return_type);
    return newVarStmt(type, name, newLiteralExpr(value));
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
    if (match_type(parser,RETURN)){
        parser->current = parser->current->next;
        Expr *expr = expression(parser);
        if (expr == NULL){
            fprintf(stderr, "Expect expression after return\n");
            exit(1);
        }
        return newExpressionStmt(newRetExpr(expr));
    }
    return newExpressionStmt(expression(parser));
}

List_Stmt *parse(Parser* parser){
    List_Stmt *stmts = newList_Stmt();
    if (parser->current == NULL) parser->current = parser->list->head;
    if (match_type(parser, TOKEN_EOF)) parser->current = parser->current->next;
    while (!match_type(parser, TOKEN_EOF)){
        add_Stmt(stmts,statement(parser));
    }
    return stmts;
}