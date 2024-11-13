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
  // Allocate transposed matrix with proper alignment
  float** trans = (float**) aligned_alloc(32, size * sizeof(float*));
  if (trans == NULL) {
    printf("Memory not allocated for transposed matrix.\n");
    return NULL;
  }

  size_t row_size = ((size * sizeof(float) + 31) / 32) * 32; // Align rows to 32 bytes
  for (int i = 0; i < size; i++) {
    trans[i] = (float*) aligned_alloc(32, row_size);
    if (trans[i] == NULL) {
      printf("Memory not allocated for row %d in transposed matrix.\n", i);
      for (int j = 0; j < i; j++) free(trans[j]);
      free(trans);
      return NULL;
    }
  }

  // Transpose without blocking, using AVX2 for 8 elements at a time
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j += 8) {
      if (j + 8 <= size) {
        // Use AVX2 to load 8 floats from matrix[i][j] and store in transposed position
        __m256 row = _mm256_load_ps(&matrix[i][j]);  // Load 8 floats from row `i`
        _mm256_store_ps(&trans[j][i], row);          // Store them as column `j` in transposed matrix
      } else {
        // Handle the case where fewer than 8 elements remain (e.g., near end of row)
        for (int k = j; k < size; k++) {
          trans[k][i] = matrix[i][k];
        }
      }
    }
  }

  return trans;
}
