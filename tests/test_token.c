#include "token.h"

int main(){
    Token * t = newToken(NUMBER,"123",NULL,0);
    char *s = TokenToString(t);
    printf("token: %s\n",s);
    freeToken(t);
    free(s);
    return 0;
}