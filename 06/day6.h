#ifndef DAY_06_H
#define DAY_06_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "common/testing.h"

#define REPR_CYCLE 6
#define CHILDHOOD 8

typedef struct
{
	unsigned long fish[CHILDHOOD+1]; // fish[i] are the amount of fish i days away from reproducing
} School;

School ParseSchool(FILE * file);
unsigned long CountFish(const School * const school);
void PrintSchool(const School * const s);

void NextDay(School * school);
unsigned long RunSimulation(const bool is_test, const size_t n_days);

// Solving
typedef unsigned long solution_t;
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
