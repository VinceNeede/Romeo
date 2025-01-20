#include "interpreter.h"
#include "environment.h"
#include "callable.h"

Interpreter interpreter;

Literal *evaluate(Expr* expr){
    return (Literal *)ExprAccept(interpreter.expr_visitor, expr);
}

void execute(Stmt* stmt){
    StmtAccept(interpreter.stmt_visitor, stmt);
    return;
}

void* interpret_Literal(Expr* expr){
    return (void*)expr->expr.literal.value;
}

void* interpret_Grouping(Expr* expr){
    return evaluate(expr->expr.grouping.expression);
}

void* interpret_Unary(Expr* expr){
    Token* operatorT = expr->expr.unary.operatorT;
    Expr* right = expr->expr.unary.right;
    Literal* rightValue = evaluate(right);
    Literal* res;

    switch (operatorT->type){
        case MINUS:
            if (cmp_types(rightValue->type, "int")){
                int * value = (int*)malloc(sizeof(int));
                *value = -1 * (*((int*)rightValue->data));
                res = newLiteral("int", (void*)value,1);
            }else if (cmp_types(rightValue->type, "double")){
                double * doubleValue = (double*)malloc(sizeof(double));
                *doubleValue = -1 * (*((double*)rightValue->data));
                res = newLiteral("double", (void*)doubleValue,1);
            } else {
                fprintf(stderr, "Invalid type for unary minus\n");
                exit(1);
            }
            break;
        case BANG:
            if (cmp_types(rightValue->type, "int")){
                int * value = (int*)malloc(sizeof(int));
                *value =  !(*((int*)rightValue->data));
                res = newLiteral("int", (void*)value,1);
                break;
            } else {
                fprintf(stderr, "Invalid type for unary bang\n");
                exit(1);
            }
            break;
    }
    freeLiteral(rightValue,0);
    return (void*)res;
}

#define EVAL(type) \
void oper_##type(type* out, type left, type right, TokenType operatorT){\
    switch (operatorT){\
        case PLUS:\
            *out = left + right;\
            break;\
        case MINUS:\
            *out = left - right;\
            break;\
        case STAR:\
            *out = left * right;\
            break;\
        case SLASH:\
            *out = left / right;\
            break;\
        default:\
            fprintf(stderr, "Invalid operator for " #type "\n");\
            exit(1);\
    }\
}

EVAL(int)
EVAL(double)

void* interpret_Binary(Expr* expr){
    Token* operatorT = expr->expr.binary.operatorT;
    Expr* left = expr->expr.binary.left;
    Expr* right = expr->expr.binary.right;
    Literal* leftValue = evaluate(left);
    Literal* rightValue = evaluate(right);
    Literal *res;
    if (cmp_types(leftValue->type, "int") && cmp_types(rightValue->type, "int")){
        int* value = (int*)malloc(sizeof(int));
        int leftv = *((int*)leftValue->data);
        int rightv = *((int*)rightValue->data);
        oper_int(value, leftv, rightv, operatorT->type);
        res = newLiteral("int", (void*)value,1);
    }
    else{
        double* value = (double*)malloc(sizeof(double));
        double leftv, rightv;
        if (cmp_types(leftValue->type, "int")){
            leftv = (double)*((int*)leftValue->data);
        } else {
            leftv = *((double*)leftValue->data);
        }
        if (cmp_types(rightValue->type, "int")){
            rightv = (double)*((int*)rightValue->data);
        } else {
            rightv = *((double*)rightValue->data);
        }
        oper_double(value, leftv, rightv, operatorT->type);
        res = newLiteral("double", (void*)value,1);
    }
    freeLiteral(leftValue,0);
    freeLiteral(rightValue,0);
    return (void*) res;
}

void * interpret_Call(Expr* expr){
    Literal * callee = evaluate(expr->expr.call.callee);
    List_Expr * arguments = expr->expr.call.arguments;
    List_Literal * args = newList_Literal();

    Node_Expr * current = arguments->head;
    while (current != NULL){
        add_Literal(args, evaluate(current->data));
        current = current->next;
    }
    Callable * function = literal_as_Callable(callee);
    Literal * res = execute_callable(function, &interpreter, args);

    // Node_Literal * current_literal = args->head;
    // Node_Literal * next;
    // while (current_literal != NULL){
    //     next = current_literal->next;
    //     printf("freeing inn pos %p\n", current_literal->data);
    //     // freeLiteral(current_literal->data);
    //     // free(current_literal);
    //     current_literal = next;
    // }
    // free((char*)callee->type);
    // free(callee);
    freeLiteral(callee,0);

    Node_Literal * current_literal = args->head;
    Node_Literal * next;
    while (current_literal != NULL){
        next = current_literal->next;
        free(current_literal);
        current_literal = next;
    }
    free(args);
    // freeCallable(function);
    // freeList_Literal(args);
    // freeCallable(function); //free the Callable?
    return NULL;//(void*)res;
}

void * interpret_Print(Stmt* stmt){
    Literal* value = evaluate(stmt->stmt.print.expression);
    if (cmp_types(value->type, "int")){
        printf("%d\n", *((int*)value->data));
    } else if (cmp_types(value->type, "double")){
        printf("%f\n", *((double*)value->data));
    } else if (cmp_types(value->type, "string")){
        printf("%s\n", (char*)value->data);
    } else {
        fprintf(stderr, "Invalid type for print\n");
        exit(1);
    }
    freeLiteral(value,0);
    return NULL;
}

