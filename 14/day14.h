#ifndef DAY_14_H
#define DAY_14_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "common/testing.h"
#include "common/vector.h"
#include "common/matrix.h"
#include "common/forward_list.h"

typedef struct {
	char input[2];
	char data;
} Instruction;

DECLARE_FIND_COMP(Instruction, FindInstruction);

typedef Instruction InstructionTarget;

TEMPLATE_VECTOR(Instruction) InstructionVector;
TEMPLATE_VECTOR(long long int) Vector;

List ReadPolymerTemplate(FILE * file);

InstructionVector ReadInstructions(FILE * file);
int CompareInstructions(const Instruction * const a, const Instruction * const b);

DECLARE_QUICKSORT_COMP(InstructionQuickSort, Instruction)

void NextStep(List * polymer, const InstructionVector * instructions);
Vector CountFrequecies(List * polymer);

SparseMatrix ReadPolymerTemplateOptimized(FILE * file, Vector * frequencies);
void NextStepOptimized(SparseMatrix * polymer, const InstructionVector * instructions, Vector * frequencies);


// Solving
typedef long long int solution_t;
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
