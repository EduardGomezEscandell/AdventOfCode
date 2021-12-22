#ifndef DAY_21_H
#define DAY_21_H

#include "common/testing.h"
#include "common/vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef long long int solution_t; // Just in case we have to switch to long

typedef int coord_t;

typedef struct {
    coord_t x0;
    coord_t y0;
    coord_t x1;
    coord_t y1;
    coord_t z0;
    coord_t z1;
    bool active;
} Instruction;


TEMPLATE_VECTOR(Instruction) InstructionVector;
TEMPLATE_VECTOR(Instruction*) InstructionPtrVector;

typedef struct {
	coord_t start, finish;
} Segment;

TEMPLATE_VECTOR(Segment) SegmentVector;

/*           v start[0]        v start[1]
 *  ---------oooooo------------ooooo-----------
 *                 ^finish[0]       ^finish[1]
 **/

// Solving
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