void *interpret_Variable(Expr* expr){
    Token* tname = expr->expr.variable.name;
    Rvariable* var = get_var(interpreter.env, tname);
    if (var == NULL){
        fprintf(stderr, "Variable %s not found\n", (char*)tname->literal->data);
        exit(1);
    }
    freeLiteral(tname->literal,1);
    return (void*)var_to_literal(var);
}

void * interpret_VarDeclaration(Stmt* stmt){
    Token* tname = stmt->stmt.var.name;
    Token* type = stmt->stmt.var.type;
    Expr* initializer = stmt->stmt.var.initializer;
    Rtype* t = searchHT_Rtype(types, (char*)type->literal->data);

    char * name = (char*)tname->literal->data;

    if (t == NULL){
        fprintf(stderr, "Type %s not found\n", (char*)type->literal->data);
        exit(1);
    }
    if (searchHT_var(interpreter.env->vars, tname)!=NULL){
        fprintf(stderr, "Variable %s already exists\n", name);
        exit(1);
    }
    Rvariable *var = newRvariable(t->name, tname, malloc(t->size));

    addHT_var(interpreter.env->vars, var, 0);
    if (initializer != NULL){
        Literal * value = evaluate(initializer);
        update_var_from_Literal(var, value);
        free(value->data);
        free(value->type);
        free(value);
    }
    freeLiteral(tname->literal,0);
    freeLiteral(type->literal,1);
    // free(value);
    return NULL;
}

void * interpret_VarAssign(Expr* expr){
    Token* tname = expr->expr.assign.name;
    Literal* value = evaluate(expr->expr.assign.value);
    Rvariable* var = get_var(interpreter.env, tname);

    if (var == NULL){
        Literal * tmp = newLiteral("string", strdup(value->type),1);
        Token *type = newToken(TYPE, value->type, tmp, tname->line,1);
        Expr * literal_expr = newLiteralExpr(value);
        Stmt* stmt = newVarStmt(type, tname, literal_expr);
        execute(stmt);

        free(literal_expr);
        free(stmt);
        freeToken(type);
        // freeLiteral(tmp);
    } else {
        update_var_from_Literal(var, value);
        freeLiteral(value,0);
        freeLiteral(tname->literal,0);
    }
    return NULL;
}

void * interpret_Expr(Stmt* stmt){
    evaluate(stmt->stmt.expression.expression);
    return NULL;
}

void *interpret_Block(Stmt* stmt){
    Environment* prev = interpreter.env;
    List_Stmt *stmts = stmt->stmt.block.statements;
    Node_Stmt *current = stmts->head;

    interpreter.env = newEnvironment(interpreter.env);
    while (current != NULL){
        execute(current->data);
        current = current->next;
    }
    freeEnvironment(interpreter.env);
    interpreter.env = prev;
    return NULL;
}

// void *interpret_FunDeclaration(Stmt* stmt){
//     Token* name = stmt->stmt.function.name;
//     List_Stmt* params = stmt->stmt.function.params;
//     List_Stmt* body = stmt->stmt.function.body;

//     Callable* function = newCallable(params, body);
    
//     Literal* value = newLiteral("function", (void*)function);
//     Expr* initializer = newLiteralExpr(value);
//     Literal* ltype = newLiteral("string", strdup("function"));
//     Token* type = newToken(TYPE, "function", ltype, 0);
//     Stmt* varStmt = newVarStmt(type, name, initializer);
//     execute(varStmt);

//     freeToken(type);
//     freeStmt(varStmt);


//     return NULL;
// }

void Interpret(List_Stmt *stmts){
    Node_Stmt *current = stmts->head;
    while (current != NULL){
        execute(current->data);
        current = current->next;
    }
    return;
}

void interpreter_init(){
    interpreter.expr_visitor = (ExprVisitor*)malloc(sizeof(ExprVisitor));
    interpreter.stmt_visitor = (StmtVisitor*)malloc(sizeof(StmtVisitor));

    interpreter.expr_visitor->visitBinary    = interpret_Binary;
    interpreter.expr_visitor->visitGrouping  = interpret_Grouping;
    interpreter.expr_visitor->visitLiteral   = interpret_Literal;
    interpreter.expr_visitor->visitUnary     = interpret_Unary;
    interpreter.expr_visitor->visitVariable  = interpret_Variable;
    interpreter.expr_visitor->visitAssign    = interpret_VarAssign;
    interpreter.expr_visitor->visitCall      = interpret_Call;

    interpreter.stmt_visitor->visitExpression = interpret_Expr;
    interpreter.stmt_visitor->visitPrint      = interpret_Print;
    interpreter.stmt_visitor->visitVar        = interpret_VarDeclaration;
    interpreter.stmt_visitor->visitBlock      = interpret_Block;

    interpreter.env = newEnvironment(NULL);
    return;
}

void freeInterpreter(){
    free(interpreter.expr_visitor);
    free(interpreter.stmt_visitor);
    freeEnvironment(interpreter.env);
    return;
}

