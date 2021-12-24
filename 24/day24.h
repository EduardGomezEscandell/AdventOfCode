#ifndef DAY_24_H
#define DAY_24_H

#include "virtual_machine.h"

#include "common/testing.h"
#include "common/vector.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


// Solving
typedef value_t solution_t; // Just in case we have to switch to long

solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
