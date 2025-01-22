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
    AMPERSAND,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL, ARROW,

    // Literals.
    IDENTIFIER, STRING, NUMBER, TYPE,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    TOKEN_EOF
} TokenType;

typedef struct Literal{
    char *type;
    void * data;
    int free_data;
} Literal;

typedef struct {
    TokenType type;
    char* lexeme;
    Literal *literal;
    int line;
    int is_copy;
} Token;

Token *newToken(TokenType type, char* lexeme, Literal *literal, int line, int is_copy);
void freeToken(Token* token);
char *TokenToString(Token* token);

Literal *newLiteral(char *type, void *data, int free_data);
void freeLiteral(Literal *literal, int free_data);
Literal *copyLiteral(Literal *literal);
Token *copyToken(const Token *token);
#endif //TOKEN_H