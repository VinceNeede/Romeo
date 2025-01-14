#ifndef LINKLIST_Token_H
#define LINKLIST_Token_H
#include "token.h"
void free_Token(Token* item);
typedef struct node_Token {
    Token* data;
    struct node_Token *next;
} Node_Token;

Node_Token* newNode_Token(Token* data);
void freeNode_Token(Node_Token *node);

typedef struct List_Token {
    Node_Token *head;
    int size;
} List_Token;

List_Token* newList_Token();
void add_Token(List_Token *list, Token* data);
Token* getIndex_Token(List_Token *list, int index);
void freeList_Token(List_Token *list);
#endif // LINKLIST_Token_H
