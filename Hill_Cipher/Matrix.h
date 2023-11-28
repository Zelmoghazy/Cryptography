#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <time.h>

/**
 *  Flatten Matrix as linear array. 
 */

#define mat_at(m,i,j) (m)->elements[(i)*(m)->columns+(j)]
#define num_elements(x) ((sizeof(x))/(sizeof((x)[0])))

typedef struct Matrix{
    size_t rows;
    size_t columns;
    double *elements;
}Matrix;

int arith_mod(int x, int y);
int mod_inv(int A, int M);


Matrix mat_new(size_t rows, size_t cols);
Matrix mat_init(size_t rows, size_t cols, const double* values);
Matrix mat_clone(Matrix *matsrc);
void mat_free(Matrix *mat);
Matrix mat_mult(Matrix *a, Matrix *b);
void mat_sum(Matrix *a, Matrix *b);
void mat_print(Matrix *mat, int precision);
void mat_print_ascii(Matrix *mat);

Matrix mat_transpose(Matrix *mat);
void mat_swap_rows(Matrix *matrix, size_t row1, size_t row2);
double mat_det(Matrix *mat);
Matrix mat_cof(Matrix *mat);
Matrix mat_inv(Matrix *mat); 
Matrix mat_inv_mod(Matrix *mat, int mod); 

Matrix mat_mod(Matrix *mat, int mod);
double rand_double(void);
int rand_int(int start, int end);
Matrix mat_rand_int(size_t rows, size_t cols, int min, int max);
Matrix mat_rand_double(size_t rows, size_t cols,int multiplier);



#endif