#include <stdint.h>
#include <stdio.h>

/* Generate Random Stream of bits */
void bbs_bit(uint64_t seed, uint64_t n)
{
    seed = (seed*seed) % n;   // TODO: Overflow check
    while(1){
        seed = (seed*seed) % n;
        printf("%d\n",seed %2); // LSB
    }
}

int main()
{
    /* From Textbook */
    uint64_t n = 192649;
    uint64_t seed = 101355;
    bbs_bit(seed,n);
    return 0;
}