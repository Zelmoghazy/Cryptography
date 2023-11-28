#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TYPE uint8_t

#define mat_at(m,i,j) (m)->elements[(i)*(m)->columns+(j)]

void swap(uint8_t *a, uint8_t *b)
{
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}

uint8_t multiply_gf2_4(uint8_t a, uint8_t b)
{
    const uint8_t overflow = 0x10;
    const uint8_t modulus  = 0x13;  
    
    uint8_t sum = 0;

    while (b > 0) 
    {
        if (b & 1){
            sum = sum ^ a;           
        }
        b = b >> 1;                  
        a = a << 1;                  
        if (a & overflow){
            a = a ^ modulus;    
        }
    }
    return sum;
}

typedef struct Matrix{
    TYPE elements[4];
    uint8_t rows;
    uint8_t columns;
}Matrix;

const uint8_t sbox_enc[]={
    9,  4,  10, 11,
    13, 1,  8,  5,
    6,  2,  0,  3,
    12, 14, 15, 7
};

const uint8_t sbox_dec[]={
    10,  5,  9,  11,
    1,   7,  8,  15,
    6,   0,  2,  3,
    12,  4,  13, 14
};

Matrix mat_new(uint8_t rows, uint8_t cols)
{
    Matrix mat;
    mat.rows = rows;
    mat.columns = cols;
    return mat;
}

void mat_xor(Matrix *mat1, Matrix *mat2)
{
    assert((mat1->rows == mat2->rows) && (mat1->columns == mat2->columns));

    for (uint8_t i = 0; i < mat1->rows; i++){
        for (uint8_t j = 0; j < mat1->columns; j++){
            mat_at(mat1,i,j) ^= mat_at(mat2, i, j) ;
        }
    }
}

/* Convert Hexadecimal Character to decimal. */
uint8_t char_ascii_hex(char ch)
{
    if(ch >= '0' && ch <= '9'){
        return ch-'0';
    }else if (ch >= 'A' && ch <= 'F'){
        return (ch-'A'+10);
    }else if (ch >= 'a' && ch <= 'f'){
        return (ch-'a'+10);
    }else{
        printf("%c is not a hexadecimal number. \n",ch);
        exit(EXIT_FAILURE);
    }
}

/* Read a 16-bit hexadecimal input into a 2x2 Matrix */
Matrix mat_read_hex_nibble(char *str)
{
    assert(strlen(str) == 4 && "Please add a 16-bit hexadecimal input.");

    Matrix mat = mat_new(2,2);

    /* Column Major Order MSB First*/
    mat.elements[0] = char_ascii_hex(str[0]);
    mat.elements[2] = char_ascii_hex(str[1]);
    mat.elements[1] = char_ascii_hex(str[2]);
    mat.elements[3] = char_ascii_hex(str[3]);

    return mat;
}

/* Write a 16-bit hexadecimal 2x2 matrix to console */
void mat_write_nibble_hex(Matrix *mat)
{
    assert(mat->columns == 2);
    assert(mat->rows == 2);

    /* Column Major Order */
    // printf("%.4X", mat->elements[3]+mat->elements[1]*16+mat->elements[2]*256+mat->elements[0]*4096);
    printf("%X",mat->elements[0]);
    printf("%X",mat->elements[2]);
    printf("%X",mat->elements[1]);
    printf("%X",mat->elements[3]);
    printf("\n");
}

void mat_s_aes_sbox(Matrix *mat, bool enc)
{
    if(enc){
        mat->elements[0] = sbox_enc[mat->elements[0]];
        mat->elements[1] = sbox_enc[mat->elements[1]];
        mat->elements[2] = sbox_enc[mat->elements[2]];
        mat->elements[3] = sbox_enc[mat->elements[3]];
    }else{
        /* Inverse S-Box */
        mat->elements[0] = sbox_dec[mat->elements[0]];
        mat->elements[1] = sbox_dec[mat->elements[1]];
        mat->elements[2] = sbox_dec[mat->elements[2]];
        mat->elements[3] = sbox_dec[mat->elements[3]];
    }
}

void mat_s_aes_shift_rows(Matrix *mat)
{
    swap(&mat_at(mat,1, 0),&mat_at(mat,1, 1));
}

void mat_s_aes_mix_columns(Matrix *mat, bool enc)
{
    uint8_t temp00 = mat_at(mat,0, 0);
    uint8_t temp01 = mat_at(mat,0, 1);
    uint8_t temp10 = mat_at(mat,1, 0);
    uint8_t temp11 = mat_at(mat,1, 1);

    /* Unroll 2x2 Matrix Multiplication */
    if(enc){
        mat_at(mat,0,0) = multiply_gf2_4(1, temp00) ^ multiply_gf2_4(4, temp10);
        mat_at(mat,0,1) = multiply_gf2_4(1, temp01) ^ multiply_gf2_4(4, temp11);
        mat_at(mat,1,0) = multiply_gf2_4(4, temp00) ^ multiply_gf2_4(1, temp10); 
        mat_at(mat,1,1) = multiply_gf2_4(4, temp01) ^ multiply_gf2_4(1, temp11);
    }else{
        /* Inverse Mix Columns */
        mat_at(mat,0,0) = multiply_gf2_4(9, temp00) ^ multiply_gf2_4(2, temp10);
        mat_at(mat,0,1) = multiply_gf2_4(9, temp01) ^ multiply_gf2_4(2, temp11);
        mat_at(mat,1,0) = multiply_gf2_4(2, temp00) ^ multiply_gf2_4(9, temp10); 
        mat_at(mat,1,1) = multiply_gf2_4(2, temp01) ^ multiply_gf2_4(9, temp11);
    }
}

