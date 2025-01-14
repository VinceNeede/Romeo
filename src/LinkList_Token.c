#include <stdio.h>
#include <stdlib.h>
#include "LinkList_Token.h"

Node_Token* newNode_Token(Token* data) {
    Node_Token *node = (Node_Token *)malloc(sizeof(Node_Token));
    if (node == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    node->data = data;
    node->next = NULL;
    return node;
}

void freeNode_Token(Node_Token *node) {
    free_Token(node->data);
    free(node);
}

List_Token* newList_Token() {
    List_Token *list = (List_Token *)malloc(sizeof(List_Token));
    if (list == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    list->head = NULL;
    list->size = 0;
    return list;
}

void add_Token(List_Token *list, Token* data) {
    Node_Token *node = newNode_Token(data);
    if (list->head == NULL) {
        list->head = node;
    } else {
        Node_Token *current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = node;
    }
    list->size++;
}

Token* getIndex_Token(List_Token *list, int index) {
    if (index < 0 || index >= list->size) {
        printf("Index out of bounds\n");
        exit(1);
    }
    Node_Token *current = list->head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    return current->data;
}

void freeList_Token(List_Token *list) {
    Node_Token *current = list->head;
    while (current != NULL) {
        Node_Token *next = current->next;
        freeNode_Token(current);
        current = next;
    }
    free(list);
}
