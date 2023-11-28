#include <stdio.h>
#include <string.h>
#include <assert.h>

char Alphabet[]= {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

void encrypt (char *text, char *key)
{
    size_t n = strlen(text);
    size_t m = strlen(key);
    assert(n>=m);
    for (size_t i = 0; i < n; i++){
        text[i] = ((text[i]-'a' + key[i%m]-'a') % 26)+'a';
    }
}

int main()
{
    char text[] = "wearediscoveredsaveyourself";
    char key[]  = "deceptive";

    encrypt(text,key);
    printf("%s\n",text);
    
    return 0;
}