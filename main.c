#include "ppc.h"
#include "immintrin.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <omp.h>

int main(){

  srand((unsigned int)time(NULL));

  struct timespec start, end;
  double st, ed;
 
  unsigned int exp;
  printf("\n Give me an exponent to 2: ");                // User input
  scanf("%d", &exp);
  const int SIZE = 1 << exp;                              // Bit shifting to get SIZE = 2^exp
  

  float** a = (float**) malloc(SIZE * sizeof(float*));    // Allocate space for matrix
  
  if (a == NULL){                                               
    printf("Memory not allocated.\n");
    return EXIT_FAILURE;
  }

  // Allocate space for each row

  for(int i = 0; i < SIZE; i++){
    a[i] = (float*) malloc( SIZE * sizeof(float));
    if(a[i] == NULL){
      printf("Memory not allocated for row %d.\n", i);
      for(int j = 0; j < i; j++){
        free(a[j]);
      }
      free(a);
      return EXIT_FAILURE;
    }
  }

  // Populate the matrix with random float values
  
  for(int i = 0; i < SIZE; i++){
    for(int j = 0; j < SIZE; j++){
      a[i][j] = getRandomFloat(0.0, 99999.9);
    }
  }

  // Eventually print original matrix for debugging purpouses
 
  /*
 
  for(int i = 0; i < SIZE; i++){
    for(int j = 0; j < SIZE; j++){
      printf("matrix[%d][%d] = %f \n", i, j, a[i][j]);
    }
  }
   
   */

  // Measure time for sequential symmetry check

  clock_gettime(CLOCK_REALTIME, &start);

  bool sym = checkSym(a, SIZE);

  clock_gettime(CLOCK_REALTIME, &end);

  double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf("\n Elapsed time for sym check:     %.11f seconds.", elapsed);

  // Measure time for SIMD symmetry check

  clock_gettime(CLOCK_REALTIME, &start);

  sym = checkSymImp(a, SIZE);

  clock_gettime(CLOCK_REALTIME, &end);

  elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf("\n Elapsed time for sym check imp: %.11f seconds.", elapsed);

  // Measure time for OMP symmetry check using omp_get_wtime()

  st = omp_get_wtime();

  sym = checkSymOMP(a, SIZE);
  
  ed = omp_get_wtime();
  
  printf("\n Elapsed time for sym check omp: %.11f seconds.\n", ed - st);



  printf("\n Is the matrix symmetric? %s", sym ? "Yes.\n\n" : "No.\n\n");

  // Measure time for sequential matrix transpose

  clock_gettime(CLOCK_REALTIME, &start);

  float** b = matTranspose(a, SIZE);

  clock_gettime(CLOCK_REALTIME, &end);

  elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf(" Elapsed time for transpose operation:     %.11f seconds.\n", elapsed);

  // Measure time for SIMD matrix transpose

  clock_gettime(CLOCK_REALTIME, &start);

  float** c = matTransposeImp(a, SIZE);

  clock_gettime(CLOCK_REALTIME, &end);

  elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf(" Elapsed time for transpose operation imp: %.11f seconds.\n", elapsed);

  
  // Measure time for OMP matrix transpose

  st = omp_get_wtime();
  
  float** d = matTransposeOMP(a, SIZE);

  ed = omp_get_wtime();

  printf(" Elapsed time for transpose operation omp: %.11f seconds.\n\n", ed - st);


  // Eventually print transposed matrix for debugging
/*
  for(int i = 0; i < SIZE; i++){
    for(int j = 0; j < SIZE; j++){
      printf("trans[%d][%d] = %f \n", i, j, d[i][j]);
    }
  }
  */
  
  
  // Free rows from memory

  for(int i = 0; i < SIZE; i++){
    free(a[i]);
    free(b[i]);
    free(d[i]);
  }

  // Free matrices from memory

  free(a);                                                       
  free(b);
  free(c[0]);
  free(c);
  free(d);


  return 0;
}


