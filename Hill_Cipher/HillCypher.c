#include "Matrix.h"

Matrix encrypt(Matrix *key, Matrix *trigraph) {
  Matrix mult = mat_mult(key, trigraph);
  Matrix res = mat_mod(&mult, 26);
  return res;
}

Matrix decrypt(Matrix *Encryption, Matrix *key) {
  Matrix inv = mat_inv_mod(key, 26);
  Matrix mult = mat_mult(&inv, Encryption);
  Matrix res = mat_mod(&mult, 26);
  return res;
}

Matrix get_key(Matrix *text, Matrix *encryption) {
  Matrix inv = mat_inv_mod(text, 26);
  Matrix mult = mat_mult(encryption, &inv);
  Matrix res = mat_mod(&mult, 26);
  return res;
}

int main(void) 
{
  Matrix digraph = mat_init(2, 1, (double[]) {'o' - 'a',
                                              'k' - 'a'});

  Matrix key = mat_init(2, 2, (double[]) {9, 4, 
                                          5, 6});

  Matrix result = encrypt(&key, &digraph);

  mat_print_ascii(&result);

  system("pause");
  return 0;
}