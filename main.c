#include "ppc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h> // Used for time measurments
#include <stdbool.h> 

int main(){

  srand((unsigned int)time(NULL));

  struct timespec start, end;
  
  unsigned int exp;
  printf(" Give me a random exponent to 2: ");
  scanf("%d", &exp);
  const int SIZE = 1 << exp;

  float** a = (float**) malloc(SIZE * sizeof(float*));       
  
  if (a == NULL){                                               
    printf("Memory not allocated.\n");
    return EXIT_FAILURE;
  }

  for(int i = 0; i < SIZE; i++){                                
    a[i] = (float*) malloc(SIZE * sizeof(float));
    if(a[i] == NULL){
      printf("Memory not allocated for row %d.\n", i);
      for(int j = 0; j < i; j++){
        free(a[j]);

      }
      free(a);
      return EXIT_FAILURE;
    }
  }
  
  for(int i = 0; i < SIZE; i++){                                
    for(int j = 0; j < SIZE; j++){
      a[i][j] = getRandomFloat(0.0, 99999.9);
    }
  }
  
  /*
  int count = 0;
  for(int i = 0; i < SIZE; i++){                                
    for(int j = 0; j < SIZE; j++){
      count += 1;
      printf(" %d Element: %.6f \n", count, a[i][j]);
    }
  }
  */ 

  clock_gettime(CLOCK_REALTIME, &start);

  bool sym = checkSym(a, SIZE);                

  clock_gettime(CLOCK_REALTIME, &end);

  double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf("\n Elapsed time for sym check: %.11f seconds.\n", elapsed);

  printf("\n Is the matrix symmetric? %s", sym ? "Yes.\n\n" : "No.\n\n");


  clock_gettime(CLOCK_REALTIME, &start);

  float** b = matTranspose(a, SIZE);

  clock_gettime(CLOCK_REALTIME, &end);

  elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf(" Elapsed time for transpose operation:     %.11f seconds.\n", elapsed);


  clock_gettime(CLOCK_REALTIME, &start);

  float** c = matTransposeImp(a, SIZE);

  clock_gettime(CLOCK_REALTIME, &end);

  elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf(" Elapsed time for transpose operation imp: %.11f seconds.\n", elapsed);
  /* 
  count = 0;
  for(int i = 0; i < SIZE; i++){                                
    for(int j = 0; j < SIZE; j++){
      count += 1;
      printf(" %d Element: %.6f \n", count, b[i][j]);
    }
  }
  */
  
  for(int i = 0; i < SIZE; i++){                                
    free(a[i]);
    free(b[i]);
    free(c[i]);
  }


  free(a);                                                       
  free(b);
  free(c);
  return 0;
}


