#ifndef STRING_UTILITIES_H
#define STRING_UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkList_char.h"
#include "LinkList_string.h"

void printCharList(List_char *list);
char* charListToString(List_char *list);
void concat_string_to_list(List_char *list, const char* str);
List_string* splitString(const char* str, char delimiter);
char* trim(char* str);
char* lower(const char* str);
char* upper(const char* str);
char* replace(const char *str, char old, char new);
#endif // STRING_UTILITIES_H