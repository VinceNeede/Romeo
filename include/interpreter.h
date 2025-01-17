#include "Expr.h"
#include "Stmt.h"
#include "token.h"
#include "LinkList_Stmt.h"

void Interpret(List_Stmt *);
void interpreter_init();
void freeInterpreter();