#ifndef DAY_11_H
#define DAY_11_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct {
	long ** data;
	size_t ncols;
	size_t nrows;
	size_t crows;
	size_t ccols;
} Matrix;

Matrix ReadMatrix(FILE * file);
void ClearMatrix(Matrix * hmap);

void PrintMatrix(const Matrix * const mat);

// Solving
typedef int solution_t;
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

#endif
