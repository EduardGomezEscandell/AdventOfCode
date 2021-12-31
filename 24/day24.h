#ifndef DAY_24_H
#define DAY_24_H

#include "common/testing.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MONAD_DIGITS 14

typedef uint_least64_t solution_t; // We need log2(1e14) = 47 bits

enum Target {MIN, MAX};
solution_t AssembleNumber(enum Target target);


// Solving
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
