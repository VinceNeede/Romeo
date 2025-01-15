#include "ASTPrinter.h"

int main(){
    Expr *expr = 
        newBinaryExpr(
            newUnaryExpr(
                newToken(MINUS, "-", NULL, 1),
                newLiteralExpr(newToken(NUMBER, "123", NULL, 1)
                )
            ),
            newToken(STAR, "*", NULL, 1),
            newGroupingExpr(
                newLiteralExpr(newToken(NUMBER, "45.67", NULL, 1)
                )
            )
        );
    // Expr*expr = newGroupingExpr(newLiteralExpr(newToken(NUMBER,"123",NULL,1)));
    ASTPrinter(expr);
    freeExpr(expr);
    return 0;
}