#include "interpreter.h"
#include "environment.h"

struct {
    ExprVisitor* expr_visitor;
    StmtVisitor* stmt_visitor;
    Environment* env;
} interpreter;

Literal *evaluate(Expr* expr){
    return (Literal *)ExprAccept(interpreter.expr_visitor, expr);
}

void execute(Stmt* stmt){
    StmtAccept(interpreter.stmt_visitor, stmt);
    return;
}

void* interpret_Literal(Expr* expr){
    return (void*)expr->expr.literal.value->literal;
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
            switch (rightValue->type){
                case C_INT:
                    int * value = (int*)malloc(sizeof(int));
                    *value = -1 * (*((int*)rightValue->data));
                    res = newLiteral(C_INT, (void*)value);
                    break;
                case C_DOUBLE:
                    double * doubleValue = (double*)malloc(sizeof(double));
                    *doubleValue = -1 * (*((double*)rightValue->data));
                    res = newLiteral(C_DOUBLE, (void*)doubleValue);
                    break;
                default:
                    fprintf(stderr, "Invalid type for unary minus\n");
                    exit(1);
            }
            break;
        case BANG:
            if (rightValue->type == C_INT){
                int * value = (int*)malloc(sizeof(int));
                *value =  !(*((int*)rightValue->data));
                res = newLiteral(C_INT, (void*)value);
                break;
            } else {
                fprintf(stderr, "Invalid type for unary bang\n");
                exit(1);
            }
            break;
    }
    freeLiteral(rightValue);
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
    if (leftValue->type == C_INT && rightValue->type == C_INT){
        int* value = (int*)malloc(sizeof(int));
        int leftv = *((int*)leftValue->data);
        int rightv = *((int*)rightValue->data);
        oper_int(value, leftv, rightv, operatorT->type);
        res = newLiteral(C_INT, (void*)value);
    }
    else{
        double* value = (double*)malloc(sizeof(double));
        double leftv, rightv;
        if (leftValue->type == C_INT){
            leftv = (double)*((int*)leftValue->data);
        } else {
            leftv = *((double*)leftValue->data);
        }
        if (rightValue->type == C_INT){
            rightv = (double)*((int*)rightValue->data);
        } else {
            rightv = *((double*)rightValue->data);
        }
        oper_double(value, leftv, rightv, operatorT->type);
        res = newLiteral(C_DOUBLE, (void*)value);
    }
    freeLiteral(leftValue);
    freeLiteral(rightValue);
    return (void*) res;
}

void * interpret_Print(Stmt* stmt){
    Literal* value = evaluate(stmt->stmt.print.expression);
    switch (value->type){
        case C_INT:
            printf("%d\n", *((int*)value->data));
            break;
        case C_DOUBLE:
            printf("%f\n", *((double*)value->data));
            break;
        case C_STRING:
            printf("%s\n", (char*)value->data);
            break;
        default:
            fprintf(stderr, "Invalid type for print\n");
            exit(1);
    }
    freeLiteral(value);
    return NULL;
}

void *interpret_Variable(Expr* expr){
    Token* tname = expr->expr.variable.name;
    Rvariable* var = searchHT_var(interpreter.env->vars, tname);
    if (var == NULL){
        fprintf(stderr, "Variable %s not found\n", (char*)tname->literal->data);
        exit(1);
    }
    freeLiteral(tname->literal);
    return (void*)var_to_literal(var);
}

void * interpret_VarDeclaration(Stmt* stmt){
    Token* tname = stmt->stmt.var.name;
    Literal* value;

    if (stmt -> stmt.var.initializer!= NULL ) value = evaluate(stmt->stmt.var.initializer);
    else value = newLiteral(C_INT, (void*)0);
    Rvariable* var = newRvariable_from_Literal(tname, value);
    if (searchHT_var(interpreter.env->vars, var -> name)!=NULL){
        fprintf(stderr, "Variable %s already exists\n", (char*)var-> name->literal->data);
        exit(1);
    }
    addHT_var(interpreter.env->vars, var, 0);

    free(value);
    // free(tname->literal);           //either here or in newRvariable_from_Token
    return NULL;
}

void * interpret_VarAssign(Expr* expr){
    Token* tname = expr->expr.assign.name;
    Literal* value = evaluate(expr->expr.assign.value);
    Rvariable* var = searchHT_var(interpreter.env->vars, tname);
    if (var == NULL){
        fprintf(stderr, "Variable %s not found\n", (char*)tname->literal->data);
        exit(1);
    }
    update_var_from_Literal(var, value);
    return (void*)value;
}

void * interpret_Expr(Stmt* stmt){
    evaluate(stmt->stmt.expression.expression);
    return NULL;
}

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

    interpreter.stmt_visitor->visitExpression = interpret_Expr;
    interpreter.stmt_visitor->visitPrint      = interpret_Print;
    interpreter.stmt_visitor->visitVar        = interpret_VarDeclaration;

    interpreter.env = newEnvironment(NULL);
    return;
}

void freeInterpreter(){
    free(interpreter.expr_visitor);
    free(interpreter.stmt_visitor);
    freeEnvironment(interpreter.env);
    return;
}

