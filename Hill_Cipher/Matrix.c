#include "Matrix.h"

int arith_mod(int x, int y) 
{
	if (-13/5 == -2          &&     // Check division truncates to zero
	    (x < 0) != (y < 0)   &&     // Check x and y have different signs
        (x % y) != 0)
		return x%y + y;
	else
		return x%y;
}

int mod_inv(int A, int M)
{
    for (int X = 1; X < M; X++)
        if (((A % M) * (X % M)) % M == 1)
            return X;
}

Matrix mat_new(size_t rows, size_t cols)
{
    Matrix mat;
    mat.rows = rows;
    mat.columns = cols;
    mat.elements = calloc(sizeof(*mat.elements),rows*cols);
    assert(mat.elements);
    return mat;
}

Matrix mat_init(size_t rows, size_t cols, const double* values)
{
    Matrix mat = mat_new(rows,cols);
    memcpy(mat.elements,values,mat.columns*mat.rows*sizeof(double));
    return mat;
}

Matrix mat_clone(Matrix *matsrc)
{
    Matrix mat = mat_new(matsrc->rows,matsrc->columns);
    memcpy(mat.elements,matsrc->elements,matsrc->rows*matsrc->columns*sizeof(double));
    return mat;
}

void mat_free(Matrix *mat)
{
    free(mat->elements);
    mat->elements = NULL;
}

Matrix mat_mult(Matrix *a, Matrix *b)
{
    assert(a->columns == b->rows);
    Matrix res = mat_new(a->rows,b->columns);

    size_t n = a->columns;
    for (size_t i = 0; i < res.rows; i++){
        for (size_t j = 0; j < res.columns; j++){
            mat_at(&res,i,j) = 0;
            for (size_t k = 0; k < n; k++){
                mat_at(&res,i,j) += mat_at(a,i,k)*mat_at(b,k,j);
            }
        }
    }
    return res;    
}

void mat_sum(Matrix *a, Matrix *b)
{
    assert(a->rows == b->rows);
    assert(a->columns == b->columns);
    for (size_t i = 0; i < a->rows; i++){
        for (size_t j = 0; j < a->columns ; j++){
            mat_at(a,i,j) += mat_at(b,i,j);
        }
    }
}

void mat_print(Matrix *mat, int precision)
{
    printf("\n");
    for (size_t i = 0; i < mat->rows; i++){
        for (size_t j = 0; j < mat->columns ; j++){
            printf("%.*f\t",precision,mat_at(mat,i,j));
        }
        printf("\n");
    }
}

void mat_print_ascii(Matrix *mat)
{
    printf("\n");
    for (size_t i = 0; i < mat->rows; i++){
        for (size_t j = 0; j < mat->columns ; j++){
            printf("%c\t",(int)mat_at(mat,i,j)+'a');
        }
        printf("\n");
    }
}

Matrix mat_transpose(Matrix *mat)
{
    Matrix mt = mat_new(mat->columns,mat->rows);
    for (size_t i = 0; i < mat->rows; i++){
        for (size_t j = 0; j < mat->columns ; j++){
            mat_at(&mt,j,i) = mat_at(mat,i,j);
        }
    }
    return mt;
}

// Function to swap two rows in a matrix
void mat_swap_rows(Matrix *matrix, size_t row1, size_t row2) 
{
    for (size_t i = 0; i < matrix->columns; i++){
        double temp = mat_at(matrix,row1,i);
        mat_at(matrix,row1,i) = mat_at(matrix,row2,i);
        mat_at(matrix,row2,i) = temp;
    }
}

double mat_det(Matrix *mat) 
{
    // Non-square matrices do not have determinants.
    assert(mat->columns == mat->rows);

    size_t dimension = mat->columns;
    Matrix detmat = mat_clone(mat);

    if(dimension == 0) {
        return 1;
    }
    if(dimension == 1) {
        return mat_at(&detmat,0,0);
    }
    if(dimension == 2) {
        return mat_at(&detmat,0,0) * mat_at(&detmat,1,1) - mat_at(&detmat,0,1) * mat_at(&detmat,1,0);
    }

    double det = 1.0;

    for (size_t i = 0; i < dimension; i++) 
    {
        // Find the maximum element in the current column
        size_t max_row = i;
        for (size_t j = i + 1; j < dimension; j++) {
            if (fabs(mat_at(&detmat,j,i)) > fabs(mat_at(&detmat,max_row,i))) {
                max_row = j;
            }
        }
        // Swap the current row with the row containing the maximum element
        if (max_row != i) {
            mat_swap_rows(&detmat, i, max_row);
            det = -det; // Change the sign of the determinant when swapping rows
        }
        double pivot = mat_at(&detmat,i,i);

        if (pivot == 0.0) {
            // If the pivot is zero, the mat is singular, and the determinant is zero
            return 0.0;
        }

        det *= pivot;

        // Perform Gaussian elimination to make the elements below the pivot zero
        for (size_t j = i + 1; j < dimension; j++) {
            double factor = mat_at(&detmat,j,i) / pivot;
            for (size_t k = i; k < dimension; k++) {
                mat_at(&detmat,j,k) -= factor * mat_at(&detmat,i,k);
            }
        }
    }
    mat_free(&detmat);
    return det;
}


