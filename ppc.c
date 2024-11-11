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


float** matTransposeImp(float** matrix, int size){

  float** trans = (float**) malloc(size * sizeof(float*));
  if(trans == NULL){
    printf("Memory not allocated for transposed matrix.\n");
    return NULL;
  }

  for(int i = 0; i < size; i++){
    trans[i] = (float*) malloc(size * sizeof(float));
    if (trans[i] == NULL){
      printf("Memory not allocated for row number %d in transposed matrix.\n", i);
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
