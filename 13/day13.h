#ifndef DAY_13_H
#define DAY_13_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "common/matrix.h"
#include "common/testing.h"
#include "common/vector.h"

typedef struct
{
	char axis;
	size_t value;
} Fold;

TEMPLATE_VECTOR(Fold) FoldVector;

void ReadLine(SparseMatrix * sp, char * line);
void ReadData(FILE * file, SparseMatrix * sp, FoldVector * folds);

void YFold(SparseMatrix * sp, const size_t y_fold);
void XFold(SparseMatrix * sp, const size_t x_fold);

// Solving
typedef int solution_t;
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
