#ifndef DAY_18_H
#define DAY_18_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "common/testing.h"
#include "common/vector.h"
#include "common/matrix.h"

typedef int solution_t; // Just in case we have to switch to long
typedef uint_least16_t binary_t;

TEMPLATE_VECTOR(bool) BoolArray;

BoolArray ReadAlgorithm(FILE * file);
SparseMatrix ReadImage(FILE * file);

binary_t ExtractNeighbourhood(SparseMatrix * image, ssize_t row, ssize_t col, bool fill_value);
void BinaryAppend(binary_t * binary, bool bit);
void Update(SparseMatrix * image, BoolArray algorithm, bool * fill_value);

// Solving
solution_t Solve(const bool is_test, const size_t n_iterations);
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
