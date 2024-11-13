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
      trans[i][j] = matrix[j][i];
    }
  }
  return trans;
}

float** matTransposeImp(float** matrix, int size) {
  // Allocate aligned memory for the transposed matrix
  float** trans = (float**) aligned_alloc(32, size * sizeof(float*));
  if (trans == NULL) {
    printf("Memory not allocated for transposed matrix.\n");
    return NULL;
  }

  for (int i = 0; i < size; i++) {
    trans[i] = (float*) aligned_alloc(32, size * sizeof(float));
    if (trans[i] == NULL) {
      printf("Memory not allocated for transposed matrix row %d.\n", i);
      for (int j = 0; j < i; j++) free(trans[j]);
      free(trans);
      return NULL;
    }
  }

  // Perform the transpose using vectorized load/store operations
  for (int i = 0; i < size; i++) {
    #pragma simd
    for (int j = 0; j < size; j += 8) { // Process 8 elements at a time
      __m256 row_data = _mm256_loadu_ps(&matrix[j][i]); // Load 8 floats from matrix column
      _mm256_storeu_ps(&trans[i][j], row_data);         // Store to transposed row
    }
  }

  return trans;
}
