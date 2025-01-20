#include"token.h"
#include "callable.h"
#include "LinkList_string.h"

Token * newToken(TokenType type, char* lexeme, Literal *literal, int line, int is_copy){
    Token * token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->lexeme = (char*)malloc(strlen(lexeme) + 1);
    strncpy(token->lexeme, lexeme, strlen(lexeme));
    token->lexeme[strlen(lexeme)]='\0';
    token->literal = literal;
    token->line = line;
    token -> is_copy = is_copy;
    return token;
}
void freeToken(Token* token){
    // if (token->literal != NULL){
    //     freeLiteral(token->literal);
    // }
    if (!token->is_copy) return;
    if (token->lexeme != NULL){
        free(token->lexeme);
    }
    free(token);
    
}
char *TokenToString(Token* token){
    char *res;
    if (token->literal != NULL){
        char literalStr[32]; // Buffer to hold the string representation of the literal
        char *type = token->literal->type;
        if (cmp_types(type,"int")){
            sprintf(literalStr, "%d", *((int*)token->literal->data));
        } else if (cmp_types(type,"double")){
            sprintf(literalStr, "%lf", *((double*)token->literal->data));
        } else if (cmp_types(type,"string")){
            return strdup((char*)token->literal->data);
        }
        res = strdup(literalStr);
    } else {
        res = (char*)malloc(strlen(token->lexeme) + 1);
        sprintf(res, "%s", token->lexeme);
    }
    return res;
}

Literal *newLiteral(char *type, void *data, int free_data){
    Literal * literal = (Literal*)malloc(sizeof(Literal));
    literal->type = strdup(type);
    literal->data = data;
    literal->free_data = free_data;
    return literal;
}
void freeLiteral(Literal *literal, int free_data){
    if (literal == NULL) return;
    if (literal->type != NULL && literal->data != NULL && (literal->free_data || free_data)){
        if (cmp_types(literal->type, "string")) free((char*)literal->data);
        else if (cmp_types(literal->type, "int")) free((int*)literal->data);
        else if (cmp_types(literal->type, "double")) free((double*)literal->data);
        else if (cmp_types(literal->type, "function")) freeCallable((Callable*)literal->data);
        else if (cmp_types(literal->type, "key_field")) free_key_field((key_field*)literal->data);
        else fprintf(stderr, "cannot free void*\n");
    }
    if (literal->type != NULL) free(literal->type);
    // if (literal->data!=NULL) {
    //     fprintf(stderr, "freeing void* data\n");
    //     free(literal->data);
    // }
    free(literal);
}

int *intdup(int *value){
    int *res = (int*)malloc(sizeof(int));
    *res = *value;
    return res;
}

double * doubledup(double *value){
    double *res = (double*)malloc(sizeof(double));
    *res = *value;
    return res;
}

Literal *copyLiteral(Literal *literal){
    if (cmp_types(literal->type, "string")){
        return newLiteral(literal->type, strdup((char*)literal->data),1);
    } else if (cmp_types(literal->type, "int")){
        return newLiteral(literal->type, intdup((int*)literal->data),1);
    } else if (cmp_types(literal->type, "double")){
        return newLiteral(literal->type, doubledup((double*)literal->data),1);
    } else if (cmp_types(literal->type, "function")){
        return newLiteral(literal->type, copyCallable((Callable*) literal->data),1);
    } else if (cmp_types(literal->type, "key_field")){
        key_field *copy = (key_field*)malloc(sizeof(key_field));
        copy->type =((key_field*)literal->data)->type;
        switch (copy->type){
            case NAME:
                copy->field.name = strdup(((key_field*)literal->data)->field.name);
                break;
            case FUNCTION:
                copy->field.function.name = strdup(((key_field*)literal->data)->field.function.name);
                copy->field.function.args_types = copyList_string(((key_field*)literal->data)->field.function.args_types);
                break;
        }
        return newLiteral(literal->type, copy ,1);
    }
    return NULL;
}

Token *copyToken(const Token *token){
    Literal* lit = NULL;
    if (token->literal!= NULL) lit = copyLiteral(token->literal);
    return newToken(token->type, token->lexeme, lit, token->line, 1);
}