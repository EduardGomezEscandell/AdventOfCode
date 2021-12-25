#ifndef DAY_24_H
#define DAY_24_H

#include "simulate.h"

#include "common/testing.h"
#include "common/vector.h"


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef long int Int;
typedef Int solution_t; // Just in case we have to switch to long

typedef struct node_
{
    Int z;
    size_t id;
    size_t children_id[9];
} Node;

TEMPLATE_VECTOR(Node) NodeVector;

typedef struct
{
	NodeVector levels[MONAD_DIGITS+1];
} Tree;

// Solving
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
