#ifndef DAY_06_H
#define DAY_06_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

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
unsigned long SolvePart1(const bool is_test);
unsigned long SolvePart2(const bool is_test);

#endif
