#include "ASTPrinter.h"
#include "LinkList_char.h"
#include "string_utilities.h"

Visitor * strVisitor;
char* str_accept(Expr* expr){
    return (char *)accept(strVisitor, expr);
}

void* str_visitBinary(Expr* expr){
    List_char* StringerBuilder = newList_char();
    char* tmp=NULL;
    add_char(StringerBuilder, '(');
    tmp = TokenToString(expr->expr.binary.operatorT);
    concat_string_to_list(StringerBuilder, tmp);
    free(tmp);
    add_char(StringerBuilder, ' ');
    tmp = str_accept(expr->expr.binary.left);
    concat_string_to_list(StringerBuilder, tmp);
    free(tmp);
    add_char(StringerBuilder, ' ');
    tmp = str_accept(expr->expr.binary.right);
    concat_string_to_list(StringerBuilder, tmp);
    free(tmp);
    add_char(StringerBuilder, ')');
    char* res = charListToString(StringerBuilder);
    freeList_char(StringerBuilder);
    return (void*)res;
}
void* str_visitGrouping(Expr* expr){
    List_char* StringerBuilder = newList_char();
    char *tmp = NULL;
    add_char(StringerBuilder, '(');
    concat_string_to_list(StringerBuilder, "group ");
    tmp = str_accept(expr->expr.grouping.expression);
    concat_string_to_list(StringerBuilder, tmp);
    free(tmp);
    add_char(StringerBuilder, ')');
    char* res = charListToString(StringerBuilder);
    freeList_char(StringerBuilder);
    return (void*)res;
}
void* str_visitLiteral(Expr* expr){
    return (void*)TokenToString(expr->expr.literal.value);
}
void* str_visitUnary(Expr* expr){
    List_char* StringerBuilder = newList_char();
    char* tmp = NULL;
    add_char(StringerBuilder, '(');
    tmp = TokenToString(expr->expr.unary.operatorT);
    concat_string_to_list(StringerBuilder, tmp);
    free(tmp);
    add_char(StringerBuilder, ' ');
    tmp = str_accept(expr->expr.unary.right);
    concat_string_to_list(StringerBuilder, tmp);
    free(tmp);
    add_char(StringerBuilder, ')');
    char* res = charListToString(StringerBuilder);
    freeList_char(StringerBuilder);
    return (void*)res;
}


void ASTPrinter(Expr *expr){
    strVisitor = (Visitor*)malloc(sizeof(Visitor));
    if (strVisitor == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    strVisitor->visitBinary = str_visitBinary;
    strVisitor->visitGrouping = str_visitGrouping;
    strVisitor->visitLiteral = str_visitLiteral;
    strVisitor->visitUnary = str_visitUnary;

    char* result = str_accept(expr);
    printf("%s\n",result);
    free(result);
    free(strVisitor);
}


