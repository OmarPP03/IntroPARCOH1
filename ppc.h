#include <stdbool.h>
#ifndef __PPC_H__
#define __PPC_H__
#define BLOCK_SIZE 8


float getRandomFloat(float min, float max);
bool checkSym(float **matrix, int size);
float** matTranspose(float** matrix, int size);
float** matTransposeImp(float** matrix, int size);


#endif // !__PPC_H__
