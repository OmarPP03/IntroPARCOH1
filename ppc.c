#include "immintrin.h"
#include "ppc.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

float getRandomFloat(float min, float max) {
  return min + ((float) rand() / RAND_MAX) * (max - min);
}

bool checkSym(float** matrix, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < i; j++) {
      if (matrix[i][j] != matrix[j][i]) return false;
    }
  }
  return true;
}

bool checkSymImp(float** matrix, int size) {
  for (int i = 0; i < size; i++) {
    int j;

    // Vectorized comparison for chunks of 8 elements
    for (j = 0; j <= i - 8; j += 8) {
      __m256 row_data = _mm256_loadu_ps(&matrix[i][j]);
      __m256 col_data = _mm256_loadu_ps(&matrix[j][i]);
      __m256 cmp = _mm256_cmp_ps(row_data, col_data, _CMP_NEQ_UQ);

      // If any elements are unequal, return false
      if (!_mm256_testz_ps(cmp, cmp)) {
        return false;
      }
    }

    // Scalar fallback for remaining elements if `i` is not a multiple of 8
    for (; j < i; j++) {
      if (matrix[i][j] != matrix[j][i]) {
        return false;
      }
    }
  }
  return true;
}

float** matTranspose(float** matrix, int size) {
  float** trans = (float**) malloc(size * sizeof(float*));
  if (trans == NULL) {
    printf("Memory not allocated for transposed matrix.\n");
    return NULL;
  }

  for (int i = 0; i < size; i++) {
    trans[i] = (float*) malloc(size * sizeof(float));
    if (trans[i] == NULL) {
      printf("Memory not allocated for transposed matrix row %d.\n", i);
      for (int j = 0; j < i; j++) free(trans[j]);
      free(trans);
      return NULL;
    }
  }

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      trans[j][i] = matrix[i][j];
    }
  }
  return trans;
}

float** matTransposeImp(float** matrix, int size) {

  float* data = (float*) malloc(size * size * sizeof(float));
  float** trans = (float**) malloc(size * sizeof(float*));
  if (trans == NULL || data == NULL) {
    printf("Memory not allocated for transposed matrix.\n");
    free(data);
    return NULL;
  }

  for (int i = 0; i < size; i++) {
    trans[i] = &data[i * size];
  }
  

  for (int i = 0; i < size; i += 8) {          // Process 8 rows at a time
    for (int j = 0; j < size; j++) {          // Process each column in these 8 rows
      // Load 8 elements from the i-th row to (i+7)-th row at column j
      __m256 row_data = _mm256_set_ps(
        matrix[i+7][j], matrix[i+6][j], matrix[i+5][j], matrix[i+4][j],
        matrix[i+3][j], matrix[i+2][j], matrix[i+1][j], matrix[i][j]
      );

      // Store transposed elements into `trans[j][i]` to `trans[j][i+7]`
      _mm256_storeu_ps(&trans[j][i], row_data);
    }
  }

  return trans;
}
