#include "immintrin.h"
#include "ppc.h"
#include <omp.h>
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
    #pragma ivdep
    #pragma GCC unroll(4)
    for (int j = 0; j <= i - 8; j += 8) {
       _mm_prefetch((const char*)&matrix[i][j+8], _MM_HINT_T0); //Pre-load data into L1
       _mm_prefetch((const char*)&matrix[j+8][i], _MM_HINT_T0);
      __m256 row = _mm256_loadu_ps(&matrix[i][j]);
      __m256 col = _mm256_loadu_ps(&matrix[j][i]);
      __m256 cmp = _mm256_cmp_ps(row, col, _CMP_NEQ_UQ);
      if (!_mm256_testz_ps(cmp, cmp)) return false;
    }
  }
  return true;
}


bool checkSymOMP(float** matrix, int size) {
  bool isSymmetric = true;
  #pragma omp parallel for shared(isSymmetric)
  for (int i = 1; i < size; i++) {
    #pragma omp simd
    for (int j = 0; j < i; j++) {
      if (!isSymmetric) continue;
      if (matrix[i][j] != matrix[j][i]) {
        isSymmetric = false;
      }
    }
  }
  return isSymmetric;
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
  
  #pragma ivdep 
  #pragma unroll(4)
  for (int i = 0; i < size; i += 8) {          
    for (int j = 0; j < size; j++) {       
      _mm_prefetch((const char*)&matrix[i + 8][j], _MM_HINT_T0);
      _mm_prefetch((const char*)&trans[j][i + 8], _MM_HINT_T0);
      __m256 row_data = _mm256_set_ps( matrix[i+7][j], 
                                       matrix[i+6][j], 
                                       matrix[i+5][j], 
                                       matrix[i+4][j],
                                       matrix[i+3][j], 
                                       matrix[i+2][j], 
                                       matrix[i+1][j], 
                                       matrix[i][j] );
      _mm256_storeu_ps(&trans[j][i], row_data);
    }
  }

  return trans;
}


float** matTransposeOMP(float** matrix, int size){

 
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

  #pragma omp parallel
  {
  #pragma omp for collapse(2)
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      trans[j][i] = matrix[i][j];
    }
  }
  }
  return trans;

}
