#ifndef DAY_07_H
#define DAY_07_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "common/vector.h"

char * ReadSingleLine(FILE * file);

int UniformCostExpense(const int source, const int destination);
int QuadraticCostExpense(const int source, const int destination);
int ComputeFuelExpense(const Vector v, int destination, int(*fuel_formula)(const int, const int));

int OptimizeFuelExpense(const Vector v, int(*fuel_formula)(const int, const int));

int Solve(const bool is_test, int(*fuel_formula)(const int, const int));

// Solving
int SolvePart1(const bool is_test);
int SolvePart2(const bool is_test);

#endif