Matrix mat_cof(Matrix *mat)
{
    assert(mat->columns == mat->rows);

    size_t dimension = mat->columns;

    Matrix solution  = mat_new(dimension, dimension);
    Matrix submatrix = mat_new(dimension - 1, dimension - 1);

    for (size_t i = 0; i < dimension; i++){
        for (size_t j = 0; j < dimension; j++){
            int sub_row = 0;
            for (size_t x = 0; x < dimension; x++){
                if (x == i){
                    continue;
                }
                int sub_col = 0;
                
                for (size_t y = 0; y < dimension; y++){
                    if (y == j){
                        continue;
                    }
                    mat_at(&submatrix,sub_row,sub_col) = mat_at(mat,x,y);
                    sub_col++;
                }
                sub_row++;
            }
            mat_at(&solution,i,j) = pow(-1, i + j) * mat_det(&submatrix);
        }
    }
    mat_free(&submatrix);
    return solution;
}

Matrix mat_inv(Matrix *mat) 
{
    size_t dimension = mat->columns;

    double det = mat_det(mat);

    if(det==0){
        printf("Determinant is zero");
        exit(EXIT_FAILURE);
    }
    Matrix coff = mat_cof(mat);
    Matrix solution = mat_transpose(&coff);

    for(size_t i = 0; i < dimension; i++) {
        for(size_t j = 0; j < dimension; j++) {
            mat_at(&solution,i,j) *= 1/det;
        }
    }
    mat_free(&coff);
    return solution;
}

Matrix mat_inv_mod(Matrix *mat, int mod) 
{
    size_t dimension = mat->columns;

    double det = mat_det(mat);
    if(det==0){
        printf("Determinant is zero");
        exit(EXIT_FAILURE);
    }
    int det_mod = mod_inv(arith_mod((int)det,mod),mod);

    Matrix cof = mat_cof(mat);
    Matrix adj  = mat_transpose(&cof);
    Matrix mod_sol = mat_mod(&adj,mod);

    for(size_t i = 0; i < dimension; i++) {
        for(size_t j = 0; j < dimension; j++) {
            mat_at(&mod_sol,i,j) *= det_mod;
        }
    }
    mat_free(&cof);
    mat_free(&adj);
    return mod_sol;
}

Matrix mat_mod(Matrix *mat, int mod)
{
    Matrix solution = mat_new(mat->rows,mat->columns);
    for (size_t i = 0; i < mat->rows; i++){
        for (size_t j = 0; j < mat->columns; j++){
            mat_at(&solution,i,j) = arith_mod((int)(mat_at(mat,i,j)),mod);
        }
    }
    return solution;
}

double rand_double(void)
{
    return (double)rand() / (double)RAND_MAX;
}

int rand_int(int start, int end)
{
    if (end + 1 - start != 0){
        int r = rand() % (end + 1 - start) + start;
        return r;
    }else{
        return -1;
    }
}

Matrix mat_rand_int(size_t rows, size_t cols, int min, int max)
{
    Matrix solution = mat_new(rows,cols);
    for (size_t i = 0; i < rows; i++){
        for (size_t j = 0; j < cols; j++){
            mat_at(&solution,i,j) = rand_int(min,max);
        }
    }
    return solution;
}

Matrix mat_rand_double(size_t rows, size_t cols, int multiplier)
{
    Matrix solution = mat_new(rows,cols);
    for (size_t i = 0; i < rows; i++){
        for (size_t j = 0; j < cols; j++){
            mat_at(&solution,i,j) = rand_double()*multiplier;
        }
    }
    return solution;
}
