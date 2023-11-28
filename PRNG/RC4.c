#include <stdio.h>
#include <stdlib.h>

#define RCL 256

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void RC4(int *K,int keylen, int n)
{
    int S[RCL];
    int T[RCL];

    for (size_t i = 0; i < RCL; i++)
    {
        S[i] = i;
        T[i] = K[i % keylen];
    }

    int j = 0;
    for (size_t i = 0; i < RCL; i++)
    {
        j = (j + S[i] + T[i]) % RCL;
        swap(&S[i],&S[j]);
    }
    int i = 0;
    j = 0;
    while(i < n){
        i = (i+1) % RCL;
        j = (j+S[i]) % RCL;
        swap(&S[i], &S[j]);
        int t = (S[i] + S[j]) % RCL;
        int k = S[t];
        printf("%d ",k);
    }
}

int main()
{
    int plain[] = {'H','e','l','l','o','W','o','r','l','d'};
    RC4(plain,sizeof(plain)/sizeof(plain[0]),10);
    printf("\n");
    return 0;
}