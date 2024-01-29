
#include "toy.h"
#include <stdint.h>


/* 
    n : degree of polynomials use
    k : size of vectors
    q : modulus (Max coefficients)

    => Creating public key
    public key1 (Matrix) * private key (Vector) + error (Vector) = public key2 (Vector)

                                ( A*s + e = t )

        - public key pair : (A,t)
        - private key : (s)

    Secret key just picks any specific point in the lattice described by A 
    and the small error shifts a bit from picked point getting the result t
    which is public but its very difficult to get from t -> s (SVP is NP-hard)

    1 - Plain text (m) -> Polynomial
    2 - Scale plain text
    3-  Encryption :
        => Cipher text (u,v)
            u = transpose(A)*error1 + error3           (Vector of polynomials)
            v = transpose(t)*error1 + error2 + m       (Polynomial)
    4- Decryption :
        d = v - transpose(s) * u = ... = m + error 
        ->(remove noise)-> = m
        ->(scale down)-> = m
 */

int16_t montgomery_reduce(int32_t a) {
    int32_t t;
    int16_t u;
    u = (int16_t) a * TK_Q_INV;
    t = (int32_t) u * TK_Q;
    t = a - t;
    t >>= 16;
    return (int16_t) t;
}

void print_bin(short msg, short n) {
    for (int i = n - 1; i >= 0; i--) {
        int bit = (msg >> i) & 1;
        printf("%d", bit);
    }
}

static void toy_fill_small(short *buf, int n)
{
    for(size_t k = 0; k < n; k++)
    {
        short val = rand()&3;
        val = (val>>1&1)-(val&1); // Small binomial distribution using hamming weight
        if(val<0){
            val+=TK_Q;
        }
        buf[k] =val;
    }
}

int bitreverse(int x, int n) {
    int result = 0;
    for (int i = 0; i < n; i++) {
        result <<= 1;
        result |= x & 1;
        x >>= 1;
    }
    return result;
}

void permute_bitreverse(int *data, int n) {
    int *temp = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        temp[i] = data[bitreverse(i, n)];
    }
    for (int i = 0; i < n; i++) {
        data[i] = temp[i];
    }
    free(temp);
}

void ntt(int *data, int n, int forward, int anti_cyclic) {
    int lg_n = log2(n);

    permute_bitreverse(data, n);

    if (anti_cyclic && forward) {
        int factors[n];
        for (int i = 0; i < n; i++) {
            factors[i] = (int)pow(TK_SQRT_W, i);
            #if naive_mod
            data[i] = (data[i] * factors[i]) % TK_Q;
            #endif
            #if montgomery
            data[i] = montgomery_reduce(data[i] * factors[i]);
            #endif
        }
    }

    for (int s = 1; s <= lg_n; s++) {
        int m = 1 << s;
        for (int b = 0; b < n; b += m) {
            int factor = 1;
            for (int op = 0; op < m / 2; op++) {
                int a0 = data[b + op];
                int a1 = (data[b + op + m / 2] * factor);
                data[b + op] = (a0 + a1) % TK_Q;
                data[b + op + m / 2] = (a0 - a1 + TK_Q) % TK_Q;
                factor = (factor * TK_W) % TK_Q;
            }
        }
    }
    if (anti_cyclic && !forward) { //correction for m(x)=X^n+1
        int factors[n];
        for (int i = 0; i < n; i++) {
            factors[i] = (int)pow(TK_SQRT_W, n - 1 - i);
            data[i] = (data[i] * factors[i]) % TK_Q;
        }
    }
}

void toy_polmul_ntt(short *dst, const short *a, const short *b, int add) {
    int n = 4;
    int a_ntt[n], b_ntt[n];
    for (int i = 0; i < n; i++) {
        a_ntt[i] = (int)a[i];
        b_ntt[i] = (int)b[i];
    }

    ntt(a_ntt, n, 1, 1);
    ntt(b_ntt, n, 1, 1);

    for (int i = 0; i < n; i++) {
        a_ntt[i] = (a_ntt[i] * b_ntt[i]) % TK_Q;
    }

    ntt(a_ntt, n, 0, 1);

    for (int i = 0; i < n; i++) {
        dst[i] = (short)((dst[i] & -add) + a_ntt[i]);
    }
}

/* add flag : if true: dst += a*b; if false: dst = a*b; */
static void toy_polmul_naive(short *dst, const short*a, const short *b, int add)
{
    #if naive_mod
    dst[0] = ((dst[0]&-add) + a[0]*b[0] + NEG(a[3])*b[1] + NEG(a[2])*b[2] + NEG(a[1])*b[3]) % TK_Q;
    dst[1] = ((dst[1]&-add) + a[1]*b[0] + a[0]*b[1]      + NEG(a[3])*b[2] + NEG(a[2])*b[3]) % TK_Q;
    dst[2] = ((dst[2]&-add) + a[2]*b[0] + a[1]*b[1]      + a[0]*b[2]      + NEG(a[3])*b[3]) % TK_Q;
    dst[3] = ((dst[3]&-add) + a[3]*b[0] + a[2]*b[1]      + a[1]*b[2]      + a[0]*b[3])      % TK_Q;
    #endif
    #if montgomery
    dst[0] = montgomery_reduce((dst[0]&-add) + a[0]*b[0] + NEG(a[3])*b[1] + NEG(a[2])*b[2] + NEG(a[1])*b[3]);
    dst[1] = montgomery_reduce((dst[1]&-add) + a[1]*b[0] + a[0]*b[1]      + NEG(a[3])*b[2] + NEG(a[2])*b[3]);
    dst[2] = montgomery_reduce((dst[2]&-add) + a[2]*b[0] + a[1]*b[1]      + a[0]*b[2]      + NEG(a[3])*b[3]);
    dst[3] = montgomery_reduce((dst[3]&-add) + a[3]*b[0] + a[2]*b[1]      + a[1]*b[2]      + a[0]*b[3])     ;
    #endif
}

