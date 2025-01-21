#include "callable.h"
#include "environment.h"

Callable *newCallable(List_Stmt* params, List_Stmt* body, char* ret_type){
    Callable *callable = (Callable*)malloc(sizeof(Callable));
    callable->params = params;
    callable->body = body;
    callable->return_type = strdup(ret_type);
    callable->function = NULL;
    return callable;
}
Callable *copyCallable(Callable* calle){
    return newCallable(copyList_Stmt(calle->params), copyList_Stmt(calle->body), calle->return_type);
}
Callable *literal_as_Callable(Literal* lit){
    if (cmp_types(lit->type, "function")){
        return (Callable*)lit->data;
    }
    return NULL;
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
    if (callable->params != NULL) freeList_Stmt(callable->params);
    if (callable->body != NULL) freeList_Stmt(callable->body);
    free(callable->return_type);
    free(callable);
}

Literal * execute_callable(Callable *callable, Interpreter *interpreter, List_Literal* args){
    if (callable->function!=NULL){
        Literal *res = callable->function(args);
        Node_Literal *current = args->head;
        Node_Literal *next;
        while (current != NULL){
            next = current->next;
            freeLiteral(current->data,1);
            current = next;
        }
        return res;
    }
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
            if (current_stmt->stmt.var.initializer == NULL){
                fprintf(stderr, "Expect argument for function parameter %s\n", current_stmt->stmt.var.name->lexeme);
                exit(1);
            }
            add_Stmt(new_params, copyStmt(current_stmt));
        }

        current_default_param = current_default_param->next;
        if (current_args!=NULL) current_args = current_args->next;
    }
    Interpret(new_params);
    Literal *ret_val = NULL;
    interpreter->ret_value = &ret_val;
    Node_Stmt* interpreter_ret = Interpret(copy_body);
    if (ret_val!= NULL && !cmp_types(callable->return_type, ret_val->type)){
        fprintf(stderr, "Expected return type %s but got %s\n", callable->return_type, ret_val->type);
        exit(1);
    } else if (ret_val == NULL && strcmp(callable->return_type, "void") != 0){
        fprintf(stderr, "Expected return type %s but got void\n", callable->return_type);
        exit(1);
    }
    new_params->free_lit_from = NULL;
    freeList_Stmt(new_params);
    freeList_Stmt(copy_params);
    copy_body->free_lit_from = interpreter_ret;
    freeList_Stmt(copy_body);
    freeEnvironment(interpreter->env);
    interpreter->env = prev_env;
    interpreter->check_return = 0;
    // freeList_Stmt(new_params,1);
    return ret_val;
}