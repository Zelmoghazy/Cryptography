#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define iterations 10
#define RAND(start,end) (rand() % ((end) + 1 - (start)) + (start))

uint64_t arith_mod(uint64_t x, uint64_t y) 
{
	if (-13/5 == -2          &&     // Check division truncates to zero
	    (x < 0) != (y < 0)   &&     // Check x and y have different signs
        (x % y) != 0)
		return x%y + y;
	else
		return x%y;
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

uint64_t extended_gcd(uint64_t b, uint64_t a, uint64_t *inv)
{
    uint64_t modulus = a; 
    
    *inv = 0;
    uint64_t t2 = 1;

    while (b){
        uint64_t rem = a % b;
        uint64_t t = *inv - t2 * (a/b);
        a = b;
        b = rem;

        *inv = t2;
        t2 = t;
    }
    *inv = arith_mod(*inv,modulus);
    return a;
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

int main(int argc, char* argv[]) 
{
	uint64_t p, q;
	uint64_t n, phin;

	uint64_t data, cipher, decrypt;

	while (1) {
		printf("Enter any two different prime numbers: ");
		scanf("%llu %llu", &p, &q);

        if (p==q){
            printf("Please Enter Different Prime Numbers.");
        }

		if (!(millerRabin(p) || millerRabin(q))){
			printf("Please enter prime numbers only...\n");
        }else{
			break;
        }
	}
	
	n = p * q;

	phin = (p - 1) * (q - 1);

	uint64_t e = 0;
	uint64_t d = 0;
	while (1) 
    {
        e = RAND(3,50);
		if (extended_gcd(e,phin,&d) == 1){
			break;
        }
	}

	printf("Value of e: %llu\nValue of d: %llu\n", e, d);

	printf("Enter some numerical data: ");
	scanf("%llu", &data);

	cipher = PowMod(data, e, n);
	printf("The cipher text is: %llu\n", cipher);

	decrypt = PowMod(cipher, d, n);
	printf("The decrypted text is: %llu\n", decrypt);
    
	return 0;
}