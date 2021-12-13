#ifndef DAY_11_H
#define DAY_11_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "common/matrix.h"

bool Flash(Matrix * mat, const size_t row, const size_t col);
unsigned int NextStep(Matrix * mat);

// Solving
typedef int solution_t;
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

#endif
