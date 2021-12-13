#ifndef DAY_10_H
#define DAY_10_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "common/vector.h"

TEMPLATE_VECTOR(long) Vector;
TEMPLATE_VECTOR(size_t) Stack;

typedef struct 
{
	size_t corruption;
	size_t autocompletion;
} Score;

void PrintCorruptionError(const size_t row, const size_t col, const size_t i_opener, const char read, const char * line);
void PrintWrongCharError(const size_t row, const size_t col, const char read, const char * line);

char Closer(char c);
int ComputeScore(char c);
size_t ComputeLineAutocompletionScore(Stack stack, const char * line);
Score ComputeLineScore(const char * line, const size_t line_id);
Score Solve(const bool is_test);

// Solving
typedef Score solution_t;
solution_t Solve(const bool is_test);

#endif
