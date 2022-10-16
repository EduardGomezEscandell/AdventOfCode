#ifndef DAY_17_H
#define DAY_17_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "common/testing.h"

typedef struct {
    int xmin, ymin, xmax, ymax;
} Target;

int ReadNextNum(char ** line, char delim_start, char delim_end);
Target ReadTarget(const bool is_test);

int ComputeMaxCoordinate(int v_initial);
bool HitsTarget(Target *t, int vx, int vy);

// Solving
typedef long int solution_t;
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