/* Multiply Matrix Vector */
static void toy_mulmv(short *dst, const short *mat, const short *vec)
{
    memset(dst, 0, TK_K*TK_N*sizeof(short));
    for(size_t kv = 0,idx=0; kv < TK_K*TK_N; kv+=TK_N)
    {
        for(size_t k = 0; k < TK_K*TK_N; k+=TK_N,idx+=TK_N){
            #if naive
            toy_polmul_naive(dst+kv, mat+idx, vec+k, 1);
            #endif
            #if ntt_mul
            toy_polmul_ntt(dst+kv, mat+idx, vec+k, 1);
            #endif

        }
    }
}

/* Multiply Matrix Transposed Vector */
static void toy_mulTmv(short *dst, const short *mat, const short *vec)
{
    memset(dst, 0, TK_K*TK_N*sizeof(short));
    for(size_t kv = 0; kv < TK_K*TK_N; kv+=TK_N)
    {
        for(size_t k = 0; k < TK_K*TK_N; k+=TK_N){
            #if naive
            toy_polmul_naive(dst+kv, mat+TK_K*k+kv, vec+k, 1);
            #endif
            #if ntt_mul 
            toy_polmul_ntt(dst+kv, mat+TK_K*k+kv, vec+k, 1);
            #endif
        }
    }
}

/* Dot product between two vectors */
static void toy_dot(short *dst, const short *v1, const short *v2)
{
    memset(dst, 0, TK_N*sizeof(short));
    for(size_t k = 0; k < TK_K*TK_N; k+=TK_N){
        #if naive
        toy_polmul_naive(dst, v1+k, v2+k, 1);
        #endif
        #if ntt_mul
        toy_polmul_ntt(dst, v1+k, v2+k, 1);
        #endif
    }
    
}

/* v2_neg flag -> to negate second vector */
static void toy_add(short *dst, const short *v1, const short *v2, int count, int v2_neg)
{
    for(size_t k = 0; k < count; k++){
        short val = v2[k];
        if(v2_neg){
            val = NEG(val);
        }
        #if naive_mod
        dst[k] = (v1[k]+val)%TK_Q;
        #endif
        #if montgomery
        dst[k] = montgomery_reduce(v1[k]+val);
        #endif

    }
}

/* Generate public and private key */
void toy_gen(short *A, short *t, short *s)
{
    short e[TK_K*TK_N];
    for(size_t k = 0; k < TK_K*TK_K*TK_N; k++){
        #if naive_mod
        A[k] = rand()%TK_Q; // Uniformly random
        #endif
        #if montgomery
        A[k] = montgomery_reduce(rand()); // Uniformly random
        #endif
    }
    /* Small random errors */
    toy_fill_small(s, TK_K*TK_N);
    toy_fill_small(e, TK_K*TK_N);
    /* t = A.s + e */
    toy_mulmv(t, A, s);
    toy_add(t, t, e, TK_K*TK_N, 0); 
}


void toy_enc(const short* A, const short* t, int plain, short* u, short* v){
    short r[TK_K*TK_N];
    short e1[TK_K*TK_N];
    short e2[TK_N];

    /* Generate small gaussian buffers */
    toy_fill_small(r, TK_K*TK_N);
    toy_fill_small(e1, TK_K*TK_N);
    toy_fill_small(e2, TK_N);

    toy_mulTmv(u, A, r);  // u = AT.r + e1
    toy_add(u, u, e1, TK_N*TK_N, 0);
    
    toy_dot(v, t, r);    // v = tT.r + e2 + plain * q/2
    toy_add(v, v, e2, TK_N, 0);

    for(size_t k = 0; k < TK_N; k++){
        #if naive_mod
        v[k]=(v[k]+((TK_Q>>1)&-(plain>>k&1)))%TK_Q;
        #endif
        #if montgomery
        v[k]=montgomery_reduce(v[k]+((TK_Q>>1)&-(plain>>k&1)));
        #endif
    }
}

int toy_dec(const short* s, const short* u, const short* v)
{
    short p[TK_N];
    short plain;

    toy_dot(p, s, u);
    toy_add(p, v, p, TK_N, 1);

    plain = 0;
    for(size_t k = 0; k < TK_N; k++){
        int val = p[k];
        if(val > TK_Q/2){
            val -= TK_Q;
        }
        printf("%5d ",val);
        int bit = abs(val) > TK_Q/4;
        plain |= bit << k;
    }
    return plain;
}
