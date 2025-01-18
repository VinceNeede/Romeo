#include"token.h"

Token * newToken(TokenType type, char* lexeme, Literal *literal, int line){
    Token * token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->lexeme = (char*)malloc(strlen(lexeme) + 1);
    strncpy(token->lexeme, lexeme, strlen(lexeme));
    token->lexeme[strlen(lexeme)]='\0';
    token->literal = literal;
    token->line = line;
    return token;
}
void freeToken(Token* token){
    // if (token->literal != NULL){
    //     freeLiteral(token->literal);
    // }
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

Literal *newLiteral(char *type, void *data){
    Literal * literal = (Literal*)malloc(sizeof(Literal));
    literal->type = type;
    literal->data = data;
    return literal;
}
void freeLiteral(Literal *literal){
    if (literal == NULL) return;
    if (literal->data!=NULL) free(literal->data);
    free(literal);
}