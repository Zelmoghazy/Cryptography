#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

char Alphabet[]= {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

int arith_mod(int x, int y) 
{
	if (-13/5 == -2          &&     // Check division truncates to zero
	    (x < 0) != (y < 0)   &&     // Check x and y have different signs
        (x % y) != 0)
		return x%y + y;
	else
		return x%y;
}

void attack (char *text)
{
    printf("\nBrute Force Attack :\n");
    printf("--------------------\n");
    char test[26]; 
    for(int shift = 1; shift < 26;++shift)
    {
        strcpy(test,text);
        for(char *p = test;*p;++p)
        {
            if(*p == ' '){
                continue;
            }
            int n = (*p - 'A') - shift;
            if(n < 0){
                *p = Alphabet[arith_mod(n,26)];
            }else{
                *p = Alphabet[n];
            }
            printf("%c",*p);
        }
        printf(" %d\n",shift);
    }

}

void decrypt (char *text, int shift)
{
    for(;*text;++text)
    {
        if(*text == ' '){
            continue;
        }
        int n = (*text - 'A') - shift;
        if(n < 0){
            *text = Alphabet[arith_mod(n,26)];
        }else{
            *text = Alphabet[n];
        }
    }
}

void encrypt (char *text, int shift)
{
    for(;*text;++text)
    {
        if(*text == ' '){
            continue;
        }
        *text = (char)tolower((int)*text);
        int n = (*text - 'a') + shift;
        if(n > 25){
            *text = Alphabet[n % 26];
        }else{
            *text = Alphabet[n];
        }
    }
}

int main()
{
    char text[] = "Caesar Cipher";
    int key = 3;
    
    attack("GCUAVQDTGCM");

    system("pause");
    return 0;
}