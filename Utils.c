#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void swap(int64_t *a, int64_t *b)
{
    int64_t temp = *a;
    *a = *b;
    *b = temp;
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

int64_t extended_gcd(int64_t a, int64_t b, int64_t *inv)
{
    if (b > a){
        swap(&a,&b);
    }
    *inv = 0;
    int64_t t2 = 1;

    while (b != 0){
        int64_t rem = a % b;
        int64_t t = *inv - t2 * (a/b);
        a = b;
        b = rem;
        *inv = t2;
        t2 = t;
    }
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

int PowMod(int a, int n, int b)
{
    if (b == 1)
        return 0;
    int result = 1;
    a = a % b;
    while (n > 0){
        if (n % 2 == 1)
            result = (result * a) % b;
        a = (a * a) % b;
        n = n / 2;
    }
    return result;
}


int main(void)
{
    printf("%d\n",phi(209));
}