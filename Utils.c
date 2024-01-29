#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define iterations 10

#define RAND(start,end) (rand() % ((end) + 1 - (start)) + (start))

void swap(int64_t *a, int64_t *b)
{
    int64_t temp = *a;
    *a = *b;
    *b = temp;
}

int arith_mod(int x, int y) 
{
	if (-13/5 == -2          &&     // Check division truncates to zero
	    (x < 0) != (y < 0)   &&     // Check x and y have different signs
        (x % y) != 0)
		return x%y + y;
	else
		return x%y;
}

bool IsPrime(uint64_t n)
{
    if (n == 2 || n == 3)
        return true;

    if (n <= 1 || n % 2 == 0 || n % 3 == 0)
        return false;

    for (int i = 5; i * i <= n; i += 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}


uint64_t PowMod(uint64_t a, uint64_t n, uint64_t b)
{
    if (b == 1){
        return 0;
    }
    uint64_t result = 1;
    a = a % b;
    while (n > 0){
        if (n % 2 == 1){
            result = (result * a) % b;
        }
        a = (a * a) % b;
        n = n / 2;
    }
    return result;
}

bool isComposite(uint64_t a, uint64_t k, uint64_t q, uint64_t n)
{
    uint64_t x = PowMod(a, q, n);
    if(x == 1 || x == n-1){
        return false;
    }
    for(size_t j = 1; j < k; j++){
        x = (x*x) % n;
        if (x == n-1){
            return false;
        }
    }
    return true;
}

bool millerRabin(uint64_t n)
{
    if (n < 4){
        return n == 2 || n == 3;
    }
    uint64_t k = 0;
    uint64_t q = n - 1;
    while ((q & 1) == 0) {
        q >>= 1;
        k++;
    }
    for(size_t i = 0; i < iterations; i++){
        uint64_t a = RAND(1,n-1);
        if(isComposite(a, k, q, n)){
            return false;
        }
    }
    return true;    
}

uint64_t gcd(uint64_t a, uint64_t b)
{
    while (b != 0)
    {
        uint64_t rem = a % b;
        a = b;
        b = rem;
    }
    return a;
}

uint64_t gcd_rec(uint64_t a, uint64_t b)
{
    return (b==0)?a:gcd_rec(b,a%b);
}

uint32_t extended_gcd(uint32_t b, uint32_t a, uint32_t *inv)
{
    uint32_t modulus = a; 
    
    *inv = 0;
    uint32_t t2 = 1;

    while (b){
        uint32_t rem = a % b;
        uint32_t t = *inv - t2 * (a/b);
        a = b;
        b = rem;

        *inv = t2;
        t2 = t;
    }
    *inv = arith_mod(*inv,modulus);
    return a;
}

int phi(int n) 
{
    int result = n;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            while (n % i == 0){
                n /= i;
            }
            result -= result / i;
        }
    }
    if (n > 1){
        result -= result / n;
    }
    return result;
}


/**************************************************************************
 *   Calculates the Montgomery reduction of a*b (mod q)                   *
 *   ------------------------------------------------------------         *
 *   DIV instruction has variable latency depending on the inputs         *
 *   Thus its subject to "Timing Attacks".                                *
 *   This can be avoided by using montgomery reduction which uses         *
 *   two multiplications instead as MUL instruction                       *
 *   has fixed latency on most CPUs                                       *                     
 **************************************************************************/

uint32_t montgomery_reduction(uint32_t a, uint32_t b, uint32_t q) 
{
    uint32_t x, lo, hi;
    uint32_t invq;

    if(extended_gcd(q,(uint32_t)1<<16, &invq) != 1){
        exit(EXIT_FAILURE);
    }

    x = (uint32_t)a * b;

    // Split 32 bits into hi and lo 16 bits
    lo = x & 0xFFFF;
    hi = x >> 16;

    // Take low 16 bits of product and multiply by invq
    lo = (lo * invq) & 0xFFFF;

    // Take high 16 bits of product
    x = hi - ((lo * q) >> 16);

    return x;
}


int main(void)
{
    printf("%d\n",173584%10);

}