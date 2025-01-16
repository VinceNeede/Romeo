#include "interpreter.h"

Visitor* interpreter = NULL;
Literal *evaluate(Expr* expr){
    return (Literal *)accept(interpreter, expr);
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


Literal* Interpret(Expr *expr){
    interpreter = (Visitor*)malloc(sizeof(Visitor));
    if (interpreter == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    interpreter->visitBinary    = interpret_Binary;
    interpreter->visitGrouping  = interpret_Grouping;
    interpreter->visitLiteral   = interpret_Literal;
    interpreter->visitUnary     = interpret_Unary;

    Literal* result = evaluate(expr);
    free(interpreter);
    return result;
}


