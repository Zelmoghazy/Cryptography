#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void LCG(uint64_t a,uint64_t c,uint64_t mod, uint64_t seed)
{
    for(size_t i = 0; i < 10; i++){
        seed = (a * seed + c) % mod;
        printf("%llu ",seed);
    }    
}

int main()
{
    uint64_t a = 16806;   
    uint64_t mod = 131071;   // 2^{17}-1
    uint64_t c = 0;   
    system("pause");
    return 0;
}