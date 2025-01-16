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
        switch (token->literal->type){
            case C_INT:
                sprintf(literalStr, "%d", *((int*)token->literal->data));
                break;
            case C_DOUBLE:
                sprintf(literalStr, "%lf", *((double*)token->literal->data));
                break;
            case C_STRING:
                return strdup((char*)token->literal->data);
        }
        return strdup(literalStr);
    } else {
        res = (char*)malloc(strlen(token->lexeme) + 1);
        sprintf(res, "%s", token->lexeme);
    }
    return res;
}

Literal *newLiteral(ctypes type, void *data){
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