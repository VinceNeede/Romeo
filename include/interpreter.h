#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "Expr.h"
#include "Stmt.h"
#include "token.h"
#include "LinkList_Stmt.h"
#include "LinkList_Expr.h"
#include "LinkList_Literal.h"
#include "environment.h"

typedef struct {
    ExprVisitor* expr_visitor;
    StmtVisitor* stmt_visitor;
    Environment* env;
} Interpreter;

void Interpret(List_Stmt *);
void interpreter_init();
void freeInterpreter();
#endif // INTERPRETER_H