#ifndef DAY_14_H
#define DAY_14_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "common/testing.h"
#include "common/vector.h"
#include "common/forward_list.h"
#include "common/hash_table.h"

typedef struct
{
	char left;
	char right;
} Target;

typedef struct {
	Target target;
	char insert;
} Instruction;

typedef unsigned long long count_t;

TEMPLATE_VECTOR(Instruction) InstructionVector;
TEMPLATE_HASH_TABLE(size_t, Target, count_t, Polymer);

TEMPLATE_HASH_TABLE(size_t, char, count_t, FreqMap);

TEMPLATE_LIST(char, List);


int CompareInstructions(const Instruction * a, const Instruction * b);
int CompareTargets(const Target * a, const Target * b);

Instruction *FindInstruction(
    Instruction * begin,
    Instruction * end,
    const Target * search);

DECLARE_QUICKSORT_COMP(InstructionQuickSort, Instruction);

List ListFromString(const char * const line);
List ReadPolymerTemplate(FILE * file);

InstructionVector ReadInstructions(FILE * file);

void NextStep(List * polymer, const InstructionVector * instructions);
FreqMap CountFrequecies(List * polymer);

Polymer ReadPolymerTemplateOptimized(FILE * file, FreqMap * frequencies);
void NextStepOptimized(Polymer * polymer, const InstructionVector * instructions, FreqMap * frequencies);

// Solving
typedef count_t solution_t;
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
