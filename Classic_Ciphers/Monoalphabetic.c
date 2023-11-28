#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

char Alphabet[]= {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

void Swap(char *a, char *b)
{
    char temp = *a;
    *a = *b;
    *b = temp;
}

void Shuffle(char *A, int n)
{
    if (n > 1)
    {
        for (int i = 0; i < n - 1; i++)
        {
            int j = rand() % (i + 1);
            Swap(&A[j], &A[i]);
        }
    }
}

void Decrypt(char *text, const char *encoding)
{
    int Indices[26];
    for(int i = 0; i < 26; i++)
    {
        Indices[encoding[i]-'A']=i;
    }
    for(;*text;++text)
    {
        if(*text < 'A' || *text > 'Z'){
            continue;
        }
        *text=Alphabet[Indices[*text-'A']];
    }
}

void Encrypt(char *text, const char *encoding)
{
    for(;*text;++text)
    {
        *text = (char)tolower((int)*text);
        if(*text < 'a' || *text > 'z'){
            continue;
        }
        *text = encoding[(*text - 'a')];
    }
}

int main(void)
{
    srand((unsigned int)time(NULL));

    char Encoding[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    int n = sizeof(Encoding) / sizeof(char);

    Shuffle(Encoding,n);    


    char text[] = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";

    Encrypt(text,Encoding);
    printf("%s\n",text);

    printf("\n");
    Decrypt(text,Encoding);
    printf("%s\n",text);

    return 0;
}
