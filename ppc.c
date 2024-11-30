#include "immintrin.h"   // Include AVX2 
#include "ppc.h"         // Include header file
#include <omp.h>         // Include OpenMP
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

  
float getRandomFloat(float min, float max) {
  return min + ((float) rand() / RAND_MAX) * (max - min);
}

  // Sequential implementation of the symmetry check algorithm 
bool checkSym(float** matrix, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < i; j++) {
      if (matrix[i][j] != matrix[j][i]) return false;
    }
  }
  return true;
}

  // Implicit parallelization using AVX2 of the symmetry check algorithm
bool checkSymImp(float** matrix, int size) {
  for (int i = 0; i < size; i++) {
    #pragma ivdep
    #pragma GCC unroll(8)
    for (int j = 0; j <= i - 8; j += 8) {
       _mm_prefetch((const char*)&matrix[i][j+8], _MM_HINT_T0); // Load next element into L1
       _mm_prefetch((const char*)&matrix[j+8][i], _MM_HINT_T0); // Load next transposed data into L1
      __m256 row = _mm256_loadu_ps(&matrix[i][j]);              // Load 8 floating-point into row
      __m256 col = _mm256_loadu_ps(&matrix[j][i]);              // Load 8 floating-point into col
      __m256 cmp = _mm256_cmp_ps(row, col, _CMP_NEQ_UQ);        // "Not equals" comparison 
      if (!_mm256_testz_ps(cmp, cmp)) return false;             // False if all bits are zero
    }
  }
  return true;
}



bool checkSymOMP(float** matrix, int size) {
  bool isSym = true;

  omp_set_num_threads(4); // Set the number of threads

  #pragma omp parallel shared(isSym)
  {
    #pragma omp for schedule(static, 16)
    for (int i = 0; i < size; i++) {
      if (!isSym){
        #pragma omp flush(isSym)
        continue; // Check early exit condition
      }
      for (int j = 0; j < i; j++) {
        if (matrix[i][j] != matrix[j][i]) {
          #pragma omp atomic write 
          isSym = false; // Set flag to false atomically
          #pragma omp flush(isSym)
          break; // Exit the inner loop early
        }
      }
    }
  }

  return isSym;
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
    free(trans);
    free(data);
    return NULL;
  }

  for (int i = 0; i < size; i++) {
    trans[i] = &data[i * size];
  }
  
  #pragma ivdep 
  #pragma GCC unroll(8)
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

  int i, j; 
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
  #pragma omp for collapse(2) schedule(static, 16) private(i, j)
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      trans[j][i] = matrix[i][j];
    }
  }
  }
  return trans;

}