uint8_t g_function(uint8_t w, uint8_t rcon)
{
    uint8_t s_boxed = (sbox_enc[(w & 0x0F)]) << 4 | (sbox_enc[((w & 0xF0) >> 4)]);
    return (s_boxed ^ rcon);
}

void mat_s_aes_key_expansion(Matrix *key, Matrix *key1, Matrix *key2)
{
    const uint8_t rcon1 = 0x80;
    const uint8_t rcon2 = 0x30;

    uint8_t w0 = (key->elements[0] & (0x0f)) << 4 | (key->elements[2] & (0x0f));
    uint8_t w1 = (key->elements[1] & (0x0f)) << 4 | (key->elements[3] & (0x0f));
    uint8_t w2 = (g_function(w1, rcon1)) ^ w0;
    uint8_t w3 = w2 ^ w1;
    uint8_t w4 = (g_function(w3, rcon2)) ^ w2;
    uint8_t w5 = w3 ^ w4;

    mat_at(key1, 0, 0) = (w2 & (0xf0)) >> 4;
    mat_at(key1, 1, 0) = (w2 & (0x0f));
    mat_at(key1, 0, 1) = (w3 & (0xf0)) >> 4;
    mat_at(key1, 1, 1) = (w3 & (0x0f));

    mat_at(key2, 0, 0) = (w4 & (0xf0)) >> 4;
    mat_at(key2, 1, 0) = (w4 & (0x0f));
    mat_at(key2, 0, 1) = (w5 & (0xf0)) >> 4;
    mat_at(key2, 1, 1) = (w5 & (0x0f));
}

void mat_s_aes_encrypt(Matrix *plain, Matrix *key)
{
    Matrix key1 = mat_new(2,2);
    Matrix key2 = mat_new(2,2);

    /* Expand key */
    mat_s_aes_key_expansion(key, &key1, &key2);

    mat_xor(plain,key);                     // Add round key

    /*  Round 1 */
    mat_s_aes_sbox(plain,true);             // Nibble Substitution
    mat_s_aes_shift_rows(plain);            // Shift row
    mat_s_aes_mix_columns(plain,true);      // Mix Columns
    mat_xor(plain,&key1);                   // Add round key

    /* Final Round */
    mat_s_aes_sbox(plain,true);             // Nibble Substitution
    mat_s_aes_shift_rows(plain);            // Shift row
    mat_xor(plain,&key2);                   // Add round key
}


void mat_s_aes_decrypt(Matrix *cipher, Matrix *key)
{
    Matrix key1 = mat_new(2,2);
    Matrix key2 = mat_new(2,2);

    /* Expand key */
    mat_s_aes_key_expansion(key, &key1, &key2); 
 
    mat_xor(cipher,&key2);                  // Add round key
    
    /* Round 1 */
    mat_s_aes_shift_rows(cipher);           // Inverse shift row
    mat_s_aes_sbox(cipher,false);           // Inverse nibble substitution
    mat_xor(cipher,&key1);                  // Add round key
    mat_s_aes_mix_columns(cipher,false);    // Inverse mix columns

    /* Final Round */
    mat_s_aes_shift_rows(cipher);           // Inverse shift row
    mat_s_aes_sbox(cipher,false);           // Inverse nibble substitution
    mat_xor(cipher,key);                    // Add round key
}

int main(int argc, char *argv[]) 
{
    // Assert number of arguments and whether they are null
    assert((argc == 4) && "You must add 3 commandline arguments: DEC/ENC key cipher/plain");
    assert(argv[1] && argv[2] && argv[3]);

    if(!(strcmp(argv[1],"ENC") && strcmp(argv[1],"enc")))
    {
        Matrix key        = mat_read_hex_nibble(argv[2]);
        Matrix plain      = mat_read_hex_nibble(argv[3]);
        mat_s_aes_encrypt(&plain,&key);
        mat_write_nibble_hex(&plain);
    }
    else if (!(strcmp(argv[1],"DEC") && strcmp(argv[1],"dec")))
    {
        Matrix key        = mat_read_hex_nibble(argv[2]);
        Matrix cipher     = mat_read_hex_nibble(argv[3]);
        mat_s_aes_decrypt(&cipher,&key);
        mat_write_nibble_hex(&cipher);
    }else{
        printf("Please state mode correctly either DEC or ENC.\n");
    }
    return 0;
}
