#include "token.h"

int main(){
    Token * t = newToken(NUMBER,"123",NULL,0);
    char *s = TokenToString(t);
    printf("token: %s\n",s);
    free_Token(t);
    free(s);
    return 0;
}