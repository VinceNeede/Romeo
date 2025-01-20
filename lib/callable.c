#include "callable.h"
#include "environment.h"

Callable *newCallable(List_Stmt* params, List_Stmt* body){
    Callable *callable = (Callable*)malloc(sizeof(Callable));
    callable->params = params;
    callable->body = body;
    return callable;
}
Callable *newCallable_from_literal(Literal* lit){
    if (cmp_types(lit->type, "function")) 
        return (Callable*)lit->data;
    else return NULL;
}

// void freeList_Stmt_Callable(List_Stmt *list){
//     Node_Stmt *current = list->head;
//     Node_Stmt *next;
//     while (current != NULL){
//         next = current->next;
//         freeStmt(current->data);
//         free(current);
//         current = next;
//     }
//     free(list);
// }

void freeCallable(Callable *callable){
    if (callable == NULL) return;
    freeList_Stmt(callable->params,1);
    freeList_Stmt(callable->body,1);
    free(callable);
}

Literal * execute_callable(Callable *callable, Interpreter *interpreter, List_Literal* args){
    Environment *prev_env = interpreter->env;
    interpreter->env = newEnvironment(interpreter->env);

    List_Stmt *new_params = newList_Stmt();
    List_Stmt *copy_params = copyList_Stmt(callable->params);
    List_Stmt *copy_body = copyList_Stmt(callable->body);

    Node_Stmt *current_default_param = copy_params->head;
    Node_Literal *current_args = args->head;


    while(current_default_param!= NULL){
        if (current_default_param->data->type != STMT_VAR){         //This should be tested in Callable definition
            fprintf(stderr, "Expect variable declaration in function parameters\n");
            exit(1);
        }

        Stmt *current_stmt = current_default_param->data;
        Stmt* new_stmt;
        if (current_args != NULL){
            new_stmt = newVarStmt(copyToken(current_stmt->stmt.var.type), copyToken(current_stmt->stmt.var.name), newLiteralExpr(current_args->data));
            add_Stmt(new_params, new_stmt);
        } else {
            if (current_default_param->data->stmt.var.initializer == NULL){
                fprintf(stderr, "Expect argument for function parameter %s\n", current_stmt->stmt.var.name->lexeme);
                exit(1);
            }
            add_Stmt(new_params, current_default_param->data);
        }

        current_default_param = current_default_param->next;
        if (current_args!=NULL) current_args = current_args->next;
    }
    Interpret(new_params);
    Interpret(copy_body);
    freeList_Stmt(new_params,0);
    freeList_Stmt(copy_params,1);
    freeList_Stmt(copy_body,0);
    freeEnvironment(interpreter->env);
    interpreter->env = prev_env;
    // freeList_Stmt(new_params,1);
    return NULL;
}