#include <stdio.h>
#include <stdlib.h>

void LCG(int a, int c,int mod, int seed)
{
    for(size_t i = 0; i < 10; i++){
        seed = (a * seed + c) % mod;
        printf("%d ",seed);
    }    
}

int main()
{
    
    system("pause");
    return 0;
}