// The scanner is responsible for reading the input file and returning a lsit of tokens.
#include "scanner.h"
#include "../lib/keyword.c"

Scanner* newScanner(const char* source) {
    Scanner* scanner = (Scanner*)malloc(sizeof(Scanner));
    scanner->source = source;
    scanner->source_length = strlen(source);
    scanner->start = 0;
    scanner->current = 0;
    scanner->line = 1;
    scanner->tokens = newList_Token();
    return scanner;
}

void freeScanner(Scanner* scanner) {
    freeList_Token(scanner->tokens);
    free(scanner);
}

int isAtEnd(Scanner* scanner) {
    return scanner->current >= scanner->source_length;
}

int match(Scanner* scanner, char expected) {
    if (isAtEnd(scanner)) return 0;
    if (scanner->source[scanner->current] != expected) return 0;
    scanner->current++;
    return 1;
}
int peek(Scanner* scanner) {
    if (isAtEnd(scanner)) return '\0';
    return scanner->source[scanner->current];
}
int peekNext(Scanner* scanner) {
    if (scanner->current + 1 >= scanner->source_length) return '\0';
    return scanner->source[scanner->current + 1];
}


void addToken_toScanner(Scanner* scanner, TokenType type, Literal* literal) {
    char* lexeme = (char*)malloc(scanner->current - scanner->start + 1);
    strncpy(lexeme, scanner->source + scanner->start, scanner->current - scanner->start);
    lexeme[scanner->current - scanner->start] = '\0';
    add_Token(scanner->tokens, newToken(type, lexeme, literal, scanner->line));
    free(lexeme);
}

void literal_string(Scanner* scanner){
    while (peek(scanner) != '"' && !isAtEnd(scanner)) {
        if (peek(scanner) == '\n') scanner->line++;
        scanner->current++;
    }
    if (isAtEnd(scanner)) {
        fprintf(stderr,"Unterminated string.\n");           // Error handling
        return;
    }

    scanner->current++;
    char* value = (char*)malloc(scanner->current - scanner->start - 2);
    //trim surrounding quotes
    strncpy(value, scanner->source + scanner->start + 1, scanner->current - scanner->start - 2);
    value[scanner->current - scanner->start - 2] = '\0';
    addToken_toScanner(scanner, STRING, newLiteral("string", value));
}

int isDigit(char c) {
    return c >= '0' && c <= '9';
}
int isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}
int isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

void literal_number(Scanner* scanner){
    char *type = "int";
    while (isDigit(peek(scanner))) scanner->current++;
    if (peek(scanner) == '.' && isDigit(peekNext(scanner))) {
        type = "double";
        scanner->current++;
        while (isDigit(peek(scanner))) scanner->current++;
    }
    char* value = (char*)malloc(scanner->current - scanner->start + 1);
    strncpy(value, scanner->source + scanner->start, scanner->current - scanner->start);
    value[scanner->current - scanner->start] = '\0';
    if (strcmp(type, "int") == 0){
        int* intValue = (int*)malloc(sizeof(int));
        *intValue = atoi(value);
        addToken_toScanner(scanner, NUMBER, newLiteral(type, (void*)intValue));
    } else {
        double* doubleValue = (double*)malloc(sizeof(double));
        *doubleValue = atof(value);
        addToken_toScanner(scanner, NUMBER, newLiteral(type, (void*)doubleValue));
    }
    free(value);
}

void literal_identifier(Scanner* scanner){
    while(isAlphaNumeric(peek(scanner))) scanner->current++;
    char* value = (char*)malloc(scanner->current - scanner->start + 1);
    strncpy(value, scanner->source + scanner->start, scanner->current - scanner->start);
    value[scanner->current - scanner->start] = '\0';
    //check if it is a keyword
    const struct keyword* key = get_keyword(value, scanner->current - scanner->start);
    if (key != NULL){
        addToken_toScanner(scanner, key->value, NULL);
        free(value);
        return;
    }
    Rtype *type = searchHT_Rtype(types, value);
    if ( type != NULL){
        addToken_toScanner(scanner, TYPE, newLiteral("string", (void*)value));
    }
    else{
        addToken_toScanner(scanner, IDENTIFIER, newLiteral("string", (void*)value));
    }
}

void scanToken(Scanner* scanner){
    char c = scanner->source[scanner->current++];
    switch (c) {
        case '(': addToken_toScanner(scanner, LEFT_PAREN  ,NULL); break;
        case ')': addToken_toScanner(scanner, RIGHT_PAREN ,NULL); break;
        case '{': addToken_toScanner(scanner, LEFT_BRACE  ,NULL); break;
        case '}': addToken_toScanner(scanner, RIGHT_BRACE ,NULL); break;
        case ',': addToken_toScanner(scanner, COMMA       ,NULL); break;
        case '.': addToken_toScanner(scanner, DOT         ,NULL); break;
        case '-': addToken_toScanner(scanner, MINUS       ,NULL); break;
        case '+': addToken_toScanner(scanner, PLUS        ,NULL); break;
        case ';': addToken_toScanner(scanner, SEMICOLON   ,NULL); break;
        case '*': addToken_toScanner(scanner, STAR        ,NULL); break; 
        //comparison operators
        case '!':
            addToken_toScanner(scanner, match(scanner,'=') ? BANG_EQUAL : BANG, NULL);
            break;
        case '=':
            addToken_toScanner(scanner, match(scanner,'=') ? EQUAL_EQUAL : EQUAL, NULL);
            break;
        case '<':
            addToken_toScanner(scanner, match(scanner,'=') ? LESS_EQUAL : LESS, NULL);
            break;
        case '>':
            addToken_toScanner(scanner, match(scanner,'=') ? GREATER_EQUAL : GREATER, NULL);
        break;
        //longer tokens
        case '/':
            if (match(scanner, '/')) {
                while (peek(scanner) != '\n' && !isAtEnd(scanner)) {
                    scanner->current++;
                }
            } else {
                addToken_toScanner(scanner, SLASH, NULL);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            scanner->line++;
            break;
        case '"':literal_string(scanner); break;
        default:
            if (isDigit(c)) literal_number(scanner);
            else if (isAlpha(c)) literal_identifier(scanner);
            else {
                fprintf(stderr,"Unexpected character.\n");           // Error handling
                return;
            }
    }
}

List_Token* scanTokens(Scanner* scanner) {
    while (!isAtEnd(scanner)) {
        scanner->start = scanner->current;
        scanToken(scanner);
    }
    add_Token(scanner->tokens, newToken(TOKEN_EOF, "", NULL, scanner->line));
    return scanner->tokens;
}

