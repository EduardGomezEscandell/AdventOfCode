#ifndef DAY_07_H
#define DAY_07_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "common/vector.h"
#include "common/testing.h"

TEMPLATE_VECTOR(long) Vector;

char * ReadSingleLine(FILE * file);

int UniformCostExpense(const int source, const int destination);
int QuadraticCostExpense(const int source, const int destination);
int ComputeFuelExpense(const Vector v, int destination, int(*fuel_formula)(const int, const int));

int OptimizeFuelExpense(const Vector v, int(*fuel_formula)(const int, const int));

int Solve(const bool is_test, int(*fuel_formula)(const int, const int));

// Solving
typedef int solution_t;
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
