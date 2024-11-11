#include "immintrin.h"
#include "ppc.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


float getRandomFloat(float min, float max){
  return min + ((float) rand() / RAND_MAX) * (max - min);
}


bool checkSym(float** matrix, int size){
  for( int i = 0; i < size; i++){
    for( int j = 0; j < i; j++){
      if( matrix[i][j] != matrix[j][i]) return false;
    }
  }
  return true;
}


float** matTranspose(float** matrix, int size){
  
  float** trans = (float**) malloc(size * sizeof(float*));
  if(trans == NULL){
    printf("Memory not allocated for transposed matrix.\n");
    return NULL;
  }

  for(int i = 0; i < size; i++){
    trans[i] = (float*) malloc(size * sizeof(float));
    if (trans[i] == NULL){
      printf("Memory not allocated for transposed matrix row number %d.\n", i);
      for(int j = 0; j < i; j++){
        free(trans[j]);
      }
      free(trans);
      return NULL;
    }
  }

  for(int i = 0; i < size; i++){
    for(int j = 0; j < size; j++){
      trans[i][j] = matrix[j][i];
    }
  }
  return trans;
}


float** matTransposeImp(float** matrix, int size) {

  float** trans = (float**) malloc(size * sizeof(float*));
  if (trans == NULL) {
    printf("Memory not allocated for transposed matrix.\n");
    return NULL;
  }

  for (int i = 0; i < size; i++) {
    trans[i] = (float*) aligned_alloc(32, size * sizeof(float)); 
    if (trans[i] == NULL) {
      printf("Memory not allocated for row number %d in transposed matrix.\n", i);
      for (int j = 0; j < i; j++) {
        free(trans[j]);
      }
      free(trans);
      return NULL;
    }
  }

  for (int i = 0; i < size; i += BLOCK_SIZE) {
    for (int j = 0; j < size; j += BLOCK_SIZE) {
      for (int ii = i; ii < size && ii < i + BLOCK_SIZE; ii++) {
        for (int jj = j; jj < size && jj < j + BLOCK_SIZE; jj += 8) {
          printf(" Processing block: i=%d, j=%d, ii=%d, jj=%d\n", i, j, ii, jj);
          __m256 row = _mm256_load_ps(&matrix[ii][jj]);
          _mm256_store_ps(&trans[jj][ii], row);
        }
      }
    }
  }

  return trans;
}
