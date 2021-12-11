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


Matrix EmptyMatrix(size_t rows, size_t cols);
void StringToRow(long * result, char * line, const size_t len);
Matrix ReadMatrix(FILE * file);

void AddRow(Matrix * mat);
void AddCol(Matrix * mat);

void PrintMatrix(const Matrix * const mat);
void ClearMatrix(Matrix * mat);

bool Flash(Matrix * mat, const size_t row, const size_t col);
unsigned int NextStep(Matrix * mat);

// Solving
typedef int solution_t;
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

#endif
