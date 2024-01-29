#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define naive 1
#define ntt_mul 0

#define montgomery 0
#define naive_mod 1


#define TK_K 3
#define TK_N 4

#define TK_Q 97
#define TK_Q_INV 41889
#define TK_SQRT_W 33
#define TK_W 22

#define NEG(X)  (TK_Q - (X))

void toy_gen(short* A, short* t, short* s);
void toy_enc(const short* A, const short* t, int plain, short* u, short* v);
int  toy_dec(const short* s, const short* u, const short* v);
void print_bin(short msg, short n);
static void toy_fill_small(short *buf, int n);
int bitreverse(int x, int n);
void permute_bitreverse(int *data, int n);
void ntt(int *data, int n, int forward, int anti_cyclic);
void toy_polmul_ntt(short *dst, const short *a, const short *b, int add);
static void toy_polmul_naive(short *dst, const short*a, const short *b, int add);
static void toy_mulmv(short *dst, const short *mat, const short *vec);
static void toy_mulTmv(short *dst, const short *mat, const short *vec);
static void toy_dot(short *dst, const short *v1, const short *v2);
static void toy_add(short *dst, const short *v1, const short *v2, int count, int v2_neg);

