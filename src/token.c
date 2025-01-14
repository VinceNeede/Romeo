#include"token.h"

Token * newToken(TokenType type, char* lexeme, void *literal, int line){
    Token * token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->lexeme = (char*)malloc(strlen(lexeme) + 1);
    strncpy(token->lexeme, lexeme, strlen(lexeme));
    token->lexeme[strlen(lexeme)]='\0';
    token->literal = literal;
    token->line = line;
    return token;
}
void free_Token(Token* token){
    if (token->literal != NULL){
        free(token->literal);
    }
    if (token->lexeme != NULL){
        free(token->lexeme);
    }
    free(token);
    
}
char *TokenToString(Token* token){
    char * res;
    if (token->literal != NULL){
        res = (char*)malloc(strlen(token->lexeme) + strlen(token->literal) + 2);
        sprintf(res, "%s %s", token->lexeme, (char*)token->literal);
    } else{
        res = (char*)malloc(strlen(token->lexeme) + 1);
        sprintf(res, "%s", token->lexeme);
    }
    return res;
}