#include "string_utilities.h"


void printCharList(List_char *list) {
    int i = 0;
    for (i = 0; i < list->size; i++) {
        printf("%c", getIndex_char(list, i));
    }
    printf("\n");
}

char* charListToString(List_char *list) {
    char* result = (char*)malloc(list->size + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    int i = 0;
    for (i = 0; i < list->size; i++) {
        result[i] = getIndex_char(list, i);
    }
    result[i] = '\0';
    return result;
}

void concat_string_to_list(List_char *list, const char* str) {
    int i = 0;
    while (str[i]) {
        add_char(list, str[i]);
        i++;
    }
}

List_string* splitString(const char* str, char delimiter) {
    List_string* res = newList_string();
    List_char* StringBuilder;;
    char *tmp = NULL;
    int i = 0;
    while(1){
        StringBuilder = newList_char();
        while (str[i] && str[i]!=delimiter) {
            add_char(StringBuilder, str[i]);
            i++;
        }
        if (str[i] == delimiter){
            tmp = charListToString(StringBuilder);
            add_string(res, tmp);
            freeList_char(StringBuilder);
            i++;
        }
        if (str[i] == '\0'){
            tmp = charListToString(StringBuilder);
            add_string(res, tmp);
            freeList_char(StringBuilder);
            break;
        }
    }
    return res;
}


char* trim(char* str){
    List_char *StringBuilder;
    int i = 0;
    int j = 0;
    int k = 0;
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') i++;
    if (!str[i]){
        return NULL;
    }
    j=i;

    while (str[++j]);
    j--;

    while (str[j] == ' ' || str[j] == '\t' || str[j] == '\n') j--;
    j++;
    StringBuilder = newList_char();
    for (k = i; k < j; k++) {
        add_char(StringBuilder, str[k]);
    }
    char* res = charListToString(StringBuilder);
    freeList_char(StringBuilder);
    return res;
}

char* lower(const char* str) {
    if (str == NULL) {
        fprintf(stderr, "Null string passed to lower\n");
        exit(1);
    }
    size_t len = strlen(str);
    char* result = (char*)malloc(len + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    for (size_t i = 0; i < len; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            result[i] = str[i] + ('a' - 'A');
        } else {
            result[i] = str[i];
        }
    }
    result[len] = '\0'; // Ensure null-termination
    return result;
}

char* upper(const char* str) {
    if (str == NULL) {
        fprintf(stderr, "Null string passed to upper\n");
        exit(1);
    }
    size_t len = strlen(str);
    char* result = (char*)malloc(len + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    for (size_t i = 0; i < len; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            result[i] = str[i] - ('a' - 'A');
        } else {
            result[i] = str[i];
        }
    }
    result[len] = '\0'; // Ensure null-termination
    return result;
}

char* replace(const char *str, char old, char new){
    if (str == NULL) {
        fprintf(stderr, "Null string passed to replace\n");
        exit(1);
    }

    char *res = (char*)malloc(strlen(str) + 1);
    if (res == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    char *head = res;
    if (!new){
        while ((*res++ = (*str++ == old ? *str++: *(str-1))));
        return head;
    }
    while ((*res++ = (*str++ == old ? new: *(str-1))));
    return head;
}

char *literal_string_to_string(char *lit){
    size_t len = strlen(lit);
    char *res = (char*)malloc(len + 1);
    if (res == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    strcpy(res, lit);
    res[len] = '\0';
    return res;
}