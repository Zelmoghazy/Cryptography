#include "toy.h"

int main(void)
{
	short A[TK_K * TK_K * TK_N]; // 36 -> publick key
	short t[TK_K * TK_N]; // 12 -> public key
	short s[TK_K * TK_N]; // 12  -> private key

    toy_gen(A, t, s);

	for (size_t msg = 0; msg < 16; msg++)
	{
        /* Cipher text -> (u,v) */
		short u[TK_K * TK_N];
		short v[TK_N];

		toy_enc(A,t, msg,u,v);

        short m2 = toy_dec(s, u, v);

        printf("%2d %2d ",msg,m2);
        print_bin(msg, TK_N);
        printf(" ");
        print_bin(m2, TK_N);
        printf(" ");
        print_bin(msg^m2, TK_N); // diff
        printf("\n");
	}
    printf("done\n");
    return 0;
}