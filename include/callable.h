#ifndef CALLABLE_H
#define CALLABLE_H
#include "types.h"
#include "LinkList_Literal.h"
#include "LinkList_Stmt.h"
#include "interpreter.h"

typedef struct Callable{
    List_Stmt* params;
    List_Stmt* body;
    char* return_type;
    Literal* (*function)(List_Literal*);
} Callable;

Callable *newCallable(List_Stmt*, List_Stmt *,char*);
Callable *copyCallable(Callable* calle);
Callable *literal_as_Callable(Literal* lit);
void freeCallable(Callable *callable);
Literal * execute_callable(Callable *, Interpreter *, List_Literal*);
#endif //CALLABLE_H