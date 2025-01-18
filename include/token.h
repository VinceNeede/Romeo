#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

typedef enum {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    TOKEN_EOF
} TokenType;

typedef struct {
    Rtype type;
    void * data;
} Literal;

typedef struct {
    TokenType type;
    char* lexeme;
    Literal *literal;
    int line;
} Token;

Token *newToken(TokenType type, char* lexeme, Literal *literal, int line);
void freeToken(Token* token);
char *TokenToString(Token* token);

Literal *newLiteral(Rtype type, void *data);
void freeLiteral(Literal *literal);
#endif //TOKEN_H